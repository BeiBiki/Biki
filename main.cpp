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
				c_red_img=img_contours_1.clone();			  //拷贝轮廓,以便发送到PC端
				pos_data_convert(rect_1,1,tx_buf_1);          //获取轮廓位置坐标
				send_pos_data(&g_fd,tx_buf_1);                //发送轮廓位置坐标
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
	int add=0;//校验和

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