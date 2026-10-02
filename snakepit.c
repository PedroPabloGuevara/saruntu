#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <string.h>

#define ANCHO         30
#define ALTO          20
#define MAX_CUERPO    500
#define VELOCIDAD_INI 150
#define VELOCIDAD_RAP 80


typedef struct {
    int x;
    int y;
} Punto;

typedef struct {
    Punto cuerpo[MAX_CUERPO];
    int   largo;
    char  direccion;
} Serpiente;

typedef struct {
    Serpiente serpiente;
    Punto     comida;
    Punto     colaAnterior;   // para borrar la cola sin redibujar todo 
    int       puntaje;
    int       nivel;
    int       vivo;
    int       primerDibujo;   // flag: 1=dibujar todo, 0=solo diferencias 
} EstadoJuego;


void pantallaBienvenida(EstadoJuego *);
void iniciarJuego(EstadoJuego *);
void dibujarTodo(EstadoJuego *);
void dibujarDelta(EstadoJuego *);
void leerTecla(EstadoJuego *);
void moverSerpiente(EstadoJuego *);
int  verificarColision(EstadoJuego *);
void verificarComida(EstadoJuego *);
void spawnComida(EstadoJuego *);
void pantallaGameOver(EstadoJuego *);
void ocultarCursor(void);
void moverCursor(int , int );
void dibujarCaracter(int , int , char );
void registrarPartida(EstadoJuego *);

int main(void) {
    EstadoJuego ej;
    srand((unsigned int)time(NULL));

    pantallaBienvenida(&ej);
    iniciarJuego(&ej);
    ocultarCursor();
    system("cls");       // limpiar pantalla de bienvenida antes de jugar 
    dibujarTodo(&ej);   // primer frame completo 

    while (ej.vivo) {
        leerTecla(&ej);

        // guardar cola antes de mover (para borrarla despues) 
        ej.colaAnterior = ej.serpiente.cuerpo[ej.serpiente.largo - 1];

        moverSerpiente(&ej);

        if (verificarColision(&ej)) {
            ej.vivo = 0;
            break;
        }

        verificarComida(&ej);
        dibujarDelta(&ej);   // solo redibujar lo que cambio 

        Sleep(ej.nivel == 1 ? VELOCIDAD_INI : VELOCIDAD_RAP);
    }

    pantallaGameOver(&ej);
    registrarPartida(&ej);
    return 0;
}
void registrarPartida(EstadoJuego *ej) {

    FILE *archivo = fopen("historial_partidas.txt", "a");
    
    if (archivo == NULL) {
        printf("\n  Error No se pudo guardar el historial de la partida.\n");
        return;
    }


    fprintf(archivo, "Nivel: %d | Puntaje: %-4d | Largo Final: %-3d\n",ej->nivel, ej->puntaje, ej->serpiente.largo);
    printf("Se creo o actualizo el historial de partidas\n");

    fclose(archivo);
}


void ocultarCursor(void) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO ci;
    ci.dwSize   = 1;
    ci.bVisible = FALSE;
    SetConsoleCursorInfo(h, &ci);
}

void moverCursor(int x, int y) {
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}


void dibujarCaracter(int gx, int gy, char c) {
  
    moverCursor(3 + gx, 1 + gy);
    if      (c == '@') printf("\033[32m@\033[0m");
    else if (c == 'o') printf("\033[92mo\033[0m");
    else if (c == '*') printf("\033[31m*\033[0m");
    else               printf(" ");
    //esto fue lo mas dificil de lo que creia no entendi por que no pude usar los simbolos ascii como ▓ para hacer mi serpiente
}

