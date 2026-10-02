#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct Piloto{
 char *nombre;
 char *nacionalidad;
 char *scuderia;
 int vecesCampeon;
};

/*funcion principal*/
struct Piloto **getPilotosNacionalidad(struct Piloto** pilotos, int pLibrePilotos, char* nacionalidad){
    int tam = 0;
    struct Piloto **pilotosNacional = NULL;

    tam = cantidadPilotos(pilotos,pLibrePilotos,nacionalidad);
    if (tam >0){
        /*creamos el arreglo compacto*/
        pilotosNacional = (struct Piloto**)malloc(tam*sizeof(struct Piloto*));
        /*llenamos el arreglo*/
        llenarArregloPilotos(pilotos,pilotosNacional,pLibrePilotos,nacionalidad);
        /*returnar el arreglo compacto de los pilotos de una nacion*/
        return pilotosNacional;
    }
    return NULL;

}
int cantidadPilotos(struct Piloto** pilotos, int pLibrePilotos, char* nacionalidad){
    int i , tam = 0;
    for(i = 0;i < pLibrePilotos;i++){
        if (strcmp(pilotos[i]->nacionalidad,nacionalidad) == 0){
            tam++;
        }
    }
    return tam; 
}

void llenarArregloPilotos(struct Piloto** pilotos, struct Piloto** pilotosNacional, int pLibrePilotos, char* nacionalidad){
    int i, tam = 0; 
    for(i = 0;i < pLibrePilotos;i++){
        if (strcmp(pilotos[i]->nacionalidad,nacionalidad) == 0){
            pilotosNacional[tam] = pilotos[i];
            tam++;  
        }
    }

}


int main(void) {
struct Piloto **pilotos;
int tamArreglo, pLibre=0;


return 0;
 }