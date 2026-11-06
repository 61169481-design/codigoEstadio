#include <iostream>
#include <string>
#include <sstream>
#include <fstream>  
#include <cstdlib>  
using namespace std;

const int NUM_PUERTAS = 3;
const int TAM_TABLA = 101; 

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

// nodo auxiliar para la tabla hash con encadenamiento.
struct NodoHash {
    int id;
    NodoEspectador* espectador;
    NodoHash* siguiente;
};

// VARIABLES GLOBALES
NodoEspectador* inicioLista = NULL; 					// MODIFICADO: antes cabeza.
NodoHistorial* parteEncima = NULL; 						// MODIFICADO: antes historial; cima de la pila.
NodoCola* inicioFila[NUM_PUERTAS + 1]; 						// MODIFICADO: antes frente.
NodoCola* finalFila[NUM_PUERTAS + 1]; 					// MODIFICADO: antes finalCola.
NodoHash* tablaHash[TAM_TABLA]; 						// NUEVO
// el ID se calcula buscando el menor numero libre.

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


//solicita nuevamente hasta recibir un valor dentro del rango.
int leerEnteroRango(string mensaje, int minimo, int maximo) {
    int numero;
    do {
        numero = leerEntero(mensaje);
        if (numero < minimo || numero > maximo) {
            cout << "Valor incorrecto. Ingrese un numero entre "
                 << minimo << " y " << maximo << ".\n";
        }
    } while (numero < minimo || numero > maximo);
    return numero;
}

NodoEspectador* buscarEspectador(int id); // Prototipo necesario.

//busca desde 1 el menor ID que no exista en la tabla hash.
int obtenerIdDisponible() {
    int id = 1;
    while (buscarEspectador(id) != NULL) {
        id++;
    }
    return id;
}

string leerTexto(string mensaje) {
    string texto;
    do {
        cout << mensaje;
        getline(cin, texto);
        if (texto.empty()) cout << "El campo no puede estar vacio.\n";
    } while (texto.empty());
    return texto;
}

// funcion hash que convierte el ID en una posicion de la tabla.
int funcionHash(int id) {
    return id % TAM_TABLA;
}

//inserta un espectador en la tabla hash. Las colisiones
// se resuelven mediante listas enlazadas (encadenamiento).
void insertarHash(NodoEspectador* espectador) {
    int posicion = funcionHash(espectador->id);
    NodoHash* nuevo = new NodoHash;
    nuevo->id = espectador->id;
    nuevo->espectador = espectador;
    nuevo->siguiente = tablaHash[posicion];
    tablaHash[posicion] = nuevo;
}

// elimina el ID de la tabla hash.
void eliminarHash(int id) {
    int posicion = funcionHash(id);
    NodoHash* actual = tablaHash[posicion];
    NodoHash* anterior = NULL;

    while (actual != NULL && actual->id != id) {
        anterior = actual;
        actual = actual->siguiente;
    }
    if (actual != NULL) {
        if (anterior == NULL) tablaHash[posicion] = actual->siguiente;
        else anterior->siguiente = actual->siguiente;
        delete actual;
    }
}
NodoEspectador* buscarEspectador(int id) {
    int posicion = funcionHash(id);
    NodoHash* actual = tablaHash[posicion];
    while (actual != NULL) {
        if (actual->id == id) return actual->espectador;
        actual = actual->siguiente;
    }
    return NULL;
}

bool estaEnCola(int id) {
    for (int p = 1; p <= NUM_PUERTAS; p++) {
        NodoCola* actual = inicioFila[p];
        while (actual != NULL) {
            if (actual->id == id) return true;
            actual = actual->siguiente;
        }
    }
    return false;
}

// OPERACIONES DEL HISTORIAL (PILA)
void agregarAlHistorial(string operacion) {
    NodoHistorial* nuevo = new NodoHistorial;
    nuevo->operacion = operacion;
    nuevo->siguiente = parteEncima;
    parteEncima = nuevo;
}

// Declaracion anticipada: esta funcion se define en persistencia.
void guardarDatos();

