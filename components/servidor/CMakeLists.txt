idf_component_register(
                    SRCS 
                        "servidor.c"
                    INCLUDE_DIRS "."
                    REQUIRES
                        esp_http_client
                        esp_timer
                        log
                        "freertos" 
                    )
