#include <stdio.h>
#include <stdlib.h>

struct Estacion {
    int numero;
    char *direccion;
    int cantidadServicios;
    float valoresCombustible[5];
};

struct NodoEstacion {
    struct Estacion *estacion;
    struct NodoEstacion *sig;
};

struct Carga {
    int numero;
    char *fecha;
    float litros;
    char *tipoCombustible;
    int valorPagado;
    char *medioPago;
    struct Estacion *estacionCarga;
};

struct Cliente {
    char *rut;
    char *nombre;
    struct Carga **cargas; //Compacto sin pLibre
    int maxCargas;
};

struct NodoCliente {
    struct Cliente *cliente;
    struct NodoCliente *sig;
};

struct Gasolinera {
    char *nombreEmpresa;
    struct NodoCliente *headClientes;
    struct NodoEstacion *headEstaciones;
};


 // 3. Dado el rut de un cliente, determinar cual es el combustible que más carga.
 // 4. Determinar la estacion que tiene mas demanda
 // 5. Determinar la estacion que mas dinero acumula

// 1. Dado un numero de estacion, determinar cual es el monto acumulado en cargas

int buscarCarga(struct Carga **cargas, int maxCargas, int numeroCarga) {
    int i;

    for (i=0; i<maxCargas; i++) {
        if (cargas[i]->numero == numeroCarga)
            return i;
    }
    return -1;
}

int montoAcumuladoCargasDeEstacionPorCliente(struct Carga **cargas, int maxCargas, int numeroEstacion) {
    int i, suma=0;

    for (i=0; i<maxCargas && cargas[i]!=NULL; i++) {
            if (cargas[i]->estacionCarga->numero == numeroEstacion) {
                suma+=cargas[i]->valorPagado;
            }
    }
    return suma;
}

int sumaMontosCargasTodosClientes(struct NodoCliente *headClientes, int numeroEstacion) {
    int suma=0;
    struct NodoCliente *rec;
    rec=headClientes;

    while (rec!=NULL) {
        suma+=montoAcumuladoCargasDeEstacionPorCliente(rec->cliente->cargas,rec->cliente->maxCargas,numeroEstacion);
        rec=rec->sig;
    }

    return suma;
}

//buscar estacion
struct Estacion *buscarEstacion(struct NodoEstacion *headEstaciones, int numeroEstacion) {
    struct NodoEstacion *rec;

    if (headEstaciones!=NULL) {
        rec=headEstaciones;
        do {
            if (rec->estacion->numero==numeroEstacion)
                return rec->estacion;
            rec=rec->sig;
        }while (rec!=headEstaciones);
    }
    return NULL;
}

int montoAcumuladoEstacion(struct Gasolinera *gasolinera, int numeroEstacion) {
    if (buscarEstacion(gasolinera->headEstaciones,numeroEstacion)!=NULL) {
        return sumaMontosCargasTodosClientes(gasolinera->headClientes,numeroEstacion);
    }
    return -1;
}

// 2. Quitar una estacion, para ello, se deben quitar todas las ventas,cargas realizadas



// compactar arreglo por cada cliente
void compactarCargas(struct Carga **cargas, int maxCargas) {
    int i, j;

    for (i=0; i<maxCargas; i++) {
        if (cargas[i]==NULL) {
            for (j=i; j<maxCargas-1; j++) {
                cargas[j]=cargas[j+1];
            }
            cargas[maxCargas-1]=NULL;
            i--;
        }
    }
}

// quitar cargas y
void quitarCargasEnCliente(struct Carga **cargas, int maxCargas, int numeroEstacion) {
    int i;
    for (i=0; i<maxCargas && cargas[i]!=NULL; i++) {
        if (cargas[i]->numero == numeroEstacion)
            cargas[i]=NULL;
    }
    compactarCargas(cargas,maxCargas);
}

// quitar cargas de todos los clientes
void quitarCargas(struct NodoCliente *headCliente, int numeroEstacion) {

    while (headCliente!=NULL) {
        quitarCargasEnCliente(headCliente->cliente->cargas,headCliente->cliente->maxCargas,numeroEstacion);
        headCliente=headCliente->sig;
    }
}

// quitar estacion
struct Estacion *quitarEstacion(struct Gasolinera *gasolinera, int numeroEstacion) {
    struct NodoEstacion *rec;
    struct Estacion *quitada;

    /*Como es una lista circular debemos validar que el head exista porque usaremos un do-while*/
    /*Aprovechamos de inmediato validar si existe la estacion que debemos eliminar*/
    if (gasolinera->headClientes!=NULL && buscarEstacion(gasolinera->headEstaciones,numeroEstacion)!=NULL) {
 
        /*Primero quitaremos las cargas asociadas a dicha estación*/
        quitarCargas(gasolinera->headClientes,numeroEstacion);

        /*Ahora procederemos a la eliminación*/
        /*Caso en que sea el head*/
        if (gasolinera->headEstaciones->estacion->numero == numeroEstacion) {
            /*Buscamos el ultimo*/
            rec=gasolinera->headEstaciones;
            quitada=gasolinera->headEstaciones->estacion;
            do {
                rec=rec->sig;
            }while (rec->sig!=gasolinera->headEstaciones);

            /*El rec está posicionado en el ultimo, ahora hacemos el movimiento*/
            rec->sig=rec->sig->sig;
            gasolinera->headEstaciones=gasolinera->headEstaciones->sig;
            return quitada;
        }
        else {
            /*Caso que no es el head*/
            rec=gasolinera->headEstaciones;
            do {
                if(rec->sig->estacion->numero == numeroEstacion) {
                    quitada=rec->sig->estacion;
                    rec->sig=rec->sig->sig;
                    return quitada;
                }
                rec=rec->sig;
            }while (rec!=gasolinera->headEstaciones);
        }
    }
    return NULL;
}





int main(void) {
    printf("Hello, World!\n");
    return 0;
}