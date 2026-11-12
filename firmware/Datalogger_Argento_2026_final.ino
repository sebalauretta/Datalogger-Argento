// *** Datalogger Argento *** 
//Autoria: Sebastián Lauretta
// Dirección Gral: Dr. Marcos Tascon - Ana Morales
// Institución : Universidad Nacional de San Martín - Laboratorio CEPyA y Centro TAREA

#include <WiFi.h>
#include <time.h>
#include <SPIFFS.h>
#include <Wire.h>
#include <BH1750.h>
#include <Adafruit_SHT31.h>
#include <Firebase_ESP_Client.h>
#include <WebServer.h>

/* ========= CONFIGURACION ========= */
#define SLEEP_TIME_SEC        1800
#define MAINTENANCE_TIME_MS  60000
#define ALERTA_LEVE         2
#define ALERTA_CONFIRMADA   6
#define EVENTO_CRITICO      10
#define INTERVALO_MIN       30
#define EVENT_FILE "/evento.txt"

/***** PARAMETROS DE CONSERVACION *****/
#define TEMP_MIN 18
#define TEMP_MAX 20
#define HUM_MIN  40
#define HUM_MAX  50

const char* ssid = "id_red";
const char* password = "contraseña_red";

/* ========= FIREBASE ========= */
#define FIREBASE_HOST "https://firebaseio.com/"
#define FIREBASE_AUTH "contraseña_firebase"

FirebaseData fbdo;
FirebaseConfig config;
FirebaseAuth auth;
FirebaseData firebaseData;

/* ========= SENSORES ========= */
BH1750 lightMeter;
Adafruit_SHT31 sht31 = Adafruit_SHT31();

/* ========= WEB ========= */
WebServer server(80);

/* ========= REGISTRO INTERNO ========= */
#define CSV_FILE     "/data.csv"
#define COUNTER_FILE "/counter.txt"

/* ========= FUNCIONES ========= */

uint32_t readCounter() {
  if (!SPIFFS.exists(COUNTER_FILE)) return 0;
  File f = SPIFFS.open(COUNTER_FILE, FILE_READ);
  if (!f) return 0;
  uint32_t n = f.readString().toInt();
  f.close();
  return n;
}

void writeCounter(uint32_t n) {
  File f = SPIFFS.open(COUNTER_FILE, FILE_WRITE);
  if (f) {
    f.print(n);
    f.close();
  }
}

void writeHeaderIfNeeded() {
  if (!SPIFFS.exists(CSV_FILE)) {
    File f = SPIFFS.open(CSV_FILE, FILE_WRITE);
    f.println("muestra,fecha,hora,temperatura,humedad,lux,firebase,alerta");
    f.close();
  }
}

void leerEstadoEvento(int &contador, int &nivelMax, bool &activo, String &fInicio, String &hInicio) {

  if (!SPIFFS.exists(EVENT_FILE)) {
    contador = 0;
    nivelMax = 0;
    activo = false;
    fInicio = "";
    hInicio = "";
    return;
  }

  File file = SPIFFS.open(EVENT_FILE, "r");
  String data = file.readString();
  file.close();

  int p1 = data.indexOf(',');
  int p2 = data.indexOf(',', p1 + 1);
  int p3 = data.indexOf(',', p2 + 1);
  int p4 = data.indexOf(',', p3 + 1);

  contador = data.substring(0, p1).toInt();
  nivelMax = data.substring(p1 + 1, p2).toInt();
  activo = data.substring(p2 + 1, p3).toInt();

  fInicio = data.substring(p3 + 1, p4);
  hInicio = data.substring(p4 + 1);
}

void guardarEstadoEvento(int contador, int nivelMax, bool activo, String fInicio, String hInicio) {

  File file = SPIFFS.open(EVENT_FILE, "w");

  file.print(String(contador) + "," +
             String(nivelMax) + "," +
             String(activo) + "," +
             fInicio + "," +
             hInicio);

  file.close();
}

void appendCSV(String line) {
  File f = SPIFFS.open(CSV_FILE, FILE_APPEND);
  if (f) {
    f.println(line);
    f.close();
  }
}

