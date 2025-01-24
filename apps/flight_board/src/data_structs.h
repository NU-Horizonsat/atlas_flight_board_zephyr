#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H

#include <zephyr/zbus/zbus.h>

struct satellite_status {
    int reboot_count;
    bool face0;
    bool face1;
    bool face2;
    bool face3;
    bool face4;
};

struct sensor_data {
    float accel_x;
    float accel_y;
    float accel_z;
    float gyro_x;
    float gyro_y;
    float gyro_z;
    float mag_x;
    float mag_y;
    float mag_z;
    float temp;
};

#endif //DATA_STRUCTS_H
