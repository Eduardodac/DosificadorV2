#ifndef PARSE_ENDPOINTS_H
#define PARSE_ENDPOINTS_H

#include "esp_err.h"

esp_err_t confirmar_parse_dosificacion(char *confirmarDosificacionStr);
char *crear_post_body();

#endif
