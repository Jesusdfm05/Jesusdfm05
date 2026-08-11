#include <iostream>
#include <cstring>  
#include <fstream>  
#include <cstdlib>  
#include <ctime>    
using namespace std;

const int INF = 999999;
const int MAX_CLIENTES = 50;
const int MAX_REPARTIDORES = 50;
const int MAX_SECTORES = 20;

struct Sector {
    int Id;
    char Direccion[20];
};

struct Cliente {
    char Cedula[10];
    char Telefono[15];
    char Nombre[30];
    int Servicios;
};

struct Repartidor {
    char Cedula[10];
    char Nombre[30];
    char Vehiculo[20];
    char Placa[10];
    int Sector;                  // Ubicación actual del repartidor
    int SectorOrigenCliente;     // Sector donde recoge al cliente (-1 si no tiene)
    int SectorDestinoCliente;    // Sector donde entrega el paquete (-1 si no tiene)
    char CedulaClienteAtendido[10]; 
    int Servicios;
    bool Disponible;             // true: libre | false: asignado o en viaje
    bool PaqueteRecogido;        // false: en camino al cliente | true: en camino al destino
};

struct NodoCola {
    char CedulaCliente[10];
    int SectorDestino;
    NodoCola* siguiente;
};

struct ColaSector {
    NodoCola* frente;
    NodoCola* final;
};

// VARIABLES GLOBALES
int contadorClientes = 0;
int contadorRepartidores = 0;
int contadorSectores = 0;

Cliente arregloClientes[MAX_CLIENTES];
Repartidor arregloRepartidores[MAX_REPARTIDORES];
Sector listaSectores[MAX_SECTORES];
ColaSector colasEspera[MAX_SECTORES];

// GRAFO: MATRIZ CUADRADA DINÁMICA DE ADYACENCIA
int** matrizGrafo = NULL;

// --- MANEJO DE MEMORIA DEL GRAFO ---
void inicializarMatrizGrafo() {
    matrizGrafo = new int*[MAX_SECTORES];
    for (int i = 0; i < MAX_SECTORES; i++) {
        matrizGrafo[i] = new int[MAX_SECTORES];
        for (int j = 0; j < MAX_SECTORES; j++) {
            if (i == j) matrizGrafo[i][j] = 0;
            else matrizGrafo[i][j] = INF;
        }
    }
}

void liberarMatrizGrafo() {
    if (matrizGrafo != NULL) {
        for (int i = 0; i < MAX_SECTORES; i++) {
            delete[] matrizGrafo[i];
        }
        delete[] matrizGrafo;
        matrizGrafo = NULL;
    }
}

// --- MANEJO DE COLAS DINAMICAS ---
void inicializarColas() {
    for (int i = 0; i < MAX_SECTORES; i++) {
        colasEspera[i].frente = NULL;
        colasEspera[i].final = NULL;
    }
}

int obtenerIndiceSectorPorId(int idSector) {
    for (int i = 0; i < contadorSectores; i++) {
        if (listaSectores[i].Id == idSector) return i;
    }
    return -1;
}

void encolarCliente(int idSectorOrigen, const char* cedula, int sectorDestino) {
    int idx = obtenerIndiceSectorPorId(idSectorOrigen);
    if (idx == -1) return;

    NodoCola* nuevo = new NodoCola();
    strcpy(nuevo->CedulaCliente, cedula);
    nuevo->SectorDestino = sectorDestino;
    nuevo->siguiente = NULL;

    if (colasEspera[idx].final == NULL) {
        colasEspera[idx].frente = nuevo;
        colasEspera[idx].final = nuevo;
    } else {
        colasEspera[idx].final->siguiente = nuevo;
        colasEspera[idx].final = nuevo;
    }
}

bool desencolarCliente(int idSectorOrigen, char* cedulaOut, int& destinoOut) {
    int idx = obtenerIndiceSectorPorId(idSectorOrigen);
    if (idx == -1 || colasEspera[idx].frente == NULL) return false;

    NodoCola* temp = colasEspera[idx].frente;
    strcpy(cedulaOut, temp->CedulaCliente);
    destinoOut = temp->SectorDestino;

    colasEspera[idx].frente = colasEspera[idx].frente->siguiente;
    if (colasEspera[idx].frente == NULL) {
        colasEspera[idx].final = NULL;
    }

    delete temp;
    return true;
}

// --- PERSISTENCIA DE DATOS Y CARGA DE GRAFO ---
void cargarSectores() {
    ifstream archivo("sectores.txt");
    if (!archivo.is_open()) return; 

    contadorSectores = 0;
    char idStr[10];
    while (archivo.getline(idStr, 10, '|')) {
        if (strlen(idStr) == 0) continue;
        listaSectores[contadorSectores].Id = atoi(idStr);
        archivo.getline(listaSectores[contadorSectores].Direccion, 20); 
        contadorSectores++;
    }
    archivo.close();
}

