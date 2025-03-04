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

static int limite_bascula = 60; // se estima que dosificará 180 grms aprox
int confirmacionDosificador = 0;

static void on_timer(void *arg) // timer que se ejecuta de forma paralela
{
    obtener_confirmacion_dosificador(&confirmacionDosificador);

    if (xSemaphoreTake(xMutexEstadoDosificacion, portMAX_DELAY) == pdTRUE)
    {
        if (confirmacionDosificador)
            estadoDosificacion = 1;
        xSemaphoreGive(xMutexEstadoDosificacion);
    }

    printf("Status Dosificación: %d\n", estadoDosificacion);
}

void app_main(void)
{
    // inicializar nvs y mutex
    ESP_ERROR_CHECK(nvs_flash_init());
    create_mutex_estadoDosificacion();
    create_mutex_ultrasonicoMedicion();
    create_mutex_basculaMedicion();

    //inicializa conexión de wifi
    wifi_connect_init();
    ESP_ERROR_CHECK(wifi_connect_sta("TT-535555", "raizProyecto01", 10000));

    //inicializa timer
    esp_timer_handle_t timer;
    esp_timer_create_args_t timer_args = {
        .callback = &on_timer,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "timerConsumo"};

    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer, 60000000));
    
    //inicializa resto de componentes
    servo_init();
    motor_dc_init();
    servo_update_angle(20);
    tarar_bascula();

    //Iniciar funciones paralelas
    xTaskCreate(medir_ultrasonico_task, "medir_ultrasonico_task", 2048, NULL, 5, NULL);
    xTaskCreate(medir_bascula, "medir_bascula", configMINIMAL_STACK_SIZE * 5, (void *)&limite_bascula, 5, NULL);
    int limpieza = 0;

    // código de accionamiento de motor en casos de liberación de croquetas
    //  while(1){
    //      motor_dc_forward();
    //              motor_dc_set_speed(450);
    //              vTaskDelay(pdMS_TO_TICKS(500));
    //  }

    while (1)
    {
        while (estadoDosificacion && basculaStatus) // mientras sea el horario y haya espacio en la báscula dosificará
        {
            // inicio de movimiento de motor DC
            motor_dc_forward();
            motor_dc_set_speed(450);
            // movimiento de servo
            servo_update_angle(-55);
            vTaskDelay(pdMS_TO_TICKS(300));
            servo_update_angle(20);
            vTaskDelay(pdMS_TO_TICKS(8000));
            limpieza = 1;
        }

        if (xSemaphoreTake(xMutexEstadoDosificacion, portMAX_DELAY) == pdTRUE)
        {
            estadoDosificacion = 0;
            xSemaphoreGive(xMutexEstadoDosificacion);
        }

        if (limpieza)// el código de limpieza solo se accionará si antes hubo ciclos de dosificación
        { 
            // Paro de motor DC
            motor_dc_reverse();
            motor_dc_set_speed(350);
            vTaskDelay(pdMS_TO_TICKS(500));
            motor_dc_set_speed(0);
            limpieza = 0;
        }
        
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
