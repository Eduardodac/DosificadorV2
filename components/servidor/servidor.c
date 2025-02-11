#include <stdio.h>
#include <string.h>
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "cJSON.h"

static const char *TAGSERVIDOR = "Server Response:";
extern const uint8_t cert[] asm("_binary_azure_crt_start");

static const char *URL = "https://sistemamonitoreoalimentacionapi.azurewebsites.net/api/Horarios/confirmarDosificacion/02121b21-7f72-4054-9df1-c43777abcf49";

typedef struct chunk_payload_t
{
    uint8_t *buffer;
    int buffer_index;
} chunk_payload_t;

esp_err_t parse_confirmar_dosificacion(char *confirmarDosificacionStr)
{
    cJSON *dataJson = cJSON_Parse(confirmarDosificacionStr);

    if (dataJson == NULL)
    {
        const char *err = cJSON_GetErrorPtr();
        if (err)
        {
            ESP_LOGE(TAGSERVIDOR, "Error al analizar json %s", err);
            return ESP_FAIL;
        }
    }

    cJSON *dosificar = cJSON_GetObjectItemCaseSensitive(dataJson, "dosificar");
    cJSON *habilitado = cJSON_GetObjectItemCaseSensitive(dataJson, "habilitado");
    cJSON *hora = cJSON_GetObjectItemCaseSensitive(dataJson, "hora");

    if (dosificar == NULL || !cJSON_IsBool(dosificar))
    {
        ESP_LOGE(TAGSERVIDOR, "Campo 'dosificar' no encontrado o no es un booleano");
        cJSON_Delete(dataJson); 
        return ESP_FAIL;
    }
    if (habilitado == NULL || !cJSON_IsBool(habilitado))
    {
        //ESP_LOGE(TAGSERVIDOR, "Campo 'habilitado' no encontrado o no es un booleano");
        cJSON_Delete(dataJson);
        return ESP_FAIL;
    }
    if (hora == NULL || !cJSON_IsString(hora))
    {
        //ESP_LOGE(TAGSERVIDOR, "Campo 'hora' no encontrado o no es una cadena");
        cJSON_Delete(dataJson); 
        return ESP_FAIL;
    }

    ESP_LOGI(TAGSERVIDOR, "dosificar: %s", cJSON_IsTrue(dosificar) ? "true" : "false");
    //ESP_LOGI(TAGSERVIDOR, "habilitado: %s", cJSON_IsTrue(habilitado) ? "true" : "false");
    ESP_LOGI(TAGSERVIDOR, "hora: %s", hora->valuestring);

    if (!cJSON_IsTrue(dosificar))
    {
        //ESP_LOGE(TAGSERVIDOR, "El campo 'dosificar' no es true");
        cJSON_Delete(dataJson);
        return ESP_FAIL;
    }

    if (!cJSON_IsTrue(habilitado))
    {
        //ESP_LOGE(TAGSERVIDOR, "El campo 'habilitado' no es true");
        cJSON_Delete(dataJson);
        return ESP_FAIL;
    }

    cJSON_Delete(dataJson);

    return ESP_OK;
}

esp_err_t on_client_data(esp_http_client_event_t *evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        // ESP_LOGI(TAGSERVIDOR, "Length=%d", evt->data_len);
        // printf("%.*s\n", evt->data_len, (char *)evt->data);
        chunk_payload_t *chunk_payload = evt->user_data;
        chunk_payload->buffer = realloc(chunk_payload->buffer, chunk_payload->buffer_index + evt->data_len + 1);
        memcpy(&chunk_payload->buffer[chunk_payload->buffer_index], (uint8_t *)evt->data, evt->data_len);
        chunk_payload->buffer_index = chunk_payload->buffer_index + evt->data_len;
        chunk_payload->buffer[chunk_payload->buffer_index] = 0;
        // printf("buffer******** %s\n",chunk_payload->buffer);
        break;

    default:
        break;
    }
    return ESP_OK;
}

void confirmar_dosificacion_get(int *confirmacion)
{
    chunk_payload_t chunk_payload = {0};
    esp_http_client_config_t esp_http_client_config = {
        .url = URL,
        .method = HTTP_METHOD_GET,
        .event_handler = on_client_data,
        .user_data = &chunk_payload,
        .cert_pem = (char *)cert};
    esp_http_client_handle_t client = esp_http_client_init(&esp_http_client_config);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK)
    {
        ESP_LOGI(TAGSERVIDOR, "HTTP GET status = %d\n", esp_http_client_get_status_code(client));
        esp_err_t dataConfirmacion = parse_confirmar_dosificacion((char *)chunk_payload.buffer);

        if(dataConfirmacion == ESP_OK){
            *confirmacion = 1;
        }else{
            *confirmacion = 0;
        }
    }
    else
    {
        ESP_LOGE(TAGSERVIDOR, "HTTP GET request failed: %s", esp_err_to_name(err));
        *confirmacion = 0;
    }
    if (chunk_payload.buffer != NULL)
    {
        free(chunk_payload.buffer);
    }
    esp_http_client_cleanup(client);
    // wifi_disconnect();
}