void cargarGrafo() {
    ifstream archivo("Grafo.txt");
    if (!archivo.is_open()) return;

    char origenStr[10], destinoStr[10], distStr[10];
    while (archivo.getline(origenStr, 10, ',')) {
        archivo.getline(destinoStr, 10, ',');
        archivo.getline(distStr, 10);

        int idOrigen = atoi(origenStr);
        int idDestino = atoi(destinoStr);
        int distancia = atoi(distStr);

        int idxO = obtenerIndiceSectorPorId(idOrigen);
        int idxD = obtenerIndiceSectorPorId(idDestino);

        if (idxO != -1 && idxD != -1) {
            matrizGrafo[idxO][idxD] = distancia;
            matrizGrafo[idxD][idxO] = distancia; // Grafo No Dirigido
        }
    }
    archivo.close();
}

void cargarClientes() {
    ifstream archivo("clientes.txt");
    if (!archivo.is_open()) return;

    contadorClientes = 0;
    while (archivo.getline(arregloClientes[contadorClientes].Cedula, 10, '|')) {
        if (strlen(arregloClientes[contadorClientes].Cedula) == 0) continue;
        archivo.getline(arregloClientes[contadorClientes].Telefono, 15, '|');
        archivo.getline(arregloClientes[contadorClientes].Nombre, 30, '|');
        char servStr[10];
        archivo.getline(servStr, 10);
        arregloClientes[contadorClientes].Servicios = atoi(servStr);
        contadorClientes++;
    }
    archivo.close();
}

void cargarRepartidores() {
    ifstream archivo("repartidores.txt");
    if (!archivo.is_open()) return;

    contadorRepartidores = 0;
    while (archivo.getline(arregloRepartidores[contadorRepartidores].Cedula, 10, '|')) {
        if (strlen(arregloRepartidores[contadorRepartidores].Cedula) == 0) continue;
        
        archivo.getline(arregloRepartidores[contadorRepartidores].Nombre, 30, '|');
        archivo.getline(arregloRepartidores[contadorRepartidores].Vehiculo, 20, '|');
        archivo.getline(arregloRepartidores[contadorRepartidores].Placa, 10, '|');
        
        char secStr[10], servStr[10], dispStr[10];
        archivo.getline(secStr, 10, '|');
        archivo.getline(servStr, 10, '|');
        archivo.getline(dispStr, 10);

        arregloRepartidores[contadorRepartidores].Sector = atoi(secStr);
        arregloRepartidores[contadorRepartidores].Servicios = atoi(servStr);
        arregloRepartidores[contadorRepartidores].Disponible = (atoi(dispStr) == 1);
        arregloRepartidores[contadorRepartidores].SectorOrigenCliente = -1;
        arregloRepartidores[contadorRepartidores].SectorDestinoCliente = -1;
        arregloRepartidores[contadorRepartidores].PaqueteRecogido = false;
        strcpy(arregloRepartidores[contadorRepartidores].CedulaClienteAtendido, "");
        
        contadorRepartidores++;
    }
    archivo.close();
}

void guardarSectores() {
    ofstream archivo("sectores.txt");
    if (!archivo.is_open()) return;
    for (int i = 0; i < contadorSectores; i++) {
        archivo << listaSectores[i].Id << "|" << listaSectores[i].Direccion << endl;
    }
    archivo.close();
}

void guardarClientes() {
    ofstream archivo("clientes.txt");
    if (!archivo.is_open()) return;
    for (int i = 0; i < contadorClientes; i++) {
        archivo << arregloClientes[i].Cedula << "|"
                << arregloClientes[i].Telefono << "|"
                << arregloClientes[i].Nombre << "|"
                << arregloClientes[i].Servicios << endl;
    }
    archivo.close();
}

void guardarRepartidores() {
    ofstream archivo("repartidores.txt");
    if (!archivo.is_open()) return;
    for (int i = 0; i < contadorRepartidores; i++) {
        archivo << arregloRepartidores[i].Cedula << "|"
                << arregloRepartidores[i].Nombre << "|"
                << arregloRepartidores[i].Vehiculo << "|"
                << arregloRepartidores[i].Placa << "|"
                << arregloRepartidores[i].Sector << "|"
                << arregloRepartidores[i].Servicios << "|"
                << (arregloRepartidores[i].Disponible ? 1 : 0) << endl;
    }
    archivo.close();
}

