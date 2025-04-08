#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include "list.h"
#include "queue.h"
#include "getdata.h"
#include "freetype.h"
#include "mqtt.h"
#define ENTER_CRITICAL_AREA(mutex)  do{pthread_mutex_lock(mutex);}while(0)
#define QUIT_CRITICAL_AREA(mutex)  do{pthread_mutex_unlock(mutex);}while(0)

unsigned pthread_index;
int fd = 0;
char buffer[1024];
LIST_LINK *end_list = NULL;
sem_t show_sem, sock_sem;
//typedef struct thread_node
//{
//    pthread_t tid;         //线程id号
//    char name[256];        //线程名字 ,必须唯一
//    Que *amil_head, *mail_tail;
//    th_fun th;              //线程函数

//}TH_NODE;

typedef struct mail_box_system
{
    pthread_mutex_t mutex;  //保护邮件系统
    LIST_LINK *thread_list;
}MBS;
MBS* mbs;

int send_msg(MBS*msb,char*recvname,DATATYPE data);
int recv_msg(MBS*msb,char*sendname,DATATYPE *data);

MBS* create_mail_box_system()
{
    MBS *temp =(MBS*)malloc(sizeof(MBS));

    if(NULL ==  temp)
    {
        perror("create_mail_box_system mutex malloc failure\n");
        return NULL;
    }
    int ret = pthread_mutex_init(&temp->mutex,NULL);
    if(0 != ret)
    {
        perror("create_mail_box_system mutex init failure\n");
        return NULL;
    }
    temp->thread_list = malloc(sizeof(LIST_LINK));
   // memset(temp->thread_list, 0, sizeof(LIST_LINK));
    temp->thread_list->next = NULL;
    printf("mail box create ok!! \n");
    return temp;
}

int destroy_mail_box_system(MBS*mbs)
{
    pthread_mutex_destroy(&mbs->mutex);
    LIST_LINK *temp = NULL;
    LIST_LINK *find = mbs->thread_list;
    while(find !=  NULL)
    {
        temp = find;
        find = find->next;
        free(temp);
    }
    free(mbs);
    return 0;
}

int  register_to_mail_system(MBS *mbs,char name[],th_fun th)
{
    LIST_LINK* temp =  malloc(sizeof(LIST_LINK));
    if(NULL == temp)
    {
        perror("register to mail malloc  \n");
        return -1;
    }
    strcpy(temp->elem.name ,name);
    temp->elem.th = th;
    init_que(temp);

    list_add(mbs->thread_list, temp);
    pthread_t ret = pthread_create(&temp->elem.tid,NULL,th,NULL);
    if(0!=ret)
    {
        perror("register to mail thread create\n");
        return -1;
    }

    printf("register mail system  |%s|  ok \n", temp->elem.name);

    return 0;
}

int unregister_from_mailbox(MBS*msb,char*name)
{
    LIST_LINK* find=msb->thread_list->next;
    LIST_LINK *temp = msb->thread_list;
	
    while(find !=  NULL)
    {
//		printf("================%s\n", find->elem.name);
        if(0 == strcmp(find->elem.name ,name))
        {
            destroy(find);
			pthread_cancel(find->elem.tid);
//			printf("cancel tid = %ld\n", find->elem.tid);
			pthread_join(find->elem.tid, NULL);
			temp->next=find->next;
            free(find);
            return 0;
        }
		else
		{
			temp = find;
			find = find->next;
		}
    }

    return -1;
}

