// San Cristobal, Julio de 2026
// Jesus David Florez Morales V-31.762.806

#include <iostream>
#include <cstring>  
#include <fstream>  
#include <cstdlib>  
#include <ctime>    
using namespace std;

// ==================== ESTRUCTURAS DE DATOS ====================

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
    int Sector;          // Sector actual
    int SectorDestino;   // Sector al que viaja si esta ocupado
    char CedulaClienteAtendido[10]; // Cliente que lleva en el viaje actual
    int Servicios;
    bool Disponible;     // true = Disponible, false = En Viaje
};

// Nodo y Cola Dinamica para la lista de espera de Clientes por Sector
struct NodoCola {
    char CedulaCliente[10];
    int SectorDestino;
    NodoCola* siguiente;
};

struct ColaSector {
    NodoCola* frente;
    NodoCola* final;
};

// ==================== CONSTANTES Y VARIABLES GLOBALES ====================

const int MAX_CLIENTES = 50;
const int MAX_REPARTIDORES = 50;
const int MAX_SECTORES = 20;

int contadorClientes = 0;
int contadorRepartidores = 0;
int contadorSectores = 0;
int opcionPrincipal;

Cliente arregloClientes[MAX_CLIENTES];
Repartidor arregloRepartidores[MAX_REPARTIDORES];
Sector listaSectores[MAX_SECTORES];

// Colas de espera por sector
ColaSector colasEspera[MAX_SECTORES];

// ==================== MANEJO DE COLAS DINAMICAS ====================

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

bool hayClientesEnEspera(int idSectorOrigen) {
    int idx = obtenerIndiceSectorPorId(idSectorOrigen);
    if (idx == -1) return false;
    return colasEspera[idx].frente != NULL;
}

// ==================== PERSISTENCIA DE DATOS (ARCHIVOS) ====================

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
        arregloRepartidores[contadorRepartidores].SectorDestino = -1;
        strcpy(arregloRepartidores[contadorRepartidores].CedulaClienteAtendido, "");
        
        contadorRepartidores++;
    }
    archivo.close();
}

