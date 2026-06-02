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

    void anadirCliente(){
        if (contadorClientes >= MAX_CLIENTES){
        cout << "\n Ya no caben mas amor asfknaslkfnaslfa" << endl;
        cout << "Presione Enter para continuar...";
        cin.ignore();
        cin.get();
        return;
        }
        
        cout <<"---Nombre del cliente---"<< endl;
        cin >> listaClientes[contadorClientes].Nombre;
        cout <<"---Cedula del cliente---"<< endl;
        cin >> listaClientes[contadorClientes].Cedula;
        cout <<"---N. telefono del cliente---"<< endl;
        cin >> listaClientes[contadorClientes].Nombre;
        cout <<"Presione enter para continuar..." << endl;
        listaClientes[contadorClientes].Servicios = 0;
        contadorClientes++;
        cin.get();

    }
    void consultarCliente(){}

    void menuClientes(){
        int opClientes;
        do{
        cout << "\033[2J\033[1;1H";
        cout <<"-----CLIENTES-----"<< endl;
        cout <<"1.Anadir cliente"<< endl;
        cout <<"2.Consultar cliente"<< endl;
        cout <<"3.Volver al menu anterior"<< endl; 
        cin >> opClientes;

        switch (opClientes){
            case 1: anadirCliente(); 
            break;
            case 2: consultarCliente(); 
            break; 
            case 3: cout << "Saliendo al menu principal..." << endl;
            cout << "\033[2J\033[1;1H";
            break;
            default:
            break;
        }
        } while (opClientes !=3);
        
    }
    void menuRepartidores(){}
    void menuSectores(){}

    void menuGestionInterna(){
        int op;
        do{
        cout << "----- GESTION INTERNA -----" << endl;
        cout << "1. Clientes" << endl;
        cout << "2. Repartidores" << endl;
        cout << "3. Sectores" << endl;
        cout << "4. Volver al Menu Principal" << endl;
        cin >> op;
 
        switch (op){
            case 1: menuClientes(); 
            break;
            case 2: menuRepartidores(); 
            break;
            case 3: menuSectores(); 
            break;  
            case 4: cout << "Saliendo al menu principal..." << endl;
            break; 
            default:
            break;
        }
        } while (op !=4);
    }




int main(){

    do{
        cout << "\033[2J\033[1;1H";
        cout <<"-----MENU PRINCIPAL-----"<< endl;
        cout <<"1. Gestion interna" << endl;
        cout <<"2. Servicio diario" << endl;
        cout <<"3. Salie del programa" << endl;
        cout <<"Seleccione una opcion" << endl;
        cin >> opcionPrincipal;

        switch (opcionPrincipal){
            case 1:
                cout << "Abriendo la gestion interna..." << endl;
                cout << "\033[2J\033[1;1H";
                menuGestionInterna();
            break;

            case 2:
                cout << "Abriendo el servicio diario..." << endl;
                cout << "\033[2J\033[1;1H";
            break;

            case 3:
                cout << "Cerrando el distema..." << endl;
                cout << "\033[2J\033[1;1H";
            break;
        
            default:
                return 0;
            break;
        }
    } while (opcionPrincipal != 3);
    
    



}