void dibujarTodo(EstadoJuego *ej) {
    int x, y, i;
    char tablero[ALTO][ANCHO];

    for (y = 0; y < ALTO; y++)
        for (x = 0; x < ANCHO; x++)
            tablero[y][x] = ' ';

    tablero[ej->comida.y][ej->comida.x] = '*';

    for (i = 1; i < ej->serpiente.largo; i++)
        tablero[ej->serpiente.cuerpo[i].y][ej->serpiente.cuerpo[i].x] = 'o';

    tablero[ej->serpiente.cuerpo[0].y][ej->serpiente.cuerpo[0].x] = '@';

    moverCursor(0, 0);

    
    printf("  +");
    for (x = 0; x < ANCHO; x++) printf("-");
    printf("+\n");

    for (y = 0; y < ALTO; y++) {
        printf("  |");
        for (x = 0; x < ANCHO; x++) {
            char c = tablero[y][x];
            if      (c == '@') printf("\033[32m@\033[0m");
            else if (c == 'o') printf("\033[92mo\033[0m");
            else if (c == '*') printf("\033[31m*\033[0m");
            else               printf(" ");
        }
        printf("|\n");
    }

    //borde abajo
    printf("  +");
    for (x = 0; x < ANCHO; x++) printf("-");
    printf("+\n");


    printf("  Puntaje: %-5d  Nivel: %-2d  Largo: %-3d          \n",ej->puntaje, ej->nivel, ej->serpiente.largo);
    printf("  [W/A/S/D] o flechas para mover   [Q] salir\n");
}


void dibujarDelta(EstadoJuego *ej) {
    Serpiente *s = &ej->serpiente;

    // Nueva cabeza 
    dibujarCaracter(s->cuerpo[0].x, s->cuerpo[0].y, '@');

    // Lo que era cabeza ahora es cuerpo 
    if (s->largo > 1)
        dibujarCaracter(s->cuerpo[1].x, s->cuerpo[1].y, 'o');

    //Borrar cola anterior solo si no comio         
    int comioBandera = (s->cuerpo[s->largo - 1].x != ej->colaAnterior.x || s->cuerpo[s->largo - 1].y != ej->colaAnterior.y);
    if (comioBandera)
        dibujarCaracter(ej->colaAnterior.x, ej->colaAnterior.y, ' ');

    
    dibujarCaracter(ej->comida.x, ej->comida.y, '*');

    
    moverCursor(0, ALTO + 2);
    printf("  Puntaje: %-5d  Nivel: %-2d  Largo: %-3d          \n",ej->puntaje, ej->nivel, ej->serpiente.largo);
}


void pantallaBienvenida(EstadoJuego *ej) {
    system("cls");
    printf("\n");
    printf("  +---------------------------------------+\n");
    printf("  |              SNAKEPIT                 |\n");
    printf("  |      Evaluacion Remedial - C          |\n");
    printf("  |      Pedro Pablo Guevara Mena         |\n");
    printf("  |          21.195.024-2                 |\n");
    printf("  +---------------------------------------+\n");
    printf("\n");
    printf("  +-------------------------------------+\n");
    printf("  |  CONTROLES:                         |\n");
    printf("  |   W / Flecha Arriba   -> subir      |\n");
    printf("  |   S / Flecha Abajo    -> bajar      |\n");
    printf("  |   A / Flecha Izq      -> izquierda  |\n");
    printf("  |   D / Flecha Der      -> derecha    |\n");
    printf("  |   Q                   -> salir      |\n");
    printf("  +-------------------------------------+\n");
    printf("  |  NIVELES:  1 = Normal  2 = Rapido   |\n");
    printf("  +-------------------------------------+\n");
    printf("  consejo recuerda que no puedes ir en la direccion\n");
    printf("  contraria de la que te mueves ejemplo si vas \n");
    printf("  hacia W que es arriba no puedes ir abajo que es S\n");
    printf("\n");

    printf("  Nivel (1 o 2): ");
    
    scanf("%d", &ej->nivel);
    
    if (ej->nivel != 1 && ej->nivel != 2) ej->nivel = 1;
    
    printf("\n  Presiona cualquier tecla para empezar...");
    getch();
}


void pantallaGameOver(EstadoJuego *ej) {
    system("cls");
    printf("\n\n");
    printf("  +--------------------------------------+\n");
    printf("  |         Fin de la Partida            |\n");
    printf("  +--------------------------------------+\n");
    printf("  |  Puntaje : %-26d|\n", ej->puntaje);
    printf("  |  Largo   : %-26d|\n", ej->serpiente.largo);
    printf("  |  Nivel   : %-26d|\n", ej->nivel);
    printf("  +--------------------------------------+\n");
    printf("\n  Presiona cualquier tecla para salir...\n");
    getch();
}


