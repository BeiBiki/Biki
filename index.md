![img](/biki.png)

## 欢迎来到Biki主页<br>

### 目录

- 前言
- 开发平台
- 库函数说明
- demo代码

### 1. 前言
- 该主页主要介绍机器鱼视觉部分，方便大家进行二次开发<br>

### 2. 开发平台
- 该款机器鱼硬件使用Nanopi-S2创客开发神器，视觉部分使用opencv2.10进行开发<br>
- 处理流程：NanoPi-S2处理视觉数据后，发送给STM32进行鱼尾的控制，从而实现自主避障，在调试模式下，NanoPi还可将调试信息发送至PC调试端<br>
- 这里附上Nanopi-S2的wiki链接[NanoPi-S2](http://wiki.friendlyarm.com/wiki/index.php/NanoPi_S2/zh)方便大家查找问题

### 3. 库函数说明

>在视觉部分，这里提供一些可以直接使用的函数接口，函数说明如下

1. `void uart_init(int *g_fd);`

	>功能：初始化NanoPi串口，以便发送图像信息至STM32单片机<br>
	g_fd：串口标识符，是一个指针<br>
	返回：无返回值<br>
  
2. `void send_pos_data(int *g_fd,unsigned char *s);`

	>功能：向单片机串口发送障碍物在视野中的位置信息<br>
	g_fd：串口标识符，是一个指针<br>
	s：障碍物在视野中的位置信息，根据通信协议，带入的数组必须是17个字节，请务必按照协议来写!<br>
	返回：无返回值<br>
  
3. `int  receive_stm_data(int *g_fd,unsigned char *s);`

	>功能：NanoPi接收由STM32发送过来的数据<br>
	g_fd：串口标识符，是一个指针<br>
	s：STM32发送过来的数据，根据通信协议，该数据为5个字节(图像处理命令或拍照命令)，或15个字节(用户的6个16位数据)<br>
	返回：无返回值<br>
  
4. `void take_photos(Mat src_img,unsigned char cnt,unsigned char name_cnt);`

	>功能：NanoPi存储图像<br>
	`src_img`：串口标识符，是一个指针<br>
	cnt：连拍的次数<br>
	name_cnt：照片命名的起始数字<br>
	返回：无返回值<br>

5. `void img_rotate(Mat src_img,Mat* rotate_img);`
  
	>功能：旋转图像，摄像头是横向安装的,这里要旋转一下<br>
	`src_img`：源图像<br>
	rotate_img：生成的旋转后的图像<br>
	返回：无返回值<br>

6. `void img_hsv(Mat src_img,Mat* hsv_img);`

	>功能：图像滤波并将源图像由RGB空间转换为HSV空间<br>
	`src_img`：源图像<br>
	hsv_img：生成的HSV空间图像<br>
	返回：无返回值<br>
  
7. `void img_process(Mat hsv_img,int* hsv_value,Mat* color_img);`

	>功能：过滤出需要的颜色，即特定颜色的障碍物<br>
	`hsv_img`：HSV空间图像<br>
	`hsv_value`：HSV范围值，{Hmin,Hmax,Smin,Smax,Vmin,Vmax}，一共6个字节<br>
	`color_img`：生成的单一颜色图像<br>
	返回：无返回值<br>
  
8. `void get_contours(Mat src_img,Mat* img_contours,RotatedRect *rect);`

	>功能：获取图像的轮廓，即障碍物的轮廓<br>
	`img_contours`：图像的轮廓<br>
	rect：轮廓外接的矩形，由矩形的坐标即可表示即障碍物在视觉中的坐标<br>
	返回：无返回值<br>
  
9. `void pos_data_convert(RotatedRect rect,unsigned char color_id,unsigned char* tx_buf);`

	>功能：将矩形的信息,(center_x,center_x,area%,width),转换到串口数据发送缓冲区<br>
	rect：障碍物外接矩形<br>
	id：根据协议，指定的颜色ID，1为红色，2为绿色，3为紫色<br>
	tx_buf：串口数据发送缓冲区<br>
	返回：无返回值<br>
  
10. `int server_socketConnect(int *sockServer,int PORT);`

	>功能：与client建立连接，即与PC客服端远程连接<br>
	sockServer：socket标识符<br>
	PORT：端口号<br>
	返回：连接失败则返回-1，连接成功返回1<br>
  
11. `void server_socketDisconnect(int sockServer);`

	>功能：断开服务器连接<br>
	sockServer：socket标识符<br>
	返回：无返回值<br>
  
12. `int server_transmit(int sockServer,Mat image);`
 
	>功能：服务端向client发送图像<br>
	sockServer：socket标识符<br>
	image：要发送的图像<br>
	返回：发送失败返回-1<br>
  
13. `int server_transmit(int sockServer,unsigned char* temp);`

	>功能：服务端向client发送用户数据<br>
	sockServer：socket标识符<br>
	temp：要发送的数据，12个字节<br>
	返回：发送失败返回-1<br>


### 4. demo代码

