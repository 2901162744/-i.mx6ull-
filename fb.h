#ifndef _FB_H
#define _FB_H

typedef struct __rgb
{
	unsigned char b;
	unsigned char g;
	unsigned char r;
	unsigned char a;
}RGB;

typedef union __color
{
	RGB rgb;
	unsigned int col;
}color;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int LONG;
#pragma pack(2)

typedef struct tagBITMAPFILEHEADER
{
	WORD bfType; // 位图文件的类型，必须为BM
	DWORD bfSize; // 位图文件的大小，以字节为单位
	WORD bfReserved1; // 位图文件保留字，必须为0
	WORD bfReserved2; // 位图文件保留字，必须为0
	DWORD bfOffBits; // 位图数据的起始位置，以相对于位图文件头的偏移量表示，以字节为单位
}BITMAPFILEHEADER ;

typedef struct tagBITMAPINFOHEADER
{
	DWORD biSize;    //本结构所占用字节数
	LONG biWidth;    //	位图的宽度，以像素为单位
	LONG biHeight;   //	位图的高度，以像素为单位
	WORD biPlanes;   //	目标设备的级别，必须为1
	WORD biBitCount; //每个像素所需的位数，必须是1(双色),
					//4(16色)，8(256色)或24(真彩色)之一
	DWORD biCompression; //位图压缩类型，必须是0(不压缩),
						//1(BI_RLE8压缩类型)或2(BI_RLE4压缩类型)之一
	DWORD biSizeImage;  //位图的大小，以字节为单位
	LONG  biXPelsPerMeter;//位图水平分辨率，每米像素数
	LONG biYPelsPerMeter;//	位图垂直分辨率，每米像素数
	DWORD	biClrUsed; //位图实际使用的颜色表中的颜色数
	DWORD	biClrImportant;//位图显示过程中重要的颜色数
}BITMAPINFOHEADER;

int fb_init();
void draw_point(int x, int y,unsigned int col);
void draw_x_line(int x, int y, int len, unsigned int col);
void draw_y_line(int x, int y, int len, unsigned int col);
void draw_cicle(int x0, int y0, int r, unsigned int col);
void fb_close();
int show_bmp(const char *filename, int x0, int y0);


#endif
