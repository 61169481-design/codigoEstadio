#include <iostream>
#include <string>
#include <sstream>
using namespace std;

const int NUM_PUERTAS = 3;

// ESTRUCTURAS

struct NodoEspectador {
    int id;
    string nombre;
    int puerta;
    string estado;
    NodoEspectador* siguiente;
};

struct NodoHistorial {
    string operacion;
    NodoHistorial* siguiente;
};

struct NodoCola {
    int id;
    NodoCola* siguiente;
};

// VARIABLES GLOBALES

NodoEspectador* cabeza = NULL;
NodoHistorial* historial = NULL;

NodoCola* frente[NUM_PUERTAS + 1];
NodoCola* finalCola[NUM_PUERTAS + 1];

// FUNCIONES AUXILIARES

string convertirNumero(int numero) {
    stringstream ss;
    ss << numero;
    return ss.str();
}

int leerEntero(string mensaje) {
    int numero;

    while (true) {
        cout << mensaje;

        if (cin >> numero) {
            cin.ignore(10000, '\n');
            return numero;
        }

        cout << "Error: Ingrese un numero entero.\n";

        cin.clear();
        cin.ignore(10000, '\n');
    }
}

string leerTexto(string mensaje) {
    string texto;

    do {
        cout << mensaje;
        getline(cin, texto);

        if (texto.empty()) {
            cout << "El campo no puede estar vacio.\n";
        }

    } while (texto.empty());

    return texto;
}