void quitarUltimaOperacion() {
    if (parteEncima == NULL) {
        cout << "El parteEncima esta vacio.\n";
        return;
    }
    NodoHistorial* aux = parteEncima;
    cout << "Operacion retirada: " << aux->operacion << endl;
    parteEncima = parteEncima->siguiente;
    delete aux;
    guardarDatos(); 											// guarda el parteEncima actualizado.
}

void mostrarHistorial() {
    if (parteEncima == NULL) {
        cout << "El parteEncima esta vacio.\n";
        return;
    }
    NodoHistorial* actual = parteEncima;
    cout << "\n--- HISTORIAL DE OPERACIONES ---\n";
    int contador = 1;
    while (actual != NULL) {
        cout << contador << ". " << actual->operacion << endl;
        actual = actual->siguiente;
        contador++;
    }
}

// guarda espectadores (E), filas (Q) e parteEncima (H) en texto.

void guardarDatos() {
    ofstream archivo("datos_estadio.txt");
    if (!archivo.is_open()) {
        cout << "Error: no se pudo guardar el archivo.\n";
        return;
    }

    NodoEspectador* e = inicioLista;
    while (e != NULL) {
        archivo << "E;" << e->id << ";" << e->nombre << ";"
                << e->puerta << ";" << e->estado << "\n";
        e = e->siguiente;
    }

    for (int p = 1; p <= NUM_PUERTAS; p++) {
        NodoCola* q = inicioFila[p];
        while (q != NULL) {
            archivo << "Q;" << p << ";" << q->id << "\n";
            q = q->siguiente;
        }
    }

    // El parteEncima se escribe desde el tope hacia abajo.
    NodoHistorial* h = parteEncima;
    while (h != NULL) {
        archivo << "H;" << h->operacion << "\n";
        h = h->siguiente;
    }
    archivo.close();
}

// reconstruye las estructuras leyendo el archivo al iniciar.
void cargarDatos() {
    ifstream archivo("datos_estadio.txt");
    if (!archivo.is_open()) return; 

    string linea;
    while (getline(archivo, linea)) {
        stringstream ss(linea);
        string tipo;
        getline(ss, tipo, ';');

        if (tipo == "E") {
            string datoId, nombre, datoPuerta, estado;
            getline(ss, datoId, ';');
            getline(ss, nombre, ';');
            getline(ss, datoPuerta, ';');
            getline(ss, estado);
            int id = atoi(datoId.c_str());
            int puerta = atoi(datoPuerta.c_str());

            if (id > 0 && puerta >= 1 && puerta <= NUM_PUERTAS && buscarEspectador(id) == NULL) {
                NodoEspectador* nuevo = new NodoEspectador;
                nuevo->id = id;
                nuevo->nombre = nombre;
                nuevo->puerta = puerta;
                nuevo->estado = estado;
                nuevo->siguiente = NULL;
                if (inicioLista == NULL) inicioLista = nuevo;
                else {
                    NodoEspectador* actual = inicioLista;
                    while (actual->siguiente != NULL) actual = actual->siguiente;
                    actual->siguiente = nuevo;
                }
                insertarHash(nuevo);
            }
        }
        else if (tipo == "Q") {
            string datoPuerta, datoId;
            getline(ss, datoPuerta, ';');
            getline(ss, datoId);
            int puerta = atoi(datoPuerta.c_str());
            int id = atoi(datoId.c_str());
            if (puerta >= 1 && puerta <= NUM_PUERTAS && buscarEspectador(id) != NULL && !estaEnCola(id)) {
                NodoCola* nuevo = new NodoCola;
                nuevo->id = id;
                nuevo->siguiente = NULL;
                if (inicioFila[puerta] == NULL) {
                    inicioFila[puerta] = nuevo;
                    finalFila[puerta] = nuevo;
                } else {
                    finalFila[puerta]->siguiente = nuevo;
                    finalFila[puerta] = nuevo;
                }
            }
        }
        else if (tipo == "H") {
            string operacion;
            getline(ss, operacion);
            NodoHistorial* nuevo = new NodoHistorial;
            nuevo->operacion = operacion;
            nuevo->siguiente = NULL;
           
            if (parteEncima == NULL) parteEncima = nuevo;
            else {
                NodoHistorial* actual = parteEncima;
                while (actual->siguiente != NULL) actual = actual->siguiente;
                actual->siguiente = nuevo;
            }
        }
    }
    archivo.close();
}

