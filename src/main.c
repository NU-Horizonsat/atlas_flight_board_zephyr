#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include <string.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/drivers/uart.h>

static const struct device *usb_device;

static int serial_init(void)
{
    uint32_t dtr = 0;

    usb_device = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));

    if (usb_enable(NULL) != 0) {
        return -1;
    }

    while (!dtr) {
        uart_line_ctrl_get(usb_device, UART_LINE_CTRL_DTR, &dtr);
        k_sleep(K_MSEC(100));
    }

    return 0;
}

void main(void)
{
    if (serial_init() != 0) {
        return;
    }

    while (1) {
        printk("Hello space\n");
        k_sleep(K_MSEC(1000));
    }
}