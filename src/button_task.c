#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"


#define GPIO_OUTPUT_IO_27    27
#define GPIO_OUTPUT_IO_26    26
#define GPIO_OUTPUT_IO_25    25
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_IO_25) | (1ULL<<GPIO_OUTPUT_IO_26) | (1ULL<<GPIO_OUTPUT_IO_27))
#define GPIO_INPUT_IO_34     34
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_IO_34))
#define ESP_INTR_FLAG_DEFAULT 0

static xQueueHandle gpio_evt_queue = NULL;
int cnt = 0;
bool button_clicked = false;
static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void input_task(void* arg)
{
    uint32_t io_num;
    while(1) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            
            int gpio_level = gpio_get_level(io_num);
            //printf("GPIO[%d] intr, val: %d\n", io_num, gpio_level);
            if(gpio_level == 1){
                
                if(cnt < 3){
                    
                    cnt++;

                }
                else
                {
                    cnt = 0;
                }
                //printf("set mode %d\n", cnt);
                button_clicked = true;
            }
        }
        vTaskDelay(10);
    }
}
static void output_task(void* arg)
{
    printf("output_task started\n");
    gpio_set_level(GPIO_OUTPUT_IO_25, 0);
    gpio_set_level(GPIO_OUTPUT_IO_26, 0);
    gpio_set_level(GPIO_OUTPUT_IO_27, 0);
    while(1) {
        
        if(button_clicked) {
            //printf("change mode\n");
        switch (cnt)
        {
        case 1:
            //printf("GPIO[%d] out, val: %d\n", GPIO_OUTPUT_IO_25, 1);
            gpio_set_level(GPIO_OUTPUT_IO_25, 1);
            break;
        case 2:
            //printf("GPIO[%d] out, val: %d\n", GPIO_OUTPUT_IO_26, 1);

            gpio_set_level(GPIO_OUTPUT_IO_26, 1);
            break;
        case 3:
            //printf("GPIO[%d] out, val: %d\n", GPIO_OUTPUT_IO_27, 1);

            gpio_set_level(GPIO_OUTPUT_IO_27, 1);
            break;
        default:
            // printf("GPIO[%d] out, val: %d\n", GPIO_OUTPUT_IO_25, 0);
            // printf("GPIO[%d] out, val: %d\n", GPIO_OUTPUT_IO_26, 0);
            // printf("GPIO[%d] out, val: %d\n", GPIO_OUTPUT_IO_27, 0);
            gpio_set_level(GPIO_OUTPUT_IO_25, 0);
            gpio_set_level(GPIO_OUTPUT_IO_26, 0);
            gpio_set_level(GPIO_OUTPUT_IO_27, 0);
            break;
        }
        button_clicked = false;
        }
        vTaskDelay(10 / portTICK_RATE_MS);
        
    }
}
void init_button(){

    gpio_config_t io_conf;
    
    io_conf.intr_type = GPIO_INTR_DISABLE;
    
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);
    gpio_set_intr_type(GPIO_INPUT_IO_34, GPIO_INTR_ANYEDGE);
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(input_task, "input_task", 2048, NULL, 10, NULL);
    xTaskCreate(output_task, "output_task", 2048, NULL, 10, NULL);
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(GPIO_INPUT_IO_34, gpio_isr_handler, (void*) GPIO_INPUT_IO_34);
    gpio_isr_handler_remove(GPIO_INPUT_IO_34);
    gpio_isr_handler_add(GPIO_INPUT_IO_34, gpio_isr_handler, (void*) GPIO_INPUT_IO_34);
}
