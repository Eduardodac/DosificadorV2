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

    int status = 1;

void on_timer(TimerHandle_t xTimer)
{
    printf("time hits %lld\n", esp_timer_get_time() / 1000);
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
    iniciar_ultrasonico();
    create_mutex_estadoActivacion();
    create_mutex_ultraMeasure();

    TimerHandle_t xTimer3 = xTimerCreate("my third timer", pdMS_TO_TICKS(30000), true, NULL, on_timer3);
    xTimerStart(xTimer3, 0);

    // xTaskCreate(hx711_task, "hx711_task", configMINIMAL_STACK_SIZE * 5, NULL, 5, NULL);
    // xTaskCreate(estado_task, "estado_task", 2048, NULL, 5, NULL);
    servo_init();
    motor_dc_init();

    while (1)
    {
        if (status == 1)
        {
            servo_update_angle(-55);
            motor_dc_forward();
            motor_dc_set_speed(400);
            vTaskDelay(pdMS_TO_TICKS(500));
            servo_update_angle(20);
            if (xSemaphoreTake(xMutexEstadoActivacion, portMAX_DELAY) == pdTRUE)
            {
                status = 0;
                xSemaphoreGive(xMutexEstadoActivacion);
            }

            vTaskDelay(pdMS_TO_TICKS(5000));
            motor_dc_reverse();
            motor_dc_set_speed(350);
            vTaskDelay(pdMS_TO_TICKS(500));
            motor_dc_set_speed(0);
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