// --- ALGORITMO DE DIJKSTRA (RUTA MÍNIMA) ---
int obtenerDistanciaYRuta(int idxOrigen, int idxDestino, int rutaOut[], int &tamRuta) {
    if (idxOrigen == idxDestino) {
        rutaOut[0] = listaSectores[idxOrigen].Id;
        tamRuta = 1;
        return 0;
    }

    int dist[MAX_SECTORES];
    bool visitado[MAX_SECTORES];
    int previo[MAX_SECTORES];

    for (int i = 0; i < contadorSectores; i++) {
        dist[i] = INF;
        visitado[i] = false;
        previo[i] = -1;
    }

    dist[idxOrigen] = 0;

    for (int i = 0; i < contadorSectores - 1; i++) {
        int minDist = INF, u = -1;
        for (int j = 0; j < contadorSectores; j++) {
            if (!visitado[j] && dist[j] < minDist) {
                minDist = dist[j];
                u = j;
            }
        }

        if (u == -1) break;
        visitado[u] = true;

        for (int v = 0; v < contadorSectores; v++) {
            if (!visitado[v] && matrizGrafo[u][v] != INF && dist[u] + matrizGrafo[u][v] < dist[v]) {
                dist[v] = dist[u] + matrizGrafo[u][v];
                previo[v] = u;
            }
        }
    }

    if (dist[idxDestino] == INF) {
        tamRuta = 0;
        return INF;
    }

    int caminoTemp[MAX_SECTORES];
    int contCamino = 0;
    int curr = idxDestino;

    while (curr != -1) {
        caminoTemp[contCamino++] = listaSectores[curr].Id;
        curr = previo[curr];
    }

    tamRuta = 0;
    for (int i = contCamino - 1; i >= 0; i--) {
        rutaOut[tamRuta++] = caminoTemp[i];
    }

    return dist[idxDestino];
}

// --- GESTIÓN DE CLIENTES ---
void anadirCliente() {
    if (contadorClientes >= MAX_CLIENTES) return;
    char tempCedula[10];
    cin.ignore(); 
    cout << "Ingrese Cedula del cliente: ";
    cin.getline(tempCedula, 10);
    
    for (int i = 0; i < contadorClientes; i++) {
        if (strcmp(arregloClientes[i].Cedula, tempCedula) == 0) {
            cout << "\n[!] El cliente ya existe." << endl;
            return;
        }
    }
    strcpy(arregloClientes[contadorClientes].Cedula, tempCedula);
    cout << "Ingrese Nombre del cliente: ";
    cin.getline(arregloClientes[contadorClientes].Nombre, 30);
    cout << "Ingrese Telefono del cliente: ";
    cin.getline(arregloClientes[contadorClientes].Telefono, 15); 
    arregloClientes[contadorClientes].Servicios = 0;
    contadorClientes++;
    cout << "\nCliente registrado exitosamente." << endl;
}

void modificarCliente() {
    char tempCedula[10];
    cin.ignore();
    cout << "Ingrese la Cedula del cliente a modificar: ";
    cin.getline(tempCedula, 10);

    for (int i = 0; i < contadorClientes; i++) {
        if (strcmp(arregloClientes[i].Cedula, tempCedula) == 0) {
            cout << "Ingrese Nuevo Nombre: ";
            cin.getline(arregloClientes[i].Nombre, 30);
            cout << "Ingrese Nuevo Telefono: ";
            cin.getline(arregloClientes[i].Telefono, 15);
            cout << "\nCliente modificado con exito." << endl;
            return;
        }
    }
    cout << "\n[!] Cliente no encontrado." << endl;
}

void eliminarCliente() {
    char tempCedula[10];
    cin.ignore();
    cout << "Ingrese la Cedula del cliente a eliminar: ";
    cin.getline(tempCedula, 10);

    for (int i = 0; i < contadorClientes; i++) {
        if (strcmp(arregloClientes[i].Cedula, tempCedula) == 0) {
            for (int j = i; j < contadorClientes - 1; j++) {
                arregloClientes[j] = arregloClientes[j + 1];
            }
            contadorClientes--;
            cout << "\nCliente eliminado correctamente." << endl;
            return;
        }
    }
    cout << "\n[!] Cliente no encontrado." << endl;
}

void listaClientes() {
    cout << "\n--- LISTA DE CLIENTES REGISTRADOS ---" << endl;
    if (contadorClientes == 0) cout << "No hay clientes registrados." << endl;
    for (int i = 0; i < contadorClientes; i++) {
        cout << i + 1 << ". Nombre: " << arregloClientes[i].Nombre 
             << " | C.I: " << arregloClientes[i].Cedula 
             << " | Tel: " << arregloClientes[i].Telefono 
             << " | Servicios: " << arregloClientes[i].Servicios << endl;
    }
    cout << "\nPresione Enter para continuar..."; cin.ignore(); cin.get();
}

void menuClientes() {
    int op;
    do {
        cout << "\n--- GESTION DE CLIENTES ---" << endl;
        cout << "1. Anadir cliente\n2. Modificar cliente\n3. Eliminar cliente\n4. Lista de clientes\n5. Volver\nOpcion: ";
        cin >> op;
        if (op == 1) anadirCliente();
        else if (op == 2) modificarCliente();
        else if (op == 3) eliminarCliente();
        else if (op == 4) listaClientes();
    } while (op != 5);
}

