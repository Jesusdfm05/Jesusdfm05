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

```cpp
#include <iostream>
#include <string>

struct EstudianteUNET {
    int cedula;
    std::string nombre;
    float indiceAcademico;
};

int buscarPorCedulaLineal(const EstudianteUNET lista[], int n, int cedulaBuscada) {
    for (int i = 0; i < n; i++) {
        if (lista[i].cedula == cedulaBuscada) {
            return i; 
        }
        if (lista[i].cedula > cedulaBuscada) {
            break; 
        }
    }
    return -1; 
}
```

#### 1.3.2. Búsqueda Binaria
* **Mecanismo Operativo:** Utiliza el paradigma "Divide y Vencerás". Calcula la posición central del intervalo de búsqueda y compara el elemento central con la clave. Si no son iguales, reduce el intervalo a la mitad: si la clave es mayor, redefine el límite inferior; si es menor, redefine el límite superior. El ciclo se repite hasta hallar el elemento o hasta que el intervalo sea nulo.
* **Precondición Crítica:** La estructura de datos debe estar **estrictamente ordenada**.
* **Criterio de Selección:** Recomendado para arreglos o vectores de tamaño mediano a grande que se ordenan una vez pero se consultan masivamente de forma repetida. No es eficiente en listas enlazadas puras debido a la falta de acceso indexado directo.
* **Complejidad Algorítmica:**
  * Peor caso y caso promedio: $O(\log n)$
  * Mejor caso: $O(1)$

```cpp
int buscarPorCedulaBinaria(const EstudianteUNET lista[], int n, int cedulaBuscada) {
    int izquierda = 0;
    int derecha = n - 1;

    while (izquierda <= derecha) {
        int centro = izquierda + (derecha - izquierda) / 2;

        if (lista[centro].cedula == cedulaBuscada) {
            return centro; 
        }
        if (lista[centro].cedula < cedulaBuscada) {
            izquierda = centro + 1; 
        } else {
            derecha = centro - 1; 
        }
    }
    return -1; 
}
```

#### 1.3.3. Búsqueda por Transformación de Claves (Hash)
* **Mecanismo Operativo:** Realiza el acceso de manera directa sin recorrer los datos. Aplica una función matemática (función hash) sobre la clave de búsqueda para calcular directamente el índice o posición del arreglo donde reside la información. Requiere un mecanismo secundario para resolver **colisiones** (cuando dos claves distintas generan la misma posición).
* **Criterio de Selección:** Se selecciona cuando se requiere la máxima velocidad de respuesta posible en estructuras estáticas en memoria RAM, como diccionarios, tablas de símbolos de compiladores o cachés de datos.
* **Complejidad Algorítmica:**
  * Caso promedio y mejor caso: $O(1)$
  * Peor caso (alta tasa de colisiones): $O(n)$

```cpp
class TablaHashEstudiantes {
private:
    static const int TAMANO_TABLA = 97; 
    EstudianteUNET tabla[TAMANO_TABLA];
    bool ocupado[TAMANO_TABLA] = {false};

    int funcionHash(int cedula) {
        return cedula % TAMANO_TABLA;
    }

public:
    void insertar(EstudianteUNET est) {
        int pos = funcionHash(est.cedula);
        while (ocupado[pos]) {
            pos = (pos + 1) % TAMANO_TABLA;
        }
        tabla[pos] = est;
        ocupado[pos] = true;
    }

    int buscar(int cedulaBuscada) {
        int pos = funcionHash(cedulaBuscada);
        int inicio = pos;
        while (ocupado[pos]) {
            if (tabla[pos].cedula == cedulaBuscada) return pos; 
            pos = (pos + 1) % TAMANO_TABLA;
            if (pos == inicio) break; 
        }
        return -1; 
    }
};
```


### 1.4. Métodos de Búsqueda Externa (Archivos)

