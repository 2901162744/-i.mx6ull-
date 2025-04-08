#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <termios.h>
#include <string.h>
#include <stdlib.h>
#include "getdata.h"
/* 38400, 8, N, 1 */

int uart_setup(int fd)
{
    struct termios options;

    // 获取原有串口配置
    if  (tcgetattr(fd, &options) < 0) {
        return -1;
    }

    // 修改控制模式，保证程序不会占用串口
    options.c_cflag  |=  CLOCAL;

    // 修改控制模式，能够从串口读取数据
    options.c_cflag  |=  CREAD;

    // 不使用流控制
    options.c_cflag &= ~CRTSCTS;

    // 设置数据位
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    // 设置奇偶校验位
    options.c_cflag &= ~PARENB;
    options.c_iflag &= ~INPCK; 

    // 设置停止位
    options.c_cflag &= ~CSTOPB;

    // 设置最少字符和等待时间
    options.c_cc[VMIN] = 1;     // 读数据的最小字节数
    options.c_cc[VTIME]  = 10;   //等待第1个数据，单位是10s
    
    // 修改输出模式，原始数据输出
    options.c_oflag &= ~OPOST;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    // 设置波特率
    cfsetispeed(&options, B38400); 
	cfsetospeed(&options, B38400);
    // 清空终端未完成的数据
    tcflush(fd, TCIFLUSH);

    // 设置新属性
    if(tcsetattr(fd, TCSANOW, &options) < 0) {
        return -1;
    }

    return 0;
}
int parse_gpsdata (char *buffer, char **gpsdata) 
{
    // 格式：$GNGGA,UTC时间,纬度,纬度半球,经度,经度半球,定位状态,卫星数,HDOP,海拔,M,大地水准面高度,M,差分时间,差分站号*校验和
	int i = 0;
	char *token;
//	char copy[256];
//	strncpy(copy, buffer,sizeof(copy));
	token = strstr(buffer, "$GNGGA");
	if(token == NULL)
	{
//		printf("NULL\n");
		return -1;
	}
    token = strtok(token, ",");
//	printf("token:%s\n",token);
    if (token == NULL || strcmp(token, "$GNGGA") != 0) 
	{
    //    token = strtok(NULL, "\r\n");
	//	token = strtok(NULL, ",");
	//	printf("if:%s\n", token);
	//	sleep(1);
			return -1;
	}	

	for(i = 0; i < 6; i++)
	{
		gpsdata[i] = strtok(NULL, ",");
		
//		printf("------------\n");
//		printf("gpsdata[%d]: %s\n",i,gpsdata[i]);
	}
	return 0;
#if 0
    // 提取关键字段
    char *utc_time = strtok(NULL, ",");  // UTC时间
    char *latitude = strtok(NULL, ",");  // 纬度
    char *lat_dir = strtok(NULL, ",");   // 纬度方向（N/S）
    char *longitude = strtok(NULL, ","); // 经度
    char *lon_dir = strtok(NULL, ",");   // 经度方向（E/W）
    char *fix_quality = strtok(NULL, ","); // 定位质量（0=无效，1=有效）

//    if (fix_quality && atoi(fix_quality) > 0) {
        printf("[GPS有效数据]\n");
        printf("UTC时间: %s\n", utc_time);
        printf("纬度: %s° %s\n", latitude, lat_dir);
        printf("经度: %s° %s\n", longitude, lon_dir);
//    } else {
//        printf("[警告] GPS信号无效\n");
//    }
#endif
}
int gps_init()
{
	int fd;
    /* 打开串口 */
    fd = open("/dev/ttymxc2", O_RDWR);
    if (fd < 0) {
        printf("open dev fail!\n");
        return -1;
    } else {
        fcntl(fd, F_SETFL, 0);
    }
	return fd;
}

//int main(int argc, char *argv[])
//{
//    int fd;
//    int ret;
//    int i = 0;
//	char buffer[1024];
//	char *gpsdata[6];
//
//	fd = gps_init();
//	if(fd < 0)
//		return fd;
//    /* 设置串口 */
//    ret = uart_setup(fd);
//    if (ret < 0) {
//        printf("uart setup fail!\n");
//        close(fd);
//        return -1;    }
//    /* 串口回传实验 */
//    while (1) 
//	{
//		memset(buffer, 0, sizeof(buffer));
//        ret = read(fd, buffer, sizeof(buffer) -1);
//		printf("buffer:%s\n",buffer);
//        if (ret < 1) 
//		{
//            printf("read fail, ret is %d\r\n", ret);
//        } 
//		else 
//		{
////			buffer[ret] = '\0';
//		    if(parse_gpsdata(buffer, gpsdata) == 0)
//			{
//				for(i = 0; i < 6; i++)
//				{
//					printf("gpsdata[%d]: %s\n",i, gpsdata[i]);
//				}
//			}
//		}
//    }
//
//    close(fd);
//}

