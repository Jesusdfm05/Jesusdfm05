# GUÍA TÉCNICA: ESTRUCTURAS DE DATOS

Esta guía técnica compila los fundamentos teóricos, mecanismos operativos y criterios de eficiencia computacional para la recuperación de información en entornos de software, estructurada bajo los requerimientos de la Universidad Nacional Experimental del Táchira (UNET).

---

## 1. MÉTODOS DE BÚSQUEDA

### 1.1. Introducción a las Búsquedas
La búsqueda es uno de los procesos más importantes en el procesamiento de datos y está referida a una actividad cotidiana de los seres humanos. En nuestro día a día buscamos en una lista de precios, en un directorio telefónico o en el índice de un libro con el propósito de estar informados.

En informática, buscar consiste en revisar un volumen de datos para localizar coincidencias con un **dato clave** y recuperar la información que se había almacenado con anticipación. El proceso consiste en revisar los elementos disponibles hasta encontrar el dato buscado o hasta llegar al final de la estructura. 

El resultado de este proceso se divide en dos escenarios:
* **Éxito:** Se localiza la clave de búsqueda y se retorna su posición o el registro asociado.
* **Fracaso (Proceso fallido):** Se agotan los elementos disponibles sin localizar la clave de búsqueda.

En la práctica, las búsquedas se realizan preferentemente sobre datos ordenados (por ejemplo, clasificados ascendentemente) para facilitar y acelerar la localización de la información. Sin embargo, también existen métodos diseñados para operar sobre estructuras no ordenadas cuando el costo de ordenar los datos es prohibitivo.

### 1.2. Clasificación: Búsqueda Interna vs. Externa
Los métodos de búsqueda se clasifican según la ubicación física de los datos dentro de la arquitectura del computador:

* **Búsqueda Interna:** Se realiza sobre datos almacenados en la **memoria principal (RAM)** del ordenador utilizando estructuras estáticas o dinámicas (arreglos, listas, árboles). Destaca por su alta velocidad debido al acceso aleatorio directo de la memoria.
* **Búsqueda Externa:** Se ejecuta sobre datos residentes en dispositivos de **almacenamiento secundario** (disco duro, unidades de estado sólido, pendrives) organizados en archivos binarios o de texto. Está supeditada a la latencia de las operaciones de Entrada/Salida (E/S), por lo que busca minimizar los accesos al medio físico.

### 1.3. Métodos de Búsqueda Interna (Memoria RAM)

#### 1.3.1. Búsqueda Secuencial o Lineal
* **Mecanismo Operativo:** El algoritmo recorre la estructura desde el primer elemento hasta el último de forma lineal, comparando cada celda con el dato clave.
* **Optimización:** Si el conjunto de datos está preordenado de forma ascendente, el proceso se interrumpe inmediatamente en caso de fracaso cuando el elemento actual de la estructura supera el valor de la clave buscada, evitando recorrer el resto del arreglo.
* **Criterio de Selección:** Ideal para colecciones de datos pequeñas, estructuras dinámicas basadas en punteros (como listas enlazadas simples) o cuando los datos no están ordenados y no se justifica el costo de ordenarlos.
* **Complejidad Algorítmica:** * Peor caso y caso promedio: $O(n)$
  * Mejor caso: $O(1)$

#### 1.3.2. Búsqueda Binaria
* **Mecanismo Operativo:** Utiliza el paradigma "Divide y Vencerás". Calcula la posición central del intervalo de búsqueda y compara el elemento central con la clave. Si no son iguales, reduce el intervalo a la mitad: si la clave es mayor, redefine el límite inferior; si es menor, redefine el límite superior. El ciclo se repite hasta hallar el elemento o hasta que el intervalo sea nulo.
* **Precondición Crítica:** La estructura de datos debe estar **estrictamente ordenada**.
* **Criterio de Selección:** Recomendado para arreglos o vectores de tamaño mediano a grande que se ordenan una vez pero se consultan masivamente de forma repetida. No es eficiente en listas enlazadas puras debido a la falta de acceso indexado directo.
* **Complejidad Algorítmica:**
  * Peor caso y caso promedio: $O(\log n)$
  * Mejor caso: $O(1)$

