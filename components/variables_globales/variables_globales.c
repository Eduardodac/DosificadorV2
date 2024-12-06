#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "variables_globales.h"

SemaphoreHandle_t xMutexEstadoActivacion = NULL;

//variables relacionadas a la lectura del botón 
int estadoActivacion = 1;

void create_mutex_estadoActivacion() {
    if (xMutexEstadoActivacion == NULL) {
        xMutexEstadoActivacion = xSemaphoreCreateMutex();
        if (xMutexEstadoActivacion == NULL) {
            printf("Error: No se pudo crear el mutex compartido\n");
        }
    }
}

//variables relacionadas a la lectura del sensor ultrasónico
float ultraMeasure;
SemaphoreHandle_t xMutexUltraMeasure = NULL;

void create_mutex_ultraMeasure() {
    if (xMutexUltraMeasure == NULL) {
        xMutexUltraMeasure = xSemaphoreCreateMutex();
        if (xMutexUltraMeasure == NULL) {
            printf("Error: No se pudo crear el mutex ultraMeasure\n");
        }
    }
}

//variables relacionada a la lectura de la báscula
float loadCellMeasure;
SemaphoreHandle_t xMutexLoadCellMeasure = NULL;

void create_mutex_loadCellMeasure() {
    if (xMutexLoadCellMeasure == NULL) {
        xMutexLoadCellMeasure = xSemaphoreCreateMutex();
        if (xMutexLoadCellMeasure == NULL) {
            printf("Error: No se pudo crear el mutex loadCellMeasure\n");
        }
    }
}