#### 1.4.1. Búsqueda Secuencial Externa
* **Mecanismo Operativo:** Abre el archivo físico y lee secuencialmente registro por registro desde el inicio, comparando el campo clave hasta encontrar concordancia o alcanzar el fin de archivo (`EOF`).
* **Optimizaciones:** * **Búsqueda secuencial por bloques:** Agrupa los registros en bloques físicos para leer múltiples datos en una sola operación de disco.
  * **Búsqueda secuencial indexada:** Utiliza un archivo de índices auxiliar (más pequeño) mapeado en memoria que indica dónde comienzan ciertos rangos de datos en el archivo principal, permitiendo saltar directamente al bloque correspondiente.
* **Criterio de Selección:** Útil para archivos pequeños o procesos de procesamiento por lotes (batch) donde se requiere procesar la totalidad de la información de forma secuencial.
* **Complejidad Algorítmica:** Condicionada por el número de lecturas en disco, siendo proporcional al número de registros analizados.

```cpp
bool buscarEstudianteFichero(const std::string& ruta, int cedulaBuscada, EstudianteUNET& resultado) {
    std::ifstream archivo(ruta, std::ios::binary);
    if (!archivo) return false;

    EstudianteUNET est;
    while (archivo.read(reinterpret_cast<char*>(&est), sizeof(EstudianteUNET))) {
        if (est.cedula == cedulaBuscada) {
            resultado = est;
            archivo.close();
            return true; 
        }
    }
    archivo.close();
    return false; 
}
```

#### 1.4.2. Búsqueda Binaria Externa
* **Mecanismo Operativo:** Sigue la misma lógica que la búsqueda binaria interna. Se posiciona en el registro central del archivo calculando su desplazamiento en bytes (utilizando funciones de posicionamiento de archivos como `seekg` en C++) y redefine los extremos del intervalo reduciendo a la mitad el espacio de búsqueda en disco en cada iteración.
* **Precondición Crítica:** El archivo binario debe estar ordenado físicamente por su campo clave y sus registros deben ser de **tamaño fijo** para permitir el cálculo exacto de las posiciones en bytes.
* **Criterio de Selección:** Adecuado para archivos binarios medianos de acceso aleatorio con baja frecuencia de inserciones (ya que mantener el archivo ordenado en disco es costoso).

```cpp
bool buscarEstudianteFicheroBinario(const std::string& ruta, int cedulaBuscada, EstudianteUNET& resultado) {
    std::ifstream archivo(ruta, std::ios::binary | std::ios::ate);
    if (!archivo) return false;

    long tamanoArchivo = archivo.tellg();
    int totalRegistros = tamanoArchivo / sizeof(EstudianteUNET);
    int izquierda = 0;
    int derecha = totalRegistros - 1;

    while (izquierda <= derecha) {
        int centro = izquierda + (derecha - izquierda) / 2;
        archivo.seekg(centro * sizeof(EstudianteUNET)); 

        EstudianteUNET est;
        archivo.read(reinterpret_cast<char*>(&est), sizeof(EstudianteUNET));

        if (est.cedula == cedulaBuscada) {
            resultado = est;
            archivo.close();
            return true; 
        }
        if (est.cedula < cedulaBuscada) izquierda = centro + 1;
        else derecha = centro - 1;
    }
    archivo.close();
    return false; 
}
```

#### 1.4.3. Búsqueda por Transformación de Claves (Hash por Cubetas)
* **Mecanismo Operativo:** Aplica el principio de hashing mapeando las claves no a celdas individuales de memoria, sino a bloques de registros en almacenamiento secundario llamados **cubetas (buckets)**. La función hash calcula el número de bloque o dirección física del archivo en donde se encuentra almacenado el registro, leyendo el bloque completo a memoria para su procesamiento.
* **Criterio de Selección:** Es el método estándar para la organización de archivos de acceso directo en bases de datos donde se requiere recuperar registros individuales instantáneamente sin importar el tamaño total del archivo en disco.

```cpp
struct Cubeta {
    EstudianteUNET alumnos[4]; 
    int contadorContenidos = 0;
};

bool buscarEnCubetaExterna(const std::string& ruta, int cedulaBuscada, EstudianteUNET& resultado) {
    std::ifstream archivo(ruta, std::ios::binary);
    if (!archivo) return false;

    int numeroCubeta = cedulaBuscada % 19; 
    archivo.seekg(numeroCubeta * sizeof(Cubeta)); 

    Cubeta bloque;
    archivo.read(reinterpret_cast<char*>(&bloque), sizeof(Cubeta));
    archivo.close();

    for (int i = 0; i < bloque.contadorContenidos; i++) {
        if (bloque.alumnos[i].cedula == cedulaBuscada) {
            resultado = bloque.alumnos[i];
            return true; 
        }
    }
    return false;
}
```

