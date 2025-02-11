#ifndef VARIABLES_GLOBALES_H
#define VARIABLES_GLOBALES_H

#include "freertos/semphr.h"

extern SemaphoreHandle_t xMutexEstadoDosificacion;
extern int estadoDosificacion;

extern float ultrasonicoMedicion;
extern SemaphoreHandle_t xMutexUltrasonicoMedicion;

extern float loadCellMeasure;
extern SemaphoreHandle_t xMutexLoadCellMeasure;

void create_mutex_estadoDosificacion();
void create_mutex_ultrasonicoMedicion();
void create_mutex_loadCellMeasure();

#endif 
