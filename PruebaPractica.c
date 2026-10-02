#include <stdio.h>
#include <stdlib.h>


struct ProgramaSocial {
    char *nombre;
    char *fechaInicio;
    char *tipo;
    float nota;
    int presupuestoRequerido;
};

struct NodoProgramaSocial{
    struct ProgramaSocial *datos;
    struct NodoProgramaSocial *ant, *sig;
};

struct Ministerio {
    char *nombre;
    char *ministro;
    struct NodoProgramaSocial *headProgramas;
};

struct EjecucionPresupuestaria {
    int numero;
    char *fecha;
    int monto;
    struct ProgramaSocial *asignacion;
};

struct PartidaPresupuestaria {
    int numero;
    char *fuenteFinanciamiento;
    int montoAutorizado;
    struct EjecucionPresupuestaria **ejecuciones;
    int pLibreEjecuciones;
};

struct NodoPartidaPresupuestaria {
    struct PartidaPresupuestaria *partida;
    struct NodoPartidaPresupuestaria *sig;
};

struct Gobierno {
    char *presidente;
    struct Ministerio **ministerios;
    int cantMinisterios;
    struct NodoPartidaPresupuestaria *headPartidas;
};

float calcularPromedioProgramasSocialesEnMinisterio(struct NodoProgramaSocial *headProgramas) {
    float suma=0;
    int contador=0;

    while (headProgramas->sig!=NULL) {
        suma+=headProgramas->sig->datos->nota;
        contador++;
        headProgramas=headProgramas->sig;
    }
    if (contador>0)
        return suma/contador;
    return 0;
}

float calcularPromedioTotalProgramasSociales(struct Ministerio **ministerios, int cantMinisterios) {
    float suma=0;
    int contador=0, i;

    for (i=0; i<cantMinisterios; i++) {
        if (ministerios[i]!=NULL) {
            suma+=calcularPromedioProgramasSocialesEnMinisterio(ministerios[i]->headProgramas);
            contador++;
        }
    }
    if (contador>0)
        return suma/contador;
    return 0;
}

int sumaEjecucionPresupuestariaEnPartida(struct EjecucionPresupuestaria **ejecuciones, int pLibreEjecuciones, char *nombrePrograma) {
    int suma=0, i;

    for (i=0; i<pLibreEjecuciones; i++) {
        if (strcmp(ejecuciones[i]->asignacion->nombre,nombrePrograma)==0){
            suma+=ejecuciones[i]->monto;
        }
    }
    return suma;
}

int sumaTotalEjecucionPresupuestariaProgramaSocial(struct NodoPartidaPresupuestaria *headPartida, char *nombrePrograma) {
    int suma=0;
    struct NodoPartidaPresupuestaria *rec;

    if (headPartida!=NULL) {
        rec=headPartida;

        do {
            suma+=sumaEjecucionPresupuestariaEnPartida(rec->partida->ejecuciones,rec->partida->pLibreEjecuciones,nombrePrograma);
            rec=rec->sig;
        }while (rec!=headPartida);
        return suma;
    }
    return 0;
}

int contarProgramasMalEvaluados(struct Gobierno *GobCL) {
    int contador=0, i;
    struct NodoProgramaSocial *rec;
    float promedioCalificacionTotalProgramas=0;
    int totalPresupuestoEjecutado=0;

    promedioCalificacionTotalProgramas=calcularPromedioTotalProgramasSociales(GobCL->ministerios,GobCL->cantMinisterios);

    for (i=0; i<GobCL->cantMinisterios; i++) {
        if (GobCL->ministerios[i]!=NULL) {

            rec=GobCL->ministerios[i]->headProgramas;
            while (rec!=NULL) {
                totalPresupuestoEjecutado=sumaTotalEjecucionPresupuestariaProgramaSocial(GobCL->headPartidas,rec->datos->nombre);

                if (rec->datos->nota<(0.3*(promedioCalificacionTotalProgramas)) && (totalPresupuestoEjecutado>(0.5*(rec->datos->presupuestoRequerido)) && totalPresupuestoEjecutado<(0.75*(rec->datos->presupuestoRequerido))))
                    contador++;

                rec=rec->sig;
            }
        }
    }
    return contador;
}

void desvincularEjecucionesDeProgramas(struct EjecucionPresupuestaria **ejecuciones, int pLibreEjecuciones, char *nombrePrograma) {
    int i;
    for (i=0; i<pLibreEjecuciones; i++) {
        if (strcmp(ejecuciones[i]->asignacion->nombre,nombrePrograma)==0){
            ejecuciones[i]->asignacion=NULL;
        }
    }
}

void desvincularPartidasDeProgramas(struct NodoPartidaPresupuestaria *headPartidas, char *nombrePrograma) {
    struct NodoPartidaPresupuestaria *rec;
    if (headPartidas!=NULL) {
        rec=headPartidas;
        do {
            desvincularEjecucionesDeProgramas(rec->partida->ejecuciones,rec->partida->pLibreEjecuciones,nombrePrograma);
            rec=rec->sig;
        }while (rec!=headPartidas);
    }
}

struct ProgramaSocial *quitarProgramaSocialDeMinisterio(struct NodoProgramaSocial *headProgramas, char *nombrePrograma) {
    struct NodoProgramaSocial *rec;
    struct ProgramaSocial *aux;

    rec=headProgramas;
    while (rec->sig!=NULL) {
        if (strcmp(rec->sig->datos->nombre,nombrePrograma)==0) {
            aux=rec->sig->datos;
            rec->sig->ant=rec->ant;
            rec->sig=rec->sig->sig;
            return aux;
        }
        rec=rec->sig;
    }
    return NULL;
}

struct ProgramaSocial **quitarProgramasMalEvaluados(struct Gobierno *GobCL) {
    struct ProgramaSocial **quitados;
    int cantidad, k, m , totalPresupuestoEjecutado=0;
    struct NodoProgramaSocial *rec;
    float promedioCalificacionTotalProgramas;

    cantidad=contarProgramasMalEvaluados(GobCL);
    if (cantidad>0) {
        promedioCalificacionTotalProgramas=calcularPromedioTotalProgramasSociales(GobCL->ministerios,GobCL->cantMinisterios);
        quitados=(struct ProgramaSocial **) malloc(cantidad*sizeof(struct ProgramaSocial *));
        /*Recorremos cada posicion a llenar*/
        for (k=0; k<cantidad; k++) {

            /*Por cada posicion, recorremos los ministerios buscando los programas a quitar*/
            for (m=0; m<GobCL->cantMinisterios; m++) {
                if (GobCL->ministerios[m]!=NULL) {
                    rec=GobCL->ministerios[m]->headProgramas;

                    while (rec->sig!=NULL) {
                        totalPresupuestoEjecutado=sumaTotalEjecucionPresupuestariaProgramaSocial(GobCL->headPartidas,rec->sig->datos->nombre);
                        if (rec->datos->nota<(0.3*(promedioCalificacionTotalProgramas)) && (totalPresupuestoEjecutado>(0.5*(rec->sig->datos->presupuestoRequerido)) && totalPresupuestoEjecutado<(0.75*(rec->sig->datos->presupuestoRequerido)))) {
                            quitados[k]=quitarProgramaSocialDeMinisterio(GobCL->ministerios[m]->headProgramas,rec->sig->datos->nombre);
                            desvincularPartidasDeProgramas(GobCL->headPartidas,quitados[k]->nombre);
                        }
                        rec=rec->sig;
                    }
                }
            }
        }
        return quitados;
    }
    return NULL;


}




int main(void) {
    printf("Hello, World!\n");
    return 0;
}