#### 1.4.4. Búsqueda con Registros Enlazados (Listas o Multilistas)
* **Mecanismo Operativo:** Cada registro guardado en el archivo físico incluye uno o más campos adicionales que actúan como punteros físicos (almacenando el número de registro o la posición exacta en bytes del siguiente elemento). El algoritmo realiza búsquedas transversales saltando de registro en registro siguiendo estas cadenas de enlaces internos.
* **Criterio de Selección:** Ideal para archivos cuyos datos cambian constantemente (altas y bajas), permitiendo mantener un orden lógico o clasificar los registros bajo múltiples criterios de búsqueda independientes (multilistas) sin necesidad de reordenar físicamente el archivo en el disco.

```cpp
struct RegistroEstudianteEnlazado {
    EstudianteUNET datosAlumno;
    long int siguienteByte; 
};

bool buscarEstudianteEnlazado(const std::string& ruta, long int primerNodoByte, int cedulaBuscada, EstudianteUNET& resultado) {
    std::ifstream archivo(ruta, std::ios::binary);
    if (!archivo) return false;

    long int posicionActual = primerNodoByte;
    RegistroEstudianteEnlazado registroNode;

    while (posicionActual != -1) { 
        archivo.seekg(posicionActual);
        archivo.read(reinterpret_cast<char*>(&registroNode), sizeof(RegistroEstudianteEnlazado));

        if (registroNode.datosAlumno.cedula == cedulaBuscada) {
            resultado = registroNode.datosAlumno;
            archivo.close();
            return true; 
        }
        posicionActual = registroNode.siguienteByte; 
    }
    archivo.close();
    return false; 
}
```

## 2. MÉTODOS DE ORDENACIÓN

### 2.1. Introducción a la Ordenación de Datos
La ordenación o clasificación de datos consiste en la operación de organizar un conjunto de elementos en una secuencia lineal determinada basada en un criterio específico (comúnmente un orden numérico o alfabético de carácter ascendente o descendente). En las ciencias de la computación y gestión de bases de datos, es una operación fundamental debido a que el software optimiza drásticamente sus procesos de acceso a la información y sus tasas de éxito en la recuperación de datos si estos se encuentran estructurados metódicamente.

### 2.2. Clasificación: Ordenación Interna vs. Externa
El volumen total de los datos dictamina la elección de la categoría del algoritmo de ordenamiento:

* **Ordenación Interna:** Se ejecuta íntegramente dentro de la memoria RAM. Es la solución óptima cuando la magnitud del conjunto de datos permite cargarlo por completo en memoria simultáneamente. Destaca por su velocidad de procesamiento, dado que las operaciones de manipulación directa e intercambio de variables en RAM toman fracciones de tiempo mínimas comparadas con los retardos mecánicos o electrónicos de los discos. Su principal restricción es el límite físico de capacidad de la memoria principal.
* **Ordenación Externa:** Se activa obligatoriamente cuando el volumen de información excede la capacidad de retención de la memoria RAM. El algoritmo requiere el uso coordinado de almacenamiento masivo secundario para albergar archivos temporales de trabajo. La prioridad de diseño en estos algoritmos se centra en **minimizar la tasa de accesos (lectura/escritura) a disco**, debido a la lentitud de estos periféricos. Emplean estrategias avanzadas de "Divide y Vencerás", procesando fracciones o tramos legibles de datos en memoria para luego fusionar las partes en un único archivo consolidado.

---

### 2.3. Métodos de Ordenación Interna (Memoria RAM)