int wait_all_end(MBS*msb)
{
    LIST_LINK *find=msb->thread_list->next;
    LIST_LINK *end=end_list;
    while(find != end)
    {
        // pthread_join(find,NULL);

        pthread_join(find->elem.tid,NULL);
        find = find->next;
    }
   
    return 0;
}
void refresh_lcd(DATATYPE data, DATATYPE data_copy)
{
	
	if(data.utc_8.hour != data_copy.utc_8.hour)
		copy_mem(120, 40, 173, 40); 
	
	if(data.utc_8.min != data_copy.utc_8.min)
		copy_mem(173, 40, 226, 40); 
	
	if(data.utc_8.seconds != data_copy.utc_8.seconds)
	{	
		copy_mem(200, 40, 60, 40); 
      //copy_mem(120, 40, 280, 40); 
	}
	if(data.N_S != data_copy.N_S)
		copy_mem(120, 120, 60, 40); 
	
	copy_mem(220, 200, 260, 40);
	if(data.E_W != data_copy.E_W)
		copy_mem(120, 280, 60, 40); 
	copy_mem(220, 360, 260, 40);
}
double *data_parse(double* data) 
{
    double temp = fabs(*data); // 处理负数（如南纬）
    int degrees = (int)temp / 100; // 3146.1234 / 100 = 31.461234 → 取整为31
    double minutes = fmod(temp, 100.0); // 3146.1234 % 100 = 46.1234
    *data = degrees + minutes / 60.0;
    if (*data < 0) 
	{
        *data = -(*data);
    }
    
    return data;
}
void time_parse(UTC_8 * utc_8, double utc_time)
{
	int time_temp;
	time_temp = (int)utc_time;
	utc_8->seconds = time_temp % 100;
	time_temp = time_temp / 100;
	utc_8->min = time_temp % 100;
	time_temp = time_temp / 100;
	utc_8->hour = time_temp % 100 + 8;
}
void* data_collect_th(void* arg)
{
	int ret = 0;
	int i = 0;
	char *gpsdata[6];
	char *endptr_utc, *endptr_lat, *endptr_long;
	while(1)
    {
        printf("this is the data th\n");
		memset(buffer, 0, sizeof(buffer));
        ret = read(fd, buffer, sizeof(buffer) -1);
		if (ret < 1) 
		{
            printf("read fail, ret is %d\r\n", ret);
			return NULL;
        } 
        else 
		{
			buffer[ret] = '\0';
		    if(parse_gpsdata(buffer, gpsdata) == 0)
		    {
		    	for(i = 0; i < 6; i++)
		    	{
		    		printf("gpsdata[%d]: %s\n",i, gpsdata[i]);
		    	}
		    }
			
		}	
		DATATYPE data;
		data.id = 1;
		data.utc_time = strtod(gpsdata[0], &endptr_utc);//string to double	
		time_parse(&data.utc_8, data.utc_time);//时间处理
		data.latitude_data = strtod(gpsdata[1], &endptr_lat);
		//纬度处理
		data.latitude_data = *data_parse(&data.latitude_data);
		data.N_S = *gpsdata[2];
		data.longitude_data = strtod(gpsdata[3], &endptr_long);
		//经度处理
		data.longitude_data = *data_parse(&data.longitude_data);
		data.E_W = *gpsdata[4];
		data.valadity = *gpsdata[5];
        send_msg(mbs,"show", data);
        sem_post(&show_sem);
		send_msg(mbs,"sock", data);
		sem_post(&sock_sem);
    }
    return NULL;
}

void* show_th(void* arg) 
{
    save_mem();
    wchar_t time_wstr[50], N_Swstr[50], latitude_wstr[50], E_Wwstr[50], longitude_wstr[50];
	int flag = 0; 
	int time_past = 0;
	DATATYPE data_copy;
    while(1) 
	{
        char sendname[256];
        char time_str[100], N_Sstr[100], latitude_datastr[100], E_Wstr[100], longitude_datastr[100];
        sem_wait(&show_sem);
		
        // 时间处理
        DATATYPE data;
        recv_msg(mbs, sendname, &data);
        if(time_past > 59) time_past = 0;	
		if(flag == 0 || (time_past == data.utc_8.seconds))
		{
			flag = 1;
			time_past = data.utc_8.seconds + 1;
			refresh_lcd(data, data_copy);
			data_copy = data;
			snprintf(time_str, sizeof(time_str), "UTC: %d:%d:%d", data.utc_8.hour, data.utc_8.min, data.utc_8.seconds);
			snprintf(N_Sstr, sizeof(N_Sstr), "Lat: %c", data.N_S);
			snprintf(latitude_datastr, sizeof(latitude_datastr), "Lat_data: %lf°", data.latitude_data);
			snprintf(E_Wstr, sizeof(E_Wstr), "Lon: %c", data.E_W);
			snprintf(longitude_datastr, sizeof(longitude_datastr), "Lon_data: %lf°", data.longitude_data);

			// 转换为宽字符
			mbstowcs(time_wstr, time_str, sizeof(time_str));
			mbstowcs(N_Swstr, N_Sstr, sizeof(N_Sstr));
			mbstowcs(latitude_wstr, latitude_datastr, sizeof(latitude_datastr));
			mbstowcs(E_Wwstr, E_Wstr, sizeof(E_Wstr));
			mbstowcs(longitude_wstr, longitude_datastr, sizeof(longitude_datastr));

			// 绘制所有数据
			lcd_draw_character(30, 80,   time_wstr, 0x123456);
			lcd_draw_character(30, 160,  N_Swstr, 0x00FF00);
			lcd_draw_character(30, 240,  latitude_wstr, 0xFF0000);
			lcd_draw_character(30, 320,  E_Wwstr, 0x777777);
			lcd_draw_character(30, 400,  longitude_wstr, 0xFFFF00);
 
		}
		// 刷新指定区域

    }
    return NULL;
}
void* sock_th(void* arg)
{
    while(1)
    {
		sem_wait(&sock_sem);
        printf("this is the sock th\n");
        DATATYPE data;
        char sendname[256];
        recv_msg(mbs,sendname,&data);
        printf("sock tids= %ld, from:%s time:%lf  %c:%lf %c:%lf\n", pthread_self(), sendname, data.utc_time, data.N_S, data.latitude_data, data.E_W, data.longitude_data);
		mqtt_send("gps_identifiers", data.latitude_data);	
		mqtt_send("gps_longtitude", data.longitude_data);	

	}
    return NULL;
}

