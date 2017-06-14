![img](/biki.png)

## 欢迎来到Biki主页<br>

### 目录

- 前言
- 开发平台
- 库函数说明

### 1. 前言
- 该主页主要介绍机器鱼视觉部分，方便大家进行二次开发<br>

### 2. 开发平台
- 该款机器鱼硬件使用Nanopi-S2创客开发神器，视觉部分使用opencv2.10进行开发<br>
- 处理流程：NanoPi-S2处理视觉数据后，发送给STM32进行鱼尾的控制，从而实现自主避障，在调试模式下，NanoPi可将调试信息发送至PC调试端<br>
- 这里附上Nanopi-S2的wiki链接[NanoPi-S2](http://wiki.friendlyarm.com/wiki/index.php/NanoPi_S2/zh)方便大家查找问题

### 3. 库函数说明
- 在视觉部分，这里提供一些可以直接使用的函数接口，函数说明如下

1. `void uart_init(int *g_fd);`<br>

  >功能：初始化NanoPi串口,以便发送图像信息至STM32单片机<br>
  `*g_fd`：串口标识符，是一个指针<br>
  返回：无返回值<br>
  
2. `void send_pos_data(int *g_fd,unsigned char *s);`<br>
  
  >功能：向单片机串口发送障碍物位置信息<br>
  `*g_fd`：串口标识符，是一个指针<br>
  `*s`：障碍物位置信息，根据通信协议，带入数组时一共是17个字节，请务必按照协议来写！<br>
  返回：无返回值<br>
  
3. `int  receive_stm_data(int *g_fd,unsigned char *s);`

  >功能：NanoPi接收由STM32发送过来的数据<br>
   `*g_fd`：串口标识符，是一个指针<br>
   `*s`：STM32发送过来的数据，根据通信协议，该数据为5个字节(处理图像或拍照命令)，或15个字节(用户6个16位数据)<br>
  返回：无返回值<br>
  
4. `void take_photos(Mat src_img,unsigned char cnt,unsigned char name_cnt);`

  >功能：NanoPi拍照存储照片<br>
  src_img：串口标识符，是一个指针<br>
  cnt：连拍的次数<br>
  name_cnt：照片命名的起始数字<br>
  返回：无返回值<br>
  
5. `void img_rotate(Mat src_img,Mat* rotate_img);`

6. `void img_hsv(Mat src_img,Mat* hsv_img);`
7. `void img_process(Mat hsv_img,int* hsv_value,Mat* color_img);`
8. `void get_contours(Mat src_img,Mat* img_contours,RotatedRect *rect);`
9. `void pos_data_convert(RotatedRect rect,unsigned char color_id,unsigned char* tx_buf);`

10. `int  server_socketConnect(int *sockServer,int PORT);`
11. `void server_socketDisconnect(int sockServer);`
12. `int  server_transmit(int sockServer,Mat image);`
13. `int  server_transmit(int sockServer,unsigned char* temp);`
14. `void pc_data_convert(unsigned char *temp);`