// --- GESTIÓN DE REPARTIDORES ---
void anadirRepartidor() {
    if (contadorRepartidores >= MAX_REPARTIDORES) return;
    char tempCedula[10];
    cin.ignore();
    cout << "Ingrese Cedula del Repartidor: ";
    cin.getline(tempCedula, 10);

    for (int i = 0; i < contadorRepartidores; i++) {
        if (strcmp(arregloRepartidores[i].Cedula, tempCedula) == 0) {
            cout << "\n[!] El repartidor ya existe." << endl;
            return;
        }
    }

    strcpy(arregloRepartidores[contadorRepartidores].Cedula, tempCedula);
    cout << "Ingrese Nombre: ";
    cin.getline(arregloRepartidores[contadorRepartidores].Nombre, 30);
    cout << "Ingrese Tipo de Vehiculo: ";
    cin.getline(arregloRepartidores[contadorRepartidores].Vehiculo, 20);
    cout << "Ingrese Placa del Vehiculo: ";
    cin.getline(arregloRepartidores[contadorRepartidores].Placa, 10);
    cout << "Ingrese ID del Sector Inicial: ";
    cin >> arregloRepartidores[contadorRepartidores].Sector;

    arregloRepartidores[contadorRepartidores].Servicios = 0;
    arregloRepartidores[contadorRepartidores].Disponible = true;
    arregloRepartidores[contadorRepartidores].SectorOrigenCliente = -1;
    arregloRepartidores[contadorRepartidores].SectorDestinoCliente = -1;
    arregloRepartidores[contadorRepartidores].PaqueteRecogido = false;
    strcpy(arregloRepartidores[contadorRepartidores].CedulaClienteAtendido, "");

    contadorRepartidores++;
    cout << "\nRepartidor registrado con exito." << endl;
}

void modificarRepartidor() {
    char tempCedula[10];
    cin.ignore();
    cout << "Ingrese Cedula del repartidor a modificar: ";
    cin.getline(tempCedula, 10);

    for (int i = 0; i < contadorRepartidores; i++) {
        if (strcmp(arregloRepartidores[i].Cedula, tempCedula) == 0) {
            cout << "Ingrese Nuevo Nombre: ";
            cin.getline(arregloRepartidores[i].Nombre, 30);
            cout << "Ingrese Nuevo Vehiculo: ";
            cin.getline(arregloRepartidores[i].Vehiculo, 20);
            cout << "Ingrese Nueva Placa: ";
            cin.getline(arregloRepartidores[i].Placa, 10);
            cout << "Ingrese Nuevo Sector: ";
            cin >> arregloRepartidores[i].Sector;
            cout << "\nRepartidor actualizado correctamente." << endl;
            return;
        }
    }
    cout << "\n[!] Repartidor no encontrado." << endl;
}

void eliminarRepartidor() {
    char tempCedula[10];
    cin.ignore();
    cout << "Ingrese Cedula del repartidor a eliminar: ";
    cin.getline(tempCedula, 10);

    for (int i = 0; i < contadorRepartidores; i++) {
        if (strcmp(arregloRepartidores[i].Cedula, tempCedula) == 0) {
            for (int j = i; j < contadorRepartidores - 1; j++) {
                arregloRepartidores[j] = arregloRepartidores[j + 1];
            }
            contadorRepartidores--;
            cout << "\nRepartidor eliminado con exito." << endl;
            return;
        }
    }
    cout << "\n[!] Repartidor no encontrado." << endl;
}

void listaRepartidores() {
    cout << "\n--- LISTA DE REPARTIDORES ---" << endl;
    if (contadorRepartidores == 0) cout << "No hay repartidores registrados." << endl;
    for (int i = 0; i < contadorRepartidores; i++) {
        cout << i + 1 << ". " << arregloRepartidores[i].Nombre 
             << " | C.I: " << arregloRepartidores[i].Cedula
             << " | Sector Ubicacion: " << arregloRepartidores[i].Sector
             << " | Estado: ";
        if (arregloRepartidores[i].Disponible) {
            cout << "Disponible";
        } else if (!arregloRepartidores[i].PaqueteRecogido) {
            cout << "En camino a buscar cliente en Sector " << arregloRepartidores[i].SectorOrigenCliente;
        } else {
            cout << "Llevando paquete a Sector " << arregloRepartidores[i].SectorDestinoCliente;
        }
        cout << " | Servicios: " << arregloRepartidores[i].Servicios << endl;
    }
    cout << "\nPresione Enter para continuar..."; cin.ignore(); cin.get();
}

