# Arquitectura del sistema – Datalogger Argentum

## 1. Descripción general

Datalogger Argentum es un sistema autónomo de adquisición y registro de variables ambientales, desarrollado sobre la plataforma ESP32. Su arquitectura integra sensores digitales, almacenamiento local, conectividad inalámbrica y servicios de almacenamiento remoto.

El sistema fue concebido para el monitoreo de las condiciones ambientales en espacios de exhibición de bienes culturales, dentro del campo de la conservación preventiva.

La arquitectura se organiza en cinco módulos funcionales:

1. Adquisición de variables ambientales.
2. Procesamiento y corrección de mediciones.
3. Almacenamiento local.
4. Comunicación y almacenamiento remoto.
5. Evaluación de condiciones ambientales y gestión de alertas.

## 2. Unidad de procesamiento

El componente central del sistema es el microcontrolador ESP32-WROOM-32E.

Este módulo administra la comunicación con los sensores, procesa las mediciones, gestiona el almacenamiento en memoria, establece la conexión WiFi y controla los ciclos de funcionamiento y bajo consumo energético.

La programación está desarrollada en lenguaje C++ mediante Arduino IDE.

## 3. Módulo de adquisición de datos

El sistema utiliza dos sensores digitales conectados al microcontrolador mediante el bus de comunicación I²C.

### 3.1. Sensor SHT31

El sensor Adafruit SHT31 permite adquirir dos variables ambientales:

- Temperatura, expresada en grados Celsius (°C).
- Humedad relativa, expresada en porcentaje (% HR).

El firmware utiliza la dirección I²C `0x45` para inicializar el sensor.

### 3.2. Sensor BH1750

El sensor BH1750 permite medir la iluminancia del entorno, expresada en lux (lx).

La lectura se utiliza para registrar las condiciones de iluminación y analizar su evolución temporal.

### 3.3. Corrección de las mediciones

Una vez adquiridas las lecturas, el firmware aplica ecuaciones lineales de corrección obtenidas mediante procedimientos de calibración:

**Temperatura:**

\[
T_{\mathrm{corregida}}=1{,}025 \times T_{\mathrm{medida}}-0{,}6151
\]

**Humedad relativa:**

\[
HR_{\mathrm{corregida}}=1{,}1684 \times HR_{\mathrm{medida}}-12{,}487
\]

**Iluminancia:**

\[
E_{\mathrm{corregida}}=1{,}0792 \times E_{\mathrm{medida}}-7{,}8014
\]

Las mediciones corregidas se utilizan posteriormente para el registro local, la transmisión y, cuando corresponde, la evaluación de las condiciones ambientales.

## 4. Módulo de almacenamiento local

El sistema utiliza SPIFFS (SPI Flash File System), un sistema de archivos integrado en la memoria flash del ESP32.

Esta modalidad permite conservar información sin depender de una conexión permanente a Internet.

El firmware contempla la administración de tres archivos:

| Archivo | Función |
|---|---|
| `/data.csv` | Almacenamiento de mediciones y registros de eventos cerrados |
| `/counter.txt` | Conservación del número de muestra |
| `/evento.txt` | Archivo previsto para conservar el estado de los eventos |

El número de muestra se recupera al iniciar el dispositivo, se incrementa y se almacena nuevamente.

Las mediciones se incorporan al archivo CSV mediante la función `appendCSV()`.

El almacenamiento local permite mantener registros durante interrupciones de conectividad. Sin embargo, la disponibilidad de fecha y hora depende de la sincronización mediante NTP.

## 5. Módulo de conectividad y almacenamiento remoto

La comunicación inalámbrica se realiza mediante WiFi, utilizando la biblioteca `WiFi.h`.

El firmware establece un tiempo máximo de espera de aproximadamente 8 segundos para intentar conectarse a la red configurada.

Cuando la conexión se establece correctamente, el dispositivo sincroniza la fecha y hora mediante servidores NTP y habilita la comunicación con Firebase Realtime Database.

### 5.1. Firebase Realtime Database

La transmisión de datos se realiza mediante la biblioteca `Firebase_ESP_Client`.

Las mediciones se envían en formato JSON a la ruta:

`/mediciones`

Cada registro contempla los siguientes campos:

- Número de muestra.
- Fecha.
- Hora.
- Temperatura.
- Humedad relativa.
- Iluminancia.

El sistema también contempla rutas destinadas al estado de las alertas y al historial de eventos.

### 5.2. Funcionamiento sin conexión

Cuando no se establece una conexión WiFi, el dispositivo continúa con la adquisición de variables y el almacenamiento local.

En esta condición, los campos de fecha y hora se identifican como `sin_wifi`, mientras que el estado de transmisión a Firebase se registra como `sin_firebase`.