#### 2.3.1. Ordenamiento por Burbuja (Bubble Sort)
* **Mecanismo Operativo:** Funciona mediante pasadas cíclicas sobre la estructura, comparando pares de elementos adyacentes e intercambiándolos de posición si se encuentran en el orden incorrecto. El proceso provoca que los elementos más grandes "floten" gradualmente hacia el extremo derecho del arreglo en cada iteración completa.
* **Criterio de Selección:** Didáctico. Solo es aceptable en entornos reales para colecciones de datos extremadamente pequeñas o vectores que ya están casi completamente ordenados.
* **Complejidad Algorítmica:**
  * Peor caso y caso promedio: $O(n^2)$
  * Mejor caso (con bandera de optimización): $O(n)$

```cpp
struct EstudianteUNET {
    int cedula;
    std::string nombre;
    float promedio;
};

void ordenarBurbuja(EstudianteUNET arr[], int n) {
    bool intercambiado;
    for (int i = 0; i < n - 1; i++) {
        intercambiado = false;
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j].cedula > arr[j + 1].cedula) {
                std::swap(arr[j], arr[j + 1]);
                intercambiado = true;
            }
        }
        if (!intercambiado) break;
    }
}
```

#### 2.3.2. Ordenamiento por Selección (Selection Sort)
* **Mecanismo Operativo:** Recorre el arreglo buscando recursivamente el elemento menor de toda la estructura y lo transfiere al inicio (posición cero) mediante un intercambio directo. En la siguiente pasada, busca el menor del subarreglo restante y lo posiciona en la celda subsiguiente, continuando de forma sucesiva hasta el último componente.
* **Criterio de Selección:** Preferible sobre la burbuja cuando el costo de escribir o mover las estructuras en memoria es alto, ya que realiza como máximo $O(n)$ intercambios físicos de registros.
* **Complejidad Algorítmica:**
  * Peor caso, caso promedio y mejor caso: $O(n^2)$

```cpp
void ordenarSeleccion(EstudianteUNET arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int indiceMaximo = i;
        for (int j = i + 1; j < n; j++) {
            if (arr[j].promedio > arr[indiceMaximo].promedio) {
                indiceMaximo = j;
            }
        }
        if (indiceMaximo != i) {
            std::swap(arr[i], arr[indiceMaximo]);
        }
    }
}
```

#### 2.3.3. Ordenamiento por Inserción (Insertion Sort)
* **Mecanismo Operativo:** Modela el comportamiento humano clásico al organizar elementos manualmente (como barajar cartas). El algoritmo evalúa cada elemento desde la segunda posición y lo desplaza hacia la izquierda, insertándolo en el punto exacto relativo que le corresponde entre los elementos que ya han sido previamente ordenados.
* **Criterio de Selección:** Altamente eficiente para conjuntos de datos pequeños o como algoritmo de terminación para métodos más complejos. Es un algoritmo de ordenamiento estable (mantiene el orden relativo de elementos con claves iguales).
* **Complejidad Algorítmica:**
  * Peor caso y caso promedio: $O(n^2)$
  * Mejor caso (datos ya ordenados): $O(n)$

```cpp
void ordenarInsercion(EstudianteUNET arr[], int n) {
    for (int i = 1; i < n; i++) {
        EstudianteUNET clave = arr[i];
        int j = i - 1;

        while (j >= 0 && arr[j].cedula > clave.cedula) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = clave;
    }
}
```

#### 2.3.4. Shell Sort
* **Mecanismo Operativo:** Se define como una optimización del método de inserción directa. Reduce el total de desplazamientos comparando y ordenando elementos que se encuentran separados por una distancia o brecha (*gap*) decreciente, en lugar de evaluar únicamente elementos contiguos. Al final, realiza una pasada con una brecha de tamaño uno, garantizando el ordenamiento definitivo.
* **Criterio de Selección:** Eficiente para arreglos de tamaño moderado. No requiere la sobrecarga de memoria de la recursividad ni estructuras auxiliares masivas.
* **Complejidad Algorítmica:** Depende directamente de la secuencia de brechas elegida. Convencionalmente oscila entre $O(n^{1.5})$ y $O(n^2)$.

