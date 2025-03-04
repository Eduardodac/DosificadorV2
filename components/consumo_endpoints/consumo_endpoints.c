#include <stdio.h>
#include <string.h>
#include "esp_http_client.h"
#include "esp_log.h"
#include "parse_endpoints.h"
#include "variables_globales.h"

static const char *TAGCONSUMO = "Server Response";

static const char *URL = "https://sistemamonitoreoalimentacionapi.azurewebsites.net/api/Horarios/confirmarDosificacion/02121b21-7f72-4054-9df1-c43777abcf49";

extern const uint8_t cert[] asm("_binary_azure_crt_start");

typedef struct chunk_payload_t
{
    uint8_t *buffer;
    int buffer_index;
} chunk_payload_t;

esp_err_t on_client_data(esp_http_client_event_t *evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        // ESP_LOGI(TAGCONSUMO, "Length=%d", evt->data_len);
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

void obtener_confirmacion_dosificador(int *confirmacion)
{
    chunk_payload_t chunk_payload = {0};
    esp_http_client_config_t esp_http_client_config = {
        .url = URL,
        .timeout_ms = 10000,
        .method = HTTP_METHOD_GET,
        .event_handler = on_client_data,
        .user_data = &chunk_payload,
        .cert_pem = (char *)cert};

    int retry_count = 0;
    const int max_retries = 3; //  reintentos

    while (retry_count < max_retries) {

        esp_http_client_handle_t client = esp_http_client_init(&esp_http_client_config);
        esp_http_client_set_header(client, "Content-Type", "application/json");
        esp_err_t err = esp_http_client_perform(client);
        if (err == ESP_OK)
        {
            retry_count = 4;
            ESP_LOGI(TAGCONSUMO, "HTTP GET status = %d\n", esp_http_client_get_status_code(client));
            esp_err_t dataConfirmacion = confirmar_parse_dosificacion((char *)chunk_payload.buffer);

            if(dataConfirmacion == ESP_OK){
                *confirmacion = 1;
            }else{
                *confirmacion = 0;
            }
        }
        else
        {
            ESP_LOGE(TAGCONSUMO, "HTTP GET request failed: %s. Intento %d de %d", esp_err_to_name(err), retry_count + 1, max_retries);
            *confirmacion = 0;
        }

        retry_count++;
        if (retry_count < max_retries) {
            vTaskDelay(pdMS_TO_TICKS(5000)); // Esperar 5 segundos 
        }

        if (chunk_payload.buffer != NULL) {
            free(chunk_payload.buffer);
            chunk_payload.buffer = NULL; // Reiniciar buffer 
        }
        esp_http_client_cleanup(client);
    }
}

void enviar_consumo_gato(float consumo)
{

    chunk_payload_t chunk_payload = {0};
    esp_http_client_config_t esp_http_client_config = {
        // .url = "http://api.quotable.io/random",
        .url = "https://sistemamonitoreoalimentacionapi.azurewebsites.net/api/Registros",
        .timeout_ms = 10000,
        .method = HTTP_METHOD_POST,
        .event_handler = on_client_data,
        .user_data = &chunk_payload,
        .cert_pem = (char *)cert};
    esp_http_client_handle_t client = esp_http_client_init(&esp_http_client_config);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    char *payload_body = crear_post_body(consumo);

    esp_http_client_set_post_field(client, payload_body, strlen(payload_body));

    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK)
    {
    ESP_LOGI(TAGCONSUMO, "HTTP POST status = %d\n", esp_http_client_get_status_code(client));
    }
    else
    {
        ESP_LOGE(TAGCONSUMO, "HTTP Post request failed: %s", esp_err_to_name(err));
    }


    if (chunk_payload.buffer != NULL)
    {
        free(chunk_payload.buffer);
    }

    if (payload_body != NULL)
    {
        free(payload_body);
    }
    esp_http_client_cleanup(client);
    // wifi_disconnect();
}


// #include <stdio.h>
// #include <string.h>
// #include "esp_http_client.h"
// #include "esp_log.h"
// #include "parse_endpoints.h"
// #include "variables_globales.h"

// static const char *TAGCONSUMO = "Server Response";

// static const char *URL = "https://sistemamonitoreoalimentacionapi.azurewebsites.net/api/Horarios/confirmarDosificacion/02121b21-7f72-4054-9df1-c43777abcf49";

// extern const uint8_t cert[] asm("_binary_azure_crt_start");

// typedef struct chunk_payload_t
// {
//     uint8_t *buffer;
//     int buffer_index;
// } chunk_payload_t;

