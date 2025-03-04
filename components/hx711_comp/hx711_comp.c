#include <inttypes.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "variables_globales.h"
#include "consumo_endpoints.h"
#include <hx711.h>

static const char *TAGHX711 = "hx711";

#define FACTOR_BASCULA 215.2728
#define PERDIDA_REPENTINA -50
#define LIMITE_GATO 1000
#define PESO_AL_COMER 5

hx711_t hx711 = {
    .dout = 22,
    .pd_sck = 21,
    .gain = HX711_GAIN_A_64};
int32_t DataSet = 0;

void tarar_bascula()
{

    ESP_ERROR_CHECK(hx711_init(&hx711));
    ESP_LOGI(TAGHX711, "Tarando...");

    int tareoFlag = 0;
    while (tareoFlag == 0)
    {
        esp_err_t err_tarado = hx711_wait(&hx711, 1000);
        if (err_tarado != ESP_OK)
        {
            ESP_LOGE(TAGHX711, "Device not found: %d (%s)\n", err_tarado, esp_err_to_name(err_tarado));
            continue;
        }

        int32_t dataTare;

        err_tarado = hx711_read_average(&hx711, 500, &dataTare);
        if (err_tarado != ESP_OK)
        {
            ESP_LOGE(TAGHX711, "Could not read data: %d (%s)\n", err_tarado, esp_err_to_name(err_tarado));
            continue;
        }
        DataSet = dataTare;
        ESP_LOGI(TAGHX711, "Tareo listo... %ld", DataSet);
        if (DataSet != 0)
        {
            tareoFlag = 1;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void medir_bascula(void *pvParameters) {
    int limiteMedicion = *(int *)pvParameters;
    TickType_t lastExecution = xTaskGetTickCount(); // Guarda el tiempo inicial
    float consumoAcumulado = 0; // Acumulador de consumo

    while (1) {
        float measure = 0;

        esp_err_t err = hx711_wait(&hx711, 200);
        if (err != ESP_OK) {
            ESP_LOGE(TAGHX711, "Dispositivo no encontrado: %d (%s)", err, esp_err_to_name(err));
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        int32_t data;
        err = hx711_read_average(&hx711, 20, &data);
        if (err != ESP_OK) {
            ESP_LOGE(TAGHX711, "No es posible la lectura: %d (%s)", err, esp_err_to_name(err));
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        measure = (float)(data - DataSet) / FACTOR_BASCULA;

        //hay dos variables de medición, una mide en tiempo real, que es mesure, y la otra es una correción de la medición
        //dependiendo de la diferencia entre ambas se calculan diferencias y se evitan datos extremos que surgan 
        //por errores de medición
        if (xSemaphoreTake(xMutexBasculaMedicion, portMAX_DELAY) == pdTRUE) {
            basculaStatus = (basculaMedicion < limiteMedicion) ? 1 : 0;
            float diferencia = measure - basculaMedicion;

            if (diferencia >= LIMITE_GATO) //se subió el gato 
                ESP_LOGI(TAGHX711, "Caso 1: Medida: %.2f grms. Medicion %.2f", measure, basculaMedicion);
            else if (diferencia <= PERDIDA_REPENTINA) // se volvó el tazón
                ESP_LOGI(TAGHX711, "Caso 2: Medida: %.2f grms. Medicion %.2f", measure, basculaMedicion);
            else if (measure == 0) // posible error en la medición o sin cambios
                ESP_LOGI(TAGHX711, "Caso 3: Medida: %.2f grms. Medicion %.2f", measure, basculaMedicion);
            else if (diferencia >= -1 && diferencia <= 1) { // deriva de valores de la celda de carga
                ESP_LOGI(TAGHX711, "Caso 4: Medida: %.2f grms. Medicion %.2f", measure, basculaMedicion);
                basculaMedicion = measure;
            } else if(diferencia <= PESO_AL_COMER && !estadoDosificacion) //peso añadido al comer
                ESP_LOGI(TAGHX711, "Caso 5: Medida: %.2f grms. Medicion %.2f", measure, basculaMedicion);
            else if (diferencia <= -0.6) { //diferencias de alimento por comida de gato
                consumoAcumulado += diferencia; 
                TickType_t currentTime = xTaskGetTickCount(); //la medición se hace cada segundo, pero el envío de dato cada 10
                if ((currentTime - lastExecution) >= pdMS_TO_TICKS(10000)) {
                    ESP_LOGI(TAGHX711, "Caso 6: consumo acumulado enviado: %.2f grms", consumoAcumulado);
                    enviar_consumo_gato(consumoAcumulado);
                    consumoAcumulado = 0; 
                    lastExecution = currentTime; 
                }

                basculaMedicion = measure;
            } else { //casos no contemplados
                ESP_LOGI(TAGHX711, "Caso 7: Medida: %.2f grms. Medicion %.2f", measure, basculaMedicion);
            }

            xSemaphoreGive(xMutexBasculaMedicion);
        }

        vTaskDelay(pdMS_TO_TICKS(1000)); // Seguir midiendo cada segundo
    }
}
