#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>
#include "data_structs.h"

static const struct device *lsm6dso_dev;
static const struct device *lis2mdl_dev;

ZBUS_CHAN_DEFINE(sensor_data_chan, struct sensor_data, NULL, NULL, ZBUS_OBSERVERS_EMPTY, ZBUS_MSG_INIT(0));

float out_ev(struct sensor_value *val)
{
    return (val->val1 + (float)val->val2 / 1000000);
}

int lsm6dso_init(void) {
    lsm6dso_dev = DEVICE_DT_GET_ONE(st_lsm6dso);
    if (!device_is_ready(lsm6dso_dev)) {
        printk("LSM6DSO device not ready\n");
        return -1;
    }

    int ret = 0;
    struct sensor_value odr_attr;

    odr_attr.val1 = 12;
    odr_attr.val2 = 500000; // 12.5 Hz

    ret = sensor_attr_set(lsm6dso_dev, SENSOR_CHAN_ACCEL_XYZ,
            SENSOR_ATTR_SAMPLING_FREQUENCY, &odr_attr);
    if (ret != 0) {
        printk("Cannot set sampling frequency for accelerometer.\n");
        return ret;
    }

    ret = sensor_attr_set(lsm6dso_dev, SENSOR_CHAN_GYRO_XYZ,
            SENSOR_ATTR_SAMPLING_FREQUENCY, &odr_attr);
    if (ret != 0) {
        printk("Cannot set sampling frequency for gyro.\n");
        return ret;
    }
    printk("LSM6DSO device ready\n");
    return 0;
}

int lis2mdl_init(void) {
    lis2mdl_dev = device_get_binding("LIS2MDL");
    if (!lis2mdl_dev) {
        printk("LIS2MDL device not found\n");
        return -1;
    }

    if (!device_is_ready(lis2mdl_dev)) {
        printk("LIS2MDL device not ready\n");
        return -1;
    }
    printk("LIS2MDL device ready\n");
    return 0;
}

void sensor_thread(void *p1, void *p2, void *p3)
{
    int ret = 0;
    ret = lsm6dso_init();
    if (ret != 0) return;

    ret = lis2mdl_init();
    if (ret != 0) return;

    struct sensor_value x, y, z, temp;
    struct sensor_data sensor_payload;

    while (1) {
        // Fetch accelerometer data
        sensor_sample_fetch_chan(lsm6dso_dev, SENSOR_CHAN_ACCEL_XYZ);
        sensor_channel_get(lsm6dso_dev, SENSOR_CHAN_ACCEL_X, &x);
        sensor_channel_get(lsm6dso_dev, SENSOR_CHAN_ACCEL_Y, &y);
        sensor_channel_get(lsm6dso_dev, SENSOR_CHAN_ACCEL_Z, &z);

        sensor_payload.accel_x = out_ev(&x);
        sensor_payload.accel_y = out_ev(&y);
        sensor_payload.accel_z = out_ev(&z);

        // Fetch gyroscope data
        sensor_sample_fetch_chan(lsm6dso_dev, SENSOR_CHAN_GYRO_XYZ);
        sensor_channel_get(lsm6dso_dev, SENSOR_CHAN_GYRO_X, &x);
        sensor_channel_get(lsm6dso_dev, SENSOR_CHAN_GYRO_Y, &y);
        sensor_channel_get(lsm6dso_dev, SENSOR_CHAN_GYRO_Z, &z);

        sensor_payload.gyro_x = out_ev(&x);
        sensor_payload.gyro_y = out_ev(&y);
        sensor_payload.gyro_z = out_ev(&z);

        // Fetch magnetometer data
        sensor_sample_fetch_chan(lis2mdl_dev, SENSOR_CHAN_MAGN_XYZ);
        sensor_channel_get(lis2mdl_dev, SENSOR_CHAN_MAGN_X, &x);
        sensor_channel_get(lis2mdl_dev, SENSOR_CHAN_MAGN_Y, &y);
        sensor_channel_get(lis2mdl_dev, SENSOR_CHAN_MAGN_Z, &z);

        sensor_payload.mag_x = out_ev(&x);
        sensor_payload.mag_y = out_ev(&y);
        sensor_payload.mag_z = out_ev(&z);

        // Fetch temperature data
        sensor_sample_fetch_chan(lsm6dso_dev, SENSOR_CHAN_DIE_TEMP);
        sensor_channel_get(lsm6dso_dev, SENSOR_CHAN_DIE_TEMP, &temp);

        sensor_payload.temp = out_ev(&temp);

        // Publish sensor data to zbus channel
        ret = zbus_chan_pub(&sensor_data_chan, &sensor_payload, K_NO_WAIT);
        if (ret != 0) {
            printk("Failed to publish sensor data (error: %d)\n", ret);
        } else {
            printk("Published sensor data: accel_x=%.2f, accel_y=%.2f, accel_z=%.2f\n",
                   sensor_payload.accel_x, sensor_payload.accel_y, sensor_payload.accel_z);
        }

        k_sleep(K_SECONDS(1));
    }
}
