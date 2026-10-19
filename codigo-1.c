/*
 * INF-123 Programación S3 - Proyecto: Análisis de incendios forestales en Chile
 *
 * Número de grupo: 2
 * Integrantes: Darío Alonso Ortiz / Sebastián Muñoz Chávez
 *
 * Comando de compilación:
 *     gcc codigo.c -o codigo
 *
 * Comando de ejecución (filas y columnas por línea de comandos):
 *     ./codigo 1000 5          (en Windows: .\codigo.exe 1000 5)
 *
 * El archivo Datos_incendios.txt debe estar en la misma carpeta.
 * El programa genera el archivo resultados.txt.
 */

#include <stdio.h>
#include <stdlib.h>

/*
El programa debe calcular e informar:
1. La superficie total afectada en cada región y la superficie total acumulada.
2. La duración promedio de los incendios en cada región.
3. El incendio que presenta la mayor superficie afectada.
4. La cantidad de incendios registrados en cada región.
5. Las regiones ordenadas según la superficie total afectada, de mayor a menor.
6. La región que presenta la mayor superficie total afectada y el porcentaje que
   representa respecto de la superficie total acumulada en las tres regiones.
*/

/*
El archivo contiene una matriz de 1000 filas y 5 columnas, donde cada fila
representa un incendio y las columnas representan:
0 codigo_region, 1 año, 2 mes, 3 duracion, 4 superficie (hectareas)
*/

/* ---------- Prototipos de las funciones ---------- */
double **crearMatriz(int filas, int columnas);
void liberarMatriz(double **matriz, int filas);
void cargarDatos(FILE *archivo, double **matriz, int FILAS, int COLUMNAS);
void encontrarRegiones(double **matriz, int *regiones, int FILAS, int *cantidadRegiones);
void ordenarCodigosRegion(int *regiones, int cantidadRegiones);
void calcularSuperficieAfectada(double **matriz, int *regiones, int cantidadRegiones, int FILAS, double *superficiePorRegion, double *superficieTotal, FILE *resultados);
void duracionPromedioIncendios(double **matriz, int *regiones, int cantidadRegiones, int FILAS, FILE *resultados);
void incendioMayorSuperficie(double **matriz, int FILAS, FILE *resultados);
void cantidadIncendiosPorRegion(double **matriz, int *regiones, int cantidadRegiones, int FILAS, FILE *resultados);
void ordenarSuperficies(double *superficiePorRegion, int *regiones, int cantidadRegiones, FILE *resultados);
void regionMayorSuperficie(double *superficiePorRegion, int *regiones, int cantidadRegiones, double superficieTotal, FILE *resultados);

/* ---------- Programa principal ---------- */

int main(int argc, char *argv[]){
    FILE *archivo;
    FILE *resultados;
    double **matriz;
    int *regiones;
    int cantidadRegiones;
    double *superficiePorRegion;
    double superficieTotal;

    if (argc != 3) {
        printf("Uso: %s <filas> <columnas>\n", argv[0]);
        printf("Ejemplo: %s 1000 5\n", argv[0]);
        return 1;
    }

    /* atoi convierte los argumentos (texto) a números enteros */
    int FILAS = atoi(argv[1]);
    int COLUMNAS = atoi(argv[2]);

    if (FILAS <= 0 || COLUMNAS != 5) {
        printf("Error: las filas deben ser mayores que 0 y las columnas deben ser 5.\n");
        return 1;
    }

    archivo = fopen("Datos_incendios.txt", "r");
    if(archivo == NULL){
        printf("No se pudo abrir el archivo\n");
        return 1;
    }

    resultados = fopen("resultados.txt", "w");
    if (resultados == NULL) {
        printf("No se pudo crear el archivo de resultados.\n");
        fclose(archivo);
        return 1;
    }

    /* Memoria dinámica: arreglo de regiones y matriz de FILAS x COLUMNAS */
    regiones = (int *)malloc(FILAS * sizeof(int));
    matriz = crearMatriz(FILAS, COLUMNAS);

    cargarDatos(archivo, matriz, FILAS, COLUMNAS);
    fclose(archivo);

    encontrarRegiones(matriz, regiones, FILAS, &cantidadRegiones);
    ordenarCodigosRegion(regiones, cantidadRegiones);   /* regiones = {7, 8, 9} */

    superficiePorRegion = (double *)calloc(cantidadRegiones, sizeof(double));

    fprintf(resultados, "Número de grupo: 2\n");
    fprintf(resultados, "Nombre integrantes del grupo:\n");
    fprintf(resultados, "1. Darío Alonso Ortiz\n");
    fprintf(resultados, "2. Sebastián Muñoz Chávez\n\n");

    /* Se llama a cada función en el orden en que el enunciado pide los resultados */
    calcularSuperficieAfectada(matriz, regiones, cantidadRegiones, FILAS, superficiePorRegion, &superficieTotal, resultados); /* 1 */
    duracionPromedioIncendios(matriz, regiones, cantidadRegiones, FILAS, resultados);                                          /* 2 */
    incendioMayorSuperficie(matriz, FILAS, resultados);                                                                        /* 3 */
    cantidadIncendiosPorRegion(matriz, regiones, cantidadRegiones, FILAS, resultados);                                         /* 4 */

    /* Se ordenan copias para no perder el orden 7, 8, 9 de los arreglos originales */
    int *regionesOrdenadas = (int *)malloc(cantidadRegiones * sizeof(int));
    double *superficiesOrdenadas = (double *)malloc(cantidadRegiones * sizeof(double));
    for (int i = 0; i < cantidadRegiones; i++) {
        regionesOrdenadas[i] = regiones[i];
        superficiesOrdenadas[i] = superficiePorRegion[i];
    }
    ordenarSuperficies(superficiesOrdenadas, regionesOrdenadas, cantidadRegiones, resultados);                                 /* 5 */
    regionMayorSuperficie(superficiePorRegion, regiones, cantidadRegiones, superficieTotal, resultados);                       /* 6 */

    fclose(resultados);
    printf("Resultados guardados en resultados.txt\n");

    // Liberar memoria dinámica reservada
    liberarMatriz(matriz, FILAS);
    free(regiones);
    free(superficiePorRegion);
    free(regionesOrdenadas);
    free(superficiesOrdenadas);

    return (0);
}