void dumpCSV() {
  File f = SPIFFS.open(CSV_FILE, FILE_READ);
  if (!f) {
    Serial.println("ERROR: no existe data.csv");
    return;
  }
  Serial.println("\n----- INICIO CSV -----");
  while (f.available()) Serial.write(f.read());
  Serial.println("\n----- FIN CSV -----");
  f.close();
}

void clearFlash() {
  SPIFFS.remove(CSV_FILE);
  SPIFFS.remove(COUNTER_FILE);
  Serial.println("✔ SPIFFS borrado");
}

bool connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 8000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  return WiFi.status() == WL_CONNECTED;
}

bool sendToFirebase(String fecha, String hora, float t, float h, float lux, uint32_t muestra) {
  if (!Firebase.ready()) return false;

  FirebaseJson json;
  json.set("muestra", muestra);
  json.set("fecha", fecha);
  json.set("hora", hora);
  json.set("temperatura", t);
  json.set("humedad", h);
  json.set("lux", lux);

  return Firebase.RTDB.pushJSON(&firebaseData, "/mediciones", &json);
}
/* ========= ALERTA + HISTORIAL ========= */

bool checkAndSendAlert(String fecha, String hora, float t, float h, uint32_t muestra) {
bool fueraDeRango = (t < TEMP_MIN || t > TEMP_MAX || h < HUM_MIN || h > HUM_MAX);
  if (!Firebase.ready()) return false;

  bool alerta = false;
  String mensaje = "";

  if (t < 18|| t > 20) {
    alerta = true;
    mensaje += "Temperatura fuera de rango. ";
  }

  if (h < 40 || h > 50) {
    alerta = true;
    mensaje += "Humedad fuera de rango. ";
  }

int contador;
int nivelMax;
bool eventoActivo;
String fechaInicio;
String horaInicio;

leerEstadoEvento(contador, nivelMax, eventoActivo, fechaInicio, horaInicio);

if (fueraDeRango) {

  contador++;

  if (contador == 1) {
    fechaInicio = fecha;
    horaInicio = hora;
  }

  int nivelActual = 0;

  if (contador >= EVENTO_CRITICO) nivelActual = 3;
  else if (contador >= ALERTA_CONFIRMADA) nivelActual = 2;
  else if (contador >= ALERTA_LEVE) nivelActual = 1;

  if (nivelActual > nivelMax) nivelMax = nivelActual;

  eventoActivo = true;

  if (nivelActual == 3) {
    Firebase.RTDB.setString(&fbdo, "/alertas/critica",
      "Evento crítico en curso - " + fecha + " " + hora);
  }

} else {

  if (eventoActivo) {

    int duracion = contador * INTERVALO_MIN;

    String registro = "EVENTO_CERRADO," +
                      fechaInicio + "," +
                      horaInicio + "," +
                      fecha + "," +
                      hora + "," +
                      String(duracion) + "," +
                      String(nivelMax);

    appendCSV(registro);
  }

  contador = 0;
  nivelMax = 0;
  eventoActivo = false;
}

  if (alerta) {
     // Estado actual
    Firebase.RTDB.setString(&firebaseData, "/estado/alerta", "ACTIVA");
    Firebase.RTDB.setString(&firebaseData, "/estado/mensaje", mensaje);

    // Historial
    FirebaseJson alertaJson;
    alertaJson.set("muestra", muestra);
    alertaJson.set("fecha", fecha);
    alertaJson.set("hora", hora);
    alertaJson.set("temperatura", t);
    alertaJson.set("humedad", h);
    alertaJson.set("mensaje", mensaje);

    Firebase.RTDB.pushJSON(&firebaseData, "/alertas_historial", &alertaJson);

  } else {
    Firebase.RTDB.setString(&firebaseData, "/estado/alerta", "NORMAL");
    Firebase.RTDB.setString(&firebaseData, "/estado/mensaje", "");
  }

  return true;
}
/* ========= WEB ========= */

void handleDownload() {
  File f = SPIFFS.open(CSV_FILE, FILE_READ);
  if (!f) {
    server.send(404, "text/plain", "No existe data.csv");
    return;
  }

  server.sendHeader("Content-Disposition", "attachment; filename=data.csv");
  server.streamFile(f, "text/csv");
  f.close();
}

