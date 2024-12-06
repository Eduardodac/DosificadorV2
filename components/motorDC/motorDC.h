#ifndef MOTORDC_H
#define MOTORDC_H

#include <stdint.h>
#include "driver/mcpwm_prelude.h"
#include "driver/gpio.h"

void motor_dc_init();
void motor_dc_forward();
void motor_dc_reverse();
void motor_dc_set_speed(uint32_t speed);

#endif // MOTOR_DC_H
