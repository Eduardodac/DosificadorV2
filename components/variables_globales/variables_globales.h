#ifndef VARIABLES_GLOBALES_H
#define VARIABLES_GLOBALES_H

#include "freertos/semphr.h"

extern const uint8_t cert[] asm("_binary_azure_crt_start");

extern SemaphoreHandle_t xMutexEstadoDosificacion;
extern int estadoDosificacion;

extern float ultrasonicoMedicion;
extern SemaphoreHandle_t xMutexUltrasonicoMedicion;

extern float basculaMedicion;
extern SemaphoreHandle_t xMutexBasculaMedicion;

void create_mutex_estadoDosificacion();
void create_mutex_ultrasonicoMedicion();
void create_mutex_basculaMedicion();

#endif 
