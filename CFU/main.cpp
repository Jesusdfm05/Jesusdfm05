#include <iostream>
#include <string>
#include <fstream>

using namespace std;

const int MAX_EVALUACIONES = 6;
const int MAX_MATERIAS = 10;

struct Materia {
    string nombre;
    int unidadesCredito;
    int cantidadEvaluaciones;
    float notas[MAX_EVALUACIONES];
    float porcentajes[MAX_EVALUACIONES];
    float notaFinal100;
    int notaFinal9;
};

void calcularMateria(Materia &m) {
    cout << "\n--- Registrar Materia: " << m.nombre << " ---\n";
    cout << "Unidades de Credito: ";
    cin >> m.unidadesCredito;
    cout << "Cantidad de evaluaciones totales: ";
    cin >> m.cantidadEvaluaciones;

    float porcentajeAcumulado = 0;
    for (int i = 0; i < m.cantidadEvaluaciones; i++) {
        cout << "Porcentaje de la evaluacion " << i + 1 << " (ej. 0.25 para 25%): ";
        cin >> m.porcentajes[i];
        porcentajeAcumulado += m.porcentajes[i];
    }

    int evaluadas;
    cout << "¿Cuantas evaluaciones te han entregado hasta hoy?: ";
    cin >> evaluadas;

    float notaAcumuladaActual = 0;
    for (int i = 0; i < evaluadas; i++) {
        cout << "Nota obtenida en parcial " << i + 1 << " (0-100): ";
        cin >> m.notas[i];
        notaAcumuladaActual += m.notas[i] * m.porcentajes[i];
    }

    if (evaluadas == m.cantidadEvaluaciones - 1) {
        float porcentajeFaltante = m.porcentajes[m.cantidadEvaluaciones - 1];
        float notaNecesaria = (55 - notaAcumuladaActual) / porcentajeFaltante;

        if (notaNecesaria > 100) {
            cout << "Alerta: Necesitas " << notaNecesaria << ". Es imposible pasar.\n";
        } else if (notaNecesaria <= 0) {
            cout << "¡Felicidades! Ya acumulaste los puntos para pasar.\n";
        } else {
            cout << "Para pasar con 55, necesitas sacar minimo: " << notaNecesaria << " en el ultimo parcial.\n";
        }
    }
}

int convertirEscalaUNET(float nota100) {
    if (nota100 < 55) return 1; 
    else if (nota100 >= 55 && nota100 <= 64) return 5;
    else if (nota100 >= 65 && nota100 <= 74) return 6;
    else if (nota100 >= 75 && nota100 <= 84) return 7;
    else if (nota100 >= 85 && nota100 <= 94) return 8;
    else if (nota100 >= 95) return 9;
    return 1;
}

void guardarReporte(Materia lista[], int cantidad) {
    ofstream archivo;
    archivo.open("reporte_cfu.txt", ios::out);

    if (archivo.fail()) return;

    archivo << "=== REPORTE CFU ===\n";
    for (int i = 0; i < cantidad; i++) {
        archivo << "Materia: " << lista[i].nombre << "\n";
        archivo << "Nota Final 100: " << lista[i].notaFinal100 << "\n";
        archivo << "Nota UNET: " << lista[i].notaFinal9 << "\n\n";
    }
    archivo.close();
}

int main() {
    Materia misMaterias[MAX_MATERIAS];
    int totalMaterias = 0;
    int opcion;

    do {
        cout << "\n=== MENU CFU ===\n1. Agregar Materia\n2. Guardar Reporte\n3. Salir\nOpcion: ";
        cin >> opcion;

        if (opcion == 1) {
            if (totalMaterias < MAX_MATERIAS) {
                cout << "Nombre de la materia: ";
                cin.ignore();
                getline(cin, misMaterias[totalMaterias].nombre);
                
                calcularMateria(misMaterias[totalMaterias]);
                totalMaterias++;
            }
        } else if (opcion == 2) {
            guardarReporte(misMaterias, totalMaterias);
        }
    } while (opcion != 3);

    return 0;
}

