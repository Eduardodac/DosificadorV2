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
#include "nvs_flash.h"
#include "wifi_connect.h"
#include "consumo_endpoints.h"

// static float limite_bascula=150;

int statusDosificacion = 0;

float medicion_bascula = 0;
int confirmacionDosificador = 0;

static void on_timer(void *arg)
{
    obtener_confirmacion_dosificador(&confirmacionDosificador);

    if (xSemaphoreTake(xMutexEstadoDosificacion, portMAX_DELAY) == pdTRUE)
    {
        if (confirmacionDosificador)
            statusDosificacion = 1;
        else
            statusDosificacion = 0;
        xSemaphoreGive(xMutexEstadoDosificacion);
    }

    printf("Status: %d\n", statusDosificacion);
}

// void app_main(void)
// {
//     tarar_bascula();
//     iniciar_ultrasonico();
//     create_mutex_estadoDosificacion();
//     create_mutex_ultraMeasure();

//     // TimerHandle_t xTimer = xTimerCreate("my first timer", pdMS_TO_TICKS(60000), true, NULL, on_timer);
//     // xTimerStart(xTimer, 0);
//     esp_timer_handle_t timer;
//     esp_timer_create_args_t timer_args = {
//         .callback = &on_timer,
//         .arg = NULL,
//         .dispatch_method = ESP_TIMER_TASK,
//         .name = "fetch_timer"
//     };

//     esp_timer_create(&timer_args, &timer);
//     esp_timer_start_periodic(timer, 60000000);

//     // xTaskCreate(hx711_task, "hx711_task", configMINIMAL_STACK_SIZE * 5, NULL, 5, NULL);
//     // xTaskCreate(estado_task, "estado_task", 2048, NULL, 5, NULL);
//     servo_init();
//     motor_dc_init();

//     while (medicion_bascula <= limite_bascula)
//     {
//         servo_update_angle(-55);
//         motor_dc_forward();
//         motor_dc_set_speed(450);
//         vTaskDelay(pdMS_TO_TICKS(300));
//         servo_update_angle(20);
//         if (xSemaphoreTake(xMutexEstadoDosificacion, portMAX_DELAY) == pdTRUE)
//         {
//             status = 0;
//             xSemaphoreGive(xMutexEstadoDosificacion);
//         }

//         vTaskDelay(pdMS_TO_TICKS(10000));
//         motor_dc_reverse();
//         motor_dc_set_speed(350);
//         vTaskDelay(pdMS_TO_TICKS(500));
//         motor_dc_set_speed(0);
//         medicion_bascula = medir_bascula();
//         printf("Medicion de bascula %f\n", medicion_bascula);
//     }
//     vTaskDelay(pdMS_TO_TICKS(500));
// }

// prueba de timers y coneión de wifi

void app_main()
{
    ESP_ERROR_CHECK(nvs_flash_init());
    create_mutex_estadoDosificacion();
    wifi_connect_init();
    ESP_ERROR_CHECK(wifi_connect_sta("TT-535555", "raizProyecto01", 10000));

    esp_timer_handle_t timer;
    esp_timer_create_args_t timer_args = {
        .callback = &on_timer,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "quote_timer"};

    esp_timer_create(&timer_args, &timer);
    esp_timer_start_periodic(timer, 60000000);
    enviar_consumo_gato();

    // if (xSemaphoreTake(xMutexEstadoDosificacion, portMAX_DELAY) == pdTRUE)
    // {
    //     if (confirmacionDosificador)
    //         statusDosificacion = 1;
    //     else
    //         statusDosificacion = 0;
    //     xSemaphoreGive(xMutexEstadoDosificacion);
    // }

    // printf("Status: %d\n", statusDosificacion);
}