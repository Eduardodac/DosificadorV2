#ifndef SERVIDOR_H
#define SERVIDOR_H

#include "esp_err.h"

void confirmar_dosificacion_get();
esp_err_t parse_confirmar_dosificacion(char *confirmarDosificacionStr);

#endif // SERVIDOR_H