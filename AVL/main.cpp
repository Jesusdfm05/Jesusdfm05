#include <iostream>
#include <string>
#include <algorithm> // Para std::max

using namespace std;

// ESTRUCTURA DEL NODO (REGISTRO ESTUDIANTIL)

struct Nodo {
    int cedula;
    string nombre;
    string carrera;

    int altura;
    Nodo* izquierdo;
    Nodo* derecho;

    // Constructor
    Nodo(int ced, string nom, string car) {
        cedula = ced;
        nombre = nom;
        carrera = car;
        altura = 1;
        izquierdo = nullptr;
        derecho = nullptr;
    }
};


// Retorna la altura de forma segura
int obtenerAltura(Nodo* n) {
    if (n == nullptr) return 0;
    return n->altura;
}

// Factor de Equilibrio
int obtenerFactorEquilibrio(Nodo* n) {
    if (n == nullptr) return 0;
    return obtenerAltura(n->izquierdo) - obtenerAltura(n->derecho);
}


// ROTACIONES AVL

// 1. Rotación Simple a la Derecha (Caso LL)
Nodo* rotacionDerecha(Nodo* y) {
    Nodo* x = y->izquierdo;
    Nodo* T2 = x->derecho;

    x->derecho = y;
    y->izquierdo = T2;

    // Recálculo de alturas
    y->altura = max(obtenerAltura(y->izquierdo), obtenerAltura(y->derecho)) + 1;
    x->altura = max(obtenerAltura(x->izquierdo), obtenerAltura(x->derecho)) + 1;

    return x;
}

// 2. Rotación Simple a la Izquierda (Caso RR)
Nodo* rotacionIzquierda(Nodo* x) {
    Nodo* y = x->derecho;
    Nodo* T2 = y->izquierdo;

    y->izquierdo = x;
    x->derecho = T2;

    // Recálculo de alturas
    x->altura = max(obtenerAltura(x->izquierdo), obtenerAltura(x->derecho)) + 1;
    y->altura = max(obtenerAltura(y->izquierdo), obtenerAltura(y->derecho)) + 1;

    return y;
}

// INSERCIÓN CON AUTO-BALANCEO

Nodo* insertar(Nodo* nodo, int cedula, string nombre, string carrera) {
    // 1. Inserción normal
    if (nodo == nullptr) {
        return new Nodo(cedula, nombre, carrera);
    }
    if (cedula < nodo->cedula) {
        nodo->izquierdo = insertar(nodo->izquierdo, cedula, nombre, carrera);
    } else if (cedula > nodo->cedula) {
        nodo->derecho = insertar(nodo->derecho, cedula, nombre, carrera);
    } else {
        cout << "\n Error: La cedula " << cedula << " ya se encuentra registrada.\n";
        return nodo;
    }

    // 2. Actualizar altura del nodo padre
    nodo->altura = 1 + max(obtenerAltura(nodo->izquierdo), obtenerAltura(nodo->derecho));

    // 3. Evaluar el Factor de Equilibrio
    int fe = obtenerFactorEquilibrio(nodo);

    // --- 4. Casos de Reequilibrio (Rotaciones) ---
    if (fe > 1 && cedula < nodo->izquierdo->cedula) {
        return rotacionDerecha(nodo);
    }
    if (fe < -1 && cedula > nodo->derecho->cedula) {
        return rotacionIzquierda(nodo);
    }
    if (fe > 1 && cedula > nodo->izquierdo->cedula) {
        nodo->izquierdo = rotacionIzquierda(nodo->izquierdo);
        return rotacionDerecha(nodo);
    }
    if (fe < -1 && cedula < nodo->derecho->cedula) {
        nodo->derecho = rotacionDerecha(nodo->derecho);
        return rotacionIzquierda(nodo);
    }
    return nodo;
}

// MOSTRAR ÁRBOL (RECORRIDO INORDEN)

void mostrarInOrden(Nodo* raiz) {
    if (raiz != nullptr) {
        mostrarInOrden(raiz->izquierdo);
        cout << "CI: " << raiz->cedula 
             << " | Nombre: " << raiz->nombre 
             << " | Carrera: " << raiz->carrera 
             << " | Altura: " << raiz->altura 
             << " | FE: " << obtenerFactorEquilibrio(raiz) << endl;
        mostrarInOrden(raiz->derecho);
    }
}


int main() {
    Nodo* raiz = nullptr;
    int opcion, cedula;
    string nombre, carrera;

    do {
        cout << "\n=== SISTEMA DE REGISTRO ESTUDIANTIL ===\n";
        cout << "1. Registrar Estudiante (Insertar)\n";
        cout << "2. Mostrar Arbol Balanceado (InOrden)\n";
        cout << "3. Salir\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        switch (opcion) {
            case 1:
                cout << "\n--- Insercion de Estudiante ---\n";
                cout << "Cedula: "; cin >> cedula;
                cin.ignore();
                cout << "Nombre completo: "; getline(cin, nombre);
                cout << "Carrera: "; getline(cin, carrera);

                raiz = insertar(raiz, cedula, nombre, carrera);
                cout << "Estudiante procesado e insertado en el AVL.\n";
                break;

            case 2:
                cout << "\n--- LISTA DE ESTUDIANTES REGISTRADOS ---\n";
                if (raiz == nullptr) {
                    cout << "El arbol esta vacio.\n";
                } else {
                    mostrarInOrden(raiz);
                }
                break;

            case 3:
                cout << "\nSaliendo del programa...\n";
                break;

            default:
                cout << "\nOpcion no valida.\n";
        }
    } while (opcion != 3);

    return 0;
}