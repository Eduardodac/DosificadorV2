idf_component_register(
                    SRCS 
                        "consumo_data.c"
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES
                        "../../cert/azure.crt"
                    REQUIRES
                        esp_http_client
                        log
                        "freertos" 
                        "parse_endpoints"
                    )
