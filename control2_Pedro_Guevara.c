#include <stdio.h>
#include <stdlib.h>


#define BLOQUES 3

struct Producto {
 char *nombre;
 int precioUnitario;
 int unidadesVendidas[BLOQUES];
};

struct NodoProducto {
 struct Producto *producto;
 struct NodoProducto *sig;
};

struct Puesto {
 char *nombre;
 char *categoria;
 struct NodoProducto *productos;
};

struct NodoPuesto {
 struct Puesto *puesto;
 struct NodoPuesto *sig;
};

struct Fonda {
 char *nombre;
 int numero;
 struct NodoPuesto *puestos;
 struct Fonda *sig;
};

struct Evento {
 char *nombre;
 struct Fonda *fondas;
};



int validarPuesto(struct Puesto *puesto){
    struct NodoProducto * rec;
    int bloque1 = 0, bloque2 = 0, bloque3 = 0;
    if( puesto->productos == NULL){
        return 0;
    }
    
    rec=puesto->productos;
    while(rec != NULL){
        if(rec->producto != NULL){
            bloque1 += rec->producto->unidadesVendidas[0];
            bloque2 += rec->producto->unidadesVendidas[1];
            bloque3 += rec->producto->unidadesVendidas[2];
        }
        rec=rec->sig;
    }
    if(bloque1 > 0 && bloque2 > 0 && bloque3 > 0){
        return 1;
    }

    return  0;
}

int validacionFonda(struct Fonda *fonda){
    struct NodoPuesto *rec;
    if(fonda->puestos == NULL  ){
        return 0;
    }
    rec = fonda->puestos;
    while(rec != NULL){
        if (!(validarPuesto(rec->puesto))){
            return 0;
        }
        rec = rec->sig;
    }
    return 1;
}



int recaudacionPuesto(struct Puesto *puesto){
    int recaudacion=0, i;
    struct NodoProducto * rec;
    if(puesto->productos == NULL){
        return 0;
    }
    
    rec=puesto->productos;

    while(rec != NULL){
        if(rec->producto !=NULL){
            for(i=0;i<BLOQUES;i++){
            recaudacion += rec->producto->unidadesVendidas[i] * rec->producto->precioUnitario;
            }
        }
        rec=rec->sig;
    }
   return recaudacion;
}

int recaudacionFonda(struct Fonda *fonda){
    int recaudacion=0;
    struct NodoPuesto *rec;
    rec = fonda->puestos;
    while(rec != NULL){
        recaudacion += recaudacionPuesto(rec->puesto);
        rec = rec->sig;
    }
    return recaudacion; 
}



struct Fonda *fondaDestacada18(struct Evento *evento){  
    struct Fonda *rec;
    struct Fonda *fondaCampeona = NULL;
    int valido,recaudacion,recaudacionMax= 0;
    if(evento == NULL || evento->fondas == NULL)
        return NULL;
    rec=evento->fondas;

    while(rec!=NULL){

        valido = validacionFonda(rec);
        if(valido == 1){
            recaudacion = recaudacionFonda(rec);
            if(recaudacion > recaudacionMax){
                recaudacionMax=recaudacion;
                fondaCampeona = rec;
            }
        }
        rec=rec->sig;
    }
    if(fondaCampeona != NULL){
        return fondaCampeona;
    }
    return NULL;

}
// comentario extra tome como supuesto el caso de que la fonda con mas recaudacion fuera cero como caso no valido para el return
// que seria vender productos de precio 0 es lo mismo que haber recaudado 0
// por lo que no llega a diferenciarse entre vender algo a nada por ello recaudacionMax parte de cero no se si 
// hace mi programa menos escalable , robusto o eficiente que haber usado recaudacion Max igual a menos 1 o NULL fondaCampeona