void startWebServer() {
  server.on("/data", HTTP_GET, handleDownload);
  server.begin();
  Serial.println("🌐 Servidor web activo");
}

/* ========= SETUP ========= */

void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println("\n===== DataLogger Argento =====");

  if (!SPIFFS.begin(true)) {
    Serial.println("ERROR SPIFFS");
    return;
  }

  /* ===== COMANDOS SERIAL ===== */
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'B') { dumpCSV(); while (true); }
    if (c == 'V') { clearFlash(); while (true); }
  }

  /* ===== CONTADOR ===== */
  uint32_t muestra = readCounter() + 1;
  writeCounter(muestra);

  /* ===== SENSORES ===== */
  Wire.begin();
  lightMeter.begin();
  sht31.begin(0x45);

  /* ===== WIFI ===== */
  bool wifiOK = connectWiFi();

  if (wifiOK) {
    Serial.print("WiFi OK - IP: ");
    Serial.println(WiFi.localIP());
  }

  /* ===== MODO MANTENIMIENTO ===== */
  if (wifiOK && SPIFFS.exists(CSV_FILE)) {
    Serial.println("🔧 MODO MANTENIMIENTO");
    Serial.println("Descarga CSV en:");
    Serial.println("http://" + WiFi.localIP().toString() + "/data");

    startWebServer();

    unsigned long t0 = millis();
    while (millis() - t0 < MAINTENANCE_TIME_MS) {
      server.handleClient();
      delay(5);
    }
    Serial.println("⏱ Fin mantenimiento");
  }

  /* ===== FECHA - DIA Y HORA ===== */
  String fecha = "sin_wifi";
  String hora  = "sin_wifi";

  if (wifiOK) {
    configTime(-3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
      char f[11], hstr[9];
      strftime(f, sizeof(f), "%d/%m/%Y", &timeinfo);
      strftime(hstr, sizeof(hstr), "%H:%M:%S", &timeinfo);
      fecha = f;
      hora = hstr;
    }
  }

    /* ===== LECTURA ===== */
      float t = sht31.readTemperature();
     float h = sht31.readHumidity();
     float lux = lightMeter.readLightLevel();
 
  /* ===== CORRECCIÓN SEGÚN CALIBRACIÓN ===== */
       t = 1.025 * t - 0.6151;
       h = 1.1684 * h - 12.487;
       lux = 1.0792 * lux - 7.8014;


  /* ===== FIREBASE ===== */
  String firebaseStatus = "sin_firebase";
  bool huboAlerta = false;
String alertaStatus = "sin_firebase";

  if (wifiOK) {
    config.database_url = FIREBASE_HOST;
    config.signer.tokens.legacy_token = FIREBASE_AUTH;
    Firebase.begin(&config, &auth);

if (sendToFirebase(fecha, hora, t, h, lux, muestra)) {

  firebaseStatus = "ok";

  huboAlerta = checkAndSendAlert(fecha, hora, t, h, muestra);

  if (huboAlerta) {
    Serial.println("⚠ ALERTA GENERADA");
    alertaStatus = "SI";
  } else {
    alertaStatus = "NO";
  }
}
}
      


  /* ===== CSV ===== */
  writeHeaderIfNeeded();

String line = String(muestra) + "," +
              fecha + "," +
              hora + "," +
              String(t, 2) + "," +
              String(h, 2) + "," +
              String(lux, 2) + "," +
              firebaseStatus + "," +
              alertaStatus;

appendCSV(line);

  /* ===== INFO ===== */
  Serial.println("\n------------  Datalogger Argento -------------------");
  Serial.println("Muestra: " + String(muestra));
  Serial.println("Fecha: " + fecha);
  Serial.println("Hora: " + hora);
  Serial.printf("Temp: %.2f °C\n", t);
  Serial.printf("Hum: %.2f %%\n", h);
  Serial.printf("Luz: %.2f lx\n", lux);
  Serial.println("Firebase: " + firebaseStatus);
  Serial.println("------------ UNSAM - CEPyA - TAREA  -------------------");


  /* ===== SLEEP ===== */
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  esp_sleep_enable_timer_wakeup(SLEEP_TIME_SEC * 1000000ULL);
  esp_deep_sleep_start();

} 
void loop() {
}

