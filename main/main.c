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

static int limite_bascula=150;

int statusDosificacion = 0;

float medicion_bascula = 0;
int confirmacionDosificador = 0;

static void on_timer(void *arg)
{
    obtener_confirmacion_dosificador(&confirmacionDosificador);

    if (xSemaphoreTake(xMutexEstadoDosificacion, portMAX_DELAY) == pdTRUE)
    {
        if (confirmacionDosificador)
            {statusDosificacion = 1;}
        else{
            if(!basculaStatus)
                statusDosificacion = 0;
        }
        
        xSemaphoreGive(xMutexEstadoDosificacion);
    }

    printf("Status: %d\n", statusDosificacion);
}

void app_main(void)
{
    //inicializar wifi
    ESP_ERROR_CHECK(nvs_flash_init());
    create_mutex_estadoDosificacion();
    wifi_connect_init();
    ESP_ERROR_CHECK(wifi_connect_sta("TT-535555", "raizProyecto01", 10000));
    //Inicializar mutex de variables globales;
    create_mutex_ultrasonicoMedicion();
    create_mutex_basculaMedicion();

    esp_timer_handle_t timer;
    esp_timer_create_args_t timer_args = {
        .callback = &on_timer,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "timerConsumo"};

        ESP_ERROR_CHECK(esp_timer_create(&timer_args, &timer));
        ESP_ERROR_CHECK(esp_timer_start_periodic(timer, 60000000));

    xTaskCreate(medir_ultrasonico_task, "medir_ultrasonico_task", 2048, NULL, 5, NULL);
    
    //primera petición al dosificador
    obtener_confirmacion_dosificador(&confirmacionDosificador);

    if (xSemaphoreTake(xMutexEstadoDosificacion, portMAX_DELAY) == pdTRUE)
    {
        if (confirmacionDosificador)
            statusDosificacion = 1;
        else
            statusDosificacion = 0;
        xSemaphoreGive(xMutexEstadoDosificacion);
    }
    tarar_bascula();
    servo_init();
    servo_update_angle(20);
    motor_dc_init();
    xTaskCreate(medir_bascula, "medir_bascula", configMINIMAL_STACK_SIZE * 5, (void *)&limite_bascula, 5, NULL);
    int limpieza = 0;

    // while(1){
    //     motor_dc_forward();
    //             motor_dc_set_speed(450);
    //             vTaskDelay(pdMS_TO_TICKS(500));
    // }
    while(1)
    {
        if(basculaStatus){ //Primero verifica si hay espacio en la báscula
            while (statusDosificacion && basculaStatus) //mientras sea el horario y haya espacio en la báscula dosificará
            {
                //inicio de movimiento de motor DC
                motor_dc_forward();
                motor_dc_set_speed(450);
                //movimiento de servo
                servo_update_angle(-55);
                vTaskDelay(pdMS_TO_TICKS(200));
                servo_update_angle(20);
                vTaskDelay(pdMS_TO_TICKS(8000));
                limpieza = 1;
            }
            if (xSemaphoreTake(xMutexEstadoDosificacion, portMAX_DELAY) == pdTRUE)
            {
                    statusDosificacion = 0;
                xSemaphoreGive(xMutexEstadoDosificacion);
            }
        }

        if(limpieza){
            //Paro de motor DC
            vTaskDelay(pdMS_TO_TICKS(5000));
            motor_dc_reverse();
            motor_dc_set_speed(350);
            vTaskDelay(pdMS_TO_TICKS(500));
            motor_dc_set_speed(0);
            limpieza = 0;
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }

}
