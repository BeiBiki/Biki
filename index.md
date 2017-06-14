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

5. `void img_rotate(Mat src_img,Mat* rotate_img,int angle);`
  
	>功能：旋转图像，摄像头是横向安装的,这里要旋转一下<br>
	`src_img`：源图像<br>
	rotate_img：生成的旋转后的图像<br>
	angle：旋转的角度
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
	temp：要发送的数据，20个字节<br>
	返回：发送失败返回-1<br>


### 4. demo代码

<pre>
#include "my_lib.h"

//定义一些标志
bool sta_flag=true;//图像处理标志
bool red_flag=true;//红色图像处理标志
bool gre_flag=true;//绿色图像处理标志
bool pur_flag=true;//紫色图像处理标志

bool img_flag=false;//单片机数据处理标志
bool dat_flag=false;//单片机数据处理标志

//定义串口标志符
int g_fd=-1;

//颜色范围,顺序依次为{Hmin,Hmax,Smin,Smax,Vmin,Vmax}
//不同的颜色修改此处的HSV范围即可
int red_hsv[6]={  0, 15, 94,226, 19,255};//红色HSV范围
int gre_hsv[6]={ 39, 53, 43,255, 46,255};//绿色HSV范围
int pur_hsv[6]={125,156, 43,255, 46,255};//紫色HSV范围

unsigned char stm_data[12];//用户在单片机中定义的数据

Mat cpy_s_img;//定义原始图像的拷贝
Mat combine;

//线程函数	
void *thread_img_process(void *tag);//图像处理线程
void *thread_rev_process(void *tag);//Pi接收单片机数据线程
void *thread_send_pcdata(void *tag);//Pi向PC端发送数据线程
void *thread_send_pcimg (void *tag);//Pi向PC端发送图像线程

int main()
{
	int pthreadErr;
	pthread_t id_1,id_2,id_3,id_4;
	
	//初始化Pi串口
	uart_init(&g_fd);
	
	//创建接收单片机数据线程
	pthreadErr=pthread_create(&id_1,NULL,thread_rev_process, NULL);
	if(pthreadErr!=0)  
	{
		perror("create thread_rev_process error!");
		exit(0);
	}
	
	//创建图像处理线程
	pthreadErr=pthread_create(&id_2,NULL,thread_img_process, NULL);
	if(pthreadErr!=0)  
	{
		perror("create thread_img_process error!");
		exit(0);
	}
	
	//创建Pi向PC端发送图像线程
	pthreadErr=pthread_create(&id_3,NULL,thread_send_pcimg, NULL);
	if(pthreadErr!=0)  
	{
		perror("create thread_send_pcimg error!");
		exit(0);
	}

	//创建Pi向PC端发送数据线程
	pthreadErr=pthread_create(&id_4,NULL,thread_send_pcdata, NULL);
	if(pthreadErr!=0)  
	{
		perror("create thread_send_pcdata error!");
		exit(0);
	}

	waitKey(1000);
	while(1);

	return 0;
}

void *thread_img_process(void *tag)
{
	Mat src_img;

	Mat combine1,combine2;//定义拼接的图像
	Mat cmprs_img,c_red_img,c_gre_img,c_pur_img;

	Mat red_img,gre_img,pur_img;//经HSV处理过后的单一颜色图像
	Mat img_contours_1,img_contours_2,img_contours_3;//获取图像中的轮廓
	RotatedRect rect_1,rect_2,rect_3;
	
	unsigned char tx_buf_1[17],tx_buf_2[17],tx_buf_3[17];
	
	//打开摄像头
	VideoCapture cap(0);

	if(!cap.isOpened())
	{
		printf("Open Camera error\n");
	}
	
	while(1)
	{
		cap>>src_img;//原始图像像素大小(640*480)
		cpy_s_img=src_img.clone();//拷贝原始图像,以便拍照

		if(sta_flag)
		{
			//将图像压缩,以加快Pi的图像处理速度
			resize(src_img,src_img,Size(160,120),0,0,CV_INTER_LINEAR);
			
			//摄像头是横向安装的,这里要变换到正向
			img_rotate(src_img,&src_img,90);

			//拷贝图像,并转为灰度,以便发送到PC端
			cmprs_img=src_img.clone();
			cvtColor(cmprs_img,cmprs_img,CV_BGR2GRAY);
			
			//将原始图像RGB格式转换到HSV空间,其中含滤波
			img_hsv(src_img,&src_img);

			//HSV过滤出原始图像中3颜色
			img_process(src_img,red_hsv,&red_img);
			img_process(src_img,gre_hsv,&gre_img);
			img_process(src_img,pur_hsv,&pur_img);

			if(red_flag)
			{
				get_contours(red_img,&img_contours_1,&rect_1);//获取图像轮廓
				c_red_img=img_contours_1.clone();//拷贝轮廓,以便发送到PC端
				pos_data_convert(rect_1,1,tx_buf_1);//获取轮廓位置坐标
				send_pos_data(&g_fd,tx_buf_1);//发送轮廓位置坐标
			}
			if(gre_flag)
			{
				get_contours(gre_img,&img_contours_2,&rect_2);
				c_gre_img=img_contours_2.clone();
				pos_data_convert(rect_2,2,tx_buf_2);
				send_pos_data(&g_fd,tx_buf_2);
			}
			
			if(pur_flag)
			{
				get_contours(pur_img,&img_contours_3,&rect_3);
				c_pur_img=img_contours_3.clone();
				pos_data_convert(rect_3,3,tx_buf_3);
				send_pos_data(&g_fd,tx_buf_3);
			}
			
			hconcat(cmprs_img,c_red_img,combine1);  
			hconcat(c_gre_img,c_pur_img,combine2);  
			vconcat(combine1,combine2,combine);
			
			img_flag=true;  

			waitKey(15);
		}	
	}
	cap.release();//释放摄像头
}

