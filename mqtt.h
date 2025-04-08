#ifndef HEAD_H
#define HEAD_H


#include <MQTTAsync.h>
#include <MQTTClient.h>

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include <string.h>
//#define OLD_ADDRESS     "tcp://218.201.45.7:1883"
#define NEW_ADDRESS     "tcp://183.230.40.96:1883"
#define DEV_NAME    "gps"
//#define DEV_NAME    "dht11"
#define CLIENTID    DEV_NAME
#define PRODUCT_ID "q12vm8uRGs"
#define PASSWD "version=2018-10-31&res=products%2Fq12vm8uRGs%2Fdevices%2Fgps&et=1837255523&method=md5&sign=5VubqepwfLQObSaULJ2kuQ%3D%3D"
//#define PASSWD "version=2018-10-31&res=products%2FM5p1gD90Qj%2Fdevices%2Fdht11&et=1837255523&method=md5&sign=2Ga9FE%2BnQf8sMplgQLxBxw%3D%3D"
#define QOS         0
#define TIMEOUT     10000L
//#define __cplusplus


void pack_topic(char * dev_name, char * pro_id);
void delivered(void *context, MQTTClient_deliveryToken dt);
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message);
void connlost(void *context, char *cause);
int mqtt_init();
int mqtt_send(char * key, int value);
void mqtt_deinit();

#endif // HEAD_H
