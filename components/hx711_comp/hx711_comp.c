#include <inttypes.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <hx711.h>

static const char *TAGHX711 = "hx711-example";

void test(void *pvParameters)
{
    hx711_t dev = {
        .dout = 21,
        .pd_sck = 22,
        .gain = HX711_GAIN_A_64
    };

    int32_t DataSet = 0;
    // initialize device
    ESP_ERROR_CHECK(hx711_init(&dev));

    // read from device
    int calibracion = 0;
    ESP_LOGI(TAGHX711, "Calibrando...");
    while (calibracion==0)
    {

        esp_err_t rCal = hx711_wait(&dev, 1500);
        if (rCal != ESP_OK)
        {
            ESP_LOGE(TAGHX711, "Device not found: %d (%s)\n", rCal, esp_err_to_name(rCal));
            continue;
        }

        int32_t dataCal;
        rCal = hx711_read_average(&dev, 20, &dataCal);
        if (rCal != ESP_OK)
        {
            ESP_LOGE(TAGHX711, "Could not read data: %d (%s)\n", rCal, esp_err_to_name(rCal));
            continue;
        }
        DataSet = dataCal;
        ESP_LOGI(TAGHX711, "Cal... %ld", DataSet);
        if (DataSet != 0)
        {
        }
            calibracion = 1;
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    ESP_LOGI(TAGHX711, "Calibrado...");
    while (1)
    {
        esp_err_t r = hx711_wait(&dev, 1000);
        if (r != ESP_OK)
        {
            ESP_LOGE(TAGHX711, "Device not found: %d (%s)\n", r, esp_err_to_name(r));
            continue;
        }

        int32_t data;
        r = hx711_read_average(&dev, 20, &data);
        if (r != ESP_OK)
        {
            ESP_LOGE(TAGHX711, "Could not read data: %d (%s)\n", r, esp_err_to_name(r));
            continue;
        }

        //ESP_LOGI(TAGHX711, "Raw data: %" PRIi32, data);
        float measure = (double)(data-DataSet)/215.2728;
        ESP_LOGI(TAGHX711, "data: %" PRIi32, (data-DataSet));
        ESP_LOGI(TAGHX711, "mesure: %f grms.", measure);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
