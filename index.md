![img](/biki.png)

## 欢迎来到Biki主页<br>

### 目录

- 前言
- 开发平台
- 库函数说明

### 1. 前言
- 该主页主要介绍机器鱼视觉部分，方便大家进行二次开发<br>

### 2. 开发平台
- 该款机器鱼硬件使用Nanopi-S2，软件使用opencv2.10进行开发<br>
- 处理流程：NanoPi-S2处理视觉数据后，发送给STM32进行鱼尾的控制，从而实现自主避障，在调试模式下，NanoPi可将调试信息发送至PC调试端<br>
- 这里附上Nanopi-S2的wiki链接[NanoPi-S2](http://wiki.friendlyarm.com/wiki/index.php/NanoPi_S2/zh)方便大家查找问题

### 3. 库函数说明
- 在视觉部分，已经提供一些可以直接使用的函数接口，这里说明一下如何使用这些函数接口

1. `void uart_init(int *g_fd);`<br>

    - 功能：初始化NanoPi串口,以便发送图像信息至STM32单片机<br>
    - 入参：g_fd串口标识符，是一个指针<br>
    - 返回：无返回值<br>
  
2. `void send_pos_data(int *g_fd,unsigned char *s);`<br>
    
    - 功能：向单片机串口发送障碍物位置信息<br>
    - 入参：位置信息，是一个数组<br>
    - 返回：无返回值<br>
    
3. `int  receive_stm_data(int *g_fd,unsigned char *s);`
4. `void take_photos(Mat src_img,unsigned char cnt,unsigned char name_cnt);`

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


