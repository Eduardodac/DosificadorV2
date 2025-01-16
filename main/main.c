#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "ultra.h"
#include "servo.h"
#include <esp_err.h>
#include <esp_log.h>
#include "variables_globales.h"
#include "motorDC.h"
#include "esp_timer.h"
#include "hx711_comp.h"

static float limite_bascula=300;
int status = 0;

float medicion_bascula = 0;

void on_timer(TimerHandle_t xTimer)
{
    printf("time hits %lld\n", esp_timer_get_time() / 1000);
    medir_bascula();
}

void on_timer2(TimerHandle_t xTimer)
{
    printf("time 2 hits %lld\n", esp_timer_get_time() / 1000);
}

void on_timer3(TimerHandle_t xTimer)
{
    printf("time 3 hits %lld\n", esp_timer_get_time() / 1000);
    if (xSemaphoreTake(xMutexEstadoActivacion, portMAX_DELAY) == pdTRUE)
    {
        status = 0;
        xSemaphoreGive(xMutexEstadoActivacion);
    }
}

void app_main(void)
{
    tarar_bascula();
    iniciar_ultrasonico();
    create_mutex_estadoActivacion();
    create_mutex_ultraMeasure();

    // TimerHandle_t xTimer = xTimerCreate("my first timer", pdMS_TO_TICKS(60000), true, NULL, on_timer);
    // xTimerStart(xTimer, 0);
    TimerHandle_t xTimer3 = xTimerCreate("my third timer", pdMS_TO_TICKS(30000), true, NULL, on_timer3);
    xTimerStart(xTimer3, 0);

    // xTaskCreate(hx711_task, "hx711_task", configMINIMAL_STACK_SIZE * 5, NULL, 5, NULL);
    // xTaskCreate(estado_task, "estado_task", 2048, NULL, 5, NULL);
    servo_init();
    motor_dc_init();

    while (medicion_bascula <= limite_bascula)
    {
        servo_update_angle(-55);
        motor_dc_forward();
        motor_dc_set_speed(450);
        vTaskDelay(pdMS_TO_TICKS(300));
        servo_update_angle(20);
        if (xSemaphoreTake(xMutexEstadoActivacion, portMAX_DELAY) == pdTRUE)
        {
            status = 0;
            xSemaphoreGive(xMutexEstadoActivacion);
        }

        vTaskDelay(pdMS_TO_TICKS(10000));
        motor_dc_reverse();
        motor_dc_set_speed(350);
        vTaskDelay(pdMS_TO_TICKS(500));
        motor_dc_set_speed(0);
        medicion_bascula = medir_bascula();
        printf("Medicion de bascula %f\n", medicion_bascula);
    }
    vTaskDelay(pdMS_TO_TICKS(500));
}
