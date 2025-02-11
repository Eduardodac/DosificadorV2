idf_component_register(
                    SRCS 
                        "servidor.c"
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES
                        "../../cert/azure.crt"
                    REQUIRES
                        esp_http_client
                        esp_timer
                        log
                        json
                        "freertos" 
                    )
