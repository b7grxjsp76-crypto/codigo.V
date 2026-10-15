/*
 * INF-123 Programación S3 - Proyecto: Análisis de incendios forestales en Chile
 *
 * Número de grupo: [Grupo 2]
 * Integrantes:     [Darío Alonso Ortiz; Sebastián Muñoz Chávez]
 *
 * Comando de compilación:
 *     gcc -Wall -Wextra -o incendios codigo.c
 *
 * Comando de ejecución (archivo de entrada y archivo de salida por línea de comandos):
 *     ./incendios Datos_incendios.txt resultados.txt
 *
 * Descripción:
 *   Lee los registros de incendios (región, año, mes, duración, superficie)
 *   en una matriz dinámica de N filas x 5 columnas y calcula:
 *   superficie total por región, duración promedio, incendio de mayor superficie,
 *   cantidad de incendios, ranking de regiones y porcentaje de la región mayor.
 */

#include <stdio.h>
#include <stdlib.h>

#define COLS 5          /* región, año, mes, duración, superficie */
#define COL_REGION 0
#define COL_ANIO 1
#define COL_MES 2
#define COL_DURACION 3
#define COL_SUPERFICIE 4

#define NREG 3          /* regiones 7, 8 y 9 */
#define REG_BASE 7      /* primera región; índice = región - REG_BASE */

/* ---------- Manejo de memoria dinámica ---------- */

/* Reserva una matriz de filas x COLS con memoria dinámica. */
double **crearMatriz(int filas)
{
    double **m = (double **)malloc(filas * sizeof(double *));
    if (m == NULL) {
        return NULL;
    }
    for (int i = 0; i < filas; i++) {
        m[i] = (double *)malloc(COLS * sizeof(double));
        if (m[i] == NULL) {
            for (int k = 0; k < i; k++) {
                free(m[k]);
            }
            free(m);
            return NULL;
        }
    }
    return m;
}

/* Libera la matriz. */
void liberarMatriz(double **m, int filas)
{
    for (int i = 0; i < filas; i++) {
        free(m[i]);
    }
    free(m);
}

/* ---------- Lectura de datos ---------- */

/* Cuenta los registros válidos (líneas con 5 datos) del archivo. */
int contarRegistros(const char *nombre)
{
    FILE *f = fopen(nombre, "r");
    if (f == NULL) {
        return -1;
    }
    int n = 0;
    double a, b, c, d, e;
    while (fscanf(f, "%lf %lf %lf %lf %lf", &a, &b, &c, &d, &e) == 5) {
        n++;
    }
    fclose(f);
    return n;
}

/* Carga los registros del archivo en la matriz. */
void leerDatos(const char *nombre, double **m, int n)
{
    FILE *f = fopen(nombre, "r");
    if (f == NULL) {
        return;
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < COLS; j++) {
            if (fscanf(f, "%lf", &m[i][j]) != 1) {
                m[i][j] = 0;
            }
        }
    }
    fclose(f);
}

/* ---------- Cálculos ---------- */

/* 1. Superficie total por región (arreglo de NREG). Retorna la suma total. */
double superficieTotal(double **m, int n, double *porRegion)
{
    double total = 0;
    for (int r = 0; r < NREG; r++) {
        porRegion[r] = 0;
    }
    for (int i = 0; i < n; i++) {
        int r = (int)m[i][COL_REGION] - REG_BASE;
        porRegion[r] += m[i][COL_SUPERFICIE];
        total += m[i][COL_SUPERFICIE];
    }
    return total;
}

/* 4. Cantidad de incendios por región. */
void cantidadPorRegion(double **m, int n, int *cantidad)
{
    for (int r = 0; r < NREG; r++) {
        cantidad[r] = 0;
    }
    for (int i = 0; i < n; i++) {
        cantidad[(int)m[i][COL_REGION] - REG_BASE]++;
    }
}

/* 2. Duración promedio por región (usa la cantidad por región). */
void duracionPromedio(double **m, int n, const int *cantidad, double *promedio)
{
    for (int r = 0; r < NREG; r++) {
        promedio[r] = 0;
    }
    for (int i = 0; i < n; i++) {
        promedio[(int)m[i][COL_REGION] - REG_BASE] += m[i][COL_DURACION];
    }
    for (int r = 0; r < NREG; r++) {
        if (cantidad[r] > 0) {
            promedio[r] /= cantidad[r];
        }
    }
}

/* 3. Índice de la fila del incendio con mayor superficie. */
int indiceIncendioMayor(double **m, int n)
{
    int imax = 0;
    for (int i = 1; i < n; i++) {
        if (m[i][COL_SUPERFICIE] > m[imax][COL_SUPERFICIE]) {
            imax = i;
        }
    }
    return imax;
}