/* ---------- Manejo de memoria dinámica ---------- */

/* Reserva una matriz de filas x columnas con memoria dinámica. */
double **crearMatriz(int filas, int columnas)
{
    double **matriz = (double **)malloc(filas * sizeof(double *));
    for (int i = 0; i < filas; i++) {
        matriz[i] = (double *)malloc(columnas * sizeof(double));
    }
    return matriz;
}

/* Libera la matriz. */
void liberarMatriz(double **matriz, int filas)
{
    for (int i = 0; i < filas; i++) {
        free(matriz[i]);
    }
    free(matriz);
}

/* ---------- Lectura de datos ---------- */

void cargarDatos(FILE *archivo, double **matriz, int FILAS, int COLUMNAS) {
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            fscanf(archivo, "%lf", &matriz[i][j]);
        }
    }
    printf("Datos cargados correctamente.\n");
}

void encontrarRegiones(double **matriz, int *regiones, int FILAS, int *cantidadRegiones) {
    *cantidadRegiones = 0;

    for (int i = 0; i < FILAS; i++) {
        int region = (int)matriz[i][0]; // Region actual, casteada a int
        int existe = 0;
        for (int j = 0; j < *cantidadRegiones; j++) { // Recorrer el arreglo regiones
            if (regiones[j] == region) {
                existe = 1;
                break;
            }
        }
        if (existe == 0) { // Añadir regiones inexistentes
            regiones[*cantidadRegiones] = region;
            (*cantidadRegiones)++;
        }
    }
    // regiones = {8, 7, 9} (en el orden en que aparecen en el archivo)
}

void ordenarCodigosRegion(int *regiones, int cantidadRegiones) {
    for (int i = 0; i < cantidadRegiones - 1; i++) {
        for (int j = 0; j < cantidadRegiones - 1 - i; j++) {
            if (regiones[j] > regiones[j + 1]) {
                int temp = regiones[j];
                regiones[j] = regiones[j + 1];
                regiones[j + 1] = temp;
            }
        }
    }
}

/* ---------- Cálculos e informe de resultados (en el orden del enunciado) ---------- */

void calcularSuperficieAfectada(double **matriz, int *regiones, int cantidadRegiones, int FILAS, double *superficiePorRegion, double *superficieTotal, FILE *resultados) {
    *superficieTotal = 0;

    for (int i = 0; i < FILAS; i++) {
        int region = (int)matriz[i][0];
        double superficie = matriz[i][4];

        for (int j = 0; j < cantidadRegiones; j++) {
            if (regiones[j] == region) {
                superficiePorRegion[j] += superficie;
                break;
            }
        }
        *superficieTotal += superficie;
    }

    fprintf(resultados, "1. Superficie total afectada\n\n");
    for (int i = 0; i < cantidadRegiones; i++) {
        fprintf(resultados, "Superficie total afectada en la región %d: %.2f hectáreas.\n",
                regiones[i], superficiePorRegion[i]);
    }
    fprintf(resultados, "Superficie total acumulada: %.2f hectáreas.\n\n", *superficieTotal);
}

