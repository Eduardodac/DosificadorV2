#ifndef VARIABLES_GLOBALES_H
#define VARIABLES_GLOBALES_H

#include "freertos/semphr.h"

extern SemaphoreHandle_t xMutexEstadoDosificacion;
extern int estadoDosificacion;

extern float ultraMeasure;
extern SemaphoreHandle_t xMutexUltraMeasure;

extern float loadCellMeasure;
extern SemaphoreHandle_t xMutexLoadCellMeasure;

void create_mutex_estadoDosificacion();
void create_mutex_ultraMeasure();
void create_mutex_loadCellMeasure();

#endif 