char *get_th_name(MBS*msb)
{
    pthread_t tid = pthread_self();
    LIST_LINK *find = msb->thread_list->next;
    LIST_LINK *end = end_list;
    while(find != end)
    {
        if(find->elem.tid == tid)
            break;
        find = find->next;
    }
    if(find != NULL && find->elem.tid == tid)
    {
        //printf("cant find the recv th\n");
        return find->elem.name;
    }
    else
        return NULL;
}

int send_msg(MBS*msb, char*recvname, DATATYPE data)
{
    MAIL_DATA* temp =  malloc(sizeof(MAIL_DATA));

    //strcpy(temp->data, data);
	memcpy(&(temp->data), &data, sizeof(DATATYPE));

    temp->id_of_sender = pthread_self();

    LIST_LINK *find = list_for_each(msb->thread_list, recvname);
    if (find == NULL)
    {
        printf("can,t find msg \n");
		return -1;
    }

    char* name = get_th_name(msb);
    strcpy(temp->name_of_sender,name);
    strcpy(temp->name_of_recver,recvname);
    ENTER_CRITICAL_AREA(&msb->mutex);
    in_queue(find, temp);
    QUIT_CRITICAL_AREA(&msb->mutex);
//    printf("send msg is ok |%s| msg is %s\n", temp->name_of_recver, temp->data);
    return 0;
}

int recv_msg(MBS*msb,char*sendname,DATATYPE *data)
{
    MAIL_DATA* temp =  malloc(sizeof(MAIL_DATA));
    pthread_t tid =  pthread_self();

    LIST_LINK *find = msb->thread_list->next;

    while(find != NULL)
    {
        if( find->elem.tid == tid)
            break;
        find = find->next;
    }

    if( find != NULL && find->elem.tid == tid)
    {
        while (1)
        {
            if(find->elem.mail_head != find->elem.mail_tail)
            {
                ENTER_CRITICAL_AREA(&msb->mutex);
                out_queue(find, temp);
                QUIT_CRITICAL_AREA(&msb->mutex);
                break;
            }
        }
    }

    strcpy(sendname, temp->name_of_sender);
    memcpy(data, &(temp->data), sizeof(DATATYPE));

    free(temp);

    return 0;

}
int main()
{    
	int ret;
	sem_init(&show_sem, 0, 1);
	sem_init(&sock_sem, 0, 1);
	fd = gps_init();
	if(fd < 0)
	{
		printf("gps_init fail()\n");
		return fd;
	}
	ret = uart_setup(fd);
	ret = fb_dev_init();
	if(ret < 0)
	{
		printf("fb_dev_init fail()\n");
		return ret;
	}
	freetype_init("SIMSUN.TTC", 0);
	mqtt_init();

	mbs = create_mail_box_system();

    register_to_mail_system(mbs,"show",show_th);
    register_to_mail_system(mbs,"sock",sock_th);
    register_to_mail_system(mbs,"data",data_collect_th);

    wait_all_end(mbs);
    destroy_mail_box_system(mbs);

    printf("Hello World!");
    return 0;
}

