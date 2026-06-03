// S/C julio de 2026
// Jesus David Florez Morales
//  V-31.762.806

#include <iostream>
#include <cstring>
#include <fstream>
using namespace std;

struct Sector{
    int Id;
    char Direccion[20];
};

struct Cliente{
    char Cedula[10];
    char Telefono[15];
    char Nombre[30];
    int Servicios;
};

struct Repartidor{
    char Cedula[10];
    char Nombre[30];
    char Vehiculo[20];
    char Placa[10];
    int Sector;
    int Servicios;
    bool Disponible;
};

const int MAX_CLIENTES = 50;
const int MAX_REPARTIDORES = 50;
const int MAX_SECTORES = 20;

int contadorClientes = 0;
int contadorRepartidores = 0;
int contadorSectores = 0;

int opcionPrincipal;

Cliente listaClientes[MAX_CLIENTES];
Repartidor listaRepartidores[MAX_REPARTIDORES];
Sector listaSectores[MAX_SECTORES];

void guardarSectores() {
    ofstream archivo("sectores.txt");
    
    if (!archivo.is_open()) {
        cout << "Error critico: No se pudo guardar sectores.txt" << endl;
        return;
    }
    
    for (int i = 0; i < contadorSectores; i++) {
        archivo << listaSectores[i].Id << "|"
                << listaSectores[i].Direccion << endl;
    }
    archivo.close();
}

void guardarClientes() {
    ofstream archivo("clientes.txt");
    
    if (!archivo.is_open()) {
        cout << "Error critico: No se pudo guardar clientes.txt" << endl;
        return;
    }
    
    for (int i = 0; i < contadorClientes; i++) {
        archivo << listaClientes[i].Cedula << "|"
                << listaClientes[i].Telefono << "|"
                << listaClientes[i].Nombre << "|"
                << listaClientes[i].Servicios << endl;
    }
    archivo.close();
}

void guardarRepartidores() {
    ofstream archivo("repartidores.txt");
    
    if (!archivo.is_open()) {
        cout << "Error critico: No se pudo guardar repartidores.txt" << endl;
        return;
    }
    
    for (int i = 0; i < contadorRepartidores; i++) {
        archivo << listaRepartidores[i].Cedula << "|"
                << listaRepartidores[i].Nombre << "|"
                << listaRepartidores[i].Vehiculo << "|"
                << listaRepartidores[i].Placa << "|"
                << listaRepartidores[i].Sector << "|"
                << listaRepartidores[i].Servicios << "|"
                // Convertimos el bool a entero (1 o 0) para el archivo de texto
                << (listaRepartidores[i].Disponible ? 1 : 0) << endl;
    }
    archivo.close();
}


void anadirCliente(){
    if (contadorClientes >= MAX_CLIENTES){
        cout << "\nYa no caben mas clientes!" << endl;
        cout << "Presione Enter para continuar...";
        cin.ignore();
        cin.get();
        return;
    }
    
    cin.ignore(); 
    cout <<"---Nombre del cliente---"<< endl;
    cin.getline(listaClientes[contadorClientes].Nombre, 30);
    
    cout <<"---Cedula del cliente---"<< endl;
    cin.getline(listaClientes[contadorClientes].Cedula, 10);
    
    cout <<"---N. telefono del cliente---"<< endl;
    cin.getline(listaClientes[contadorClientes].Telefono, 15); 
    
    listaClientes[contadorClientes].Servicios = 0;
    contadorClientes++;
    cout << "\nCliente registrado con exito!" << endl;
    cout << "Presione enter para continuar..." << endl;
    cin.get();
}

void consultarCliente(){
    char cedulaBuscar[10];
    bool encontrado = false;
    
    cout << "--- Ingrese la Cedula del Cliente a Consultar ---" << endl;
    cin.ignore();
    cin.getline(cedulaBuscar, 10);
    
    for(int i = 0; i < contadorClientes; i++){
        if(strcmp(listaClientes[i].Cedula, cedulaBuscar) == 0){
            cout << "\n--- DATOS DEL CLIENTE ---" << endl;
            cout << "Nombre: " << listaClientes[i].Nombre << endl;
            cout << "Cedula: " << listaClientes[i].Cedula << endl;
            cout << "Telefono: " << listaClientes[i].Telefono << endl;
            cout << "Servicios Solicitados: " << listaClientes[i].Servicios << endl;
            encontrado = true;
            break;
        }
    }
    
    if(!encontrado){
        cout << "\nCliente no registrado en el sistema." << endl;
    }
    cout << "\nPresione Enter para continuar...";
    cin.get();
}