void menuRepartidores() {
    int op;
    do {
        cout << "\n--- GESTION DE REPARTIDORES ---" << endl;
        cout << "1. Anadir repartidor\n2. Modificar repartidor\n3. Eliminar repartidor\n4. Lista de repartidores\n5. Volver\nOpcion: ";
        cin >> op;
        if (op == 1) anadirRepartidor();
        else if (op == 2) modificarRepartidor();
        else if (op == 3) eliminarRepartidor();
        else if (op == 4) listaRepartidores();
    } while (op != 5);
}

// --- GESTIÓN DE SECTORES ---
void anadirSector() {
    if (contadorSectores >= MAX_SECTORES) return;
    int idTemp;
    cout << "Ingrese ID del Sector: ";
    cin >> idTemp;

    if (obtenerIndiceSectorPorId(idTemp) != -1) {
        cout << "\n[!] El Sector ya existe." << endl;
        return;
    }

    listaSectores[contadorSectores].Id = idTemp;
    cin.ignore();
    cout << "Ingrese Nombre/Direccion del Sector: ";
    cin.getline(listaSectores[contadorSectores].Direccion, 20);

    contadorSectores++;
    cout << "\nSector agregado con exito." << endl;
}

void listaSectoresMenu() {
    cout << "\n--- LISTA DE SECTORES ---" << endl;
    for (int i = 0; i < contadorSectores; i++) {
        cout << "ID: " << listaSectores[i].Id 
             << " | Direccion: " << listaSectores[i].Direccion << endl;
    }
    cout << "\nPresione Enter para continuar..."; cin.ignore(); cin.get();
}

void menuSectores() {
    int op;
    do {
        cout << "\n--- GESTION DE SECTORES ---" << endl;
        cout << "1. Anadir sector\n2. Lista de sectores\n3. Volver\nOpcion: ";
        cin >> op;
        if (op == 1) anadirSector();
        else if (op == 2) listaSectoresMenu();
    } while (op != 3);
}

void menuGestionInterna() {
    int op;
    do {
        cout << "\n--- GESTION INTERNA ---\n1. Clientes\n2. Repartidores\n3. Sectores\n4. Volver\nOpcion: ";
        cin >> op;
        if (op == 1) menuClientes();
        else if (op == 2) menuRepartidores();
        else if (op == 3) menuSectores();
    } while (op != 4);
}

