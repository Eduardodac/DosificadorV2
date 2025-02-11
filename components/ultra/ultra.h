#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <ultrasonic.h>
#include <esp_err.h>

void iniciar_ultrasonico();
void medir_ultrasonico_task(void *pvParameters);

#endif // ULTRASONIC_H
