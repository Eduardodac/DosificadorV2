idf_component_register(
                    SRCS 
                        "consumo_endpoints.c"
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES
                        "../../cert/azure.crt"
                    REQUIRES
                        esp_http_client
                        log
                        "variables_globales"
                        "freertos" 
                        "parse_endpoints"
                    )