void eliminarCliente(){
    if (contadorClientes == 0){ 
        cout << "\nNo hay clientes registrados en el sistema." << endl;
        cout << "Presione Enter para continuar...";
        cin.ignore();
        cin.get();
        return;
    }

    char cedulaBuscar[10];
    bool encontrado = false;
    int indiceEliminar = -1;

    cout << "--- Ingrese la Cedula del Cliente a Eliminar ---" << endl;
    cin.ignore();
    cin.getline(cedulaBuscar, 10);

    for (int i = 0; i < contadorClientes; i++) {
        if (strcmp(listaClientes[i].Cedula, cedulaBuscar) == 0) {
            encontrado = true;
            indiceEliminar = i;
            break;
        }
    }

    if (encontrado) {
        for (int i = indiceEliminar; i < contadorClientes - 1; i++) {
            listaClientes[i] = listaClientes[i + 1];
        }
        contadorClientes--;
        
        cout << "\nCliente eliminado de forma exitosa del sistema." << endl;
    } else {
        cout << "\nError: La cedula ingresada no coincide con ningun cliente." << endl;
    }
    cout << "Presione Enter para continuar...";
    cin.get();
}

void menuClientes(){
    int opClientes;
    do{
        cout << "\033[2J\033[1;1H";
        cout <<"----- MENU CLIENTES -----"<< endl;
        cout <<"1. Anadir cliente"<< endl;
        cout <<"2. Consultar cliente"<< endl;
        cout <<"3. Eliminar cliente" <<  endl;
        cout <<"4. Volver al menu anterior"<< endl; 
        cin >> opClientes;

        switch (opClientes){
            case 1: anadirCliente(); break;
            case 2: consultarCliente(); break;
            case 3: eliminarCliente(); break; 
            case 4: cout << "Saliendo al menu de gestion..." << endl; break;
            default: break;
        }
    } while (opClientes != 4);
}


void anadirRepartidor(){
    if (contadorRepartidores >= MAX_REPARTIDORES){
        cout << "\nCapacidad de repartidores al maximo!" << endl;
        cout << "Presione Enter para continuar...";
        cin.ignore();
        cin.get();
        return;
    }
    
    cin.ignore();
    cout <<"---Nombre del Repartidor---"<< endl;
    cin.getline(listaRepartidores[contadorRepartidores].Nombre, 30);
    
    cout <<"---Cedula---"<< endl;
    cin.getline(listaRepartidores[contadorRepartidores].Cedula, 10);
    
    cout <<"---Vehiculo---"<< endl;
    cin.getline(listaRepartidores[contadorRepartidores].Vehiculo, 20);
    
    cout <<"---Placa---"<< endl;
    cin.getline(listaRepartidores[contadorRepartidores].Placa, 10);
    
    cout <<"---ID del Sector Asignado (Numero)---"<< endl;
    cin >> listaRepartidores[contadorRepartidores].Sector;
    
    listaRepartidores[contadorRepartidores].Servicios = 0;
    listaRepartidores[contadorRepartidores].Disponible = true;
    
    contadorRepartidores++;
    cout << "\nRepartidor registrado con exito!" << endl;
    cout << "Presione enter para continuar..." << endl;
    cin.ignore();
    cin.get();
}

void consultarRepartidor(){
    char cedulaBuscar[10];
    bool encontrado = false;
    
    cout << "--- Ingrese la Cedula del Repartidor a Consultar ---" << endl;
    cin.ignore();
    cin.getline(cedulaBuscar, 10);
    
    for(int i = 0; i < contadorRepartidores; i++){
        if(strcmp(listaRepartidores[i].Cedula, cedulaBuscar) == 0){
            cout << "\n--- DATOS DEL REPARTIDOR ---" << endl;
            cout << "Nombre: " << listaRepartidores[i].Nombre << endl;
            cout << "Cedula: " << listaRepartidores[i].Cedula << endl;
            cout << "Vehiculo: " << listaRepartidores[i].Vehiculo << " | Placa: " << listaRepartidores[i].Placa << endl;
            cout << "Sector Asignado: " << listaRepartidores[i].Sector << endl;
            cout << "Servicios Realizados: " << listaRepartidores[i].Servicios << endl;
            cout << "Estado: " << (listaRepartidores[i].Disponible ? "Disponible" : "En ruta / No disponible") << endl;
            encontrado = true;
            break;
        }
    }
    
    if(!encontrado){
        cout << "\nRepartidor no encontrado." << endl;
    }
    cout << "\nPresione Enter para continuar...";
    cin.get();
}

void eliminarRepartidor(){
    if (contadorRepartidores == 0){ 
        cout << "\nNo hay repartidores registrados en el sistema." << endl;
        cout << "Presione Enter para continuar...";
        cin.ignore();
        cin.get();
        return;
    }

    char cedulaBuscar[10];
    bool encontrado = false;
    int indiceEliminar = -1;

    cout << "--- Ingrese la Cedula del repartidor a Eliminar ---" << endl;
    cin.ignore();
    cin.getline(cedulaBuscar, 10);

    for (int i = 0; i < contadorRepartidores; i++) {
        if (strcmp(listaRepartidores[i].Cedula, cedulaBuscar) == 0) {
            encontrado = true;
            indiceEliminar = i;
            break;
        }
    }

    if (encontrado) {
        for (int i = indiceEliminar; i < contadorRepartidores - 1; i++) {
            listaRepartidores[i] = listaRepartidores[i + 1];
        }
        contadorRepartidores--;
        
        cout << "\Repartidor eliminado de forma exitosa del sistema" << endl;
    } else {
        cout << "\nError: La cedula ingresada no coincide con ningun repartidor" << endl;
    }
    cout << "Presione Enter para continuar...";
    cin.get();
}