```cpp
void ordenarShell(EstudianteUNET arr[], int n) {
    for (int gap = n / 2; gap > 0; gap /= 2) {
        for (int i = gap; i < n; i++) {
            EstudianteUNET temporal = arr[i];
            int j;

            for (j = i; j >= gap && arr[j - gap].cedula > temporal.cedula; j -= gap) {
                arr[j] = arr[j - gap];
            }
            arr[j] = temporal;
        }
    }
}
```

#### 2.3.5. Quick Sort
* **Mecanismo Operativo:** Algoritmo de alta eficiencia basado en el paradigma "Divide y Vencerás". Selecciona un elemento del vector llamado **pivote** y reorganiza los componentes de modo que todos los datos menores al pivote queden a su izquierda y los mayores a su derecha (proceso de partición). Posteriormente, aplica el algoritmo de forma recursiva a los dos subarreglos resultantes.
* **Criterio de Selección:** Es el estándar general para ordenamiento en memoria RAM debido a su velocidad en el caso promedio. No se recomienda si se requiere un algoritmo estable o si existe riesgo real de caer en su peor caso.
* **Complejidad Algorítmica:**
  * Caso promedio y mejor caso: $O(n \log n)$
  * Peor caso (pivote pésimamente elegido sobre datos preordenados): $O(n^2)$

```cpp
int particionPromedio(EstudianteUNET arr[], int bajo, int alto) {
    float pivote = arr[alto].promedio; 
    int i = (bajo - 1); 

    for (int j = bajo; j <= alto - 1; j++) {
        if (arr[j].promedio > pivote) {
            i++; 
            std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[i + 1], arr[alto]);
    return (i + 1);
}

void ordenarQuickSort(EstudianteUNET arr[], int bajo, int alto) {
    if (bajo < alto) {
        int indiceParticion = particionPromedio(arr, bajo, alto);
        ordenarQuickSort(arr, bajo, indiceParticion - 1);
        ordenarQuickSort(arr, indiceParticion + 1, alto);
    }
}
```

#### 2.3.6. Radix Sort
* **Mecanismo Operativo:** Algoritmo que no emplea comparaciones directas entre claves. Procesa los elementos descomponiendo los valores numéricos de forma posicional, distribuyéndolos en contenedores lógicos desde el dígito menos significativo (unidades) hasta el de mayor peso (decenas, centenas), apoyándose internamente en un ordenamiento estable como Counting Sort para procesar cada posición de los dígitos.
* **Precondición:** Funciona sobre claves de tipo entero o valores discretos característicos.
* **Criterio de Selección:** Ideal cuando las claves de ordenamiento son números enteros con un número fijo y limitado de dígitos (como las cédulas de identidad o códigos de carrera), logrando superar la barrera teórica de los algoritmos de comparación.
* **Complejidad Algorítmica:**
  * Peor caso, caso promedio y mejor caso: $O(d \cdot (n + k))$, donde $d$ es el número de dígitos y $k$ la base de numeración (sistema decimal, $k=10$).

```cpp
int obtenerMaxCedula(EstudianteUNET arr[], int n) {
    int maximo = arr[0].cedula;
    for (int i = 1; i < n; i++) {
        if (arr[i].cedula > maximo) maximo = arr[i].cedula;
    }
    return maximo;
}

void countingSortPorDigito(EstudianteUNET arr[], int n, int exp) {
    EstudianteUNET* salida = new EstudianteUNET[n];
    int conteo[10] = {0};

    for (int i = 0; i < n; i++) {
        conteo[(arr[i].cedula / exp) % 10]++;
    }
    for (int i = 1; i < 10; i++) {
        conteo[i] += conteo[i - 1];
    }
    for (int i = n - 1; i >= 0; i--) {
        int digito = (arr[i].cedula / exp) % 10;
        salida[conteo[digito] - 1] = arr[i];
        conteo[digito]--;
    }
    for (int i = 0; i < n; i++) {
        arr[i] = salida[i];
    }
    delete[] salida;
}

void ordenarRadix(EstudianteUNET arr[], int n) {
    int m = obtenerMaxCedula(arr, n);
    for (int exp = 1; m / exp > 0; exp *= 10) {
        countingSortPorDigito(arr, n, exp);
    }
}
```