// OPERACIONES DE LA LISTA
void registrarEspectador() {
    int id = obtenerIdDisponible();
    cout << "ID asignado automaticamente: " << id << endl;
    string nombre = leerTexto("Ingrese nombre: ");
    int puerta = leerEnteroRango("Ingrese puerta (1-3): ", 1, NUM_PUERTAS); // MODIFICADO

    NodoEspectador* nuevo = new NodoEspectador;
    nuevo->id = id;
    nuevo->nombre = nombre;
    nuevo->puerta = puerta;
    nuevo->estado = "Pendiente";
    nuevo->siguiente = NULL;
    if (inicioLista == NULL) inicioLista = nuevo;
    else {
        NodoEspectador* actual = inicioLista;
        while (actual->siguiente != NULL) actual = actual->siguiente;
        actual->siguiente = nuevo;
    }
    insertarHash(nuevo); 
    cout << "Espectador registrado correctamente.\n";
    agregarAlHistorial("Registro del espectador ID " + convertirNumero(id));
    guardarDatos(); 
}

void mostrarEspectadores() {
    if (inicioLista == NULL) {
        cout << "No hay espectadores registrados.\n";
        return;
    }
    NodoEspectador* actual = inicioLista;
    cout << "\n--- LISTA DE ESPECTADORES ---\n";
    while (actual != NULL) {
        cout << "ID: " << actual->id << endl;
        cout << "Nombre: " << actual->nombre << endl;
        cout << "Puerta: " << actual->puerta << endl;
        cout << "Estado: " << actual->estado << endl;
        cout << "--------------------------\n";
        actual = actual->siguiente;
    }
}

void eliminarEspectador() {
    if (inicioLista == NULL) {
        cout << "No hay espectadores registrados.\n";
        return;
    }
    int id = leerEntero("Ingrese el ID a eliminar: ");
    NodoEspectador* actual = inicioLista;
    NodoEspectador* anterior = NULL;
    while (actual != NULL && actual->id != id) {
        anterior = actual;
        actual = actual->siguiente;
    }
    if (actual == NULL) {
        cout << "Espectador no encontrado.\n";
        return;
    }
    if (estaEnCola(id)) {
        cout << "No se puede eliminar: el espectador esta en una fila.\n";
        return;
    }
    if (anterior == NULL) inicioLista = actual->siguiente;
    else anterior->siguiente = actual->siguiente;
    eliminarHash(id); //elimina tambien la entrada hash.
    delete actual;
    cout << "Espectador eliminado correctamente.\n";
    agregarAlHistorial("Eliminacion del espectador ID " + convertirNumero(id));
    guardarDatos(); //persistencia inmediata.
}

void cambiarEstado() {
    int id = leerEntero("Ingrese el ID del espectador: ");
    NodoEspectador* actual = buscarEspectador(id);
    if (actual == NULL) {
        cout << "Espectador no encontrado.\n";
        return;
    }
    if (actual->estado == "Pendiente") actual->estado = "Ingresado";
    else actual->estado = "Pendiente";
    cout << "Nuevo estado: " << actual->estado << endl;
    agregarAlHistorial("Cambio de estado del ID " + convertirNumero(id));
    guardarDatos(); //guarda el estado actualizado.
}

// OPERACIONES DE LAS COLAS
void agregarAFila() {
    int puerta = leerEnteroRango("Ingrese puerta (1-3): ", 1, NUM_PUERTAS); // MODIFICADO

    int id;
    NodoEspectador* espectador;

    // vuelve a pedir el ID hasta encontrar un espectador registrado.
    do {
        id = leerEntero("Ingrese ID del espectador: ");
        espectador = buscarEspectador(id);
        if (espectador == NULL) {
            cout << "Ese ID no esta registrado. Intente nuevamente.\\n";
        }
    } while (espectador == NULL);
    if (espectador->estado == "Ingresado") {
        cout << "El espectador ya ingreso al estadio.\n";
        return;
    }
    if (estaEnCola(id)) {
        cout << "El espectador ya esta en una fila.\n";
        return;
    }
    NodoCola* nuevo = new NodoCola;
    nuevo->id = id;
    nuevo->siguiente = NULL;
    if (inicioFila[puerta] == NULL) {
        inicioFila[puerta] = nuevo;
        finalFila[puerta] = nuevo;
    } else {
        finalFila[puerta]->siguiente = nuevo;
        finalFila[puerta] = nuevo;
    }
    cout << "Espectador agregado a la fila de la puerta " << puerta << ".\n";
    agregarAlHistorial("Ingreso a fila del ID " + convertirNumero(id));
    guardarDatos(); //guarda el contenido de las filas.
}