/* 5. Ordena los índices de región de mayor a menor superficie (burbuja). */
void ordenarRegiones(const double *porRegion, int *orden)
{
    for (int r = 0; r < NREG; r++) {
        orden[r] = r;
    }
    for (int i = 0; i < NREG - 1; i++) {
        for (int j = 0; j < NREG - 1 - i; j++) {
            if (porRegion[orden[j]] < porRegion[orden[j + 1]]) {
                int tmp = orden[j];
                orden[j] = orden[j + 1];
                orden[j + 1] = tmp;
            }
        }
    }
}

/* ---------- Salida ---------- */

void escribirResultados(const char *nombre, double **m, int n,
                        const double *porRegion, double total,
                        const double *promedio, const int *cantidad,
                        int imax, const int *orden)
{
    FILE *f = fopen(nombre, "w");
    if (f == NULL) {
        printf("Error: no se pudo crear %s\n", nombre);
        return;
    }
    (void)n;

    fprintf(f, "Número de grupo: [COMPLETAR]\n");
    fprintf(f, "Nombre integrantes del grupo:\n");
    fprintf(f, "1. [COMPLETAR NOMBRE 1]\n\n");

    fprintf(f, "1. Superficie total afectada\n");
    for (int r = 0; r < NREG; r++) {
        fprintf(f, "Superficie total afectada en la región %d: %.2f hectáreas.\n",
                r + REG_BASE, porRegion[r]);
    }
    fprintf(f, "Superficie total acumulada: %.2f hectáreas.\n\n", total);

    fprintf(f, "2. Duración promedio de los incendios\n");
    for (int r = 0; r < NREG; r++) {
        fprintf(f, "Duración promedio en la región %d: %.2f minutos.\n",
                r + REG_BASE, promedio[r]);
    }

    fprintf(f, "\n3. Incendio con mayor superficie afectada\n");
    fprintf(f, "Región: %d\n", (int)m[imax][COL_REGION]);
    fprintf(f, "Año: %d\n", (int)m[imax][COL_ANIO]);
    fprintf(f, "Mes: %d\n", (int)m[imax][COL_MES]);
    fprintf(f, "Duración: %d minutos.\n", (int)m[imax][COL_DURACION]);
    fprintf(f, "Superficie afectada: %.2f hectáreas.\n", m[imax][COL_SUPERFICIE]);

    fprintf(f, "\n4. Cantidad de incendios por región\n");
    for (int r = 0; r < NREG; r++) {
        fprintf(f, "Cantidad de incendios en la región %d: %d\n",
                r + REG_BASE, cantidad[r]);
    }

    fprintf(f, "\n5. Regiones ordenadas por superficie total afectada\n");
    for (int k = 0; k < NREG; k++) {
        fprintf(f, "Región %d : %.2f hectáreas.\n",
                orden[k] + REG_BASE, porRegion[orden[k]]);
    }

    fprintf(f, "\n6. Región con mayor superficie quemada\n");
    fprintf(f, "Región: %d\n", orden[0] + REG_BASE);
    fprintf(f, "Superficie total: %.2f hectareas\n", porRegion[orden[0]]);
    fprintf(f, "Porcentaje de la superficie total: %.2f%%\n",
            porRegion[orden[0]] * 100.0 / total);

    fclose(f);
}

/* ---------- Programa principal ---------- */

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Uso: %s <archivo_entrada> <archivo_salida>\n", argv[0]);
        printf("Ejemplo: %s Datos_incendios.txt resultados.txt\n", argv[0]);
        return 1;
    }

    int n = contarRegistros(argv[1]);
    if (n <= 0) {
        printf("Error: no se pudo leer datos desde %s\n", argv[1]);
        return 1;
    }

    double **datos = crearMatriz(n);
    if (datos == NULL) {
        printf("Error: memoria insuficiente\n");
        return 1;
    }
    leerDatos(argv[1], datos, n);

    double porRegion[NREG], promedio[NREG];
    int cantidad[NREG], orden[NREG];

    double total = superficieTotal(datos, n, porRegion);
    cantidadPorRegion(datos, n, cantidad);
    duracionPromedio(datos, n, cantidad, promedio);
    int imax = indiceIncendioMayor(datos, n);
    ordenarRegiones(porRegion, orden);

    escribirResultados(argv[2], datos, n, porRegion, total, promedio,
                       cantidad, imax, orden);

    printf("Se procesaron %d registros. Resultados guardados en %s\n", n, argv[2]);

    liberarMatriz(datos, n);
    return 0;
}
