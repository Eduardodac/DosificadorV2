idf_component_register(SRCS "servo.c"
                    INCLUDE_DIRS "."
                    REQUIRES 
                        "driver"
                        "freertos"
                    )