Esto permite conservar mediciones durante interrupciones de conectividad, aunque sin sincronización temporal mediante NTP ni transmisión remota.

## 6. Módulo de evaluación ambiental y alertas

El firmware incorpora una lógica de evaluación basada en umbrales de temperatura y humedad relativa.

Los valores de referencia definidos son:

| Variable | Límite inferior | Límite superior |
|---|---:|---:|
| Temperatura | 18 °C | 20 °C |
| Humedad relativa | 40 % | 50 % |

La evaluación se realiza mediante la función `checkAndSendAlert()`.

El sistema contempla tres niveles de severidad, determinados por la cantidad de mediciones consecutivas fuera de los rangos establecidos:

- **Alerta leve:** 2 mediciones consecutivas.
- **Alerta confirmada:** 6 mediciones consecutivas.
- **Evento crítico:** 10 o más mediciones consecutivas.

La duración estimada de los eventos se calcula a partir del contador de mediciones y del intervalo programado de 30 minutos.

Cuando las condiciones vuelven a encontrarse dentro de los rangos establecidos, el firmware contempla la generación de un registro de cierre del evento en el archivo CSV.

**Consideración funcional:** en la versión actual, la evaluación de alertas se ejecuta únicamente después de una transmisión exitosa a Firebase. Por lo tanto, esta función no opera de manera independiente de la conectividad.

## 7. Servidor web y mantenimiento

El firmware incorpora un servidor web local mediante la biblioteca `WebServer.h`.

Su función principal es permitir la descarga del archivo de mediciones almacenado en SPIFFS.

Cuando existe conexión WiFi y el archivo `data.csv` está disponible, se habilita un período de mantenimiento de aproximadamente 60 segundos.

Durante ese intervalo, el dispositivo atiende las solicitudes realizadas a la ruta:

`/data`

El archivo se entrega en formato CSV para su posterior consulta y procesamiento.

## 8. Gestión energética

El sistema utiliza el modo Deep Sleep del ESP32 para reducir el consumo energético durante los períodos de inactividad.

El intervalo de suspensión está definido mediante la constante:

`SLEEP_TIME_SEC = 1800`

Esto equivale a 30 minutos.

Una vez completadas las operaciones del ciclo, el firmware desconecta la interfaz WiFi, configura el temporizador de despertar y entra en modo Deep Sleep.

Al finalizar el período programado, el microcontrolador reinicia el ciclo de ejecución.

## 9. Secuencia de funcionamiento

El funcionamiento general puede representarse mediante la siguiente secuencia:

```text
INICIO
  |
  v
Inicialización del ESP32
  |
  v
Inicialización de SPIFFS
  |
  v
Recuperación del contador
  |
  v
Inicialización de sensores
  |
  v
Intento de conexión WiFi
  |
  +---- ¿Hay conexión?
  |         |
  |         +---- SÍ: habilitación del modo
  |         |       mantenimiento y sincronización NTP
  |         |
  |         +---- NO: continúa sin conexión
  |
  v
Adquisición de variables
  |
  v
Corrección de mediciones
  |
  v
¿Hay conexión WiFi?
  |
  +---- SÍ: intento de transmisión a Firebase
  |       |
  |       +---- ¿Transmisión exitosa?
  |                  |
  |                  +---- SÍ: evaluación de alertas
  |                  |
  |                  +---- NO: sin evaluación de alertas
  |
  +---- NO: continúa sin transmisión
  |
  v
Almacenamiento local en CSV
  |
  v
Desconexión WiFi
  |
  v
Deep Sleep (1800 segundos)
  |
  v
NUEVO CICLO
```

## 10. Consideraciones y limitaciones de la versión actual

La arquitectura documentada corresponde a la versión de firmware incluida en este repositorio. Algunas funciones presentan aspectos pendientes de validación y mejora:

- La función `guardarEstadoEvento()` está implementada, pero no se invoca en el ciclo actual. Por lo tanto, el estado de los eventos no se persiste mediante `/evento.txt`.
- La evaluación de alertas se ejecuta después de una transmisión exitosa a Firebase. En consecuencia, las interrupciones de conectividad también afectan la ejecución de esta función.
- El archivo CSV incorpora registros de mediciones y de cierre de eventos con estructuras de columnas diferentes. Esto debe considerarse al realizar su procesamiento automático.
- La variable que informa el estado de alerta puede indicar `SI` cuando la evaluación se ejecutó correctamente, aunque no se haya producido una alerta efectiva.
- La inicialización de los sensores no incluye una verificación explícita de éxito antes de realizar las lecturas.

Estas características deben tenerse en cuenta al interpretar los registros y utilizar el firmware. Su identificación permite establecer aspectos concretos para futuras etapas de desarrollo y validación.

---

Universidad Nacional de San Martín – Laboratorio Centro CEPyA – Centro TAREA.
