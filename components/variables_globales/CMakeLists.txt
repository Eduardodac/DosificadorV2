idf_component_register(SRCS "variables_globales.c"
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES
                    "../../cert/azure.crt"
                    REQUIRES    
                        "freertos"
                    )
