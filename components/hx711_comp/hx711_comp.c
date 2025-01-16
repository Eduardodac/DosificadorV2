#include <inttypes.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <hx711.h>

static const char *TAGHX711 = "hx711-example";

hx711_t hx711 = {
    .dout = 21,
    .pd_sck = 22,
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

float medir_bascula()
{
    esp_err_t err_tarado = hx711_wait(&hx711, 1000);
    if (err_tarado != ESP_OK)
    {
        ESP_LOGE(TAGHX711, "Device not found: %d (%s)\n", err_tarado, esp_err_to_name(err_tarado));
        return 0;
    }

    int32_t data;
    err_tarado = hx711_read_average(&hx711, 20, &data);
    if (err_tarado != ESP_OK)
    {
        ESP_LOGE(TAGHX711, "Could not read data: %d (%s)\n", err_tarado, esp_err_to_name(err_tarado));
        return 0;
    }

    // ESP_LOGI(TAGHX711, "Raw data: %" PRIi32, data);
    float measure = (double)(data - DataSet) / 215.2728;
    ESP_LOGI(TAGHX711, "Info: %" PRIi32, (data - DataSet));
    ESP_LOGI(TAGHX711, "Medida: %f grms.", measure);
    vTaskDelay(pdMS_TO_TICKS(100));

    return measure;

}
