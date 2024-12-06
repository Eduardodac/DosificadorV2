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

void ultrasonic_task(void *pvParameters)
{
    ultrasonic_sensor_t sensor = {
        .trigger_pin = TRIGGER_GPIO,
        .echo_pin = ECHO_GPIO
    };

    ultrasonic_init(&sensor);

    while (true)
    {
        float distance;
        esp_err_t res = ultrasonic_measure(&sensor, MAX_DISTANCE_CM, &distance);
        if (res != ESP_OK)
        {
            ESP_LOGI(TAGULTRA, "Error %d: ", res);
            switch (res)
            {
                case ESP_ERR_ULTRASONIC_PING:
                    ESP_LOGI(TAGULTRA, "Cannot ping (device is in invalid state)");
                    break;
                case ESP_ERR_ULTRASONIC_PING_TIMEOUT:
                    ESP_LOGI(TAGULTRA, "Ping timeout (no device found)\n");
                    break;
                case ESP_ERR_ULTRASONIC_ECHO_TIMEOUT:
                    ESP_LOGI(TAGULTRA, "Echo timeout (i.e. distance too big)\n");
                    break;
                default:
                ESP_LOGI(TAGULTRA, "%s\n", esp_err_to_name(res));
            }
        }
        else{
            ESP_LOGI(TAGULTRA, "Distance: %0.04f cm\n", distance*100);

            if (xSemaphoreTake(xMutexUltraMeasure, portMAX_DELAY) == pdTRUE) {
                ultraMeasure = distance*100;
                xSemaphoreGive(xMutexUltraMeasure);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}