void *thread_rev_process(void *tag)
{
	unsigned char len;
	unsigned char tmp[18];
	unsigned char name_cnt=0;
	unsigned char old_cnt=0;
	int val_1,val_2,val_3,val_4,val_5,val_6;
	
	while(1)
	{
		//获取单片机数据
		len=receive_stm_data(&g_fd,tmp);
		if(len>0)
		{
			if(len==5)
			{
				if((tmp[0]==0xAA)&&(tmp[4]==0xBB))
				{
					switch(tmp[2])
						{
							//解析图像标志位
							//[AA 01 10 CMD BB]
							case 0x10:
								if((tmp[3]&0x01)==1)
									gre_flag=true;
								else
									gre_flag=false;
								if(((tmp[3]>>1)&0x01)==1)
									red_flag=true;
								else
									red_flag=false;
								if(((tmp[3]>>2)&0x01)==1)
									pur_flag=true;
								else
									pur_flag=false;
								break;
							//拍照命令[AA 01 11 cnt BB]
							//cnt为拍照次数
							case 0x11:
								take_photos(cpy_s_img,tmp[3],name_cnt);
								name_cnt=tmp[3]+old_cnt;
								old_cnt=name_cnt;
								break;
							default:
								break;
						}
				}
			}
			//获取到的单片机数据(用户自定义)
			if(len==15)
			{
				for(int i=0;i<12;i++)
					stm_data[i]=tmp[i+2];
				dat_flag=true;
				
				val_1=tmp[3]+tmp[2]*256;
				val_2=tmp[5]+tmp[4]*256;
				val_3=tmp[7]+tmp[6]*256;
				val_4=tmp[9]+tmp[8]*256;
				val_5=tmp[11]+tmp[10]*256;
				val_6=tmp[13]+tmp[12]*256;
				printf("1:%d\t2:%d\t3:%d\t4:%d\t5:%d\t6:%d\n",val_1,val_2,val_3,val_4,val_5,val_6);
			}
		}
	}
}

void *thread_send_pcimg(void *tag)
{
	int socketMat;

	//建立2017端口连接,传输图像数据至PC端
	//图像由压缩后的原始图像cmprs_img以及红绿紫图像,共4个图像拼接而成
	if(server_socketConnect(&socketMat,2017)<0)
	{  
        	printf("socketMat error\n"); 
	}

	while(1)
	{
		if(img_flag)
		{
			img_flag=false;
			server_transmit(socketMat,combine);
		} 
	}
	//断开网络连接
	server_socketDisconnect(socketMat);
}

void *thread_send_pcdata(void *tag)
{
	int socketDat;

	unsigned char tmp[20];
	int add=0;//add

	tmp[0]=0xAA;//start
	tmp[1]=0x0C;//len:12
	tmp[2]=0x69;//cmd

	add=tmp[1]+tmp[2];

	if(server_socketConnect(&socketDat,2066)<0)
	{  
		printf("socketDat error\n"); 
	}

	while(1)
	{
		if(dat_flag)
		{
			dat_flag=false;
			for(int i=0;i<12;i++)
			{
				add=add+stm_data[i];
				tmp[i+3]=stm_data[i];
			}

			tmp[15]=(add>>24)&0xff;
			tmp[16]=(add>>16)&0xff;
			tmp[17]=(add>>8)&0xff;
			tmp[18]=(add>>0)&0xff;

			tmp[19]=0xBB;//stop

			add=tmp[1]+tmp[2];

			server_transmit(socketDat,tmp);
		}
	}
	//断开网络连接
	server_socketDisconnect(socketDat);
}

</pre>