void menuRepartidores(){
    int opRep;
    do{
        cout << "\033[2J\033[1;1H";
        cout <<"----- MENU REPARTIDORES -----"<< endl;
        cout <<"1. Anadir repartidor"<< endl;
        cout <<"2. Consultar repartidor"<< endl;
        cout <<"3. Eliminar repartidor"<< endl;
        cout <<"4. Volver al menu anterior"<< endl; 
        cin >> opRep;

        switch (opRep){
            case 1: anadirRepartidor(); break;
            case 2: consultarRepartidor(); break; 
            case 3: consultarRepartidor(); break; 
            case 4: cout << "Saliendo al menu de gestion..." << endl; break;
            default: break;
        }
    } while (opRep != 4);
}


void anadirSector(){
    if (contadorSectores >= MAX_SECTORES){
        cout << "\nLimite de sectores alcanzado!" << endl;
        cout << "Presione Enter para continuar...";
        cin.ignore();
        cin.get();
        return;
    }
    
    cout <<"--- ID del Sector (Numero) ---"<< endl;
    cin >> listaSectores[contadorSectores].Id;
    
    cin.ignore();
    cout <<"--- Direccion/Nombre del Sector ---"<< endl;
    cin.getline(listaSectores[contadorSectores].Direccion, 20);
    
    contadorSectores++;
    cout << "\nSector registrado de forma exitosa!" << endl;
    cout << "Presione enter para continuar..." << endl;
    cin.get();
}

void consultarSectores(){
    cout << "\n--- SECTORES REGISTRADOS ---" << endl;
    if(contadorSectores == 0){
        cout << "No hay sectores cargados en el sistema." << endl;
    } else {
        for(int i = 0; i < contadorSectores; i++){
            cout << "ID: " << listaSectores[i].Id << " | Direccion: " << listaSectores[i].Direccion << endl;
        }
    }
    cout << "\nPresione Enter para continuar...";
    cin.ignore();
    cin.get();
}

void menuSectores(){
    int opSec;
    do{
        cout << "\033[2J\033[1;1H";
        cout <<"----- MENU SECTORES -----"<< endl;
        cout <<"1. Anadir sector"<< endl;
        cout <<"2. Ver todos los sectores"<< endl;
        cout <<"3. Volver al menu anterior"<< endl; 
        cin >> opSec;

        switch (opSec){
            case 1: anadirSector(); break;
            case 2: consultarSectores(); break; 
            case 3: cout << "Saliendo al menu de gestion..." << endl; break;
            default: break;
        }
    } while (opSec != 3);
}


void menuGestionInterna(){
    int op;
    do{
        cout << "\033[2J\033[1;1H";
        cout << "----- GESTION INTERNA -----" << endl;
        cout << "1. Clientes" << endl;
        cout << "2. Repartidores" << endl;
        cout << "3. Sectores" << endl;
        cout << "4. Volver al Menu Principal" << endl;
        cin >> op;
 
        switch (op){
            case 1: menuClientes(); break;
            case 2: menuRepartidores(); break;
            case 3: menuSectores(); break;  
            case 4: cout << "Saliendo al menu principal..." << endl; break;
            default: break;
        }
    } while (op != 4);
}


int main(){
    do{
        cout << "\033[2J\033[1;1H";
        cout <<"-----MENU PRINCIPAL-----"<< endl;
        cout <<"1. Gestion interna" << endl;
        cout <<"2. Servicio diario" << endl;
        cout <<"3. Salir del programa" << endl;
        cout <<"Seleccione una opcion: ";
        cin >> opcionPrincipal;

        switch (opcionPrincipal){
            case 1:
                menuGestionInterna();
                break;
            case 2:
                cout << "Abriendo el servicio diario..." << endl;
                cin.ignore(); cin.get();
                break;
            case 3:
                cout << "\033[2J\033[1;1H";
                cout << "=== RESPALDANDO HARDWARE DE DATOS ===" << endl;
                cout << "Guardando clientes..." << endl;
                guardarClientes();
                
                cout << "Guardando repartidores..." << endl;
                guardarRepartidores();
                
                cout << "Guardando sectores de cobertura..." << endl;
                guardarSectores();
                
                cout << "\nDatos guardados con exito en el disco duro" << endl;
                cout << "Cerrando el sistema SpeedDelivery..." << endl;
            break;
            default:
                break;
        }
    } while (opcionPrincipal != 3);
    return 0;
}
