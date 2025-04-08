#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <math.h> //数学库函数头文件

#include <wchar.h>
#include <ft2build.h>
#include "freetype.h"
#include FT_FREETYPE_H

static unsigned int width;                 //LCD宽度
static unsigned int height;                //LCD高度
static unsigned int *screen_base = NULL; //LCD显存基地址 RGB565
static unsigned long screen_size;
static unsigned char *pmem;
static FT_Library library;
static FT_Face face;

struct fb_var_screeninfo fb_var;
struct fb_fix_screeninfo fb_fix;

int fd_fb = -1;

int fb_dev_init(void)
{

    /* 打开framebuffer设备 */
    fd_fb = open("/dev/fb0", O_RDWR);
    if (0 > fd_fb)
    {
        return -1;
    }

    /* 获取framebuffer设备信息 */
    ioctl(fd_fb, FBIOGET_VSCREENINFO, &fb_var);
    ioctl(fd_fb, FBIOGET_FSCREENINFO, &fb_fix);

    screen_size = fb_fix.line_length * fb_var.yres;
    width = fb_var.xres;
    height = fb_var.yres;

    /* 内存映射 */
    screen_base = mmap(NULL, screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_fb, 0);
    if (MAP_FAILED == (void *)screen_base)
    {
        perror("mmap error");
        close(fd_fb);
        return -1;
    }

    /* LCD背景刷成黑色 */
    memset(screen_base, 0xFF, screen_size);
    return 0;
}
int freetype_init(const char *font, int angle)
{
    FT_Error error;
    FT_Vector pen;
    FT_Matrix matrix;
    float rad; //旋转角度

    /* FreeType初始化 */
    FT_Init_FreeType(&library);

    /* 加载face对象 */
    error = FT_New_Face(library, font, 0, &face);
    if (error)
    {
        fprintf(stderr, "FT_New_Face error: %d\n", error);
        exit(EXIT_FAILURE);
    }

    /* 原点坐标 */
    pen.x = 0 * 64;
    pen.y = 0 * 64; //原点设置为(0, 0)

    /* 2x2矩阵初始化 */
    rad = (1.0 * angle / 180) * M_PI; //（角度转换为弧度）M_PI是圆周率
#if 0                                 //非水平方向
    matrix.xx = (FT_Fixed)( cos(rad) * 0x10000L);
    matrix.xy = (FT_Fixed)(-sin(rad) * 0x10000L);
    matrix.yx = (FT_Fixed)( sin(rad) * 0x10000L);
    matrix.yy = (FT_Fixed)( cos(rad) * 0x10000L);
#endif

#if 1 //斜体  水平方向显示的
    matrix.xx = (FT_Fixed)(cos(rad) * 0x10000L);
    matrix.xy = (FT_Fixed)(sin(rad) * 0x10000L);
    matrix.yx = (FT_Fixed)(0 * 0x10000L);
    matrix.yy = (FT_Fixed)(1 * 0x10000L);
#endif

    /* 设置 */
    FT_Set_Transform(face, &matrix, &pen);
    FT_Set_Pixel_Sizes(face, 40, 0); //设置字体大小

    return 0;
}

void lcd_draw_character(int x, int y,
                               const wchar_t *str, unsigned int color)
{
    unsigned int rgb565_color = argb8888_to_rgb565(color); //得到RGB565颜色值
    FT_GlyphSlot slot = face->glyph;
    size_t len = wcslen(str); //计算字符的个数
    long int temp;
    int n;
    int i, j, p, q;
    int max_x, max_y, start_y, start_x;
    // 循环加载各个字符
    for (n = 0; n < len; n++)
    {

        // 加载字形、转换得到位图数据
        if (FT_Load_Char(face, str[n], FT_LOAD_RENDER))
            continue;

        start_y = y - slot->bitmap_top; //计算字形轮廓上边y坐标起点位置 注意是减去bitmap_top
        if (0 > start_y)
        { //如果为负数 如何处理？？
            q = -start_y;
            temp = 0;
            j = 0;
        }
        else
        { // 正数又该如何处理??
            q = 0;
            temp = width * start_y;
            j = start_y;
        }

        max_y = start_y + slot->bitmap.rows; //计算字形轮廓下边y坐标结束位置
        if (max_y > (int)height)
            max_y = height;

        for (; j < max_y; j++, q++, temp += width)
        {

            start_x = x + slot->bitmap_left; //起点位置要加上左边空余部分长度
            if (0 > start_x)
            {
                p = -start_x;
                i = 0;
            }
            else
            {
                p = 0;
                i = start_x;
            }

            max_x = start_x + slot->bitmap.width;
            if (max_x > (int)width)
                max_x = width;

            for (; i < max_x; i++, p++)
            {
                // 如果数据不为0，则表示需要填充颜色
                if (slot->bitmap.buffer[q * slot->bitmap.width + p])
                    screen_base[temp + i] = rgb565_color;
            }
        }

        //调整到下一个字形的原点
        x += slot->advance.x / 64; //26.6固定浮点格式
	    y -= slot->advance.y / 64;
    }
}
void save_mem()
{
	
	int size = fb_fix.line_length * fb_var.yres;
	printf("size:%d\n",size);
	pmem = (unsigned char *)malloc(size);
	if(NULL == pmem)
	{
		perror("malloc failed");
		return ;
	}
	unsigned char * pdst = pmem;
	unsigned char * psrc = (unsigned char *)screen_base;
	int i = 0;
	for(i = 0; i < size; i++)
	{
		*pdst++ = *psrc++;		
	}	
}

void copy_mem(int x, int y, int width, int height)
{
	unsigned char * psrc = pmem + (y * fb_var.xres_virtual + x) * fb_var.bits_per_pixel / 8;
	unsigned char * pdst = (unsigned char *)screen_base + (y * fb_var.xres_virtual + x) * fb_var.bits_per_pixel / 8;

	int i = 0, j = 0;
	for(j = 0; j < height; j++)
	{
		for(i = 0; i < width; i++)
		{
			*((unsigned int *)pdst + j * fb_var.xres_virtual + i) = *((unsigned int *)psrc + j * fb_var.xres_virtual + i);
		}
	}
}

#if 0
int main(int argc, char *argv[])
{

    /* LCD初始化 */
    if (fb_dev_init())
        exit(EXIT_FAILURE);

    /* freetype初始化 */
    if (freetype_init("SIMSUN.TTC", 0))
        exit(EXIT_FAILURE);

    /* 在LCD上显示中文 */
	save_mem();
	while(1)
	{
		lcd_draw_character(30, 80, L"生命像一把无情的刀", 0x123456);
		sleep(1);
		copy_mem(30,40,360,45);
		lcd_draw_character(30, 80, L"改变了我们模样", 0x777777);
		sleep(1);
		copy_mem(30,40,360,45);
		
		lcd_draw_character(30, 80, L"未曾绽放就要枯萎啊", 0xFF0000);
		sleep(1);
		copy_mem(30,40,360,45);
		lcd_draw_character(30, 80, L"我曾有梦想", 0x999999);
		sleep(1);
		copy_mem(30,40,360,45);
	}	
	/* 退出程序 */
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    munmap(screen_base, screen_size);
    close(fd_fb);
    exit(EXIT_SUCCESS);
}
#endif