### 2.4. Métodos de Ordenación Externa (Archivos Binarios)

#### 2.4.1. Mezcla Directa (Straight Merge)
* **Mecanismo Operativo:** Es el algoritmo base de la ordenación en almacenamiento secundario. Funciona dividiendo el proceso en fases repetitivas de partición y fusión. En la partición, el archivo original se divide en dos archivos auxiliares (por ejemplo, F1 y F2), distribuyendo los registros en secuencias de tamaño fijo balanceado (inicialmente de tamaño 1). En la fusión, se leen en paralelo los elementos de ambos archivos temporales, combinándolos de forma ordenada en tramos del doble de tamaño (tamaño 2) en el archivo de salida. El ciclo completo se repite duplicando el tamaño de los tramos (1, 2, 4, 8, 16...) en cada fase, hasta que una sola secuencia abarque la totalidad de los registros.
* **Criterio de Selección:** Se emplea cuando el volumen de datos de los estudiantes excede por completo la memoria RAM disponible y se requiere un algoritmo predecible y sistemático de implementar en sistemas de archivos básicos.
* **Complejidad Algorítmica:** Está ligada directamente al número de pasadas necesarias sobre el archivo, siendo del orden de $O(n \log n)$ operaciones de lectura/escritura en disco.

```cpp
struct EstudianteUNET {
    int cedula;
    char nombre[50];
    float promedio;
};

// Distribuye los registros del archivo original en secuencias de tamaño 'tramos' en F1 y F2
void particionDirecta(const std::string& orig, const std::string& f1, const std::string& f2, int tramos) {
    std::ifstream archivoOrig(orig, std::ios::binary);
    std::ofstream archivoF1(f1, std::ios::binary);
    std::ofstream archivoF2(f2, std::ios::binary);

    EstudianteUNET est;
    int contador = 0;
    bool escribirEnF1 = true;

    while (archivoOrig.read(reinterpret_cast<char*>(&est), sizeof(EstudianteUNET))) {
        if (escribirEnF1) {
            archivoF1.write(reinterpret_cast<char*>(&est), sizeof(EstudianteUNET));
        } else {
            archivoF2.write(reinterpret_cast<char*>(&est), sizeof(EstudianteUNET));
        }
        
        contador++;
        if (contador == tramos) {
            contador = 0;
            escribirEnF1 = !escribirEnF1; // Alterna el archivo de destino por tramo
        }
    }
}

// Fusiona los tramos de F1 y F2 de forma ordenada en el archivo original
bool fusionDirecta(const std::string& orig, const std::string& f1, const std::string& f2, int tramos) {
    std::ofstream archivoOrig(orig, std::ios::binary);
    std::ifstream archivoF1(f1, std::ios::binary);
    std::ifstream archivoF2(f2, std::ios::binary);

    EstudianteUNET est1, est2;
    bool leerF1 = true, leerF2 = true;
    bool hayDatosF1 = false, hayDatosF2 = false;
    bool huboFusiones = false;

    hayDatosF1 = (bool)archivoF1.read(reinterpret_cast<char*>(&est1), sizeof(EstudianteUNET));
    hayDatosF2 = (bool)archivoF2.read(reinterpret_cast<char*>(&est2), sizeof(EstudianteUNET));

    while (hayDatosF1 || hayDatosF2) {
        int c1 = 0, c2 = 0;
        huboFusiones = true;

        // Mezcla dos tramos específicos
        while ((hayDatosF1 && c1 < tramos) && (hayDatosF2 && c2 < tramos)) {
            if (est1.cedula <= est2.cedula) {
                archivoOrig.write(reinterpret_cast<char*>(&est1), sizeof(EstudianteUNET));
                hayDatosF1 = (bool)archivoF1.read(reinterpret_cast<char*>(&est1), sizeof(EstudianteUNET));
                c1++;
            } else {
                archivoOrig.write(reinterpret_cast<char*>(&est2), sizeof(EstudianteUNET));
                hayDatosF2 = (bool)archivoF2.read(reinterpret_cast<char*>(&est2), sizeof(EstudianteUNET));
                c2++;
            }
        }

        // Vacía los elementos restantes del tramo de F1 si F2 terminó su tramo
        while (hayDatosF1 && c1 < tramos) {
            archivoOrig.write(reinterpret_cast<char*>(&est1), sizeof(EstudianteUNET));
            hayDatosF1 = (bool)archivoF1.read(reinterpret_cast<char*>(&est1), sizeof(EstudianteUNET));
            c1++;
        }

        // Vacía los elementos restantes del tramo de F2 si F1 terminó su tramo
        while (hayDatosF2 && c2 < tramos) {
            archivoOrig.write(reinterpret_cast<char*>(&est2), sizeof(EstudianteUNET));
            hayDatosF2 = (bool)archivoF2.read(reinterpret_cast<char*>(&est2), sizeof(EstudianteUNET));
            c2++;
        }
    }
    return huboFusiones;
}

// Algoritmo principal de Mezcla Directa
void ordenarMezclaDirecta(const std::string& archivoOriginal) {
    std::string f1 = "temporal1.dat";
    std::string f2 = "temporal2.dat";
    int tramos = 1;
    bool continuar = true;

    // Supongamos que el archivo tiene N registros. El bucle corre hasta que el tamaño del tramo arrope todo
    while (continuar) {
        particionDirecta(archivoOriginal, f1, f2, tramos);
        continuar = fusionDirecta(archivoOriginal, f1, f2, tramos);
        tramos *= 2; // Duplica el tamaño del tramo lógicamente
        
        // Condición de parada real si una sola fusión procesó todo
        std::ifstream f(f1, std::ios::binary | std::ios::ate);
        if (f.tellg() == 0) break; 
    }
}
```

