#include "ultrasonic.h"
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "variables_globales.h"
#include "esp_log.h"

#define MAX_DISTANCE_CM 1000 // 5m max
#define TRIGGER_GPIO 19
#define ECHO_GPIO 18

static const char *TAGULTRA = "ultrasonic:";

ultrasonic_sensor_t sensor = {
    .trigger_pin = TRIGGER_GPIO,
    .echo_pin = ECHO_GPIO};

void iniciar_ultrasonico()
{
    ultrasonic_init(&sensor);
}

void medir_ultrasonico_task(void *pvParameters)
{
    float distancia;
    esp_err_t res = ultrasonic_measure(&sensor, MAX_DISTANCE_CM, &distancia);

    if (res != ESP_OK)
    {
        ESP_LOGE(TAGULTRA, "Error %d: ", res);
        switch (res)
        {
        case ESP_ERR_ULTRASONIC_PING:
            ESP_LOGE(TAGULTRA, "Ping inválido (dispositivo en estado inválido)");
            break;
        case ESP_ERR_ULTRASONIC_PING_TIMEOUT:
            ESP_LOGE(TAGULTRA, "Ping timeout (dispositivo no encontrado)\n");
            break;
        case ESP_ERR_ULTRASONIC_ECHO_TIMEOUT:
            ESP_LOGI(TAGULTRA, "Echo timeout (i.e. distancia fuera de rango)\n");
            break;
        default:
            ESP_LOGI(TAGULTRA, "%s\n", esp_err_to_name(res));
        }
    }
    else
    {
        ESP_LOGI(TAGULTRA, "Distancia: %0.04f cm\n", distancia * 100);

        if (xSemaphoreTake(xMutexUltrasonicoMedicion, portMAX_DELAY) == pdTRUE)
        {
            ultrasonicoMedicion = distancia * 100;
            xSemaphoreGive(xMutexUltrasonicoMedicion);
        }
    }

    vTaskDelay(pdMS_TO_TICKS(1000));
}