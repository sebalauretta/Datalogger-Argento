# Operación del sistema – Datalogger Argento

## 1. Introducción

El Datalogger Argento está diseñado para realizar el monitoreo autónomo y periódico de las condiciones ambientales en espacios de exhibición y conservación de bienes culturales.

El dispositivo adquiere mediciones de temperatura, humedad relativa e iluminancia, las almacena en su memoria interna y, cuando dispone de conexión a internet, las transmite a una base de datos remota.

El sistema contempla la posibilidad de funcionar sin conexión WiFi, manteniendo la adquisición y el almacenamiento local de los registros.

## 2. Ciclo de funcionamiento

El funcionamiento del dispositivo se organiza en ciclos periódicos de adquisición de datos.

Cada ciclo comprende las siguientes etapas:

1. **Inicialización:** configuración del microcontrolador y de los periféricos necesarios para el funcionamiento del sistema.
2. **Adquisición:** lectura de los sensores de temperatura, humedad relativa e iluminancia.
3. **Procesamiento:** aplicación de las ecuaciones de calibración correspondientes.
4. **Almacenamiento local:** registro de las mediciones en la memoria interna SPIFFS.
5. **Conectividad:** intento de conexión a una red WiFi.
6. **Sincronización:** obtención de fecha y hora mediante NTP y transmisión de los registros a Firebase Realtime Database, cuando existe conectividad.
7. **Gestión energética:** entrada en modo de bajo consumo hasta el siguiente ciclo de medición.

El intervalo programado entre ciclos de adquisición es de 30 minutos.

## 3. Almacenamiento local

El dispositivo utiliza el sistema de archivos SPIFFS, integrado en la memoria flash del ESP32, para conservar información durante su funcionamiento.

Entre los archivos utilizados se encuentran:

- `/data.csv`: archivo destinado al almacenamiento de las mediciones y los estados asociados al proceso de transmisión.
- `/counter.txt`: archivo utilizado para conservar el contador de muestras.

El almacenamiento local permite mantener un registro de las mediciones incluso cuando no se dispone de conexión WiFi o cuando se producen inconvenientes durante la transmisión de información.

## 4. Conectividad y transmisión de datos

El sistema utiliza conectividad WiFi para establecer comunicación con Firebase Realtime Database.

Cuando la conexión está disponible, el dispositivo obtiene la referencia temporal mediante NTP y transmite los registros a la plataforma remota.

La información se organiza en la base de datos bajo la estructura correspondiente a las mediciones del dispositivo.

Si no se logra establecer conexión WiFi, el sistema continúa con su funcionamiento autónomo y conserva los registros en la memoria interna. En estas circunstancias, no puede garantizarse la sincronización de fecha y hora ni la transmisión a Firebase.

Asimismo, pueden producirse fallas de comunicación con el servicio remoto aun cuando exista conexión WiFi. Por este motivo, el estado de conectividad y el resultado de la transmisión se registran como condiciones diferenciadas.

## 5. Sistema de alertas

El firmware incorpora un mecanismo de evaluación de las condiciones ambientales a partir de umbrales establecidos para temperatura y humedad relativa.

Los valores de referencia utilizados son:

| Variable | Límite inferior | Límite superior |
|---|---:|---:|
| Temperatura | 18 °C | 20 °C |
| Humedad relativa | 40 % | 50 % |

La lógica de alertas considera la persistencia de las condiciones fuera de los rangos establecidos:

- **Alerta leve:** dos mediciones consecutivas fuera de rango.
- **Alerta confirmada:** seis mediciones consecutivas fuera de rango.
- **Evento crítico:** diez o más mediciones consecutivas fuera de rango.

La evaluación de las alertas está condicionada al resultado satisfactorio de la transmisión a Firebase, de acuerdo con la implementación actual del firmware.

## 6. Modo de bajo consumo

Para reducir el consumo energético y favorecer la autonomía del dispositivo, se utiliza el modo Deep Sleep del ESP32.

Una vez finalizado el ciclo de adquisición y las operaciones correspondientes, el microcontrolador ingresa en un período de suspensión de 1800 segundos (30 minutos).

Al finalizar este intervalo, el dispositivo se reactiva y comienza un nuevo ciclo de funcionamiento.

## 7. Modo de mantenimiento y recuperación de datos

El firmware incorpora un servidor web local para facilitar las tareas de mantenimiento y recuperación de información.

Durante el modo de mantenimiento, el dispositivo habilita temporalmente una interfaz web desde la cual es posible acceder a los registros almacenados localmente.

La ruta `/data` permite descargar el archivo CSV con las mediciones registradas.

El modo de mantenimiento permanece disponible durante un período aproximado de 60 segundos.

## 8. Consideraciones operativas

El funcionamiento del sistema depende de diferentes condiciones de infraestructura y comunicación:

- La ausencia de WiFi no interrumpe la adquisición de mediciones ni su almacenamiento local.
- La falta de sincronización NTP impide disponer de una referencia temporal actualizada para los registros.
- La conexión WiFi no garantiza por sí sola la correcta transmisión de datos a Firebase.
- Las interrupciones del servicio de internet pueden generar discontinuidades en los registros disponibles en la plataforma remota.
- El mecanismo de alertas presenta limitaciones asociadas a su dependencia de la transmisión a Firebase y a la persistencia del estado de los eventos.

Por estas razones, se recomienda contemplar tanto el almacenamiento local como la información disponible en la plataforma remota al momento de recuperar y analizar los registros ambientales.

## 9. Estado de desarrollo

El Datalogger Argento constituye un desarrollo tecnológico en evolución. Las características descriptas corresponden a la implementación documentada del firmware y pueden modificarse a medida que se realicen nuevas pruebas y mejoras del sistema.
