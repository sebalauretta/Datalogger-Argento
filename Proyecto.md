## Registrador de datos ambientales

### Sistema IoT de monitoreo ambiental aplicado a la conservación preventiva del patrimonio cultural

**Datalogger Argento** es un dispositivo de monitoreo ambiental desarrollado para registrar y supervisar las condiciones microclimáticas en espacios de exhibición patrimonial. El proyecto busca contribuir a la conservación preventiva mediante la adquisición periódica de datos ambientales, su almacenamiento local y su transmisión a una plataforma de visualización remota.

El desarrollo integra electrónica, programación, sensores ambientales y tecnologías de Internet de las Cosas (IoT), con un enfoque de bajo costo, bajo consumo energético y fabricación local.

---

## 1. Objetivos del proyecto

- Registrar periódicamente las condiciones ambientales de espacios de exhibición.
- Monitorear temperatura, humedad relativa e iluminancia.
- Generar registros históricos que permitan analizar las variaciones ambientales a lo largo del tiempo.
- Identificar condiciones que puedan representar riesgos para la conservación de los bienes culturales.
- Facilitar el acceso y la consulta de los datos mediante almacenamiento local y servicios en la nube.
- Explorar el desarrollo de tecnologías nacionales aplicadas a la conservación del patrimonio cultural.

## 2. Desarrollo tecnológico

El dispositivo fue desarrollado desde cero en el Laboratorio Centro CEPyA de la Universidad Nacional de San Martín (UNSAM), en articulación con el Centro TAREA, en el marco del Trabajo Final Integrador de la Licenciatura en Conservación y Restauración del Patrimonio Cultural.

El sistema utiliza una placa ESP32 como unidad de procesamiento y comunicación. Su programación está realizada en lenguaje C++ mediante Arduino IDE.

La carcasa fue diseñada específicamente para alojar los componentes electrónicos y fabricada mediante impresión 3D.

**Autoría:** Sebastián Lauretta  
**Dirección:** Dr. Marcos Tascon y Ana Morales  
**Instituciones:** Universidad Nacional de San Martín – Laboratorio Centro CEPyA y Centro TAREA.  
**Aplicación:** Museo Histórico Nacional – Vitrina del daguerrotipo.

## 3. Componentes de hardware

| Componente | Función |
|---|---|
| ESP32-WROOM-32E | Procesamiento y comunicación inalámbrica |
| Adafruit SHT31 | Medición de temperatura y humedad relativa |
| BH1750 | Medición de iluminancia |
| Memoria SPIFFS | Almacenamiento local de registros |
| Batería recargable | Alimentación autónoma |
| Carcasa impresa en 3D | Protección y alojamiento de los componentes |

## 4. Variables ambientales

El sistema registra tres variables ambientales:

- **Temperatura (°C):** permite evaluar las condiciones térmicas del entorno de exhibición.
- **Humedad relativa (% HR):** permite identificar fluctuaciones y condiciones potencialmente desfavorables para los materiales patrimoniales.
- **Iluminancia (lux):** permite registrar los niveles de iluminación y analizar su evolución temporal.

Las lecturas de los sensores incorporan ecuaciones de corrección obtenidas mediante procedimientos de calibración.

## 5. Arquitectura y funcionamiento

El funcionamiento del dispositivo se organiza en ciclos automáticos:

1. Inicialización del microcontrolador y del sistema de archivos SPIFFS.
2. Recuperación e incremento del contador de muestras.
3. Inicialización de los sensores ambientales.
4. Intento de conexión a una red WiFi.
5. Habilitación de un modo de mantenimiento para la descarga local de registros, cuando se cumplen las condiciones de conexión.
6. Sincronización de fecha y hora mediante servidores NTP.
7. Adquisición de temperatura, humedad relativa e iluminancia.
8. Aplicación de las ecuaciones de corrección de calibración.
9. Transmisión de las mediciones a Firebase Realtime Database, cuando existe conexión.
10. Evaluación de las condiciones ambientales y gestión de alertas.
11. Almacenamiento local de las mediciones en formato CSV.
12. Desconexión de WiFi y entrada en modo Deep Sleep.

El intervalo programado entre ciclos de medición es de 30 minutos.