// esp_err_t on_client_data(esp_http_client_event_t *evt)
// {
//     switch (evt->event_id)
//     {
//     case HTTP_EVENT_ON_DATA:
//         // ESP_LOGI(TAGCONSUMO, "Length=%d", evt->data_len);
//         // printf("%.*s\n", evt->data_len, (char *)evt->data);
//         chunk_payload_t *chunk_payload = evt->user_data;
//         chunk_payload->buffer = realloc(chunk_payload->buffer, chunk_payload->buffer_index + evt->data_len + 1);
//         memcpy(&chunk_payload->buffer[chunk_payload->buffer_index], (uint8_t *)evt->data, evt->data_len);
//         chunk_payload->buffer_index = chunk_payload->buffer_index + evt->data_len;
//         chunk_payload->buffer[chunk_payload->buffer_index] = 0;
//         // printf("buffer******** %s\n",chunk_payload->buffer);
//         break;

//     default:
//         break;
//     }
//     return ESP_OK;
// }

// void obtener_confirmacion_dosificador(int *confirmacion)
// {
//     chunk_payload_t chunk_payload = {0};
//     esp_http_client_config_t esp_http_client_config = {
//         .url = URL,
//         .timeout_ms = 10000,
//         .method = HTTP_METHOD_GET,
//         .event_handler = on_client_data,
//         .user_data = &chunk_payload,
//         .cert_pem = (char *)cert
//     };

//     int retry_count = 0;
//     const int max_retries = 3; // reintentos
//     esp_err_t err = ESP_FAIL;

//     while (retry_count < max_retries) {
//         esp_http_client_handle_t client = esp_http_client_init(&esp_http_client_config);
//         esp_http_client_set_header(client, "Content-Type", "application/json");

//         err = esp_http_client_perform(client);

//         if (err == ESP_OK)
//         {
//             ESP_LOGI(TAGCONSUMO, "HTTP GET status = %d\n", esp_http_client_get_status_code(client));
//             esp_err_t dataConfirmacion = confirmar_parse_dosificacion((char *)chunk_payload.buffer);

//             if (dataConfirmacion == ESP_OK) {
//                 *confirmacion = 1;
//                 break; // Cortar bucle
//             } else {
//                 *confirmacion = 0;
//                 ESP_LOGE(TAGCONSUMO, "Error al parsear la confirmación. Intento %d de %d", retry_count + 1, max_retries);
//             }
//         }
//         else
//         {
//             ESP_LOGE(TAGCONSUMO, "HTTP GET request failed: %s. Intento %d de %d", esp_err_to_name(err), retry_count + 1, max_retries);
//             *confirmacion = 0;
//         }

//         retry_count++;
//         if (retry_count < max_retries) {
//             vTaskDelay(pdMS_TO_TICKS(1000)); // 1 segundo espera
//         }

//         // Liberar recursos
//         if (chunk_payload.buffer != NULL) {
//             free(chunk_payload.buffer);
//             chunk_payload.buffer = NULL; // Reiniciar 
//         }
//         esp_http_client_cleanup(client);
//     }

//     // Liberar buffer 
//     if (chunk_payload.buffer != NULL) {
//         free(chunk_payload.buffer);
//     }

//     if (err != ESP_OK || *confirmacion == 0) {
//         ESP_LOGE(TAGCONSUMO, "HTTP GET request falló después de %d intentos", max_retries);
//     }
// }

// void enviar_consumo_gato(float consumo)
// {
//     chunk_payload_t chunk_payload = {0};
//     esp_http_client_config_t esp_http_client_config = {
//         .url = "https://sistemamonitoreoalimentacionapi.azurewebsites.net/api/Registros",
//         .timeout_ms = 10000,
//         .method = HTTP_METHOD_POST,
//         .event_handler = on_client_data,
//         .user_data = &chunk_payload,
//         .cert_pem = (char *)cert
//     };

//     int retry_count = 0;
//     const int max_retries = 3; // Número máximo de reintentos
//     esp_err_t err = ESP_FAIL;

//     while (retry_count < max_retries) {
//         esp_http_client_handle_t client = esp_http_client_init(&esp_http_client_config);
//         esp_http_client_set_header(client, "Content-Type", "application/json");
//         char *payload_body = crear_post_body(consumo);
//         esp_http_client_set_post_field(client, payload_body, strlen(payload_body));

//         err = esp_http_client_perform(client);

//         if (err == ESP_OK)
//         {
//             ESP_LOGI(TAGCONSUMO, "HTTP POST status = %d\n", esp_http_client_get_status_code(client));
//             break; // Salir del bucle si la solicitud fue exitosa
//         }
//         else
//         {
//             ESP_LOGE(TAGCONSUMO, "HTTP Post request failed: %s. Intento %d de %d", esp_err_to_name(err), retry_count + 1, max_retries);
//             retry_count++;
//             vTaskDelay(pdMS_TO_TICKS(5000)); // Esperar 5 segundos antes de reintentar
//         }

//         // Liberar recursos
//         if (payload_body != NULL)
//         {
//             free(payload_body);
//         }
//         esp_http_client_cleanup(client);
//     }

//     // Liberar el buffer de respuesta si existe
//     if (chunk_payload.buffer != NULL)
//     {
//         free(chunk_payload.buffer);
//     }

//     if (err != ESP_OK)
//     {
//         ESP_LOGE(TAGCONSUMO, "HTTP Post request falló después de %d intentos", max_retries);
//     }
// }