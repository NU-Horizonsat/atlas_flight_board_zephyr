#include "lora_radio.h"
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/lora.h>

static const struct device *lora_dev;

int lora_radio_init(void)
{
    struct lora_modem_config config;
    lora_dev = DEVICE_DT_GET_ONE(semtech_sx1276);
    if (!device_is_ready(lora_dev)) {
        printk("LoRa device not ready\n");
        return -1;
    }

    config.frequency = 433000000;
    config.bandwidth = BW_125_KHZ;
    config.datarate = SF_10;
    config.preamble_len = 8;
    config.coding_rate = CR_4_5;
    config.tx_power = 4;
    config.iq_inverted = false;
    config.public_network = false;
    config.tx = true;

    int ret = lora_config(lora_dev, &config);
    if (ret < 0) {
        printk("LoRa configuration failed: %d\n", ret);
        return ret;
    }

    printk("LoRa Radio Initialized\n");
    return 0;
}

void lora_radio_thread(void *p1, void *p2, void *p3)
{
    int ret = lora_radio_init();
    if (ret != 0) {
        printk("Failed to initialize LoRa radio\n");
        return;
    }

    uint8_t tx_buf[32];
    int tx_counter = 0;

    while (1) {
        snprintf(tx_buf, sizeof(tx_buf), "LoRa %d", tx_counter++);
        // printk("Sending: %s\n", tx_buf);

        ret = lora_send(lora_dev, tx_buf, strlen(tx_buf));
        if (ret < 0) {
            printk("LoRa send failed: %d (errno: %d)\n", ret, errno);
        } else {
            // printk("LoRa send success\n");
        }

        k_sleep(K_SECONDS(5));
    }
}