#### 2.4.2. Mezcla Natural o Equilibrada (Natural Merge)
* **Mecanismo Operativo:** Constituye una optimización avanzada de la mezcla directa. Su objetivo primordial es reducir el número de accesos y pasadas por disco aprovechando las secuencias ordenadas que ya existen de forma nativa (tramos naturales) en el archivo original. En lugar de forzar un tamaño de tramo rígido y artificial que se duplica matemáticamente, el algoritmo detecta activamente cuándo se rompe el orden ascendente en los registros del archivo para delimitar los bloques. Luego, realiza la partición y posterior fusión combinando de forma adaptativa estos tramos de tamaño variable hasta que el archivo queda unificado y completamente consolidado.
* **Criterio de Selección:** Es sustancialmente superior a la mezcla directa cuando se trabaja con archivos de datos reales que ya poseen cierto grado de ordenamiento previo (por ejemplo, un listado de estudiantes que se actualiza periódicamente o que viene parcialmente ordenado por número de cédula), disminuyendo drásticamente el desgaste y la latencia del disco.
* **Complejidad Algorítmica:** En el peor de los casos (datos en orden inverso) iguala a la mezcla directa con $O(n \log n)$, pero en el mejor caso (archivo ya ordenado) su eficiencia se aproxima a $O(n)$ en accesos a disco.

