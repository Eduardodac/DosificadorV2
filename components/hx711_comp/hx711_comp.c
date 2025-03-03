#include <inttypes.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "variables_globales.h"
#include "consumo_endpoints.h"
#include <hx711.h>

static const char *TAGHX711 = "hx711";

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

void medir_bascula(void *pvParameters)
{
    while(1)
    {
        int limiteMedicion = *(int *)pvParameters;
        float measure = 0;

        esp_err_t err_tarado = hx711_wait(&hx711, 500);
        if (err_tarado != ESP_OK)
        {
            ESP_LOGE(TAGHX711, "Dispositivo no encontrado: %d (%s)\n", err_tarado, esp_err_to_name(err_tarado));
        }
    
        int32_t data;
        err_tarado = hx711_read_average(&hx711, 20, &data);
        if (err_tarado != ESP_OK)
        {
            ESP_LOGE(TAGHX711, "No es posible la lectura: %d (%s)\n", err_tarado, esp_err_to_name(err_tarado));
        }
    
        // ESP_LOGI(TAGHX711, "Raw data: %" PRIi32, data);
        measure = (double)(data - DataSet) / 215.2728;
        //ESP_LOGI(TAGHX711, "Info: %" PRIi32, (data - DataSet));
        //ESP_LOGI(TAGHX711, "Medida: %f grms.", measure);
        vTaskDelay(pdMS_TO_TICKS(100));
    
        if (xSemaphoreTake(xMutexBasculaMedicion, portMAX_DELAY) == pdTRUE)
            {
                if(basculaMedicion < limiteMedicion)
                    basculaStatus = 1;
                else{
                    basculaStatus = 0;
                }
                xSemaphoreGive(xMutexBasculaMedicion);
            }

        if (xSemaphoreTake(xMutexBasculaMedicion, portMAX_DELAY) == pdTRUE)
            {
                float diferencia = basculaMedicion - measure;
                ESP_LOGI(TAGHX711, "Diferencia: %f grms.", diferencia);
                if(diferencia <= -1000 || diferencia >= 100){
                    ESP_LOGI(TAGHX711, "Caso 1: Medida: %f grms. Medicion %f", measure, basculaMedicion);
                }
                else if(measure == 0){
                    ESP_LOGI(TAGHX711, "Caso 2: Medida: %f grms. Medicion %f", measure, basculaMedicion);
                }
                else if((diferencia <= 1) && (diferencia >= -1)){
                    ESP_LOGI(TAGHX711, "Caso 3: Medida: %f grms. Medicion %f", measure, basculaMedicion);
                    basculaMedicion = measure;
                }else if(diferencia > 1){
                    ESP_LOGI(TAGHX711, "Caso 4: Medida: %f grms. Medicion %f", measure, basculaMedicion);
                    enviar_consumo_gato(diferencia);
                }else{
                    basculaMedicion = measure;
                    ESP_LOGI(TAGHX711, "Caso 5: Medida: %f grms. Medicion %f", measure, basculaMedicion);
                }
                xSemaphoreGive(xMutexBasculaMedicion);
            }
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