#### 1.3.3. Búsqueda por Transformación de Claves (Hash)
* **Mecanismo Operativo:** Realiza el acceso de manera directa sin recorrer los datos. Aplica una función matemática (función hash) sobre la clave de búsqueda para calcular directamente el índice o posición del arreglo donde reside la información. Requiere un mecanismo secundario para resolver **colisiones** (cuando dos claves distintas generan la misma posición).
* **Criterio de Selección:** Se selecciona cuando se requiere la máxima velocidad de respuesta posible en estructuras estáticas en memoria RAM, como diccionarios, tablas de símbolos de compiladores o cachés de datos.
* **Complejidad Algorítmica:**
  * Caso promedio y mejor caso: $O(1)$
  * Peor caso (alta tasa de colisiones): $O(n)$

### 1.4. Métodos de Búsqueda Externa (Archivos)

#### 1.4.1. Búsqueda Secuencial Externa
* **Mecanismo Operativo:** Abre el archivo físico y lee secuencialmente registro por registro desde el inicio, comparando el campo clave hasta encontrar concordancia o alcanzar el fin de archivo (`EOF`).
* **Optimizaciones:** * **Búsqueda secuencial por bloques:** Agrupa los registros en bloques físicos para leer múltiples datos en una sola operación de disco.
  * **Búsqueda secuencial indexada:** Utiliza un archivo de índices auxiliar (más pequeño) mapeado en memoria que indica dónde comienzan ciertos rangos de datos en el archivo principal, permitiendo saltar directamente al bloque correspondiente.
* **Criterio de Selección:** Útil para archivos pequeños o procesos de procesamiento por lotes (batch) donde se requiere procesar la totalidad de la información de forma secuencial.
* **Complejidad Algorítmica:** Condicionada por el número de lecturas en disco, siendo proporcional al número de registros analizados.

#### 1.4.2. Búsqueda Binaria Externa
* **Mecanismo Operativo:** Sigue la misma lógica que la búsqueda binaria interna. Se posiciona en el registro central del archivo calculando su desplazamiento en bytes (utilizando funciones de posicionamiento de archivos como `seekg` en C++) y redefine los extremos del intervalo reduciendo a la mitad el espacio de búsqueda en disco en cada iteración.
* **Precondición Crítica:** El archivo binario debe estar ordenado físicamente por su campo clave y sus registros deben ser de **tamaño fijo** para permitir el cálculo exacto de las posiciones en bytes.
* **Criterio de Selección:** Adecuado para archivos binarios medianos de acceso aleatorio con baja frecuencia de inserciones (ya que mantener el archivo ordenado en disco es costoso).

#### 1.4.3. Búsqueda por Transformación de Claves (Hash por Cubetas)
* **Mecanismo Operativo:** Aplica el principio de hashing mapeando las claves no a celdas individuales de memoria, sino a bloques de registros en almacenamiento secundario llamados **cubetas (buckets)**. La función hash calcula el número de bloque o dirección física del archivo en donde se encuentra almacenado el registro, leyendo el bloque completo a memoria para su procesamiento.
* **Criterio de Selección:** Es el método estándar para la organización de archivos de acceso directo en bases de datos donde se requiere recuperar registros individuales instantáneamente sin importar el tamaño total del archivo en disco.

#### 1.4.4. Búsqueda con Registros Enlazados (Listas o Multilistas)
* **Mecanismo Operativo:** Cada registro guardado en el archivo físico incluye uno o más campos adicionales que actúan como punteros físicos (almacenando el número de registro o la posición exacta en bytes del siguiente elemento). El algoritmo realiza búsquedas transversales saltando de registro en registro siguiendo estas cadenas de enlaces internos.
* **Criterio de Selección:** Ideal para archivos cuyos datos cambian constantemente (altas y bajas), permitiendo mantener un orden lógico o clasificar los registros bajo múltiples criterios de búsqueda independientes (multilistas) sin necesidad de reordenar físicamente el archivo en el disco.