#include "queue.h"


/*
typedef	 int*  pi;
#define	p	int*
pi a, b;
p  c, d;

Que* head, tail;
*/

//pQue head, tail;
Que *head, *tail;

int init_que(LIST_LINK *list_head)
{
	Que* tmp = malloc(sizeof(Que));
	tmp->next = NULL;
	
    list_head->elem.mail_head = list_head->elem.mail_tail = tmp;

	return 0;
}

int in_queue(LIST_LINK *list_head, MAIL_DATA *data)
{
	Que* tmp = malloc(sizeof(Que));
    memcpy(&tmp->data, data, sizeof(MAIL_DATA));
	tmp->next = NULL;
	
    list_head->elem.mail_tail->next = tmp;
    list_head->elem.mail_tail = list_head->elem.mail_tail->next;
	
	return 0;
}

int out_queue(LIST_LINK *list_head, MAIL_DATA *data)
{
    if(list_head->elem.mail_head == list_head->elem.mail_tail){
		printf("queue is empty. \n");
		return -1;
	}

    if(list_head->elem.mail_head->next == list_head->elem.mail_tail){
        list_head->elem.mail_tail = list_head->elem.mail_head;
	}
    Que* del = list_head->elem.mail_head->next;
    list_head->elem.mail_head->next = del->next;

    memcpy(data, &del->data, sizeof(MAIL_DATA));
	free(del);
	
    return 0;
}

void destroy(LIST_LINK *list_head)
{
    Que* tmp = list_head->elem.mail_head;

    while(list_head->elem.mail_head){
        tmp = list_head->elem.mail_head;
        list_head->elem.mail_head = list_head->elem.mail_head->next;
		free(tmp);		
	}
}










