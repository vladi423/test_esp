#include <stdlib.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "driver/dac.h"
#include "esp_system.h"
#include "esp_adc_cal.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "adc_task.h"
#include "uart_task.h"
#include "button_task.h"




void app_main() {

    printf("Test\n");
    xTaskCreate(adc_task, "adc_task", 2048, NULL, configMAX_PRIORITIES, NULL);
    init_button();
   
}