## 6. Almacenamiento y transmisión de datos

El dispositivo contempla dos modalidades de almacenamiento:

**Almacenamiento local**

Los registros se guardan en la memoria SPIFFS del ESP32, en un archivo denominado `data.csv`. Este mecanismo permite conservar mediciones cuando no existe conexión a Internet.

El archivo contempla los siguientes campos:

- Número de muestra.
- Fecha.
- Hora.
- Temperatura.
- Humedad relativa.
- Iluminancia.
- Estado de conexión WiFi.
- Estado de transmisión a Firebase.
- Estado de alerta.

**Almacenamiento remoto**

Cuando existe conexión WiFi y Firebase se encuentra disponible, las mediciones se transmiten a Firebase Realtime Database mediante la biblioteca `Firebase_ESP_Client`.

La información se organiza en diferentes rutas de la base de datos destinadas al registro de mediciones y a la gestión de estados e historial de alertas.

## 7. Sistema de alertas ambientales

El firmware incorpora criterios de evaluación basados en los siguientes parámetros:

| Variable | Límite inferior | Límite superior |
|---|---:|---:|
| Temperatura | 18 °C | 20 °C |
| Humedad relativa | 40 % | 50 % |

La lógica contempla distintos niveles de severidad según la cantidad de mediciones consecutivas fuera de rango:

| Nivel | Condición |
|---|---|
| Alerta leve | 2 mediciones consecutivas |
| Alerta confirmada | 6 mediciones consecutivas |
| Evento crítico | 10 o más mediciones consecutivas |

Los parámetros están definidos para un intervalo de medición de 30 minutos y deben ser configurados específicamente para cada caso de aplicación según las recomendaciones de conservación preventiva.

## 8. Gestión energética

Para reducir el consumo energético, el firmware incorpora el modo Deep Sleep del ESP32.

El dispositivo permanece activo durante el ciclo de adquisición, almacenamiento y transmisión de datos, y posteriormente ingresa en un estado de bajo consumo durante 1800 segundos (30 minutos).

Este mecanismo permite optimizar la autonomía de alimentación del sistema.

## 9. Descarga y mantenimiento

El firmware incorpora un servidor web local que permite descargar el archivo CSV almacenado en la memoria SPIFFS.

Cuando existe conexión WiFi y el archivo de mediciones está disponible, se habilita una ventana de mantenimiento de aproximadamente 60 segundos.

La descarga se realiza accediendo desde un dispositivo conectado a la misma red WiFi a la dirección IP del ESP32, seguida de la ruta `/data`.

## 10. Tecnologías y bibliotecas

- Arduino IDE
- Lenguaje C++
- ESP32
- WiFi
- SPIFFS
- Firebase Realtime Database
- NTP
- WebServer
- BH1750
- Adafruit SHT31
- Firebase_ESP_Client

## 11. Estructura del repositorio

```text
Datalogger-Argento/
│
├── README.md
│
├── firmware/
│   └── Datalogger_Argento_2026.ino
│
├── docs/
│   └── arquitectura.md
│   └── calibración.md
│   └── hardware.md
│   └── operación.md
│
├── 3d_models/
│   └── comentarios.md
│   └── ESP32-WROOM-32E.stl
│   └── porta bateria con modulos.stl
│
└── .gitignore
```

## 12. Configuración y utilización

Para utilizar el firmware se requiere:

1. Instalar Arduino IDE.
2. Incorporar el soporte para placas ESP32.
3. Instalar las bibliotecas correspondientes a los sensores y Firebase.
4. Configurar las credenciales de conexión WiFi y Firebase de manera privada.
5. Seleccionar la placa ESP32 correspondiente y cargar el firmware.

## 13. Estado del desarrollo

Datalogger Argento es un desarrollo tecnológico en evolución. El firmware publicado corresponde a una versión de trabajo que reúne funciones de adquisición, registro, transmisión y evaluación de condiciones ambientales.

La publicación tiene como finalidad documentar el desarrollo, facilitar su consulta y contribuir a futuras mejoras, validaciones y adaptaciones del sistema.

---

**Tecnología aplicada a la conservación preventiva del patrimonio cultural.**

Universidad Nacional de San Martín – Laboratorio Centro CEPyA – Centro TAREA.
