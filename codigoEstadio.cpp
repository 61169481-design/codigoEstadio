#include <iostream>
#include <string>
#include <sstream>
using namespace std;

const int NUM_PUERTAS = 3;

// ESTRUCTURAS

struct NodoEspectador
{
    int id;
    string nombre;
    int puerta;
    string estado;
    NodoEspectador *siguiente;
};

struct NodoHistorial
{
    string operacion;
    NodoHistorial *siguiente;
};

struct NodoCola
{
    int id;
    NodoCola *siguiente;
};

// VARIABLES GLOBALES

NodoEspectador *cabeza = NULL;
NodoHistorial *historial = NULL;

NodoCola *frente[NUM_PUERTAS + 1];
NodoCola *finalCola[NUM_PUERTAS + 1];

// FUNCIONES AUXILIARES

string convertirNumero(int numero)
{
    stringstream ss;
    ss << numero;
    return ss.str();
}

int leerEntero(string mensaje)
{
    int numero;

    while (true)
    {
        cout << mensaje;

        if (cin >> numero)
        {
            cin.ignore(10000, '\n');
            return numero;
        }

        cout << "Error: Ingrese un numero entero.\n";

        cin.clear();
        cin.ignore(10000, '\n');
    }
}

string leerTexto(string mensaje)
{
    string texto;

    do
    {
        cout << mensaje;
        getline(cin, texto);

        if (texto.empty())
        {
            cout << "El campo no puede estar vacio.\n";
        }

    } while (texto.empty());

    return texto;
}
NodoEspectador *buscarEspectador(int id)
{
    NodoEspectador *actual = cabeza;

    while (actual != NULL)
    {
        if (actual->id == id)
        {
            return actual;
        }

        actual = actual->siguiente;
    }

    return NULL;
}

bool estaEnCola(int id)
{
    for (int p = 1; p <= NUM_PUERTAS; p++)
    {
        NodoCola *actual = frente[p];

        while (actual != NULL)
        {
            if (actual->id == id)
            {
                return true;
            }

            actual = actual->siguiente;
        }
    }

    return false;
}

// OPERACIONES DE PILA

void agregarAlHistorial(string operacion)
{
    NodoHistorial *nuevo = new NodoHistorial;

    nuevo->operacion = operacion;
    nuevo->siguiente = historial;

    historial = nuevo;
}

void quitarUltimaOperacion()
{
    if (historial == NULL)
    {
        cout << "El historial esta vacio.\n";
        return;
    }

    NodoHistorial *aux = historial;

    cout << "Operacion retirada: " << aux->operacion << endl;

    historial = historial->siguiente;

    delete aux;
}

void mostrarHistorial()
{
    if (historial == NULL)
    {
        cout << "El historial esta vacio.\n";
        return;
    }

    NodoHistorial *actual = historial;

    cout << "\n--- HISTORIAL DE OPERACIONES ---\n";

    int contador = 1;

    while (actual != NULL)
    {
        cout << contador << ". " << actual->operacion << endl;

        actual = actual->siguiente;
        contador++;
    }
}

// OPERACIONES DE LA LISTA

void registrarEspectador()
{
    int id = leerEntero("Ingrese ID: ");

    if (id <= 0)
    {
        cout << "El ID debe ser mayor que cero.\n";
        return;
    }

    if (buscarEspectador(id) != NULL)
    {
        cout << "Este ID ya esta registrado.\n";
        return;
    }

    string nombre = leerTexto("Ingrese nombre: ");

    int puerta = leerEntero("Ingrese puerta (1-3): ");

    if (puerta < 1 || puerta > NUM_PUERTAS)
    {
        cout << "Puerta incorrecta.\n";
        return;
    }

    NodoEspectador *nuevo = new NodoEspectador;

    nuevo->id = id;
    nuevo->nombre = nombre;
    nuevo->puerta = puerta;
    nuevo->estado = "Pendiente";
    nuevo->siguiente = NULL;

    if (cabeza == NULL)
    {
        cabeza = nuevo;
    }
    else
    {
        NodoEspectador *actual = cabeza;

        while (actual->siguiente != NULL)
        {
            actual = actual->siguiente;
        }

        actual->siguiente = nuevo;
    }

    cout << "Espectador registrado correctamente.\n";

    agregarAlHistorial("Registro del espectador ID " + convertirNumero(id));
}