// --- FASE 1: SOLICITAR ENVÍO (DESPLAZAMIENTO HACIA CLIENTE O COLA) ---
void solicitarEnvio() {
    if (contadorClientes == 0 || contadorRepartidores == 0) {
        cout << "\nSe requieren clientes y repartidores registrados." << endl;
        cout << "Presione Enter..."; cin.ignore(); cin.get();
        return;
    }

    char cedulaCliente[10];
    int sectorOrigen, sectorDestino;
    bool clienteExiste = false;

    cout << "\n--- SOLICITAR SERVICIO DE DELIVERY ---" << endl;
    cin.ignore();
    cout << "Ingrese la Cedula del Cliente: ";
    cin.getline(cedulaCliente, 10);

    for (int i = 0; i < contadorClientes; i++) {
        if (strcmp(arregloClientes[i].Cedula, cedulaCliente) == 0) {
            clienteExiste = true;
            break;
        }
    }

    if (!clienteExiste) {
        cout << "\n[!] Error: El cliente no esta registrado." << endl;
        cout << "Presione Enter..."; cin.get();
        return;
    }

    cout << "Ingrese ID del Sector Origen (Donde esta el cliente): ";
    cin >> sectorOrigen;
    cout << "Ingrese ID del Sector Destino (Donde entrega el paquete): ";
    cin >> sectorDestino;

    int idxOrigen = obtenerIndiceSectorPorId(sectorOrigen);
    int idxDestino = obtenerIndiceSectorPorId(sectorDestino);

    if (idxOrigen == -1 || idxDestino == -1) {
        cout << "\n[!] Error: Sectores invalidos." << endl;
        cout << "Presione Enter..."; cin.ignore(); cin.get();
        return;
    }

    // Buscar repartidores disponibles
    int indicesDisponibles[MAX_REPARTIDORES];
    int contDisponibles = 0;

    for (int i = 0; i < contadorRepartidores; i++) {
        if (arregloRepartidores[i].Disponible) {
            indicesDisponibles[contDisponibles] = i;
            contDisponibles++;
        }
    }

    if (contDisponibles == 0) {
        cout << "\n========================================================" << endl;
        cout << "[SIN REPARTIDORES DISPONIBLES]" << endl;
        cout << "El pedido fue asignado a la COLA DE ESPERA del Sector " << sectorOrigen << "." << endl;
        cout << "========================================================" << endl;
        encolarCliente(sectorOrigen, cedulaCliente, sectorDestino);
        cout << "\nPresione Enter para continuar..."; cin.ignore(); cin.get();
        return;
    }

    cout << "\n--- REPARTIDORES DISPONIBLES EN LA CIUDAD ---" << endl;
    for (int i = 0; i < contDisponibles; i++) {
        int idxRep = indicesDisponibles[i];
        int idxUbic = obtenerIndiceSectorPorId(arregloRepartidores[idxRep].Sector);

        int rutaAux[MAX_SECTORES], tamAux = 0;
        int distLlegada = obtenerDistanciaYRuta(idxUbic, idxOrigen, rutaAux, tamAux);

        cout << i + 1 << ". " << arregloRepartidores[idxRep].Nombre 
             << " | Ubicacion: Sector " << arregloRepartidores[idxRep].Sector;
        if (distLlegada != INF) cout << " | Distancia al Cliente: " << distLlegada << " km";
        else cout << " | [SIN CONEXION VIAL]";
        cout << endl;
    }

    int seleccion;
    cout << "\nSeleccione el numero del repartidor a enviar: ";
    cin >> seleccion;

    if (seleccion < 1 || seleccion > contDisponibles) {
        cout << "\n[!] Seleccion invalida." << endl;
        cout << "Presione Enter..."; cin.ignore(); cin.get();
        return;
    }

    int idxElegido = indicesDisponibles[seleccion - 1];
    int idxUbicElegido = obtenerIndiceSectorPorId(arregloRepartidores[idxElegido].Sector);

    int rutaLlegada[MAX_SECTORES], tamLlegada = 0;
    int distLlegada = obtenerDistanciaYRuta(idxUbicElegido, idxOrigen, rutaLlegada, tamLlegada);

    if (distLlegada == INF) {
        cout << "\n[!] Error: No hay ruta disponible para que este repartidor llegue al sector del cliente." << endl;
        cout << "Presione Enter..."; cin.ignore(); cin.get();
        return;
    }

    // Asignar primera fase (En camino a buscar paquete)
    arregloRepartidores[idxElegido].Disponible = false;
    arregloRepartidores[idxElegido].PaqueteRecogido = false;
    arregloRepartidores[idxElegido].SectorOrigenCliente = sectorOrigen;
    arregloRepartidores[idxElegido].SectorDestinoCliente = sectorDestino;
    strcpy(arregloRepartidores[idxElegido].CedulaClienteAtendido, cedulaCliente);

    cout << "\n========================================================" << endl;
    cout << "             FASE 1: REPARTIDOR EN CAMINO               " << endl;
    cout << "========================================================" << endl;
    cout << "Repartidor " << arregloRepartidores[idxElegido].Nombre 
         << " va en camino al Sector " << sectorOrigen << endl;
    cout << "Distancia a recorrer: " << distLlegada << " km" << endl;
    cout << "Ruta mínima a seguir: ";
    for (int i = 0; i < tamLlegada; i++) {
        cout << rutaLlegada[i] << (i < tamLlegada - 1 ? " -> " : "");
    }
    cout << "\n========================================================" << endl;

    cout << "\nPresione Enter para continuar..."; cin.ignore(); cin.get();
}

// --- FASE 2: CONFIRMAR LLEGADA Y DESPACHAR A DESTINO ---
void confirmarLlegadaYDespachar() {
    int indicesEnCamino[MAX_REPARTIDORES];
    int contEnCamino = 0;

    for (int i = 0; i < contadorRepartidores; i++) {
        if (!arregloRepartidores[i].Disponible && !arregloRepartidores[i].PaqueteRecogido) {
            indicesEnCamino[contEnCamino] = i;
            contEnCamino++;
        }
    }

    if (contEnCamino == 0) {
        cout << "\nNo hay repartidores en camino a recoger clientes actualmente." << endl;
        cout << "\nPresione Enter para continuar..."; cin.ignore(); cin.get();
        return;
    }

    cout << "\n--- REPARTIDORES EN CAMINO A BUSCAR PAQUETE ---" << endl;
    for (int i = 0; i < contEnCamino; i++) {
        int idx = indicesEnCamino[i];
        cout << i + 1 << ". Repartidor: " << arregloRepartidores[idx].Nombre 
             << " | Dirigiendose al Sector Origen: " << arregloRepartidores[idx].SectorOrigenCliente 
             << " (Cliente C.I: " << arregloRepartidores[idx].CedulaClienteAtendido << ")" << endl;
    }

    int sel;
    cout << "\nSeleccione el repartidor que ha LLEGADO donde el cliente: ";
    cin >> sel;

    if (sel < 1 || sel > contEnCamino) {
        cout << "\n[!] Seleccion invalida." << endl;
        cout << "Presione Enter..."; cin.ignore(); cin.get();
        return;
    }

    int idxRep = indicesEnCamino[sel - 1];
    
    // Actualizar ubicación física al sector del cliente y cambiar estado
    arregloRepartidores[idxRep].Sector = arregloRepartidores[idxRep].SectorOrigenCliente;
    arregloRepartidores[idxRep].PaqueteRecogido = true;

    int idxOrigen = obtenerIndiceSectorPorId(arregloRepartidores[idxRep].SectorOrigenCliente);
    int idxDestino = obtenerIndiceSectorPorId(arregloRepartidores[idxRep].SectorDestinoCliente);

    int rutaEntrega[MAX_SECTORES], tamEntrega = 0;
    int distEntrega = obtenerDistanciaYRuta(idxOrigen, idxDestino, rutaEntrega, tamEntrega);

    cout << "\n========================================================" << endl;
    cout << "             FASE 2: PAQUETE RECOGIDO                   " << endl;
    cout << "========================================================" << endl;
    cout << "El repartidor " << arregloRepartidores[idxRep].Nombre 
         << " recogio el paquete en Sector " << arregloRepartidores[idxRep].SectorOrigenCliente << endl;
    cout << "Iniciando viaje hacia Sector Destino: " << arregloRepartidores[idxRep].SectorDestinoCliente << endl;
    cout << "Distancia del viaje: " << distEntrega << " km" << endl;
    cout << "Ruta mínima a seguir: ";
    for (int i = 0; i < tamEntrega; i++) {
        cout << rutaEntrega[i] << (i < tamEntrega - 1 ? " -> " : "");
    }
    cout << "\n========================================================" << endl;

    cout << "\nPresione Enter para continuar..."; cin.ignore(); cin.get();
}

