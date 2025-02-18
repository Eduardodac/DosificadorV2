idf_component_register(
                    SRCS 
                        "consumo_data.c"
                    INCLUDE_DIRS "."
                    REQUIRES
                        esp_http_client
                        log
                        "variables_globales"
                        "freertos" 
                        "parse_endpoints"
                    )
