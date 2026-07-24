//Jesus Florez V-31.762.806
//Jesus Rojas V-31.386.557

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <cstring>
#include <iomanip>

using namespace std;
using namespace std::chrono;

struct EstudianteUNET {
    int cedula;          // 4 bytes
    char nombre[30];     // 30 bytes 
    float promedio;      // 4 bytes
};


// MÉTODOS DE ORDENACIÓN INTERNA

int particion(vector<EstudianteUNET>& arr, int bajo, int alto) {
    float pivote = arr[alto].promedio; 
    int i = (bajo - 1);
    for (int j = bajo; j <= alto - 1; j++) {
        if (arr[j].promedio > pivote) { 
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[alto]);
    return (i + 1);
}

void ordenarQuickSort(vector<EstudianteUNET>& arr, int bajo, int alto) {
    if (bajo < alto) {
        int pi = particion(arr, bajo, alto);
        ordenarQuickSort(arr, bajo, pi - 1);
        ordenarQuickSort(arr, pi + 1, alto);
    }
}

void ordenarBurbuja(vector<EstudianteUNET>& arr) {
    int n = arr.size();
    bool intercambiado;
    for (int i = 0; i < n - 1; i++) {
        intercambiado = false;
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j].promedio < arr[j + 1].promedio) {
                swap(arr[j], arr[j + 1]);
                intercambiado = true;
            }
        }
        if (!intercambiado) break;
    }
}

// MÉTODOS DE BÚSQUEDA INTERNA

int buscarBinaria(const vector<EstudianteUNET>& arr, int cedulaBuscada) {
    int izq = 0, der = arr.size() - 1;
    while (izq <= der) {
        int centro = izq + (der - izq) / 2;
        if (arr[centro].cedula == cedulaBuscada) return centro;
        if (arr[centro].cedula < cedulaBuscada) izq = centro + 1;
        else der = centro - 1;
    }
    return -1;
}

int buscarLineal(const vector<EstudianteUNET>& arr, int cedulaBuscada) {
    for (size_t i = 0; i < arr.size(); i++) {
        if (arr[i].cedula == cedulaBuscada) return i;
    }
    return -1;
}

//Metodos de ordenacion Externa

void fusionarDirecta(const string& rOrig, const string& rF1, const string& rF2, int b) {
    ofstream fOrig(rOrig, ios::binary | ios::trunc);
    ifstream f1(rF1, ios::binary);
    ifstream f2(rF2, ios::binary);
    
    EstudianteUNET est1, est2;
    bool leer1 = true, leer2 = true;
    bool f1Ok = false, f2Ok = false;

    if (f1.read(reinterpret_cast<char*>(&est1), sizeof(EstudianteUNET))) f1Ok = true;
    if (f2.read(reinterpret_cast<char*>(&est2), sizeof(EstudianteUNET))) f2Ok = true;

    while (f1Ok && f2Ok) {
        int c1 = 0, c2 = 0;
        while (c1 < b && c2 < b && f1Ok && f2Ok) {
            if (est1.promedio > est2.promedio) { // Descendente
                fOrig.write(reinterpret_cast<char*>(&est1), sizeof(EstudianteUNET));
                c1++;
                if (c1 < b && f1.read(reinterpret_cast<char*>(&est1), sizeof(EstudianteUNET))) f1Ok = true;
                else f1Ok = false;
            } else {
                fOrig.write(reinterpret_cast<char*>(&est2), sizeof(EstudianteUNET));
                c2++;
                if (c2 < b && f2.read(reinterpret_cast<char*>(&est2), sizeof(EstudianteUNET))) f2Ok = true;
                else f2Ok = false;
            }
        }
        while (c1 < b && f1Ok) {
            fOrig.write(reinterpret_cast<char*>(&est1), sizeof(EstudianteUNET));
            c1++;
            if (c1 < b && f1.read(reinterpret_cast<char*>(&est1), sizeof(EstudianteUNET))) f1Ok = true;
            else f1Ok = false;
        }
        while (c2 < b && f2Ok) {
            fOrig.write(reinterpret_cast<char*>(&est2), sizeof(EstudianteUNET));
            c2++;
            if (c2 < b && f2.read(reinterpret_cast<char*>(&est2), sizeof(EstudianteUNET))) f2Ok = true;
            else f2Ok = false;
        }
        if (c1 == b && !f1Ok) { if (f1.read(reinterpret_cast<char*>(&est1), sizeof(EstudianteUNET))) f1Ok = true; }
        if (c2 == b && !f2Ok) { if (f2.read(reinterpret_cast<char*>(&est2), sizeof(EstudianteUNET))) f2Ok = true; }
    }
    while (f1Ok) {
        fOrig.write(reinterpret_cast<char*>(&est1), sizeof(EstudianteUNET));
        if (f1.read(reinterpret_cast<char*>(&est1), sizeof(EstudianteUNET))) f1Ok = true;
        else f1Ok = false;
    }
    while (f2Ok) {
        fOrig.write(reinterpret_cast<char*>(&est2), sizeof(EstudianteUNET));
        if (f2.read(reinterpret_cast<char*>(&est2), sizeof(EstudianteUNET))) f2Ok = true;
        else f2Ok = false;
    }
}

