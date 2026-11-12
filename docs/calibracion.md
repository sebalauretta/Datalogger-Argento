# Calibración de sensores – Datalogger Argento

## 1. Objetivo

La calibración de los sensores tiene como objetivo mejorar la correspondencia entre los valores registrados por el Datalogger Argento y los valores de referencia obtenidos mediante instrumentos de medición externos.

Para ello, se establecen relaciones matemáticas entre las mediciones originales y los valores de referencia. Estas relaciones permiten aplicar correcciones a los datos adquiridos por el dispositivo.

## 2. Calibración de temperatura y humedad relativa

Las mediciones obtenidas mediante el sensor SHT31 se corrigen utilizando ecuaciones lineales de ajuste.

### 2.1. Humedad relativa

La ecuación de calibración obtenida para el higrómetro es:

\[
y = 1,1684x - 12,487
\]

Coeficiente de determinación:

\[
R^2 = 0,9904
\]

### 2.2. Temperatura

La ecuación de calibración obtenida para el termómetro es:

\[
y = 1,025x - 0,6151
\]

Coeficiente de determinación:

\[
R^2 = 0,9960
\]

## 3. Calibración de iluminancia

Para evaluar la respuesta del sensor BH1750 se realizaron mediciones comparativas con un luxómetro de referencia OEM DLY-1801C.

El procedimiento contempló la obtención de diez pares de mediciones bajo iluminación artificial, registrando los valores de ambos instrumentos para analizar su correspondencia.

La ecuación de calibración obtenida es:

\[
y = 1,0792x - 7,8014
\]

Coeficiente de determinación:

\[
R^2 = 0,9998
\]

## 4. Resumen de resultados

| Variable | Ecuación de calibración | R² |
|---|---|---:|
| Humedad relativa | y = 1,1684x − 12,487 | 0,9904 |
| Temperatura | y = 1,025x − 0,6151 | 0,9960 |
| Iluminancia | y = 1,0792x − 7,8014 | 0,9998 |

## 5. Aplicación de las ecuaciones

Las ecuaciones de ajuste se incorporan al procesamiento de las mediciones, de modo que los valores corregidos puedan utilizarse en el registro y seguimiento de las condiciones ambientales.

En las expresiones presentadas, \(x\) representa el valor medido originalmente por el sensor e \(y\) el valor corregido mediante la ecuación de calibración.

## 6. Consideraciones y limitaciones

Los coeficientes de determinación obtenidos muestran una elevada correspondencia lineal entre los valores medidos y los valores de referencia en los ensayos realizados.

Sin embargo, un valor elevado de R² no constituye por sí solo una medida de exactitud ni permite determinar la incertidumbre de medición. La interpretación de los resultados debe considerar también los instrumentos de referencia, las condiciones de ensayo y el rango de medición evaluado.

La conservación de las ecuaciones y sus parámetros estadísticos permite documentar la trazabilidad del proceso de calibración y facilita futuras verificaciones del sistema.
