/*
 * LED blink with FreeRTOS
 */
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>
#include "ssd1306.h"
#include "gfx.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>
#include "hardware/adc.h"
#include "hardware/dma.h"

QueueHandle_t xQueue;
    
void Task_ReadADC(void *pvParameters);
void Task_UpdateOLED(void *pvParameters);
void ssd1306_put_page_dma(uint8_t *data, size_t length);

void app_main() {
    xQueue = xQueueCreate(10, sizeof(int));

    TaskHandle_t adcTaskHandle, oledTaskHandle;

    xTaskCreate(Task_ReadADC, "Read ADC", 2048, NULL, 1, &adcTaskHandle);
    vTaskCoreAffinitySet(adcTaskHandle, (1 << 0)); // Define a tarefa no core 0

    xTaskCreate(Task_UpdateOLED, "Update OLED", 2048, NULL, 1, &oledTaskHandle);
    vTaskCoreAffinitySet(oledTaskHandle, (1 << 1)); // Define a tarefa no core 1
}

void Task_ReadADC(void *pvParameters) {
    // adc1_config_width(ADC_WIDTH_BIT_12);
    // adc1_config_channel_atten(ADC_CHANNEL_6, ADC_ATTEN_DB_11);

    while (1) {
        int adc_value = 0;
        xQueueSend(xQueue, &adc_value, 0);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void Task_UpdateOLED(void *pvParameters) {
    ssd1306_init();
    uint8_t oled_data[128]; // Buffer para o OLED, ajuste conforme necessário

    while (1) {
        int adc_value;
        if (xQueueReceive(xQueue, &adc_value, portMAX_DELAY)) {
            snprintf((char *)oled_data, sizeof(oled_data), "ADC: %d", adc_value);
            ssd1306_put_page_dma(oled_data, sizeof(oled_data));
        }
    }
}

void ssd1306_put_page_dma(uint8_t *data, size_t length) {
    int dma_channel = dma_claim_unused_channel(true);

    dma_channel_config config = dma_channel_get_default_config(dma_channel);
    channel_config_set_transfer_data_size(&config, DMA_SIZE_8);
    channel_config_set_read_increment(&config, true);
    channel_config_set_write_increment(&config, false);

    dma_channel_configure(
        dma_channel,
        &config,
        (void *)&dma_channel, // Endereço de escrita do OLED
        data,                           // Buffer de dados para enviar
        length,                         // Tamanho dos dados
        true                            // Iniciar imediatamente
    );

    dma_channel_wait_for_finish_blocking(dma_channel);
    dma_channel_unclaim(dma_channel);
}