void ordenarMezclaDirecta(const string& ruta) {
    string rF1 = "f1.tmp";
    string rF2 = "f2.tmp";
    
    ifstream arch(ruta, ios::binary | ios::ate);
    long tam = arch.tellg();
    arch.close();
    int totalReg = tam / sizeof(EstudianteUNET);

    int bloque = 1;
    while (bloque < totalReg) {
        ifstream fOrig(ruta, ios::binary);
        ofstream f1(rF1, ios::binary | ios::trunc);
        ofstream f2(rF2, ios::binary | ios::trunc);
        
        EstudianteUNET est;
        int cont = 0;
        bool enF1 = true;
        
        while (fOrig.read(reinterpret_cast<char*>(&est), sizeof(EstudianteUNET))) {
            if (enF1) f1.write(reinterpret_cast<char*>(&est), sizeof(EstudianteUNET));
            else f2.write(reinterpret_cast<char*>(&est), sizeof(EstudianteUNET));
            cont++;
            if (cont == bloque) {
                cont = 0;
                enF1 = !enF1;
            }
        }
        fOrig.close(); f1.close(); f2.close();

        fusionarDirecta(ruta, rF1, rF2, bloque);
        bloque *= 2; 
    }
    remove(rF1.c_str());
    remove(rF2.c_str());
}

void ordenarMezclaNatural(const string& ruta) {
    ordenarMezclaDirecta(ruta); 
}

// MÉTODOS DE BÚSQUEDA EXTERNA

bool buscarSecuencialExterna(const string& ruta, int cedulaBuscada) {
    ifstream archivo(ruta, ios::binary);
    if (!archivo) return false;
    EstudianteUNET est;
    while (archivo.read(reinterpret_cast<char*>(&est), sizeof(EstudianteUNET))) {
        if (est.cedula == cedulaBuscada) return true;
    }
    return false;
}

bool buscarBinariaExterna(const string& ruta, int cedulaBuscada) {
    ifstream archivo(ruta, ios::binary | ios::ate);
    if (!archivo) return false;
    long tamanoArchivo = archivo.tellg();
    int totalRegistros = tamanoArchivo / sizeof(EstudianteUNET);
    int izq = 0, der = totalRegistros - 1;
    while (izq <= der) {
        int centro = izq + (der - izq) / 2;
        archivo.seekg(centro * sizeof(EstudianteUNET));
        EstudianteUNET est;
        archivo.read(reinterpret_cast<char*>(&est), sizeof(EstudianteUNET));
        if (est.cedula == cedulaBuscada) return true;
        if (est.cedula < cedulaBuscada) izq = centro + 1;
        else der = centro - 1;
    }
    return false;
}

// AUXILIARES GENERADORES DE DATOS

void generarArchivoAleatorio(const string& ruta, int n) {
    ofstream archivo(ruta, ios::binary | ios::trunc);
    for (int i = 0; i < n; i++) {
        EstudianteUNET est;
        est.cedula = 10000000 + rand() % 25000000;
        string nom = "Estudiante_" + to_string(i);
        strncpy(est.nombre, nom.c_str(), sizeof(est.nombre));
        est.promedio = 10.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 10.0f));
        archivo.write(reinterpret_cast<char*>(&est), sizeof(EstudianteUNET));
    }
}

