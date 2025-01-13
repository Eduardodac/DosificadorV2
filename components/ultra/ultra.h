#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <ultrasonic.h>
#include <esp_err.h>

void ultrasonic_task(void *pvParameters);
void iniciar_ultrasonico();
void obtener_medicion_ultrasonico();

#endif // ULTRASONIC_H