// --- FASE 3: FINALIZAR ENTREGA Y REVISAR COLA DE ESPERA ---
void finalizarEnvio() {
    int indicesLlevando[MAX_REPARTIDORES];
    int contLlevando = 0;

    for (int i = 0; i < contadorRepartidores; i++) {
        if (!arregloRepartidores[i].Disponible && arregloRepartidores[i].PaqueteRecogido) {
            indicesLlevando[contLlevando] = i;
            contLlevando++;
        }
    }

    if (contLlevando == 0) {
        cout << "\nNo hay repartidores realizando la entrega del paquete actualmente." << endl;
        cout << "\nPresione Enter para continuar..."; cin.ignore(); cin.get();
        return;
    }

    cout << "\n--- REPARTIDORES EN TRAYECTO DE ENTREGA ---" << endl;
    for (int i = 0; i < contLlevando; i++) {
        int idx = indicesLlevando[i];
        cout << i + 1 << ". Repartidor: " << arregloRepartidores[idx].Nombre 
             << " | Llevando a Cliente C.I: " << arregloRepartidores[idx].CedulaClienteAtendido 
             << " | Destino Final: Sector " << arregloRepartidores[idx].SectorDestinoCliente << endl;
    }

    int sel;
    cout << "\nSeleccione el repartidor que ha COMPLETADO la entrega: ";
    cin >> sel;

    if (sel < 1 || sel > contLlevando) {
        cout << "\n[!] Seleccion invalida." << endl;
        cout << "Presione Enter..."; cin.ignore(); cin.get();
        return;
    }

    int idxRep = indicesLlevando[sel - 1];
    
    // Actualizar estadísticas y moverlo al sector de destino
    arregloRepartidores[idxRep].Servicios++;
    arregloRepartidores[idxRep].Sector = arregloRepartidores[idxRep].SectorDestinoCliente;

    for (int i = 0; i < contadorClientes; i++) {
        if (strcmp(arregloClientes[i].Cedula, arregloRepartidores[idxRep].CedulaClienteAtendido) == 0) {
            arregloClientes[i].Servicios++;
            break;
        }
    }

    cout << "\n[+] Envío completado con exito por " << arregloRepartidores[idxRep].Nombre 
         << ". Ahora se encuentra en Sector " << arregloRepartidores[idxRep].Sector << "." << endl;

    // Verificar si hay clientes esperando en la COLA de este Sector
    int sectorActual = arregloRepartidores[idxRep].Sector;
    char clienteSiguiente[10];
    int destinoSiguiente;

    if (desencolarCliente(sectorActual, clienteSiguiente, destinoSiguiente)) {
        arregloRepartidores[idxRep].Disponible = false;
        arregloRepartidores[idxRep].PaqueteRecogido = true; // Ya está en el sector del cliente
        arregloRepartidores[idxRep].SectorOrigenCliente = sectorActual;
        arregloRepartidores[idxRep].SectorDestinoCliente = destinoSiguiente;
        strcpy(arregloRepartidores[idxRep].CedulaClienteAtendido, clienteSiguiente);

        cout << "\n[!] ATENCION: El repartidor tomó inmediatamente a un cliente de la COLA DE ESPERA." << endl;
        cout << "    Cliente C.I: " << clienteSiguiente << " con destino al Sector " << destinoSiguiente << "." << endl;
    } else {
        arregloRepartidores[idxRep].Disponible = true;
        arregloRepartidores[idxRep].PaqueteRecogido = false;
        arregloRepartidores[idxRep].SectorOrigenCliente = -1;
        arregloRepartidores[idxRep].SectorDestinoCliente = -1;
        strcpy(arregloRepartidores[idxRep].CedulaClienteAtendido, "");
        cout << "[i] No hay cola en este sector. Repartidor marcado como DISPONIBLE." << endl;
    }

    cout << "\nPresione Enter para continuar..."; cin.ignore(); cin.get();
}

