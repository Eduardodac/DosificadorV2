idf_component_register(SRCS "motorDC.c"
                    INCLUDE_DIRS "."
                    REQUIRES 
                        "driver"
                        "freertos"
                    )
