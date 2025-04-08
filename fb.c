#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <math.h>
#include <string.h>
#include "fb.h"
static struct fb_var_screeninfo vinfo;
static int fd;
static int size = 0;
unsigned char *screen_base = NULL;

int fb_init()
{
	fd = open("/dev/fb0", O_RDWR);	
	if(fd < 0)
	{
		perror("open fb0 fail");
		return -1;
	}
	int ret = ioctl(fd, FBIOGET_VSCREENINFO, &vinfo);	
	if(-1 == ret)
	{
		perror("ioctl err");
		return -2;
	}
	printf("xres = %d yres = %d\n", vinfo.xres, vinfo.yres);
	printf("xres_virtual = %d yres_virtual = %d\n",vinfo.xres_virtual, vinfo.yres_virtual);
	printf("bits_per_pixel = %d\n", vinfo.bits_per_pixel);
	printf("%d,%d,%d\n",vinfo.red.length,vinfo.green.length,vinfo.blue.length);

	size = vinfo.xres_virtual * vinfo.yres_virtual * vinfo .bits_per_pixel / 8;
	screen_base = (unsigned char *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(NULL == screen_base)
	{
		perror("mmap error");
		return -3;
	}
	return 0;
}
void draw_point(int x, int y,unsigned int col)
{
	if(x > vinfo.xres_virtual || y > vinfo.yres_virtual )
		return ;
	*((unsigned int *)screen_base + y * vinfo.xres_virtual + x) = col;
}
void draw_x_line(int x, int y, int len, unsigned int col)
{
	int i = 0;
	for(i = 0; i < len; i++)
	{
		draw_point(x + i, y, col);
	}
}
void draw_y_line(int x, int y, int len, unsigned int col)
{
	int i = 0;
	for(i = 0; i < len; i++)
	{
		draw_point(x, y + i, col);
	}
}

#define pi 3.1415926
void draw_cicle(int x0, int y0, int r, unsigned int col)
{
	float seita = 0;
	int x = 0, y = 0;
	for(seita = 0; seita < 360; seita += 0.1)
	{
		x = x0 + r * cos(2 * pi * seita / 360.0);
		y = y0 + r * sin(2 * pi * seita / 360.0);
		draw_point(x, y, col);
	}
}
void fb_close()
{
	munmap(screen_base, size);
	close(fd);
}

#pragma pack(4)
int show_bmp(const char *filename, int x0, int y0)
{
	BITMAPFILEHEADER file_head;
	BITMAPINFOHEADER info_head;
	int fd = open(filename, O_RDONLY);
	if(fd < 0)
	{
		perror("open bmp file");
		return -1;
	}
	read(fd, &file_head, sizeof(file_head));
	read(fd, &info_head, sizeof(info_head));
	
	int size = info_head.biHeight * info_head.biWidth * 3;
	unsigned char * p = (unsigned char *)malloc(size);
	read(fd, p, size);
	unsigned char * p1 = p;
	int i = 0, j = 0;
	for(j = 0; j < info_head.biHeight; j++)
	{
		for(i = 0; i < info_head.biWidth; i++)
		{
			color c;
			c.col = 0;
			c.rgb.b = *p1++;
			c.rgb.g = *p1++;
			c.rgb.r = *p1++;
			draw_point(x0 + i, y0 + info_head.biHeight - j, c.col);
		}
	}

	free(p);
	p = NULL;
	close(fd);
	return 0;
}

int main(int argc, const char *argv[])
{
	fb_init();
//	draw_point(7, 7, 0x00FF0000);	
//	draw_x_line(7, 7, 500, 0x00FF0000);
//	draw_y_line(7, 7, 400, 0x00FF0000);
	memset(screen_base, 0x0000FF00, size);
//	draw_cicle(200, 200, 100, 0x000000FF);
	show_bmp("./1.bmp", 0, 0);
	fb_close();
	return 0;
}
