#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>
#include <zephyr/device.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/zbus/zbus.h>
#include "usb_serial.h"
#include "lora_radio.h"
#include "sensor.h"
LOG_MODULE_REGISTER(main);

#define USB_STACK_SIZE 1024
#define LORA_STACK_SIZE 1024
#define SENSOR_STACK_SIZE 1024

K_THREAD_STACK_DEFINE(usb_stack_area, USB_STACK_SIZE);
K_THREAD_STACK_DEFINE(lora_stack_area, LORA_STACK_SIZE);
K_THREAD_STACK_DEFINE(sensor_stack_area, SENSOR_STACK_SIZE);

struct k_thread usb_thread_data;
struct k_thread lora_thread_data;
struct k_thread sensor_thread_data;

void main(void)
{
    printk("Starting main loop with threads...\n");

    k_thread_create(&usb_thread_data, usb_stack_area, K_THREAD_STACK_SIZEOF(usb_stack_area),
                    usb_serial_thread, NULL, NULL, NULL, 5, 0, K_NO_WAIT);

    k_thread_create(&lora_thread_data, lora_stack_area, K_THREAD_STACK_SIZEOF(lora_stack_area),
                    lora_radio_thread, NULL, NULL, NULL, 5, 0, K_NO_WAIT);

    k_thread_create(&sensor_thread_data, sensor_stack_area, K_THREAD_STACK_SIZEOF(sensor_stack_area),
                    sensor_thread, NULL, NULL, NULL, 5, 0, K_NO_WAIT);

    // while (true) {
    //     struct sensor_data sensor_data_channel_msg;
    //     zbus_chan_read(&sensor_data_chan, &sensor_data_channel_msg,K_MSEC(500));
    //     printk("Accel: %f %f %f\n", sensor_data_channel_msg.accel_x, sensor_data_channel_msg.accel_y, sensor_data_channel_msg.accel_z);
    //     printk("Gyro: %f %f %f\n", sensor_data_channel_msg.gyro_x, sensor_data_channel_msg.gyro_y, sensor_data_channel_msg.gyro_z);
    //     printk("Mag: %f %f %f\n", sensor_data_channel_msg.mag_x, sensor_data_channel_msg.mag_y, sensor_data_channel_msg.mag_z);
    //     printk("Temp: %f\n", sensor_data_channel_msg.temp);
    // }
}
