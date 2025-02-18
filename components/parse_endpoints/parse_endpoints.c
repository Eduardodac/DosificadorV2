#include <stdio.h>
#include <string.h>
#include "variables_globales.h"
#include "esp_log.h"
#include "cJSON.h"

static const char *TAGPARSE = "Parse:";

esp_err_t confirmar_parse_dosificacion(char *confirmarDosificacionStr)
{
    cJSON *dataJson = cJSON_Parse(confirmarDosificacionStr);

    if (dataJson == NULL)
    {
        const char *err = cJSON_GetErrorPtr();
        if (err)
        {
            ESP_LOGE(TAGPARSE, "Error al analizar json %s", err);
            return ESP_FAIL;
        }
    }

    cJSON *dosificar = cJSON_GetObjectItemCaseSensitive(dataJson, "dosificar");
    cJSON *habilitado = cJSON_GetObjectItemCaseSensitive(dataJson, "habilitado");
    cJSON *hora = cJSON_GetObjectItemCaseSensitive(dataJson, "hora");

    if (dosificar == NULL || !cJSON_IsBool(dosificar))
    {
        ESP_LOGE(TAGPARSE, "Campo 'dosificar' no encontrado o no es un booleano");
        cJSON_Delete(dataJson); 
        return ESP_FAIL;
    }
    if (habilitado == NULL || !cJSON_IsBool(habilitado))
    {
        //ESP_LOGE(TAGPARSE, "Campo 'habilitado' no encontrado o no es un booleano");
        cJSON_Delete(dataJson);
        return ESP_FAIL;
    }
    if (hora == NULL || !cJSON_IsString(hora))
    {
        //ESP_LOGE(TAGPARSE, "Campo 'hora' no encontrado o no es una cadena");
        cJSON_Delete(dataJson); 
        return ESP_FAIL;
    }

    ESP_LOGI(TAGPARSE, "dosificar: %s", cJSON_IsTrue(dosificar) ? "true" : "false");
    //ESP_LOGI(TAGPARSE, "habilitado: %s", cJSON_IsTrue(habilitado) ? "true" : "false");
    ESP_LOGI(TAGPARSE, "hora: %s", hora->valuestring);

    if (!cJSON_IsTrue(dosificar))
    {
        //ESP_LOGE(TAGPARSE, "El campo 'dosificar' no es true");
        cJSON_Delete(dataJson);
        return ESP_FAIL;
    }

    if (!cJSON_IsTrue(habilitado))
    {
        //ESP_LOGE(TAGPARSE, "El campo 'habilitado' no es true");
        cJSON_Delete(dataJson);
        return ESP_FAIL;
    }

    cJSON_Delete(dataJson);

    return ESP_OK;
}

char *crear_post_body()
{
    cJSON *json_payload = cJSON_CreateObject();
    cJSON_AddStringToObject(json_payload, "dosificadorId", UUIDDosificador);
    cJSON_AddStringToObject(json_payload, "collarId", UUIDCollar);
    cJSON_AddStringToObject(json_payload, "duracion", "5000");
    cJSON_AddStringToObject(json_payload, "consumo", "10.85");

    char *payload_body = cJSON_Print(json_payload);

    printf("body: %s\n", payload_body);
    cJSON_Delete(json_payload);
    return payload_body;
}