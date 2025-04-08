#include <stdio.h>
#include <string.h>

int main() 
{
    double a = 84039.800;// 主字符串
	int hour, min, secods;
	secods =(int)a % 100;
	a = a / 100;
	min = (int)a % 100;
	a = a / 100;
	hour = (int)a % 100 + 8;
	printf("%d:%d:%d\n",hour, min, secods);
    return 0;
}