void iniciarJuego(EstadoJuego *ej) {
    ej->puntaje             = 0;
    ej->vivo                = 1;
    ej->primerDibujo        = 1;
    ej->serpiente.largo     = 3;
    ej->serpiente.direccion = 'D';

    ej->serpiente.cuerpo[0].x = ANCHO / 2;
    ej->serpiente.cuerpo[0].y = ALTO  / 2;
    ej->serpiente.cuerpo[1].x = ANCHO / 2 - 1;
    ej->serpiente.cuerpo[1].y = ALTO  / 2;
    ej->serpiente.cuerpo[2].x = ANCHO / 2 - 2;
    ej->serpiente.cuerpo[2].y = ALTO  / 2;

    
    ej->colaAnterior = ej->serpiente.cuerpo[2];

    spawnComida(ej);
}


void leerTecla(EstadoJuego *ej) {
    if (!kbhit()) return;

    char tecla = (char)getch();

    if (tecla == 0 || tecla == -32) {
        tecla = (char)getch();
        switch (tecla) {
            case 72: tecla = 'W'; break;
            case 80: tecla = 'S'; break;
            case 75: tecla = 'A'; break;
            case 77: tecla = 'D'; break;
        }
    }

    if (tecla >= 'a' && tecla <= 'z') tecla -= 32;

    char dir = ej->serpiente.direccion;
    if (tecla == 'W' && dir != 'S') ej->serpiente.direccion = 'W';
    if (tecla == 'S' && dir != 'W') ej->serpiente.direccion = 'S';
    if (tecla == 'A' && dir != 'D') ej->serpiente.direccion = 'A';
    if (tecla == 'D' && dir != 'A') ej->serpiente.direccion = 'D';
    if (tecla == 'Q') ej->vivo = 0;
}

//wasd
void moverSerpiente(EstadoJuego *ej) {
    int i;
    for (i = ej->serpiente.largo - 1; i > 0; i--)
        ej->serpiente.cuerpo[i] = ej->serpiente.cuerpo[i - 1];

    switch (ej->serpiente.direccion) {
        case 'W': ej->serpiente.cuerpo[0].y--; break;
        case 'S': ej->serpiente.cuerpo[0].y++; break;
        case 'A': ej->serpiente.cuerpo[0].x--; break;
        case 'D': ej->serpiente.cuerpo[0].x++; break;
    }
}

//choques
int verificarColision(EstadoJuego *ej) {
    int i;
    Punto cabeza = ej->serpiente.cuerpo[0];

    if (cabeza.x < 0 || cabeza.x >= ANCHO || cabeza.y < 0 || cabeza.y >= ALTO)
        return 1;

    for (i = 1; i < ej->serpiente.largo; i++) {
        if (cabeza.x == ej->serpiente.cuerpo[i].x &&
            cabeza.y == ej->serpiente.cuerpo[i].y)
            return 1;
    }
    return 0;
}

// comida
void spawnComida(EstadoJuego *ej) {
    int i, ok;
    do {
        ok = 1;
        ej->comida.x = rand() % ANCHO;
        ej->comida.y = rand() % ALTO;
        for (i = 0; i < ej->serpiente.largo; i++) {
            if (ej->comida.x == ej->serpiente.cuerpo[i].x && ej->comida.y == ej->serpiente.cuerpo[i].y) {
                ok = 0;
                break;
            }
        }
    } while (!ok);
}

void verificarComida(EstadoJuego *ej) {
    Punto cabeza = ej->serpiente.cuerpo[0];

    if (cabeza.x == ej->comida.x && cabeza.y == ej->comida.y) {
        if (ej->serpiente.largo < MAX_CUERPO)
            ej->serpiente.largo++;

        ej->puntaje += (ej->nivel == 1) ? 10 : 20;
        spawnComida(ej);
    }
}
