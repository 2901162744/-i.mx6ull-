#ifndef _GETDATA_H
#define _GETDATA_H


int uart_setup(int fd);
int parse_gpsdata (char *buffer, char **gpsdata);
int gps_init();
#endif