void guardarSectores() {
    ofstream archivo("sectores.txt");
    if (!archivo.is_open()) return;
    
    for (int i = 0; i < contadorSectores; i++) {
        archivo << listaSectores[i].Id << "|"
                << listaSectores[i].Direccion << endl;
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

void generarReporteEstadisticas() {
    ofstream archivo("reporte_estadisticas.txt");
    if (!archivo.is_open()) return;

    archivo << "=========================================" << endl;
    archivo << "    SPEEDDELIVERY - REPORTE DE JORNADA    " << endl;
    archivo << "=========================================" << endl;

    int maxServiciosCliente = -1;
    for(int i = 0; i < contadorClientes; i++) {
        if(arregloClientes[i].Servicios > maxServiciosCliente) {
            maxServiciosCliente = arregloClientes[i].Servicios;
        }
    }
    archivo << "\n[CLIENTE(S) MAS FRECUENTES] (Max Servicios: " << maxServiciosCliente << ")" << endl;
    for(int i = 0; i < contadorClientes; i++) {
        if(arregloClientes[i].Servicios == maxServiciosCliente && maxServiciosCliente > 0) {
            archivo << "- " << arregloClientes[i].Nombre << " (Cedula: " << arregloClientes[i].Cedula << ")" << endl;
        }
    }

    int maxServiciosRep = -1;
    for(int i = 0; i < contadorRepartidores; i++) {
        if(arregloRepartidores[i].Servicios > maxServiciosRep) {
            maxServiciosRep = arregloRepartidores[i].Servicios;
        }
    }
    archivo << "\n[REPARTIDOR(ES) ESTRELLA] (Max Entregas: " << maxServiciosRep << ")" << endl;
    for(int i = 0; i < contadorRepartidores; i++) {
        if(arregloRepartidores[i].Servicios == maxServiciosRep && maxServiciosRep > 0) {
            archivo << "- " << arregloRepartidores[i].Nombre << " (Placa: " << arregloRepartidores[i].Placa << ")" << endl;
        }
    }
    archivo.close();
}

// ==================== GESTION DE CLIENTES ====================

void anadirCliente() {
    if (contadorClientes >= MAX_CLIENTES) {
        cout << "\nYa no caben mas clientes!" << endl;
        cout << "Presione Enter para continuar..."; cin.ignore(); cin.get();
        return;
    }
    
    char tempCedula[10];
    cin.ignore(); 
    cout << "---Cedula del cliente---" << endl;
    cin.getline(tempCedula, 10);

    for (int i = 0; i < contadorClientes; i++) {
        if (strcmp(arregloClientes[i].Cedula, tempCedula) == 0) {
            cout << "\n[!] Error: Ya existe un cliente registrado con esa cedula." << endl;
            cout << "Presione Enter para continuar..."; cin.get();
            return;
        }
    }

    strcpy(arregloClientes[contadorClientes].Cedula, tempCedula);
    cout << "---Nombre del cliente---" << endl;
    cin.getline(arregloClientes[contadorClientes].Nombre, 30);
    cout << "---N. telefono del cliente---" << endl;
    cin.getline(arregloClientes[contadorClientes].Telefono, 15); 
    
    arregloClientes[contadorClientes].Servicios = 0;
    contadorClientes++;
    cout << "\nCliente registrado con exito!" << endl;
    cout << "Presione Enter para continuar..."; cin.get();
}

void listaClientes() {
    cout << "\n--- LISTA DE CLIENTES ---" << endl;
    if (contadorClientes == 0) {
        cout << "No hay clientes registrados en el sistema." << endl;
    } else {
        for (int i = 0; i < contadorClientes; i++) {
            cout << i + 1 << ". Nombre: " << arregloClientes[i].Nombre 
                 << " | C.I: " << arregloClientes[i].Cedula
                 << " | Telefono: " << arregloClientes[i].Telefono 
                 << " | Servicios Realizados: " << arregloClientes[i].Servicios << endl;
        }
    }
    cout << "\nPresione Enter para continuar..."; cin.ignore(); cin.get();
}

void modificarCliente() {
    char cedulaBuscar[10];
    bool encontrado = false;
    
    cout << "--- Ingrese la Cedula del Cliente a Modificar ---" << endl;
    cin.ignore();
    cin.getline(cedulaBuscar, 10);
    
    for(int i = 0; i < contadorClientes; i++) {
        if(strcmp(arregloClientes[i].Cedula, cedulaBuscar) == 0) {
            cout << "\n--- MODIFICANDO DATOS ---" << endl;
            cout << "Nombre Actual (" << arregloClientes[i].Nombre << "): ";
            cin.getline(arregloClientes[i].Nombre, 30);
            cout << "Telefono Actual (" << arregloClientes[i].Telefono << "): ";
            cin.getline(arregloClientes[i].Telefono, 15);
            
            cout << "\nCliente modificado con exito!" << endl;
            encontrado = true;
            break;
        }
    }
    if(!encontrado) cout << "\nCliente no registrado en el sistema." << endl;
    cout << "\nPresione Enter para continuar..."; cin.get();
}

void eliminarCliente() {
    if (contadorClientes == 0) { 
        cout << "\nNo hay clientes registrados en el sistema." << endl;
        cout << "Presione Enter..."; cin.ignore(); cin.get();
        return;
    }

    char cedulaBuscar[10];
    int indiceEliminar = -1;

    cout << "--- Ingrese la Cedula del Cliente a Eliminar ---" << endl;
    cin.ignore();
    cin.getline(cedulaBuscar, 10);

    for (int i = 0; i < contadorClientes; i++) {
        if (strcmp(arregloClientes[i].Cedula, cedulaBuscar) == 0) {
            indiceEliminar = i;
            break;
        }
    }

    if (indiceEliminar != -1) {
        for (int i = indiceEliminar; i < contadorClientes - 1; i++) {
            arregloClientes[i] = arregloClientes[i + 1];
        }
        contadorClientes--;
        cout << "\nCliente eliminado de forma exitosa." << endl;
    } else {
        cout << "\nError: Cedula no encontrada." << endl;
    }
    cout << "Presione Enter para continuar..."; cin.get();
}

void menuClientes() {
    int opClientes;
    do {
        cout << "\033[2J\033[1;1H";
        cout << "----- MENU CLIENTES -----" << endl;
        cout << "1. Anadir cliente" << endl;
        cout << "2. Lista de clientes" << endl;
        cout << "3. Modificar cliente" << endl;
        cout << "4. Eliminar cliente" << endl;
        cout << "5. Volver al menu anterior" << endl; 
        cout << "Seleccione una opcion: ";
        cin >> opClientes;

        switch (opClientes) {
            case 1: anadirCliente(); break;
            case 2: listaClientes(); break;
            case 3: modificarCliente(); break;
            case 4: eliminarCliente(); break; 
            case 5: break;
            default: break;
        }
    } while (opClientes != 5);
}

// ==================== GESTION DE REPARTIDORES ====================

void anadirRepartidor() {
    if (contadorRepartidores >= MAX_REPARTIDORES) {
        cout << "\nCapacidad de repartidores al maximo!" << endl;
        cout << "Presione Enter para continuar..."; cin.ignore(); cin.get();
        return;
    }
    
    char tempCedula[10];
    cin.ignore();
    cout << "---Cedula del Repartidor---" << endl;
    cin.getline(tempCedula, 10);

    for (int i = 0; i < contadorRepartidores; i++) {
        if (strcmp(arregloRepartidores[i].Cedula, tempCedula) == 0) {
            cout << "\n[!] Error: Ya existe un repartidor con esa cedula." << endl;
            cout << "Presione Enter para continuar..."; cin.get();
            return;
        }
    }

    strcpy(arregloRepartidores[contadorRepartidores].Cedula, tempCedula);
    cout << "---Nombre del Repartidor---" << endl;
    cin.getline(arregloRepartidores[contadorRepartidores].Nombre, 30);
    cout << "---Vehiculo---" << endl;
    cin.getline(arregloRepartidores[contadorRepartidores].Vehiculo, 20);
    cout << "---Placa---" << endl;
    cin.getline(arregloRepartidores[contadorRepartidores].Placa, 10);
    cout << "---ID del Sector Asignado (Numero)---" << endl;
    cin >> arregloRepartidores[contadorRepartidores].Sector;
    
    arregloRepartidores[contadorRepartidores].Servicios = 0;
    arregloRepartidores[contadorRepartidores].Disponible = true;
    arregloRepartidores[contadorRepartidores].SectorDestino = -1;
    strcpy(arregloRepartidores[contadorRepartidores].CedulaClienteAtendido, "");

    contadorRepartidores++;
    cout << "\nRepartidor registrado con exito!" << endl;
    cout << "Presione Enter para continuar..."; cin.ignore(); cin.get();
}

void listaRepartidores() {
    cout << "\n--- LISTA DE REPARTIDORES ---" << endl;
    if (contadorRepartidores == 0) {
        cout << "No hay repartidores registrados en el sistema." << endl;
    } else {
        for (int i = 0; i < contadorRepartidores; i++) {
            cout << i + 1 << ". Nombre: " << arregloRepartidores[i].Nombre 
                 << " | Placa: " << arregloRepartidores[i].Placa 
                 << " | Sector Actual: " << arregloRepartidores[i].Sector
                 << " | Estado: ";
            if (arregloRepartidores[i].Disponible) {
                cout << "[DISPONIBLE]";
            } else {
                cout << "[EN VIAJE -> Sector " << arregloRepartidores[i].SectorDestino << "]";
            }
            cout << " | Entregas Totales: " << arregloRepartidores[i].Servicios << endl;
        }
    }
    cout << "\nPresione Enter para continuar..."; cin.ignore(); cin.get();
}

void modificarRepartidor() {
    char cedulaBuscar[10];
    bool encontrado = false;
    
    cout << "--- Ingrese la Cedula del Repartidor a Modificar ---" << endl;
    cin.ignore();
    cin.getline(cedulaBuscar, 10);
    
    for(int i = 0; i < contadorRepartidores; i++) {
        if(strcmp(arregloRepartidores[i].Cedula, cedulaBuscar) == 0) {
            cout << "\n--- MODIFICANDO DATOS ---" << endl;
            cout << "Nombre Actual (" << arregloRepartidores[i].Nombre << "): ";
            cin.getline(arregloRepartidores[i].Nombre, 30);
            cout << "Vehiculo Actual (" << arregloRepartidores[i].Vehiculo << "): ";
            cin.getline(arregloRepartidores[i].Vehiculo, 20);
            cout << "Placa Actual (" << arregloRepartidores[i].Placa << "): ";
            cin.getline(arregloRepartidores[i].Placa, 10);
            
            cout << "\nRepartidor modificado con exito!" << endl;
            encontrado = true;
            break;
        }
    }
    if(!encontrado) cout << "\nRepartidor no encontrado." << endl;
    cout << "\nPresione Enter para continuar..."; cin.get();
}

void eliminarRepartidor() {
    if (contadorRepartidores == 0) { 
        cout << "\nNo hay repartidores registrados." << endl;
        cout << "Presione Enter..."; cin.ignore(); cin.get();
        return;
    }

    char cedulaBuscar[10];
    int indiceEliminar = -1;

    cout << "--- Ingrese la Cedula del repartidor a Eliminar ---" << endl;
    cin.ignore();
    cin.getline(cedulaBuscar, 10);

    for (int i = 0; i < contadorRepartidores; i++) {
        if (strcmp(arregloRepartidores[i].Cedula, cedulaBuscar) == 0) {
            indiceEliminar = i;
            break;
        }
    }

    if (indiceEliminar != -1) {
        for (int i = indiceEliminar; i < contadorRepartidores - 1; i++) {
            arregloRepartidores[i] = arregloRepartidores[i + 1];
        }
        contadorRepartidores--;
        cout << "\nRepartidor eliminado con exito." << endl;
    } else {
        cout << "\nError: Cedula no encontrada." << endl;
    }
    cout << "Presione Enter para continuar..."; cin.get();
}

void menuRepartidores() {
    int opRep;
    do {
        cout << "\033[2J\033[1;1H";
        cout << "----- MENU REPARTIDORES -----" << endl;
        cout << "1. Anadir repartidor" << endl;
        cout << "2. Lista de repartidores" << endl;
        cout << "3. Modificar repartidor" << endl;
        cout << "4. Eliminar repartidor" << endl;
        cout << "5. Volver al menu anterior" << endl; 
        cout << "Seleccione una opcion: ";
        cin >> opRep;

        switch (opRep) {
            case 1: anadirRepartidor(); break;
            case 2: listaRepartidores(); break; 
            case 3: modificarRepartidor(); break;
            case 4: eliminarRepartidor(); break; 
            case 5: break;
            default: break;
        }
    } while (opRep != 5);
}

// ==================== GESTION DE SECTORES ====================

void anadirSector() {
    if (contadorSectores >= MAX_SECTORES) {
        cout << "\nLimite de sectores alcanzado!" << endl;
        cout << "Presione Enter..."; cin.ignore(); cin.get();
        return;
    }
    
    int tempId;
    cout << "--- ID del Sector (Numero) ---" << endl;
    cin >> tempId;

    for(int i = 0; i < contadorSectores; i++) {
        if(listaSectores[i].Id == tempId) {
            cout << "\n[!] Error: Ya existe un sector registrado con ese ID." << endl;
            cout << "Presione Enter..."; cin.ignore(); cin.get();
            return;
        }
    }
    
    listaSectores[contadorSectores].Id = tempId;
    cin.ignore();
    cout << "--- Direccion/Nombre del Sector ---" << endl;
    cin.getline(listaSectores[contadorSectores].Direccion, 20);
    
    contadorSectores++;
    cout << "\nSector registrado con exito!" << endl;
    cout << "Presione Enter..."; cin.get();
}

void consultarSectores() {
    cout << "\n--- SECTORES REGISTRADOS ---" << endl;
    if(contadorSectores == 0) {
        cout << "No hay sectores cargados en el sistema." << endl;
    } else {
        for(int i = 0; i < contadorSectores; i++) {
            cout << "ID: " << listaSectores[i].Id << " | Direccion: " << listaSectores[i].Direccion << endl;
        }
    }
    cout << "\nPresione Enter para continuar..."; cin.ignore(); cin.get();
}

void modificarSector() {
    int idBuscar;
    bool encontrado = false;
    
    cout << "--- Ingrese el ID del Sector a Modificar ---" << endl;
    cin >> idBuscar;
    
    for(int i = 0; i < contadorSectores; i++) {
        if(listaSectores[i].Id == idBuscar) {
            cin.ignore();
            cout << "Direccion Actual (" << listaSectores[i].Direccion << "): ";
            cin.getline(listaSectores[i].Direccion, 20);
            
            cout << "\nSector modificado con exito!" << endl;
            encontrado = true;
            break;
        }
    }
    if(!encontrado) cout << "\nSector no registrado." << endl;
    cout << "\nPresione Enter para continuar..."; cin.ignore(); cin.get();
}

void menuSectores() {
    int opSec;
    do {
        cout << "\033[2J\033[1;1H";
        cout << "----- MENU SECTORES -----" << endl;
        cout << "1. Anadir sector" << endl;
        cout << "2. Ver todos los sectores" << endl;
        cout << "3. Modificar sector" << endl;
        cout << "4. Volver al menu anterior" << endl; 
        cout << "Seleccione una opcion: ";
        cin >> opSec;

        switch (opSec) {
            case 1: anadirSector(); break;
            case 2: consultarSectores(); break; 
            case 3: modificarSector(); break;
            case 4: break;
            default: break;
        }
    } while (opSec != 4);
}

void menuGestionInterna() {
    int op;
    do {
        cout << "\033[2J\033[1;1H";
        cout << "----- GESTION INTERNA -----" << endl;
        cout << "1. Clientes" << endl;
        cout << "2. Repartidores" << endl;
        cout << "3. Sectores" << endl;
        cout << "4. Volver al Menu Principal" << endl;
        cout << "Seleccione una opcion: ";
        cin >> op;

        switch (op) {
            case 1: menuClientes(); break;
            case 2: menuRepartidores(); break;
            case 3: menuSectores(); break;  
            case 4: break;
            default: break;
        }
    } while (op != 4);
}

// ==================== SERVICIO DIARIO ====================

void iniciarJornada() {
    if (contadorRepartidores == 0 || contadorSectores == 0) {
        cout << "\nError: No se puede iniciar jornada sin datos cargados." << endl;
        cout << "Presione Enter para continuar..."; cin.ignore(); cin.get();
        return;
    }
    srand(time(NULL));

    for (int i = 0; i < contadorRepartidores; i++) {
        int indiceAleatorio = rand() % contadorSectores;
        arregloRepartidores[i].Sector = listaSectores[indiceAleatorio].Id;
        arregloRepartidores[i].Disponible = true; 
        arregloRepartidores[i].SectorDestino = -1;
        strcpy(arregloRepartidores[i].CedulaClienteAtendido, "");
    }

    cout << "\nJornada Iniciada. Motorizados distribuidos por la ciudad." << endl;
    cout << "Presione Enter para continuar..."; cin.ignore(); cin.get();
}

void solicitarEnvio() {
    if (contadorClientes == 0 || contadorRepartidores == 0) {
        cout << "\nSe requieren clientes y repartidores registrados." << endl;
        cout << "Presione Enter..."; cin.ignore(); cin.get();
        return;
    }

    char cedulaCliente[10];
    int sectorOrigen, sectorDestino;
    bool clienteExiste = false;

    cout << "--- SOLICITAR SERVICIO DE DELIVERY ---" << endl;
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
        cout << "\n[!] Error: El cliente no esta registrado en Gestion Interna." << endl;
        cout << "Presione Enter..."; cin.get();
        return;
    }

    cout << "Ingrese ID del Sector Origen (Donde se busca el paquete): ";
    cin >> sectorOrigen;
    cout << "Ingrese ID del Sector Destino (Donde se entrega): ";
    cin >> sectorDestino;

    // Buscar repartidores DISPONIBLES en el sector origen
    int indicesDisponibles[MAX_REPARTIDORES];
    int contDisponibles = 0;

    for (int i = 0; i < contadorRepartidores; i++) {
        if (arregloRepartidores[i].Sector == sectorOrigen && arregloRepartidores[i].Disponible) {
            indicesDisponibles[contDisponibles] = i;
            contDisponibles++;
        }
    }

    if (contDisponibles == 0) {
        cout << "\n========================================================" << endl;
        cout << "[SIN REPARTIDORES DISPONIBLES EN EL SECTOR " << sectorOrigen << "]" << endl;
        cout << "Su pedido ha sido registrado con exito en la COLA DE ESPERA." << endl;
        cout << "Saldra automaticamente cuando un motorizado este libre." << endl;
        cout << "========================================================" << endl;
        
        encolarCliente(sectorOrigen, cedulaCliente, sectorDestino);

        cout << "\nPresione Enter para continuar..."; cin.ignore(); cin.get();
        return;
    }

    cout << "\n--- REPARTIDORES DISPONIBLES EN EL SECTOR ---" << endl;
    for (int i = 0; i < contDisponibles; i++) {
        int idx = indicesDisponibles[i];
        cout << i + 1 << ". " << arregloRepartidores[idx].Nombre 
             << " | Vehiculo: " << arregloRepartidores[idx].Vehiculo 
             << " (Placa: " << arregloRepartidores[idx].Placa << ")" << endl;
    }

    int seleccion;
    cout << "\nSeleccione el numero del repartidor: ";
    cin >> seleccion;

    if (seleccion < 1 || seleccion > contDisponibles) {
        cout << "\n[!] Seleccion invalida. Operacion abortada." << endl;
        cout << "Presione Enter..."; cin.ignore(); cin.get();
        return;
    }

    int idxElegido = indicesDisponibles[seleccion - 1];

    // Asignacion de envio (El repartidor pasa a estar "EN VIAJE")
    arregloRepartidores[idxElegido].Disponible = false;
    arregloRepartidores[idxElegido].SectorDestino = sectorDestino;
    strcpy(arregloRepartidores[idxElegido].CedulaClienteAtendido, cedulaCliente);

    cout << "\n========================================================" << endl;
    cout << "[ENVIO EN CAMINO]" << endl;
    cout << "El repartidor " << arregloRepartidores[idxElegido].Nombre 
         << " va desde Sector " << sectorOrigen 
         << " hacia Sector " << sectorDestino << "." << endl;
    cout << "========================================================" << endl;
    
    cout << "\nPresione Enter para continuar..."; cin.ignore(); cin.get();
}

void verEstadoPedidosYEnEspera() {
    cout << "=========================================================" << endl;
    cout << "           ESTADO DE PEDIDOS Y ENVIOS EN SISTEMA         " << endl;
    cout << "=========================================================" << endl;

    // 1. Mostrar Pedidos En Camino
    cout << "\n--- 1. ENVIOS EN CAMINO (EN VIAJE) ---" << endl;
    bool hayEnViaje = false;
    for (int i = 0; i < contadorRepartidores; i++) {
        if (!arregloRepartidores[i].Disponible) {
            hayEnViaje = true;
            cout << "- Repartidor: " << arregloRepartidores[i].Nombre 
                 << " (Placa: " << arregloRepartidores[i].Placa << ")"
                 << " | Cliente C.I: " << arregloRepartidores[i].CedulaClienteAtendido
                 << " | Origen: Sector " << arregloRepartidores[i].Sector
                 << " -> Destino: Sector " << arregloRepartidores[i].SectorDestino << endl;
        }
    }
    if (!hayEnViaje) {
        cout << "No hay repartidores en camino en este momento." << endl;
    }

    // 2. Mostrar Pedidos En Cola de Espera (Por Sectores y Orden FIFO)
    cout << "\n--- 2. PEDIDOS EN COLA DE ESPERA (POR SECTOR - ORDEN FIFO) ---" << endl;
    bool hayColaGeneral = false;
    for (int i = 0; i < contadorSectores; i++) {
        int idSec = listaSectores[i].Id;
        int idx = obtenerIndiceSectorPorId(idSec);
        
        if (idx != -1 && colasEspera[idx].frente != NULL) {
            hayColaGeneral = true;
            cout << "\n[SECTOR " << idSec << " - " << listaSectores[i].Direccion << "]:" << endl;
            
            NodoCola* actual = colasEspera[idx].frente;
            int pos = 1;
            while (actual != NULL) {
                cout << "  Turno " << pos << " -> Cliente C.I: " << actual->CedulaCliente 
                     << " | Destino Solicitado: Sector " << actual->SectorDestino << endl;
                actual = actual->siguiente;
                pos++;
            }
        }
    }
    if (!hayColaGeneral) {
        cout << "No hay clientes esperando en cola en ningun sector." << endl;
    }

    cout << "\n=========================================================" << endl;
    cout << "Presione Enter para continuar..."; cin.ignore(); cin.get();
}

void actualizarUbicacionRepartidor() {
    if (contadorRepartidores == 0) {
        cout << "\nNo hay repartidores registrados." << endl;
        cout << "Presione Enter..."; cin.ignore(); cin.get();
        return;
    }

    char cedulaBuscar[10];
    int idxRepartidor = -1;

    cout << "--- MOVER REPARTIDOR LIBRE DE SECTOR ---" << endl;
    cin.ignore();
    cout << "Ingrese Cedula del Repartidor: ";
    cin.getline(cedulaBuscar, 10);

    for (int i = 0; i < contadorRepartidores; i++) {
        if (strcmp(arregloRepartidores[i].Cedula, cedulaBuscar) == 0) {
            idxRepartidor = i;
            break;
        }
    }

    if (idxRepartidor == -1) {
        cout << "\n[!] Error: Repartidor no encontrado." << endl;
        cout << "Presione Enter..."; cin.get();
        return;
    }

    if (!arregloRepartidores[idxRepartidor].Disponible) {
        cout << "\n[!] El repartidor esta actualmente en un viaje en curso." << endl;
        cout << "Debe finalizar su entrega en 'Confirmar Llegada / Finalizar Viaje'." << endl;
        cout << "Presione Enter..."; cin.get();
        return;
    }

    int nuevoSector;
    cout << "Sector Actual: " << arregloRepartidores[idxRepartidor].Sector << endl;
    cout << "Ingrese ID del Nuevo Sector al que se traslada: ";
    cin >> nuevoSector;

    arregloRepartidores[idxRepartidor].Sector = nuevoSector;

    cout << "\n========================================================" << endl;
    cout << "Ubicacion actualizada. Repartidor " << arregloRepartidores[idxRepartidor].Nombre 
         << " movido al Sector " << nuevoSector << endl;

    // Evaluacion e integracion automatica de la Cola de Espera
    if (hayClientesEnEspera(nuevoSector)) {
        char cedulaCliente[10];
        int sectorDestino;

        desencolarCliente(nuevoSector, cedulaCliente, sectorDestino);

        arregloRepartidores[idxRepartidor].Disponible = false;
        arregloRepartidores[idxRepartidor].SectorDestino = sectorDestino;
        strcpy(arregloRepartidores[idxRepartidor].CedulaClienteAtendido, cedulaCliente);

        cout << "[AUTOMATICO] Se asigno el pedido pendiente del cliente " << cedulaCliente 
             << " hacia Sector " << sectorDestino << "." << endl;
    }
    cout << "========================================================" << endl;

    cout << "\nPresione Enter para continuar..."; cin.ignore(); cin.get();
}

void confirmarLlegadaEnvio() {
    int indicesEnViaje[MAX_REPARTIDORES];
    int contEnViaje = 0;

    for (int i = 0; i < contadorRepartidores; i++) {
        if (!arregloRepartidores[i].Disponible) {
            indicesEnViaje[contEnViaje] = i;
            contEnViaje++;
        }
    }

    if (contEnViaje == 0) {
        cout << "\nNo hay envios en camino actualmente." << endl;
        cout << "Presione Enter para continuar..."; cin.ignore(); cin.get();
        return;
    }

    cout << "--- CONFIRMAR LLEGADA DE ENVIOS EN CURSO ---" << endl;
    for (int i = 0; i < contEnViaje; i++) {
        int idx = indicesEnViaje[i];
        cout << i + 1 << ". Repartidor: " << arregloRepartidores[idx].Nombre 
             << " | Cliente C.I: " << arregloRepartidores[idx].CedulaClienteAtendido
             << " | Destino: Sector " << arregloRepartidores[idx].SectorDestino << endl;
    }

    int seleccion;
    cout << "\nSeleccione el numero de la entrega que acaba de LLEGAR: ";
    cin >> seleccion;

    if (seleccion < 1 || seleccion > contEnViaje) {
        cout << "\n[!] Seleccion invalida." << endl;
        cout << "Presione Enter..."; cin.ignore(); cin.get();
        return;
    }

    int idxElegido = indicesEnViaje[seleccion - 1];

    // 1. Sumar servicio al cliente
    int idxCliente = -1;
    for (int i = 0; i < contadorClientes; i++) {
        if (strcmp(arregloClientes[i].Cedula, arregloRepartidores[idxElegido].CedulaClienteAtendido) == 0) {
            idxCliente = i;
            break;
        }
    }

    if (idxCliente != -1) {
        arregloClientes[idxCliente].Servicios++;
    }

    // 2. Sumar servicio al repartidor y ubicarlo en el sector destino
    int sectorDondeLlego = arregloRepartidores[idxElegido].SectorDestino;

    arregloRepartidores[idxElegido].Servicios++;
    arregloRepartidores[idxElegido].Sector = sectorDondeLlego; // Nueva ubicacion
    arregloRepartidores[idxElegido].Disponible = true;        // Se libera temporalmente
    arregloRepartidores[idxElegido].SectorDestino = -1;
    strcpy(arregloRepartidores[idxElegido].CedulaClienteAtendido, "");

    cout << "\n========================================================" << endl;
    cout << "ENTREGA CONFIRMADA Y FINALIZADA CON EXITO." << endl;
    cout << "El repartidor " << arregloRepartidores[idxElegido].Nombre 
         << " se encuentra ahora libre en el Sector " << sectorDondeLlego << "." << endl;

    // Evaluacion e integracion automatica de la Cola de Espera al desocuparse
    if (hayClientesEnEspera(sectorDondeLlego)) {
        char cedulaClienteEnEspera[10];
        int nuevoSectorDestino;

        desencolarCliente(sectorDondeLlego, cedulaClienteEnEspera, nuevoSectorDestino);

        arregloRepartidores[idxElegido].Disponible = false; // Vuelve a ponerse ocupado
        arregloRepartidores[idxElegido].SectorDestino = nuevoSectorDestino;
        strcpy(arregloRepartidores[idxElegido].CedulaClienteAtendido, cedulaClienteEnEspera);

        cout << "[AUTOMATICO] Habia pedido en cola. Se asigno al cliente " << cedulaClienteEnEspera 
             << " hacia Sector " << nuevoSectorDestino << "." << endl;
    }
    cout << "========================================================" << endl;

    cout << "\nPresione Enter para continuar..."; cin.ignore(); cin.get();
}

void menuServicioDiario() {
    int opServicio;
    do {
        cout << "\033[2J\033[1;1H";
        cout << "----- SERVICIO DIARIO -----" << endl;
        cout << "1. Iniciar Jornada" << endl;
        cout << "2. Solicitar Envio" << endl;
        cout << "3. Ver Estado de Pedidos y Envios en Espera" << endl;
        cout << "4. Mover / Cambiar Sector de Repartidor Libre" << endl;
        cout << "5. Confirmar Llegada / Finalizar Viaje" << endl;
        cout << "6. Volver al Menu Principal" << endl;
        cout << "Seleccione una opcion: ";
        cin >> opServicio;

        switch (opServicio) {
            case 1: iniciarJornada(); break;
            case 2: solicitarEnvio(); break;
            case 3: verEstadoPedidosYEnEspera(); break;
            case 4: actualizarUbicacionRepartidor(); break;
            case 5: confirmarLlegadaEnvio(); break;
            case 6: break;
            default: break;
        }
    } while (opServicio != 6);
}

// ==================== MAIN ====================

int main() {
    inicializarColas();
    cargarSectores();
    cargarClientes();
    cargarRepartidores();

    do {
        cout << "\033[2J\033[1;1H";
        cout << "----- MENU PRINCIPAL -----" << endl;
        cout << "1. Gestion interna" << endl;
        cout << "2. Servicio diario" << endl;
        cout << "3. Salir del programa" << endl;
        cout << "Seleccione una opcion: ";
        cin >> opcionPrincipal;

        switch (opcionPrincipal) {
            case 1: menuGestionInterna(); break;
            case 2: menuServicioDiario(); break;
            case 3:
                cout << "\033[2J\033[1;1H";
                cout << "=== RESPALDANDO DATOS Y GENERANDO REPORTE ===" << endl;
                generarReporteEstadisticas();
                guardarClientes();
                guardarRepartidores();
                guardarSectores();
                cout << "\nDatos y reporte estadistico guardados con exito." << endl;
                break;
            default: break;
        }
    } while (opcionPrincipal != 3);
    return 0;
}