```cpp
struct EstudianteUNET {
    int cedula;
    char nombre[50];
    float promedio;
};

// Realiza la partición basada en rupturas de secuencia (tramos naturales)
void particionNatural(const std::string& orig, const std::string& f1, const std::string& f2) {
    std::ifstream archivoOrig(orig, std::ios::binary);
    std::ofstream archivoF1(f1, std::ios::binary);
    std::ofstream archivoF2(f2, std::ios::binary);

    EstudianteUNET estActual, estAnterior;
    bool escribirEnF1 = true;

    if (!archivoOrig.read(reinterpret_cast<char*>(&estActual), sizeof(EstudianteUNET))) return;
    
    // Escribe el primer registro
    archivoF1.write(reinterpret_cast<char*>(&estActual), sizeof(EstudianteUNET));
    estAnterior = estActual;

    while (archivoOrig.read(reinterpret_cast<char*>(&estActual), sizeof(EstudianteUNET))) {
        // Si la clave actual es menor que la anterior, se rompió la secuencia natural
        if (estActual.cedula < estAnterior.cedula) {
            escribirEnF1 = !escribirEnF1; // Cambia de archivo auxiliar
        }

        if (escribirEnF1) {
            archivoF1.write(reinterpret_cast<char*>(&estActual), sizeof(EstudianteUNET));
        } else {
            archivoF2.write(reinterpret_cast<char*>(&estActual), sizeof(EstudianteUNET));
        }
        estAnterior = estActual;
    }
}

// Fusiona los tramos variables respetando el orden natural de cada archivo temporal
int fusionNatural(const std::string& orig, const std::string& f1, const std::string& f2) {
    std::ofstream archivoOrig(orig, std::ios::binary);
    std::ifstream archivoF1(f1, std::ios::binary);
    std::ifstream archivoF2(f2, std::ios::binary);

    EstudianteUNET est1, est2, estAnterior;
    bool hayF1 = (bool)archivoF1.read(reinterpret_cast<char*>(&est1), sizeof(EstudianteUNET));
    bool hayF2 = (bool)archivoF2.read(reinterpret_cast<char*>(&est2), sizeof(EstudianteUNET));
    int cantidadTramosFusiinados = 0;

    while (hayF1 && hayF2) {
        cantidadTramosFusiinados++;
        bool finTramoF1 = false, finTramoF2 = false;

        while (!finTramoF1 && !finTramoF2) {
            if (est1.cedula <= est2.cedula) {
                archivoOrig.write(reinterpret_cast<char*>(&est1), sizeof(EstudianteUNET));
                estAnterior = est1;
                hayF1 = (bool)archivoF1.read(reinterpret_cast<char*>(&est1), sizeof(EstudianteUNET));
                // Verifica fin de tramo si el siguiente registro leído es menor (ruptura)
                if (!hayF1 || est1.cedula < estAnterior.cedula) finTramoF1 = true;
            } else {
                archivoOrig.write(reinterpret_cast<char*>(&est2), sizeof(EstudianteUNET));
                estAnterior = est2;
                hayF2 = (bool)archivoF2.read(reinterpret_cast<char*>(&est2), sizeof(EstudianteUNET));
                if (!hayF2 || est2.cedula < estAnterior.cedula) finTramoF2 = true;
            }
        }

        // Copiar el resto del tramo que quedó colgado de F1
        while (!finTramoF1) {
            archivoOrig.write(reinterpret_cast<char*>(&est1), sizeof(EstudianteUNET));
            estAnterior = est1;
            hayF1 = (bool)archivoF1.read(reinterpret_cast<char*>(&est1), sizeof(EstudianteUNET));
            if (!hayF1 || est1.cedula < estAnterior.cedula) finTramoF1 = true;
        }

        // Copiar el resto del tramo que quedó colgado de F2
        while (!finTramoF2) {
            archivoOrig.write(reinterpret_cast<char*>(&est2), sizeof(EstudianteUNET));
            estAnterior = est2;
            hayF2 = (bool)archivoF2.read(reinterpret_cast<char*>(&est2), sizeof(EstudianteUNET));
            if (!hayF2 || est2.cedula < estAnterior.cedula) finTramoF2 = true;
        }
    }

    // Si quedaron tramos sueltos en un solo archivo porque el otro se vació antes
    while (hayF1) {
        archivoOrig.write(reinterpret_cast<char*>(&est1), sizeof(EstudianteUNET));
        hayF1 = (bool)archivoF1.read(reinterpret_cast<char*>(&est1), sizeof(EstudianteUNET));
    }
    while (hayF2) {
        archivoOrig.write(reinterpret_cast<char*>(&est2), sizeof(EstudianteUNET));
        hayF2 = (bool)archivoF2.read(reinterpret_cast<char*>(&est2), sizeof(EstudianteUNET));
    }

    return cantidadTramosFusiinados;
}

// Algoritmo principal de Mezcla Natural
void ordenarMezclaNatural(const std::string& archivoOriginal) {
    std::string f1 = "tempNatural1.dat";
    std::string f2 = "tempNatural2.dat";
    int numTramos = 0;

    do {
        particionNatural(archivoOriginal, f1, f2);
        numTramos = fusionNatural(archivoOriginal, f1, f2);
    } while (numTramos > 1); 
}
```
