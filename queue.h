#ifndef __QUEUE_H__
#define __QUEUE_H__
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "list.h"




extern int init_que(LIST_LINK *list_head);
extern int in_queue(LIST_LINK *list_head, MAIL_DATA *data);
extern int out_queue(LIST_LINK *list_head, MAIL_DATA *data);
extern void destroy(LIST_LINK *list_head);

#endif
