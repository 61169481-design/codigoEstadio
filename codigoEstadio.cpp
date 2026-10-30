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

int main()
{
    for (int p = 0; p <= NUM_PUERTAS; p++)
    {
        frente[p] = NULL;
        finalCola[p] = NULL;
    }

    int opcion;

        do {

        // MENU PRINCIPAL

        cout << "\n================================\n";
        cout << "   SISTEMA DE INGRESO AL ESTADIO\n";
        cout << "================================\n";
        cout << "1. Registrar espectador\n";
        cout << "2. Mostrar espectadores\n";
        cout << "3. Eliminar espectador\n";
        cout << "4. Cambiar estado\n";
        cout << "5. Agregar espectador a fila\n";
        cout << "6. Atender fila\n";
        cout << "7. Mostrar filas\n";
        cout << "8. Mostrar historial\n";
        cout << "9. Quitar ultima operacion del historial\n";
        cout << "0. Salir\n";
        cout << "================================\n";

        opcion = leerEntero("Seleccione una opcion: ");

        cout << endl;

        switch (opcion) {

            case 1:
                registrarEspectador();
                break;

            case 2:
                mostrarEspectadores();
                break;

            case 3:
                eliminarEspectador();
                break;

            case 4:
                cambiarEstado();
                break;

            case 5:
                agregarAFila();
                break;

            case 6:
                quitarDeFila();
                break;

            case 7:
                mostrarFilas();
                break;

            case 8:
                mostrarHistorial();
                break;

            case 9:
                quitarUltimaOperacion();
                break;

            case 0:
                cout << "Saliendo del sistema...\n";
                break;

            default:
                cout << "Opcion incorrecta. Intente nuevamente.\n";
        }

    } while (opcion != 0);

    // LIBERAR MEMORIA

    while (cabeza != NULL) {
        NodoEspectador* aux = cabeza;
        cabeza = cabeza->siguiente;
        delete aux;
    }

    while (historial != NULL) {
        NodoHistorial* aux = historial;
        historial = historial->siguiente;
        delete aux;
    }

    for (int i = 1; i <= NUM_PUERTAS; i++) {
        while (frente[i] != NULL) {
            NodoCola* aux = frente[i];
            frente[i] = frente[i]->siguiente;
            delete aux;
        }
    }

    return 0;
}