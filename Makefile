#arm-linux-gnueabihf-gcc freetype.c -I/home/linux/imx/tool/libfreetype/include/freetype2 
#-L/home/linux/imx/tool/libfreetype/lib -L/home/linux/imx/tool/zlib/lib -L/home/linux/imx/tool/libpng/lib -lfreetype -lm -lpng -lz

# 交叉编译配置
export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabihf-
CC = $(CROSS_COMPILE)gcc
TARGET = main
BUILD_DIR = build

# 路径配置
SRC = freetype.c main.c getdata.c list.c queue.c mqtt.c
#SRC = test.c
OBJS = $(addprefix $(BUILD_DIR)/, $(SRC:.c=.o))

# 头文件路径
INC_DIR = -I/home/linux/imx/tool/libfreetype/include/freetype2 \
          -I/home/linux/imx/tool/zlib/include \
          -I/home/linux/imx/tool/libpng/include \
		  -I/home/linux/imx/tool/paho-mqtt/paho.mqtt.c-master/src

# 库文件路径
LIB_DIR = -L/home/linux/imx/tool/libfreetype/lib \
          -L/home/linux/imx/tool/zlib/lib \
          -L/home/linux/imx/tool/libpng/lib \
		  -L/home/linux/imx/tool/paho-mqtt/paho.mqtt.c-master/build/output \
		  -L/home/linux/imx/tool/paho-mqtt/openssl-1.0.0s/__install/lib

# 编译参数
CFLAGS = -Wall -g -MMD -MP $(INC_DIR)
LDFLAGS = $(LIB_DIR) -lfreetype -lpng -lz -lm -lpthread -lpaho-mqtt3cs -lrt -lssl -lcrypto 

# 构建规则
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) -c $< -o $@ $(CFLAGS)

# 清理规则
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
