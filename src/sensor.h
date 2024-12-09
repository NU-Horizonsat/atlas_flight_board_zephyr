#ifndef SENSOR_H
#define SENSOR_H

int lsm6dso_init(void);
void sensor_thread(void *p1, void *p2, void *p3);

#endif