void duracionPromedioIncendios(double **matriz, int *regiones, int cantidadRegiones, int FILAS, FILE *resultados) {
    double *duracionTotalPorRegion = (double *)calloc(cantidadRegiones, sizeof(double)); // calloc inicializa la memoria en 0's
    int *cantidadIncendiosPorRegion = (int *)calloc(cantidadRegiones, sizeof(int));

    for (int i = 0; i < FILAS; i++) {
        int region = (int)matriz[i][0];
        double duracion = matriz[i][3];

        for (int j = 0; j < cantidadRegiones; j++) {
            if (regiones[j] == region) {
                duracionTotalPorRegion[j] += duracion;
                cantidadIncendiosPorRegion[j]++;
                break;
            }
        }
    }

    fprintf(resultados, "2. Duración promedio de los incendios\n\n");
    for (int i = 0; i < cantidadRegiones; i++) {
        double duracionPromedio = duracionTotalPorRegion[i] / cantidadIncendiosPorRegion[i];
        fprintf(resultados, "Duración promedio en la región %d: %.2f minutos.\n",
                regiones[i], duracionPromedio);
    }
    fprintf(resultados, "\n");

    free(duracionTotalPorRegion);
    free(cantidadIncendiosPorRegion);
}

void incendioMayorSuperficie(double **matriz, int FILAS, FILE *resultados){
    double mayorSuperficie = 0;
    int indiceMayor = -1;

    for (int i = 0; i < FILAS; i++) {
        double superficie = matriz[i][4];
        if (superficie > mayorSuperficie) {
            mayorSuperficie = superficie;
            indiceMayor = i;
        }
    }

    fprintf(resultados, "3. Incendio con mayor superficie afectada\n\n");
    if (indiceMayor != -1) {
        fprintf(resultados, "Región: %.0f\nAño: %.0f\nMes: %.0f\nDuración: %.0f minutos.\nSuperficie afectada: %.2f hectáreas.\n\n",
                matriz[indiceMayor][0], matriz[indiceMayor][1], matriz[indiceMayor][2],
                matriz[indiceMayor][3], matriz[indiceMayor][4]);
    } else {
        fprintf(resultados, "No se encontraron incendios.\n\n");
    }
}

void cantidadIncendiosPorRegion(double **matriz, int *regiones, int cantidadRegiones, int FILAS, FILE *resultados) {
    int *cantidad = (int *)calloc(cantidadRegiones, sizeof(int));

    for (int i = 0; i < FILAS; i++) {
        int region = (int)matriz[i][0];
        for (int j = 0; j < cantidadRegiones; j++) {
            if (regiones[j] == region) {
                cantidad[j]++;
                break;
            }
        }
    }

    fprintf(resultados, "4. Cantidad de incendios por región\n\n");
    for (int i = 0; i < cantidadRegiones; i++) {
        fprintf(resultados, "Cantidad de incendios en la región %d: %d\n",
                regiones[i], cantidad[i]);
    }
    fprintf(resultados, "\n");

    free(cantidad);
}

void ordenarSuperficies(double *superficiePorRegion, int *regiones, int cantidadRegiones, FILE *resultados) {
    for (int i = 0; i < cantidadRegiones - 1; i++) {
        for (int j = 0; j < cantidadRegiones - 1; j++) {
            if (superficiePorRegion[j] < superficiePorRegion[j + 1]) {
                double tempSuperficie = superficiePorRegion[j];
                superficiePorRegion[j] = superficiePorRegion[j + 1];
                superficiePorRegion[j + 1] = tempSuperficie;

                int tempRegion = regiones[j];
                regiones[j] = regiones[j + 1];
                regiones[j + 1] = tempRegion;
            }
        }
    }
    fprintf(resultados, "5. Regiones ordenadas por superficie total afectada\n\n");
    for (int i = 0; i < cantidadRegiones; i++) {
        fprintf(resultados, "Región %d : %.2f hectáreas.\n",
                regiones[i], superficiePorRegion[i]);
    }
    fprintf(resultados, "\n");
}

void regionMayorSuperficie(double *superficiePorRegion, int *regiones, int cantidadRegiones, double superficieTotal, FILE *resultados) {
    double mayorSuperficie = 0;
    int regionMayor = -1;

    for (int i = 0; i < cantidadRegiones; i++) {
        if (superficiePorRegion[i] > mayorSuperficie) {
            mayorSuperficie = superficiePorRegion[i];
            regionMayor = regiones[i];
        }
    }

    fprintf(resultados, "6. Región con mayor superficie quemada\n\n");
    if (regionMayor != -1) {
        double porcentaje = (mayorSuperficie / superficieTotal) * 100;
        fprintf(resultados, "Región: %d\nSuperficie total: %.2f hectareas\nPorcentaje de la superficie total: %.2f%%\n\n",
                regionMayor, mayorSuperficie, porcentaje);

        fprintf(resultados, "Porcentaje de la superficie total por región\n");
        for (int i = 0; i < cantidadRegiones; i++) {
            fprintf(resultados, "Región %d: %.2f%%\n",
                    regiones[i], (superficiePorRegion[i] / superficieTotal) * 100);
        }
    } else {
        fprintf(resultados, "No se encontraron regiones.\n");
    }
}
