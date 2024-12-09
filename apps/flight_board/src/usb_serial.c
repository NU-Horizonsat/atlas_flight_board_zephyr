#include "usb_serial.h"
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/drivers/uart.h>

static const struct device *usb_device;

int serial_init(void)
{
    uint32_t dtr = 0;

    usb_device = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));

    if (usb_enable(NULL) != 0) {
        printk("Failed to enable USB\n");
        return -1;
    }

    while (!dtr) {
        uart_line_ctrl_get(usb_device, UART_LINE_CTRL_DTR, &dtr);
        k_sleep(K_MSEC(100));
    }

    printk("USB Serial Initialized\n");
    return 0;
}

void usb_serial_thread(void *p1, void *p2, void *p3)
{
    int ret = serial_init();
    if (ret != 0) {
        printk("USB Serial initialization failed\n");
        return;
    }

    printk("USB Serial Initialized successfully\n");
}