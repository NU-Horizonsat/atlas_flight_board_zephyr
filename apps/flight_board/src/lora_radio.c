#include "lora_radio.h"
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/lora.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>
#include "data_structs.h"

static const struct device *lora_dev;

int enable_rx() {
    struct lora_modem_config config;
    config.frequency = 437400000;
    config.bandwidth = BW_125_KHZ;
    config.datarate = SF_8;
    config.preamble_len = 8;
    config.coding_rate = CR_4_5;
    config.tx_power = 4;
    config.iq_inverted = false;
    config.public_network = false;
    config.tx = false;

    const int ret = lora_config(lora_dev, &config);

    return ret;
}

int enable_tx() {
    struct lora_modem_config config;
    config.frequency = 437400000;
    config.bandwidth = BW_125_KHZ;
    config.datarate = SF_8;
    config.preamble_len = 8;
    config.coding_rate = CR_4_5;
    config.tx_power = 4;
    config.iq_inverted = false;
    config.public_network = false;
    config.tx = true;

    const int ret = lora_config(lora_dev, &config);
    return ret;
}

int lora_radio_init(void) {
    struct lora_modem_config config;
    lora_dev = DEVICE_DT_GET_ONE(semtech_sx1276);
    if (!device_is_ready(lora_dev)) {
        printk("LoRa device not ready\n");
        return -1;
    }

    int ret = enable_rx();
    printk("LoRa Radio Initialized\n");
    return ret;
}

void lora_receive_cb(const struct device *dev, uint8_t *data, uint16_t size,
                     int16_t rssi, int8_t snr) {
    ARG_UNUSED(dev);
    ARG_UNUSED(size);
    printk("LoRa RX RSSI: %d dBm, SNR: %d dB", rssi, snr);
    LOG_HEXDUMP_INF(data, size, "LoRa RX payload");
}


int lora_send_data(uint8_t *data, uint16_t size) {
    int ret;

    // Disable async receive callback before sending
    ret = lora_recv_async(lora_dev, NULL);
    if (ret < 0) {
        printk("Failed to disable LoRa async receive: %d\n", ret);
        return ret;
    }

    ret = enable_tx();
    if (ret < 0) {
        printk("Failed to enable TX mode: %d\n", ret);
        return ret;
    }

    ret = lora_send(lora_dev, data, size);
    if (ret < 0) {
        printk("Failed to send LoRa packet: %d\n", ret);
        return ret;
    }

    ret = enable_rx();
    if (ret < 0) {
        printk("Failed to enable RX mode: %d\n", ret);
        return ret;
    }

    ret = lora_recv_async(lora_dev, lora_receive_cb);
    if (ret < 0) {
        printk("Failed to reinitialize LoRa async receive: %d\n", ret);
        return ret;
    }

    printk("LoRa data sent successfully, RX mode restored\n");
    return 0;
}

void lora_radio_thread(void *p1, void *p2, void *p3) {
    int ret = lora_radio_init();
    if (ret != 0) {
        printk("Failed to initialize LoRa radio\n");
        return;
    }
    ret = lora_recv_async(lora_dev, lora_receive_cb);
}
