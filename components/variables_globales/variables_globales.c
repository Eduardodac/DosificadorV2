#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

SemaphoreHandle_t xMutexEstadoDosificacion = NULL;
//variables relacionadas a los id disponibles

char *UUIDDosificador = "02121b21-7f72-4054-9df1-c43777abcf49";
char *UUIDCollar = "D8C9D49C-5711-48C9-819D-698EB0A67815";
//variables relacionadas a la lectura del botón 
int estadoDosificacion = 1;

void create_mutex_estadoDosificacion() {
    if (xMutexEstadoDosificacion == NULL) {
        xMutexEstadoDosificacion = xSemaphoreCreateMutex();
        if (xMutexEstadoDosificacion == NULL) {
            printf("Error: No se pudo crear el mutex compartido\n");
        }
    }
}

//variables relacionadas a la lectura del sensor ultrasónico
float ultrasonicoMedicion;
SemaphoreHandle_t xMutexUltrasonicoMedicion = NULL;

void create_mutex_ultrasonicoMedicion() {
    if (xMutexUltrasonicoMedicion == NULL) {
        xMutexUltrasonicoMedicion = xSemaphoreCreateMutex();
        if (xMutexUltrasonicoMedicion == NULL) {
            printf("Error: No se pudo crear el mutex ultrasonicoMedicion\n");
        }
    }
}

//variables relacionada a la lectura de la báscula
float basculaMedicion;
SemaphoreHandle_t xMutexBasculaMedicion = NULL;

void create_mutex_basculaMedicion() {
    if (xMutexBasculaMedicion == NULL) {
        xMutexBasculaMedicion = xSemaphoreCreateMutex();
        if (xMutexBasculaMedicion == NULL) {
            printf("Error: No se pudo crear el mutex basculaMedicion\n");
        }
    }
}

