#ifndef _FREETYPE_H
#define _FREETYPE_H
#include <sys/mman.h>
#include <linux/fb.h>
#include <math.h> //数学库函数头文件

#include <wchar.h>
#define FB_DEV "/dev/fb0" //LCD设备节点

#define argb8888_to_rgb565(color) (    \
    {                                  \
        unsigned int temp = (color);   \
        ((temp & 0xF80000UL) >> 8) |   \
            ((temp & 0xFC00UL) >> 5) | \
            ((temp & 0xF8UL) >> 3);    \
    })


int fb_dev_init(void);
int freetype_init(const char *font, int angle);
void lcd_draw_character(int x, int y, const wchar_t *str, unsigned int color);
unsigned short *get_screen_base();
unsigned long get_screen_size();
void save_mem();
void copy_mem(int x, int y, int width, int height);
#endif