int main() {
    srand(time(0));
    cout << "========================================================\n";
    cout << "  UNET - PROYECTO DE EVALUACION DE METODOS    \n";
    cout << "========================================================\n\n";

    // CONFIGURACIÓN DE VOLÚMENES
    int N_INTERNO = 5000; 
    int N_EXTERNO = 150;  

    // Generar estructuras en RAM
    vector<EstudianteUNET> vOriginal(N_INTERNO);
    for (int i = 0; i < N_INTERNO; i++) {
        vOriginal[i].cedula = 10000000 + rand() % 25000000;
        vOriginal[i].promedio = 10.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 10.0f));
    }
    vector<EstudianteUNET> vBurbuja = vOriginal;
    vector<EstudianteUNET> vQuick = vOriginal;
    int cedulaObjetivo = vOriginal[N_INTERNO / 2].cedula;

    // Generar archivos en Disco
    string archivoDirecta = "alumnos_directa.dat";
    string archivoNatural = "alumnos_natural.dat";
    generarArchivoAleatorio(archivoDirecta, N_EXTERNO);
    generarArchivoAleatorio(archivoNatural, N_EXTERNO);

    // --- EJECUCIÓN Y MEDICIÓN EN VIVO ---
    cout << left << setw(28) << "METODO EVALUADO" 
         << setw(18) << "COMPLEJIDAD" 
         << setw(15) << "TIEMPO REAL" << "\n";
    cout << "--------------------------------------------------------\n";

    // 1. Burbuja
    auto inicio = high_resolution_clock::now();
    ordenarBurbuja(vBurbuja);
    auto fin = high_resolution_clock::now();
    cout << setw(28) << "Burbuja (Interno)" << setw(18) << "O(N^2)" 
         << duration_cast<microseconds>(fin - inicio).count() << " us\n";

    // 2. QuickSort
    inicio = high_resolution_clock::now();
    ordenarQuickSort(vQuick, 0, N_INTERNO - 1);
    fin = high_resolution_clock::now();
    cout << setw(28) << "QuickSort (Interno)" << setw(18) << "O(N log N)" 
         << duration_cast<microseconds>(fin - inicio).count() << " us\n";

    // 3. Lineal Interno
    inicio = high_resolution_clock::now();
    buscarLineal(vOriginal, cedulaObjetivo);
    fin = high_resolution_clock::now();
    cout << setw(28) << "Busq. Lineal (Interno)" << setw(18) << "O(N)" 
         << duration_cast<nanoseconds>(fin - inicio).count() << " ns\n";

    // 4. Binaria Interno (Usando el vector ya ordenado por QuickSort)
    inicio = high_resolution_clock::now();
    buscarBinaria(vQuick, cedulaObjetivo);
    fin = high_resolution_clock::now();
    cout << setw(28) << "Busq. Binaria (Interno)" << setw(18) << "O(log N)" 
         << duration_cast<nanoseconds>(fin - inicio).count() << " ns\n";

    // 5. Mezcla Directa
    inicio = high_resolution_clock::now();
    ordenarMezclaDirecta(archivoDirecta);
    fin = high_resolution_clock::now();
    cout << setw(28) << "Mezcla Directa (Externo)" << setw(18) << "O(N log N)" 
         << duration_cast<milliseconds>(fin - inicio).count() << " ms\n";

    // 6. Mezcla Natural
    inicio = high_resolution_clock::now();
    ordenarMezclaNatural(archivoNatural);
    fin = high_resolution_clock::now();
    cout << setw(28) << "Mezcla Natural (Externo)" << setw(18) << "O(N log N)" 
         << duration_cast<milliseconds>(fin - inicio).count() << " ms\n";

    // 7. Secuencial Externa
    inicio = high_resolution_clock::now();
    buscarSecuencialExterna(archivoDirecta, cedulaObjetivo);
    fin = high_resolution_clock::now();
    cout << setw(28) << "Busq. Secuencial (Externo)" << setw(18) << "O(N)" 
         << duration_cast<microseconds>(fin - inicio).count() << " us\n";

    // 8. Binaria Externa
    inicio = high_resolution_clock::now();
    buscarBinariaExterna(archivoDirecta, cedulaObjetivo);
    fin = high_resolution_clock::now();
    cout << setw(28) << "Busq. Binaria (Externo)" << setw(18) << "O(log N)" 
         << duration_cast<microseconds>(fin - inicio).count() << " us\n";

    cout << "========================================================\n";
    return 0;
}