#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <cmath>
#include <chrono>
#include <string>

using namespace std;

const int INF = 1e9;

// Estructura que representa cada casilla/nodo del laberinto
struct Nodo {
    int x, y;
    int g;
    int h;
    int f;
    Nodo* padre;

    Nodo(int _x, int _y, int _g, int _h, Nodo* _padre = nullptr) {
        x = _x;
        y = _y;
        g = _g;
        h = _h;
        f = g + h;
        padre = _padre;
    }
};

// Comparador para la Cola de Prioridad (Min-Heap basado en 'f')
struct CompararF {
    bool operator()(const Nodo* a, const Nodo* b) const {
        return a->f > b->f; // El de menor 'f' tiene mayor prioridad
    }
};

// Estructura para almacenar las dimensiones y la matriz del laberinto
struct Laberinto {
    vector<vector<char>> matriz;
    int filas = 0;
    int columnas = 0;
    int inicioX = -1, inicioY = -1;
    int metaX = -1, metaY = -1;
};

// Función para calcular la Distancia Manhattan: |x1 - x2| + |y1 - y2|
int calcularManhattan(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

// Carga el laberinto desde un archivo .txt
bool cargarLaberinto(const string& nombreArchivo, Laberinto& lab) {
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cout << "[!] Error: No se pudo abrir el archivo " << nombreArchivo << endl;
        return false;
    }

    string linea;
    lab.matriz.clear();
    lab.filas = 0;

    while (getline(archivo, linea)) {
        if (linea.empty()) continue;
        
        vector<char> fila;
        for (size_t i = 0; i < linea.size(); i++) {
            char c = linea[i];
            
            // Ignorar caracteres invisibles o de retorno de carro
            if (c == '\r' || c == '\n') continue;

            if (c == 'I') {
                lab.inicioX = lab.filas;
                lab.inicioY = fila.size();
            } else if (c == 'F') {
                lab.metaX = lab.filas;
                lab.metaY = fila.size();
            }
            fila.push_back(c);
        }
        
        if (!fila.empty()) {
            lab.matriz.push_back(fila);
            lab.filas++;
        }
    }

    archivo.close();

    if (!lab.matriz.empty()) {
        lab.columnas = lab.matriz[0].size();
    }

    if (lab.inicioX == -1 || lab.metaX == -1) {
        cout << "[!] Error: El laberinto debe contener un Inicio [I] y una Meta [F]." << endl;
        return false;
    }

    return true;
}

// Algoritmo A*
void resolverLaberintoAStar(Laberinto& lab) {
    auto tiempoInicio = chrono::high_resolution_clock::now();

    int R = lab.filas;
    int C = lab.columnas;

    // Matriz de distancias registradas para control de la Lista Cerrada / Optimización
    vector<vector<int>> gScore(R, vector<int>(C, INF));

    // Cola de prioridad (Lista Abierta)
    priority_queue<Nodo*, vector<Nodo*>, CompararF> listaAbierta;

    // Nodo inicial
    int hInicial = calcularManhattan(lab.inicioX, lab.inicioY, lab.metaX, lab.metaY);
    Nodo* nodoInicio = new Nodo(lab.inicioX, lab.inicioY, 0, hInicial);
    
    listaAbierta.push(nodoInicio);
    gScore[lab.inicioX][lab.inicioY] = 0;

    int nodosExplorados = 0;
    Nodo* nodoMetaAlcanzado = nullptr;

    // Desplazamientos ortogonales: Arriba, Abajo, Izquierda, Derecha
    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};

    while (!listaAbierta.empty()) {
        Nodo* actual = listaAbierta.top();
        listaAbierta.pop();

        nodosExplorados++;

        // Si alcanzamos la meta, detenemos la búsqueda
        if (actual->x == lab.metaX && actual->y == lab.metaY) {
            nodoMetaAlcanzado = actual;
            break;
        }

        // Explorar los 4 vecinos ortogonales
        for (int i = 0; i < 4; i++) {
            int nx = actual->x + dx[i];
            int ny = actual->y + dy[i];

            // Validar límites de la matriz
            if (nx >= 0 && nx < R && ny >= 0 && ny < C) {
                char celda = lab.matriz[nx][ny];

                // Ignorar obstáculos
                if (celda == '#') continue;

                // Determinar el costo del movimiento
                int costoPaso = 1; // Por defecto '.' o 'F' o 'I'
                if (celda == '~') {
                    costoPaso = 5; // Terreno pesado (Lodo)
                }

                int nuevoG = actual->g + costoPaso;

                // Si encontramos un camino más corto hacia este vecino
                if (nuevoG < gScore[nx][ny]) {
                    gScore[nx][ny] = nuevoG;
                    int hNuevo = calcularManhattan(nx, ny, lab.metaX, lab.metaY);
                    Nodo* vecino = new Nodo(nx, ny, nuevoG, hNuevo, actual);
                    listaAbierta.push(vecino);
                }
            }
        }
    }

    auto tiempoFin = chrono::high_resolution_clock::now();
    auto duracionMicro = chrono::duration_cast<chrono::microseconds>(tiempoFin - tiempoInicio).count();

    // Impresión de Resultados y Métricas
    cout << "\n==================================================" << endl;
    cout << "             RESULTADOS DE LA BÚSQUEDA A*         " << endl;
    cout << "==================================================" << endl;

    if (nodoMetaAlcanzado != nullptr) {
        int costoTotal = nodoMetaAlcanzado->g;
        int longitudRuta = 0;

        // Reconstrucción de la ruta marcando con '*' en el mapa
        Nodo* curr = nodoMetaAlcanzado;
        while (curr != nullptr) {
            if (lab.matriz[curr->x][curr->y] != 'I' && lab.matriz[curr->x][curr->y] != 'F') {
                lab.matriz[curr->x][curr->y] = '*';
            }
            longitudRuta++;
            curr = curr->padre;
        }

        cout << " [!] ¡RUTA ENCONTRADA CON ÉXITO!" << endl;
        cout << " -> Tiempo de ejecución : " << duracionMicro << " us (" << duracionMicro / 1000.0 << " ms)" << endl;
        cout << " -> Nodos explorados    : " << nodosExplorados << endl;
        cout << " -> Costo total camino  : " << costoTotal << " (Tomando en cuenta peso del Lodo)" << endl;
        cout << " -> Longitud del camino : " << longitudRuta - 1 << " pasos" << endl;

        cout << "\n--- MAPA CON LA RUTA ENCONTRADA (*) ---" << endl;
        for (int i = 0; i < R; i++) {
            for (int j = 0; j < C; j++) {
                cout << lab.matriz[i][j] << " ";
            }
            cout << endl;
        }
    } else {
        cout << " [!] NO EXISTE UN CAMINO POSIBLE HACIA LA META." << endl;
        cout << " -> Tiempo de ejecución : " << duracionMicro << " us (" << duracionMicro / 1000.0 << " ms)" << endl;
        cout << " -> Nodos explorados    : " << nodosExplorados << endl;
    }
    cout << "==================================================\n" << endl;
}

int main() {
    Laberinto lab;
    string nombreArchivo;

    cout << "==================================================" << endl;
    cout << "   SISTEMA DE BUSQUEDA A* (LABERINTO)  " << endl;
    cout << "==================================================" << endl;
    cout << "Ingrese el nombre del archivo de prueba (ej: laberinto.txt): ";
    cin >> nombreArchivo;

    if (cargarLaberinto(nombreArchivo, lab)) {
        resolverLaberintoAStar(lab);
    }

    return 0;
}