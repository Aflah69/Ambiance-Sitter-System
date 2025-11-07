#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "mqtt_client.h"
#include "driver/gpio.h"
#include "esp_rom_sys.h"
#include "esp_timer.h"

#define TAG "ESP32_DHT_MQTT"
#define DHT_PIN 4


#define WIFI_SSID     "S24"
#define WIFI_PASS     "12345678"


#define MQTT_BROKER   "mqtt://broker.hivemq.com"
#define MQTT_TOPIC    "esp32/dht112"

static esp_mqtt_client_handle_t mqtt_client;


static int dht11_read(int *temperature, int *humidity)
{
    uint8_t data[5] = {0};

    gpio_set_direction(DHT_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(DHT_PIN, 0);
    vTaskDelay(pdMS_TO_TICKS(20));

    gpio_set_level(DHT_PIN, 1);
    esp_rom_delay_us(40);

    gpio_set_direction(DHT_PIN, GPIO_MODE_INPUT);

    int timeout = 0;
    while (gpio_get_level(DHT_PIN) == 1 && timeout++ < 200) esp_rom_delay_us(1);
    timeout = 0;
    while (gpio_get_level(DHT_PIN) == 0 && timeout++ < 200) esp_rom_delay_us(1);
    timeout = 0;
    while (gpio_get_level(DHT_PIN) == 1 && timeout++ < 200) esp_rom_delay_us(1);

    for (int i = 0; i < 40; i++)
    {
        timeout = 0;
        while (gpio_get_level(DHT_PIN) == 0 && timeout++ < 200) esp_rom_delay_us(1);

        int64_t start = esp_timer_get_time();
        timeout = 0;
        while (gpio_get_level(DHT_PIN) == 1 && timeout++ < 200) esp_rom_delay_us(1);

        if ((esp_timer_get_time() - start) > 50)
            data[i / 8] = (data[i / 8] << 1) | 1;
        else
            data[i / 8] = (data[i / 8] << 1);
    }

    uint8_t checksum = data[0] + data[1] + data[2] + data[3];
    if (checksum == data[4]) {
        *humidity = data[0];
        *temperature = data[2];
        return 0;
    }
    return -1;
}


static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGW(TAG, "WiFi disconnected. Reconnecting...");
        esp_wifi_connect();
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ESP_LOGI(TAG, "WiFi connected");
    }
}

static void wifi_init()
{
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS
        },
    };

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();
}


static void mqtt_event_handler(void *handler_args,
                               esp_event_base_t base,
                               int32_t event_id,
                               void *event_data)
{
    switch (event_id)
    {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT CONNECTED");
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, "MQTT disconnected → Reconnecting...");
            esp_mqtt_client_reconnect(mqtt_client);
            break;

        default:
            break;
    }
}


void app_main()
{
    nvs_flash_init();
    wifi_init();

    
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER,
    };

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);

    int temp, hum;
    char payload[64];

    while (1)
    {
        if (dht11_read(&temp, &hum) == 0)
        {
            sprintf(payload, "{\"temperature\": %d, \"humidity\": %d}", temp, hum);

            ESP_LOGI(TAG, "%s", payload);

            esp_mqtt_client_publish(mqtt_client, MQTT_TOPIC, payload, 0, 1, 0);
        }
        else
        {
            ESP_LOGE(TAG, "DHT11 READ ERROR");
        }

        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
