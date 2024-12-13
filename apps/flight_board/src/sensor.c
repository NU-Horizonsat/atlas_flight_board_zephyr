#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>

static const struct device *lsm6dso_dev;
static const struct device *lis2mdl_dev;

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

    return 0;
}

void sensor_thread(void *p1, void *p2, void *p3)
{
    int ret = lsm6dso_init();
    if (ret != 0) {
        printk("Failed to initialize LSM6DSO\n");
        return;
    }

    ret = lis2mdl_init();
    if (ret != 0) {
        printk("Failed to initialize LIS2MDL\n");
        return;
    }


    struct sensor_value x, y, z, temp;

    while (1) {
        sensor_sample_fetch_chan(lsm6dso_dev, SENSOR_CHAN_ACCEL_XYZ);
        sensor_channel_get(lsm6dso_dev, SENSOR_CHAN_ACCEL_X, &x);
        sensor_channel_get(lsm6dso_dev, SENSOR_CHAN_ACCEL_Y, &y);
        sensor_channel_get(lsm6dso_dev, SENSOR_CHAN_ACCEL_Z, &z);

        // printk("LSM6DSO accel x: %f ms^2, y: %f ms^2, z: %f ms^2\n",
        //        (double)out_ev(&x), (double)out_ev(&y), (double)out_ev(&z));

        sensor_sample_fetch_chan(lsm6dso_dev, SENSOR_CHAN_GYRO_XYZ);
        sensor_channel_get(lsm6dso_dev, SENSOR_CHAN_GYRO_X, &x);
        sensor_channel_get(lsm6dso_dev, SENSOR_CHAN_GYRO_Y, &y);
        sensor_channel_get(lsm6dso_dev, SENSOR_CHAN_GYRO_Z, &z);

        // printk("LSM6DSO gyro x: %f rad/s, y: %f rad/s, z: %f rad/s\n",
        //        (double)out_ev(&x), (double)out_ev(&y), (double)out_ev(&z));

        sensor_sample_fetch_chan(lis2mdl_dev, SENSOR_CHAN_MAGN_XYZ);
        sensor_channel_get(lis2mdl_dev, SENSOR_CHAN_MAGN_X, &x);
        sensor_channel_get(lis2mdl_dev, SENSOR_CHAN_MAGN_Y, &y);
        sensor_channel_get(lis2mdl_dev, SENSOR_CHAN_MAGN_Z, &z);

        // printk("LIS2MDL mag x: %f gauss, y: %f gauss, z: %f gauss\n",
        //        (double)out_ev(&x), (double)out_ev(&y), (double)out_ev(&z));

        sensor_sample_fetch_chan(lsm6dso_dev, SENSOR_CHAN_DIE_TEMP);
        sensor_channel_get(lsm6dso_dev, SENSOR_CHAN_DIE_TEMP, &temp);

        // printk("LSM6DSO temperature: %f °C\n", (double)out_ev(&temp));

        k_sleep(K_SECONDS(1));
    }
}
