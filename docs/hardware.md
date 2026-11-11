# Hardware – Datalogger Argento

## 1. Unidad de procesamiento

El sistema utiliza una placa de desarrollo basada en el microcontrolador **ESP32-WROOM-32E**, encargado de gestionar la adquisición de datos ambientales, el almacenamiento local, la comunicación inalámbrica y la transmisión de información hacia la plataforma remota.

La elección de este microcontrolador responde a sus capacidades de procesamiento, conectividad WiFi y modos de bajo consumo energético, características que permiten implementar un sistema autónomo de monitoreo ambiental.

## 2. Sensores

El dispositivo incorpora dos sensores digitales para el registro de las condiciones ambientales:

### 2.1. Temperatura y humedad relativa

Se utiliza un sensor **Adafruit SHT31**, encargado de medir la temperatura y la humedad relativa del aire.

El sensor se comunica con el microcontrolador mediante el protocolo I²C y permite obtener mediciones digitales de ambas variables.

### 2.2. Iluminancia

Para el registro de la iluminancia se utiliza un sensor **BH1750**, que permite medir la intensidad de la luz incidente y expresar los resultados en lux (lx).

Al igual que el SHT31, se comunica con el ESP32 mediante el protocolo I²C.

## 3. Conexiones de hardware

Los sensores SHT31 y BH1750 comparten el bus de comunicación I²C del microcontrolador ESP32-WROOM-32E.

La asignación de pines utilizada es la siguiente:

| Señal | Pin del ESP32 |
|---|---|
| SCL | GPIO 22 |
| SDA | GPIO 21 |

La alimentación y las conexiones de ambos sensores se integran al circuito del dispositivo, de acuerdo con los requerimientos eléctricos de cada módulo.

## 4. Sistema de alimentación

El dispositivo funciona mediante una batería recargable de ion de litio **modelo 18650**, con las siguientes características nominales:

- Tensión de salida: 3,7–4,2 V CC.
- Capacidad declarada: 6800 mAh.

Para la recarga de la batería se incorpora un módulo **TP4056**, de la empresa ASIC, destinado a la gestión de carga de baterías de ion de litio de una celda.

La alimentación mediante batería permite que el dispositivo funcione de manera autónoma, sin necesidad de permanecer conectado a una fuente de alimentación externa durante su operación habitual.

## 5. Gabinete y montaje físico

Los componentes electrónicos se alojan en una carcasa diseñada e impresa en 3D, desarrollada para contener y proteger el circuito, facilitar su instalación y reducir su impacto visual dentro del espacio expositivo.

En la aplicación realizada en el Museo Histórico Nacional, el dispositivo se instaló en el interior de la vitrina que exhibe un daguerrotipo. El conjunto se ubicó detrás de la pieza, sobre un soporte dispuesto con una inclinación equivalente a la del objeto patrimonial.

Esta disposición permite registrar las condiciones ambientales en el entorno inmediato de la obra y medir la iluminancia en una posición representativa de su exposición a la luz.

## 6. Consideraciones de diseño

El diseño del hardware prioriza los siguientes aspectos:

- **Autonomía energética:** funcionamiento mediante batería recargable y utilización de modos de bajo consumo.
- **Dimensiones reducidas:** integración de los componentes en una carcasa compacta.
- **Bajo impacto visual:** instalación discreta dentro del espacio expositivo.
- **Monitoreo simultáneo:** adquisición de temperatura, humedad relativa e iluminancia.
- **Desarrollo local:** utilización de componentes electrónicos accesibles y fabricación de la carcasa mediante impresión 3D.

- ## 7. Esquema eléctrico de conexión
  <img width="624" height="356" alt="image" src="https://github.com/user-attachments/assets/e3549e46-923f-4401-916d-f8170c5b8aae" />
  