void quitarDeFila() {
    int puerta = leerEnteroRango("Ingrese puerta (1-3): ", 1, NUM_PUERTAS); // MODIFICADO
    if (inicioFila[puerta] == NULL) {
        cout << "No hay espectadores en esta fila.\n";
        return;
    }
    NodoCola* aux = inicioFila[puerta];
    int id = aux->id;
    inicioFila[puerta] = inicioFila[puerta]->siguiente;
    if (inicioFila[puerta] == NULL) finalFila[puerta] = NULL;
    delete aux;
    NodoEspectador* espectador = buscarEspectador(id);
    if (espectador != NULL) espectador->estado = "Ingresado";
    cout << "Espectador ID " << id << " ingreso al estadio.\n";
    agregarAlHistorial("Atencion del ID " + convertirNumero(id));
    guardarDatos(); // guarda la cola y el estado actualizado.
}

void mostrarFilas() {
    cout << "\n--- FILAS POR PUERTA ---\n";
    for (int p = 1; p <= NUM_PUERTAS; p++) {
        cout << "Puerta " << p << ": ";
        NodoCola* actual = inicioFila[p];
        if (actual == NULL) cout << "Fila vacia";
        while (actual != NULL) {
            cout << "[" << actual->id << "] ";
            actual = actual->siguiente;
        }
        cout << endl;
    }
}

// PROGRAMA PRINCIPAL
int main() {
    for (int i = 1; i <= NUM_PUERTAS; i++) {
        inicioFila[i] = NULL;
        finalFila[i] = NULL;
    }
    for (int i = 0; i < TAM_TABLA; i++) {
        tablaHash[i] = NULL; // inicializa la tabla hash.
    }

    cargarDatos(); // carga el archivo al iniciar el programa.

    int opcion = -1;
    do {
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
        cout << "8. Mostrar parteEncima\n";
        cout << "9. Quitar ultima operacion del parteEncima\n";
        cout << "0. Salir\n";
        cout << "================================\n";
        opcion = leerEntero("Seleccione una opcion: ");
        cout << endl;
        switch (opcion) {
            case 1: registrarEspectador(); break;
            case 2: mostrarEspectadores(); break;
            case 3: eliminarEspectador(); break;
            case 4: cambiarEstado(); break;
            case 5: agregarAFila(); break;
            case 6: quitarDeFila(); break;
            case 7: mostrarFilas(); break;
            case 8: mostrarHistorial(); break;
            case 9: quitarUltimaOperacion(); break;
            case 0: cout << "Saliendo del sistema...\n"; break;
            default: cout << "Opcion incorrecta. Intente nuevamente.\n";
        }
    } while (opcion != 0);

    guardarDatos(); // ultimo guardado antes de cerrar.

    // LIBERAR MEMORIA 
    while (inicioLista != NULL) {
        NodoEspectador* aux = inicioLista;
        inicioLista = inicioLista->siguiente;
        delete aux;
    }
    while (parteEncima != NULL) {
        NodoHistorial* aux = parteEncima;
        parteEncima = parteEncima->siguiente;
        delete aux;
    }
    for (int i = 1; i <= NUM_PUERTAS; i++) {
        while (inicioFila[i] != NULL) {
            NodoCola* aux = inicioFila[i];
            inicioFila[i] = inicioFila[i]->siguiente;
            delete aux;
        }
    }
    for (int i = 0; i < TAM_TABLA; i++) {
        while (tablaHash[i] != NULL) {
            NodoHash* aux = tablaHash[i];
            tablaHash[i] = tablaHash[i]->siguiente;
            delete aux;
        }
    }
    return 0;
}