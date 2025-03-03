idf_component_register(SRCS "hx711_comp.c"
                    INCLUDE_DIRS "."
                    REQUIRES
                        "variables_globales"
                        "consumo_endpoints"
                        hx711
                    )
