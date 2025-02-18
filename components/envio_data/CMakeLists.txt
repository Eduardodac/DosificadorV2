idf_component_register(
                    SRCS 
                        "envio_data.c"
                    INCLUDE_DIRS "."
                    REQUIRES
                        esp_http_client
                        log
                        "variables_globales"
                        "freertos" 
                        "parse_endpoints"
                    )