// --- MENÚ Y PROCESAR COLA MANUALMENTE ---
void consultarColasDeEspera() {
    cout << "\n--- ESTADO DE LAS COLAS DE ESPERA POR SECTOR ---" << endl;
    bool hayColas = false;

    for (int i = 0; i < contadorSectores; i++) {
        int idSec = listaSectores[i].Id;
        if (colasEspera[i].frente != NULL) {
            hayColas = true;
            cout << "Sector " << idSec << " (" << listaSectores[i].Direccion << "):" << endl;
            NodoCola* curr = colasEspera[i].frente;
            int pos = 1;
            while (curr != NULL) {
                cout << "   " << pos++ << ". Cliente C.I: " << curr->CedulaCliente 
                     << " -> Destino: Sector " << curr->SectorDestino << endl;
                curr = curr->siguiente;
            }
        }
    }

    if (!hayColas) {
        cout << "No hay clientes en cola de espera en ningun sector." << endl;
    }
    cout << "\nPresione Enter para continuar..."; cin.ignore(); cin.get();
}

void menuServicioDiario() {
    int op;
    do {
        cout << "\n--- SERVICIO DIARIO ---" << endl;
        cout << "1. Solicitar Envio (Fase 1: Asignar Repartidor)" << endl;
        cout << "2. Confirmar Llegada a Cliente (Fase 2: En Camino a Destino)" << endl;
        cout << "3. Finalizar Envio (Fase 3: Entregar y Procesar Cola)" << endl;
        cout << "4. Consultar Colas de Espera" << endl;
        cout << "5. Volver" << endl;
        cout << "Opcion: ";
        cin >> op;

        if (op == 1) solicitarEnvio();
        else if (op == 2) confirmarLlegadaYDespachar();
        else if (op == 3) finalizarEnvio();
        else if (op == 4) consultarColasDeEspera();
    } while (op != 5);
}

// --- REPORTES ---
void reporteClientes() {
    cout << "\n--- REPORTE DE CLIENTES CON MAS SERVICIOS ---" << endl;
    for (int i = 0; i < contadorClientes - 1; i++) {
        for (int j = 0; j < contadorClientes - i - 1; j++) {
            if (arregloClientes[j].Servicios < arregloClientes[j + 1].Servicios) {
                Cliente temp = arregloClientes[j];
                arregloClientes[j] = arregloClientes[j + 1];
                arregloClientes[j + 1] = temp;
            }
        }
    }
    listaClientes();
}

void reporteRepartidores() {
    cout << "\n--- REPORTE DE REPARTIDORES CON MAS SERVICIOS ---" << endl;
    for (int i = 0; i < contadorRepartidores - 1; i++) {
        for (int j = 0; j < contadorRepartidores - i - 1; j++) {
            if (arregloRepartidores[j].Servicios < arregloRepartidores[j + 1].Servicios) {
                Repartidor temp = arregloRepartidores[j];
                arregloRepartidores[j] = arregloRepartidores[j + 1];
                arregloRepartidores[j + 1] = temp;
            }
        }
    }
    listaRepartidores();
}

void menuReportes() {
    int op;
    do {
        cout << "\n--- MENU DE REPORTES ---" << endl;
        cout << "1. Clientes con mas servicios\n2. Repartidores con mas servicios\n3. Volver\nOpcion: ";
        cin >> op;
        if (op == 1) reporteClientes();
        else if (op == 2) reporteRepartidores();
    } while (op != 3);
}

int main() {
    inicializarMatrizGrafo();
    inicializarColas();

    cargarSectores();
    cargarGrafo();
    cargarClientes();
    cargarRepartidores();

    int opcion;
    do {
        cout << "\n=========================================" << endl;
        cout << "         SPEEDDELIVERY - SISTEMA         " << endl;
        cout << "=========================================" << endl;
        cout << "1. Gestion Interna" << endl;
        cout << "2. Servicio Diario" << endl;
        cout << "3. Reportes" << endl;
        cout << "4. Salir" << endl;
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        switch (opcion) {
            case 1: menuGestionInterna(); break;
            case 2: menuServicioDiario(); break;
            case 3: menuReportes(); break;
            case 4:
                guardarSectores();
                guardarClientes();
                guardarRepartidores();
                liberarMatrizGrafo();
                cout << "\nSaliendo del programa y liberando memoria del Grafo..." << endl;
                break;
            default: break;
        }
    } while (opcion != 4);

    return 0;
}