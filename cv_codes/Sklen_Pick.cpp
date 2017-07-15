#define use_pi

#ifdef use_pi
	#define use_serial
	#define use_up 
#endif

#include <sys/types.h>

#ifdef use_serial
	#include <unistd.h>
	#include <termios.h>
#endif

#include <fstream> 
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <iostream>
#include <opencv/cv.h>
#include <opencv/cvaux.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include <string>
#include <sstream>
#include <map>
#include <set>
#include <stack>
using namespace std;
using namespace cv;
#define TREE_THRE_VALUE 20

//长度大于TREE_THRE_VALUE的数量的子路径保留
int thresholdValue=55;
Mat globalImg;

int array[256]={
	0,0,1,1,0,0,1,1,1,1,0,1,1,1,0,1,
	1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,1,
	0,0,1,1,0,0,1,1,1,1,0,1,1,1,0,1,
	1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,1,
	1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,0,0,1,1,0,0,1,1,0,1,1,1,0,1,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,1,1,0,0,1,1,1,1,0,1,1,1,0,1,
	1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,1,
	0,0,1,1,0,0,1,1,1,1,0,1,1,1,0,1,
	1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0,
	1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,
	1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0,
	1,1,0,0,1,1,0,0,1,1,0,1,1,1,0,0,
	1,1,0,0,1,1,1,0,1,1,0,0,1,0,0,0
};

#ifdef use_serial
int speed_arr[] = { B38400, B19200, B9600, B4800, B2400, B1200, B300,
          B38400, B19200, B9600, B4800, B2400, B1200, B300, B115200 };
int name_arr[] = {38400,  19200,  9600,  4800,  2400,  1200,  300, 38400,  
          19200,  9600, 4800, 2400, 1200,  300, 115200 };

int fd;
int nread;
char buff[512];
char *dev  = "/dev/ttyUSB0"; //串口二

int set_Parity(int fd,int databits,int stopbits,int parity)
{ 
	struct termios options; 
	if  ( tcgetattr( fd,&options)  !=  0) { 
		perror("SetupSerial 1");     
		return(FALSE);  
	}
	options.c_cflag &= ~CSIZE; 
	switch (databits) /*设置数据位数*/
	{   
	case 7:		
		options.c_cflag |= CS7; 
		break;
	case 8:     
		options.c_cflag |= CS8;
		break;   
	default:    
		fprintf(stderr,"Unsupported data size\n"); return (FALSE);  
	}
switch (parity) 
{   
	case 'n':
	case 'N':    
		options.c_cflag &= ~PARENB;   /* Clear parity enable */
		options.c_iflag &= ~INPCK;     /* Enable parity checking */ 
		break;  
	case 'o':   
	case 'O':     
		options.c_cflag |= (PARODD | PARENB); /* 设置为奇效验*/  
		options.c_iflag |= INPCK;             /* Disnable parity checking */ 
		break;  
	case 'e':  
	case 'E':   
		options.c_cflag |= PARENB;     /* Enable parity */    
		options.c_cflag &= ~PARODD;   /* 转换为偶效验*/     
		options.c_iflag |= INPCK;       /* Disnable parity checking */
		break;
	case 'S': 
	case 's':  /*as no parity*/   
	    options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB;break;  
	default:   
		fprintf(stderr,"Unsupported parity\n");    
		return (FALSE);  
	}  
/* 设置停止位*/  
switch (stopbits)
{   
	case 1:    
		options.c_cflag &= ~CSTOPB;  
		break;  
	case 2:    
		options.c_cflag |= CSTOPB;  
	   break;
	default:    
		 fprintf(stderr,"Unsupported stop bits\n");  
		 return (FALSE); 
} 
/* Set input parity option */ 
if (parity != 'n')   
	options.c_iflag |= INPCK; 
tcflush(fd,TCIFLUSH);
options.c_cc[VTIME] = 150; /* 设置超时15 seconds*/   
options.c_cc[VMIN] = 0; /* Update the options and do it NOW */
if (tcsetattr(fd,TCSANOW,&options) != 0)   
{ 
	perror("SetupSerial 3");   
	return (FALSE);  
} 
return (TRUE);  
}


void set_speed(int fd, int speed){
  int   i; 
  int   status; 
  struct termios   Opt;
  tcgetattr(fd, &Opt); 
  for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++) { 
    if  (speed == name_arr[i]) {     
      tcflush(fd, TCIOFLUSH);     
      cfsetispeed(&Opt, speed_arr[i]);  
      cfsetospeed(&Opt, speed_arr[i]);   
      status = tcsetattr(fd, TCSANOW, &Opt);  
      if  (status != 0) {        
        perror("tcsetattr fd1");  
        return;     
      }    
      tcflush(fd,TCIOFLUSH);   
    }  
  }
}
int OpenDev(char *Dev)
{
	int	fd = open( Dev, O_RDWR );         //| O_NOCTTY | O_NDELAY	
	if (-1 == fd)	
	{ 			
		perror("Can't Open Serial Port");
		return -1;		
	}	
	else	
		return fd;
}
void Serial_begin(char devs[],int speed_num=115200)
{

	fd = OpenDev(devs);
	set_speed(fd,speed_num);
	if (set_Parity(fd,8,1,'N') == FALSE)  {
		printf("Set Parity Error\n");
		exit (0);
	}
}

void Serial_print(string data)
{
	
	const char *buffer=data.c_str();
	int    Length=data.length();
	int    nByte;
	nByte = write(fd, buffer ,Length);
}
#endif


class LeastSquare{  //最小二乘法
   
public:  
	double a, b;  
    LeastSquare(const vector<Point2f>xy)  
    {  
        double t1=0, t2=0, t3=0, t4=0;  
        for(int i=0; i<xy.size(); ++i)  
        {  
			t1+=xy[i].x*xy[i].x;
			t2+=xy[i].x;
			t3+=xy[i].x*xy[i].y;
			t4+=xy[i].y;
        }  
        a = (t3*xy.size() - t2*t4) / (t1*xy.size() - t2*t2);  
        b = (t1*t4 - t2*t3) / (t1*xy.size() - t2*t2);  
    }  
  
    double getY(const double x) const  
    {  
        return a*x + b;  
    }  
  
    void print() const  
    {  
        cout<<"y = "<<a<<"x + "<<b<<"\n";  
    }  
  
};  
  




Point poiLeftUp;
Point poiRightDown;

typedef pair<int, int> PAIR;  //定义键值对
struct CmpByValue {  //定义键值排序函数  大的数字在上面
  bool operator()(const PAIR& lhs, const PAIR& rhs) {  
    return lhs.second > rhs.second;  
  }  
};  

struct CmpByValueSmallTop {  //定义键值排序函数  小的数字在上面
  bool operator()(const PAIR& lhs, const PAIR& rhs) {  
    return lhs.second < rhs.second;  
  }  
};  

ostream& operator<<(ostream& out, const PAIR& p) {  //重载cout
  return out << p.first << "\t" << p.second;  
}  

void console_display(Mat input)
{
	if(input.channels()!=1)return;
	for(int i=0;i<input.rows;i++)
	{
		for(int j=0;j<input.cols;j++)
		{
				if(input.at<uchar>(i,j)==255)cout<<"_ ";
				else cout<<"0 ";//如果有线条是0  没有线条是255
		}
		cout<<endl;
	}
}

void cvMouseCallback(int mouseEvent,int x,int y,int flags,void* param)
{
  switch(mouseEvent)
  {
	case CV_EVENT_LBUTTONUP://鼠标左键弹起
		printf("(%d,%d)=%d\n",x,y,(unsigned int)globalImg.at<uchar>(y,x));
		poiLeftUp=Point(x,y);
    break;

	case CV_EVENT_RBUTTONUP://鼠标右键弹起
		poiRightDown=Point(x,y);
		printf("POI:");cout<<poiLeftUp<<" "<<poiRightDown<<endl;
		Mat poi=Mat(globalImg,Rect(poiLeftUp,poiRightDown));
		console_display(poi);
	break;
  }
}

Point poiLeftUp1;
Point poiRightDown1;
Mat   globalImg1;
void cvMouseCallback2(int mouseEvent,int x,int y,int flags,void* param)
{
  switch(mouseEvent)
  {
	case CV_EVENT_LBUTTONUP://鼠标左键弹起
		printf("(%d,%d)\n",x,y);
		poiLeftUp1=Point(x,y);
    break;

	case CV_EVENT_RBUTTONUP://鼠标右键弹起
		poiRightDown1=Point(x,y);
		printf("POI:");cout<<poiLeftUp1<<" "<<poiRightDown1<<endl;
		globalImg1=Mat(globalImg,Rect(poiLeftUp1,poiRightDown1));
		imshow("2",globalImg1);
	break;
  }
}

void VThin(Mat &image,int array[])//垂直方向进行向内漫水算法
{
	int h = image.rows;
	int w = image.cols;
	int NEXT=1;
	for(int i=0;i<h;i++)
	{
		try{
		for(int j=0;j<w;j++)
		{
			
			if(NEXT==0)
			{
				NEXT=1;
			}
			else
			{
				int M;
				if(0<j&&j<w-1)
					M=(unsigned int)image.at<uchar>(i,j-1)+(unsigned int)image.at<uchar>(i,j)+(unsigned int)image.at<uchar>(i,j+1);
				else
					M=1;
				if(image.at<uchar>(i,j)==0 && M!=0)
				{
					int a[9]={0,0,0,0,0,0,0,0,0};
					for(int k=0;k<3;k++)
					{
						for(int l=0;l<3;l++)
						{
							if(-1<(i-1+k)&& (i-1+k)<h && -1<(j-1+l) && (j-1+1)<w && image.at<uchar>(i-1+k,j-1+l)==255)
							{
								a[k*3+l]=1;
							}
						}
					}
					int sum=a[0]*1+a[1]*2+a[2]*4+a[3]*8+a[5]*16+a[6]*32+a[7]*64+a[8]*128;
					image.at<uchar>(i,j) = array[sum] * 255;
					if(array[sum]==1)
						NEXT=0;
				}
			}
		}
		}
		catch(exception e)
		{
			cout<<e.what()<<endl;
		}
	}

}

void HThin(Mat &image,int array[])//水平方向进行向内漫水算法
{
	int h=image.rows;
	int w=image.cols;
	int NEXT=1;
	for(int j=0;j<w;j++)
	{
		for(int i=0;i<h;i++)
		{
			if(NEXT==0)
			{
				NEXT=1;
			}
			else
			{
				int M;
				if(0<i&&i<h-1)
					M=(unsigned int)image.at<uchar>(i-1,j)+(unsigned int)image.at<uchar>(i,j)+(unsigned int)image.at<uchar>(i+1,j);
				else
					M=1;
				if(image.at<uchar>(i,j)==0 && M!=0)
				{
					int a[9]={0,0,0,0,0,0,0,0,0};
					for(int k=0;k<3;k++)
					{
						for(int l=0;l<3;l++)
						{
							if(-1<(i-1+k)&& (i-1+k)<h && -1<(j-1+l) && (j-1+1)<w && image.at<uchar>(i-1+k,j-1+l)==255)
							{
								a[k*3+l]=1;
							}
						}
					}
					int sum=a[0]*1+a[1]*2+a[2]*4+a[3]*8+a[5]*16+a[6]*32+a[7]*64+a[8]*128;
					image.at<uchar>(i,j) = array[sum] * 255;
					if(array[sum]==1)
						NEXT=0;
				}
			}
		}
	}

}

Mat Sklen(Mat image,int array1[],int num=20)
{
	Mat iXihua=image.clone();
	int all_cols=iXihua.cols;
	int all_rows=iXihua.rows;
	int middle_col=all_cols/2;
	int midlle_row=all_rows/2;


	float radio=0.95;//涂白边缘
	int start_paint_row=midlle_row-all_rows*radio/2;
	int end_paint_row=midlle_row+all_rows*radio/2;

	int start_paint_col=middle_col-all_cols*radio/2;
	int end_paint_col=middle_col+all_cols*radio/2;

	//首先将所有边缘涂白
	for(int i=0;i<start_paint_row;i++)//将最上边沿涂白
		for(int j=0;j<all_cols;j++)
			iXihua.at<uchar>(i,j)=255;
	for(int i=end_paint_row;i<all_rows;i++)//将最下边沿涂白
		for(int j=0;j<all_cols;j++)
			iXihua.at<uchar>(i,j)=255;
	for(int i=0;i<all_rows;i++)
		for(int j=0;j<start_paint_col;j++)//将最左边沿涂白
			iXihua.at<uchar>(i,j)=255;
	for(int i=0;i<all_rows;i++)
		for(int j=end_paint_col;j<all_cols;j++)//将最左边沿涂白
			iXihua.at<uchar>(i,j)=255;

	for(int i=0;i<num;i++)
	{
		printf("#%d\n",i);
		VThin(iXihua,array1);
		#define debug
	
		HThin(iXihua,array1);

		#ifdef debug
		imshow("3",iXihua);
		waitKey(300);
		printf("Sklen iter %d times\n",i);
		#endif
	}
	#ifdef debug
	destroyWindow("3");
	#endif
	return iXihua;
}




void trackbar(int input,void *u)  
{  
    thresholdValue=input;
} 

vector<Point> Find_Endpoint(const Mat& input)
{
	//从上到下 从左到右 扫描二值图，根据九宫格方法判断一个点是否是端点
	//如果是线段 二值图中为0  空白区域用非0 表示
	//之后还需要加上 二次检测 防止错误检测
	vector<Point> result;
	if(input.channels()!=1){cout<<"Error@ Fun (Find_EndPoint):Input image`s channel != 1\n";return result;}
	for(int i=1;i<input.rows-1;i++)//扫描的时候避开最外面的边框
	{
		for(int j=1;j<input.cols-1;j++)//扫描的时候避开最外面的边框
		{
			if(input.at<uchar>(i,j)==0)//遇到了一个黑点，开始检测四周围有几个黑点
			{
				//如果周围8个像素只能找到一个黑点，那么将该点初步认为是端点，加入到结果集中
				//定义非黑点为255
				#define BlankValue 255 
				int sum=input.at<uchar>(i,j-1)+input.at<uchar>(i-1,j-1)+input.at<uchar>(i-1,j)+input.at<uchar>(i-1,j+1)+input.at<uchar>(i,j+1)+input.at<uchar>(i+1,j+1)+input.at<uchar>(i+1,j)+input.at<uchar>(i+1,j-1);
				if(sum==BlankValue*7)//周围有7个白色点，也就是只有1个黑色点
					result.push_back(Point(j,i));
			}
		}
	}
	return result;
}

Mat Diffuse(Mat input)//单通道、二值化后的图像  
{  

	Mat srcimage=~input;

    vector<Point> deletelist1;  
    int Zhangmude[9];  
    int nl = srcimage.rows;  
    int nc = srcimage.cols;  
    while (true)  
    {  
        for (int j = 1; j<(nl - 1); j++)  
        {  
            uchar* data_last = srcimage.ptr<uchar>(j - 1);  
            uchar* data = srcimage.ptr<uchar>(j);  
            uchar* data_next = srcimage.ptr<uchar>(j + 1);  
            for (int i = 1; i<(nc - 1); i++)  
            {  
                if (data[i] == 255)  
                {  
                    Zhangmude[0] = 1;  
                    if (data_last[i] == 255) Zhangmude[1] = 1;  
                    else  Zhangmude[1] = 0;  
                    if (data_last[i + 1] == 255) Zhangmude[2] = 1;  
                    else  Zhangmude[2] = 0;  
                    if (data[i + 1] == 255) Zhangmude[3] = 1;  
                    else  Zhangmude[3] = 0;  
                    if (data_next[i + 1] == 255) Zhangmude[4] = 1;  
                    else  Zhangmude[4] = 0;  
                    if (data_next[i] == 255) Zhangmude[5] = 1;  
                    else  Zhangmude[5] = 0;  
                    if (data_next[i - 1] == 255) Zhangmude[6] = 1;  
                    else  Zhangmude[6] = 0;  
                    if (data[i - 1] == 255) Zhangmude[7] = 1;  
                    else  Zhangmude[7] = 0;  
                    if (data_last[i - 1] == 255) Zhangmude[8] = 1;  
                    else  Zhangmude[8] = 0;  
                    int whitepointtotal = 0;  
                    for (int k = 1; k < 9; k++)  
                    {  
                        whitepointtotal = whitepointtotal + Zhangmude[k];  
                    }  
                    if ((whitepointtotal >= 2) && (whitepointtotal <= 6))  
                    {  
                        int ap = 0;  
                        if ((Zhangmude[1] == 0) && (Zhangmude[2] == 1)) ap++;  
                        if ((Zhangmude[2] == 0) && (Zhangmude[3] == 1)) ap++;  
                        if ((Zhangmude[3] == 0) && (Zhangmude[4] == 1)) ap++;  
                        if ((Zhangmude[4] == 0) && (Zhangmude[5] == 1)) ap++;  
                        if ((Zhangmude[5] == 0) && (Zhangmude[6] == 1)) ap++;  
                        if ((Zhangmude[6] == 0) && (Zhangmude[7] == 1)) ap++;  
                        if ((Zhangmude[7] == 0) && (Zhangmude[8] == 1)) ap++;  
                        if ((Zhangmude[8] == 0) && (Zhangmude[1] == 1)) ap++;  
                        if (ap == 1)  
                        {  
                            if ((Zhangmude[1] * Zhangmude[7] * Zhangmude[5] == 0) && (Zhangmude[3] * Zhangmude[5] * Zhangmude[7] == 0))  
                            {  
                                deletelist1.push_back(Point(i, j));  
                            }  
                        }  
                    }  
                }  
            }  
        }  
        if (deletelist1.size() == 0) break;  
        for (size_t i = 0; i < deletelist1.size(); i++)  
        {  
            Point tem;  
            tem = deletelist1[i];  
            uchar* data = srcimage.ptr<uchar>(tem.y);  
            data[tem.x] = 0;  
        }  
        deletelist1.clear();  
  
        for (int j = 1; j<(nl - 1); j++)  
        {  
            uchar* data_last = srcimage.ptr<uchar>(j - 1);  
            uchar* data = srcimage.ptr<uchar>(j);  
            uchar* data_next = srcimage.ptr<uchar>(j + 1);  
            for (int i = 1; i<(nc - 1); i++)  
            {  
                if (data[i] == 255)  
                {  
                    Zhangmude[0] = 1;  
                    if (data_last[i] == 255) Zhangmude[1] = 1;  
                    else  Zhangmude[1] = 0;  
                    if (data_last[i + 1] == 255) Zhangmude[2] = 1;  
                    else  Zhangmude[2] = 0;  
                    if (data[i + 1] == 255) Zhangmude[3] = 1;  
                    else  Zhangmude[3] = 0;  
                    if (data_next[i + 1] == 255) Zhangmude[4] = 1;  
                    else  Zhangmude[4] = 0;  
                    if (data_next[i] == 255) Zhangmude[5] = 1;  
                    else  Zhangmude[5] = 0;  
                    if (data_next[i - 1] == 255) Zhangmude[6] = 1;  
                    else  Zhangmude[6] = 0;  
                    if (data[i - 1] == 255) Zhangmude[7] = 1;  
                    else  Zhangmude[7] = 0;  
                    if (data_last[i - 1] == 255) Zhangmude[8] = 1;  
                    else  Zhangmude[8] = 0;  
                    int whitepointtotal = 0;  
                    for (int k = 1; k < 9; k++)  
                    {  
                        whitepointtotal = whitepointtotal + Zhangmude[k];  
                    }  
                    if ((whitepointtotal >= 2) && (whitepointtotal <= 6))  
                    {  
                        int ap = 0;  
                        if ((Zhangmude[1] == 0) && (Zhangmude[2] == 1)) ap++;  
                        if ((Zhangmude[2] == 0) && (Zhangmude[3] == 1)) ap++;  
                        if ((Zhangmude[3] == 0) && (Zhangmude[4] == 1)) ap++;  
                        if ((Zhangmude[4] == 0) && (Zhangmude[5] == 1)) ap++;  
                        if ((Zhangmude[5] == 0) && (Zhangmude[6] == 1)) ap++;  
                        if ((Zhangmude[6] == 0) && (Zhangmude[7] == 1)) ap++;  
                        if ((Zhangmude[7] == 0) && (Zhangmude[8] == 1)) ap++;  
                        if ((Zhangmude[8] == 0) && (Zhangmude[1] == 1)) ap++;  
                        if (ap == 1)  
                        {  
                            if ((Zhangmude[1] * Zhangmude[3] * Zhangmude[5] == 0) && (Zhangmude[3] * Zhangmude[1] * Zhangmude[7] == 0))  
                            {  
                                deletelist1.push_back(Point(i, j));  
                            }  
                        }  
                    }  
                }  
            }  
        }  
        if (deletelist1.size() == 0) break;  
        for (size_t i = 0; i < deletelist1.size(); i++)  
        {  
            Point tem;  
            tem = deletelist1[i];  
            uchar* data = srcimage.ptr<uchar>(tem.y);  
            data[tem.x] = 0;  
        }  
        deletelist1.clear();  
    } 

	return ~srcimage;
}


Mat Prune(const Mat & input_raw)//对骨架图像进行剪枝
{
	Mat input=input_raw.clone();
	Mat showImg(input.size(),CV_8UC1,Scalar(255));

	vector<Point> three_around_points;
	vector<Point> endian;
	if(input.channels()!=1){cout<<"Error@ Fun (Find_EndPoint):Input image`s channel != 1\n";return showImg ;}
	endian=Find_Endpoint(input);
	
	stack <Point> pointStack;//广度优先遍历，存储点
	set   <int>   visitedPoints;//记录已经经过的点映射集 (x,y) -> x<<12|y
	#define _Point(p) (p).x<<15|(p).y  

	//vector<vector<Point> > chains;
	vector<vector<Point> > subPaths;

	printf("Begin Get Chain!\n");
	printf("Endian amount:%d\n",endian.size());
	for(int i=0;i<endian.size();i++)//遍历每一个端点 先测试一个端点   endian.size()
	{
		printf("Handle:%d\n",i);
		//潜在的围墙bug
		visitedPoints.insert(_Point(endian[i]));//首先将端点插入到集合中
		vector<Point> sub_chain;//存放链条 这是要返回的结果
		Point currentPoint=endian[i];
		Point nextPoint;
		Point lastPoint;
		bool end=false;//算法结束标记
		Point D_P_Map[9]={Point(0,0),Point(1,0),Point(1,-1),Point(0,-1),Point(-1,-1),Point(-1,0),Point(-1,1),Point(0,1),Point(1,1)};
									//  1			2			3			4			5			6			7			8
		
		
		subPaths.push_back(vector<Point>());
		int panic=0;
		
		while(!end)
		{
			panic++;
			if(panic>1000)
			{
				printf("Panic state!Jump out!\n");
				break;//恐慌模式，因为迭代次数过多，进入死循环了，跳出；注意这里end没有设置为true
			}
			visitedPoints.insert(_Point(currentPoint));//将当前点插入到已经访问过的点集中
			//sub_chain.push_back(currentPoint);//将当前点作为链条中的点加入到chain中
			
			subPaths[subPaths.size()-1].push_back(currentPoint);//debug 插入当前点到子路中
			//if(currentPath>=0)pathLength[currentPath]++;//分叉路径长度记录
			//std::cout<<currentPoint<<endl;

			vector<int> foundBlack;
			foundBlack.clear();
			//检测周围8个点哪些是黑点
			for(int D_Walker=1;D_Walker<=8;D_Walker++)
			{
				if(input.at<uchar>(currentPoint+D_P_Map[D_Walker])==0)
					foundBlack.push_back(D_Walker);
			}



			if(foundBlack.size()==1)//周围只有1个黑点，说明当前是端点
			{
				nextPoint=currentPoint+D_P_Map[ foundBlack[0] ];//nextPoind中存放的是下个点的位置
				//需要判断这个点之前有没有经过
				if(visitedPoints.count(_Point(nextPoint))==1)//之前遇到过这个点，说明我走到了尽头 lastPoint==nextPoint
				{
					if(pointStack.size()==0)//转折点不存在，说明这条线段我走完了
					{
						end=true;//跳出循环
					}
					else//还有转折点,我需要回到转折点的地方重新走
					{
						subPaths.push_back(vector<Point>());//debug增加新的子路
						nextPoint=pointStack.top();//回到转折点
						pointStack.pop();//出栈一个转折点
					}
				}
				else//之前没有遇到这个点
				{}
				lastPoint=currentPoint;
				currentPoint=nextPoint;
			}
			else if(foundBlack.size()==2)//周围有两个黑点，说明当前点是处于线段的中间的点
			{
				Point p0,p1;
				p0=currentPoint+D_P_Map[ foundBlack[0] ];
				p1=currentPoint+D_P_Map[ foundBlack[1] ];
				if(visitedPoints.count(_Point(p0))==1)//p1是新找到的点 需要判断这个点之前有没有经过
				{
					nextPoint=currentPoint+D_P_Map[ foundBlack[1] ];
				}
				else//p0是新找到的点
				{
					nextPoint=currentPoint+D_P_Map[ foundBlack[0] ];
				}
				lastPoint=currentPoint;
				currentPoint=nextPoint;
			}
			else if(foundBlack.size()>=3)//当前的点是一个分叉口 一般情况下应该是最多为3
			{//?
				three_around_points.push_back(currentPoint);//debug将分叉路口加入
				subPaths.push_back(vector<Point>());//debug增加新的子路
				if(foundBlack.size()>3)cout<<"##########Found Exception Points############\n";//如果点数超过3  认为是异常状态
				//以下默认按照只有3个点进行处理
				int kk_b=0;//用来判断是否找到了新的下一点
				for(int kk=0;kk<3;kk++)//kk<foundBlack.size() 遍历3个黑点，找到没有被visited过的
				{
					nextPoint=currentPoint+D_P_Map[ foundBlack[kk] ];
					if(visitedPoints.count(_Point(nextPoint))==0)
					{
						//这个点没有被visited
						if(kk_b==0)
						{
							lastPoint=currentPoint;
							currentPoint=nextPoint;
						}
						kk_b++;
					}
				}
				if(kk_b==0)//上一轮循环中没有找到新的可用点 ，结束
				{
					end=true;
				}
				else if(kk_b>=2)//上一轮循环中找到不止1个可用点 压栈
				{
					pointStack.push(lastPoint);
					//压栈之后开始记录分叉路的长度
					//cout<<"新的分叉路口"<<endl;
					//pathLength.push_back(0);
					//if(currentPath==-1)
					//	currentPath=0;
					//else
					//	currentPath++;

				}
				else//只有这一条分叉路没有走了
				{
					//cout<<"上一个分叉路的长度为:"<<pathLength[currentPath]<<endl;
					//cout<<"新的分叉路口"<<endl;
					//pathLength.push_back(0);
					//currentPath++;
				}
				//imshow("5",showImg);
				//waitKey(30);
			}
		
		}

		//chains.push_back(sub_chain);
		
	}

	map<int,int> path_length;
	
	
	cout<<"Print origin order sub paths...\n";
	for(int i=0;i<subPaths.size();i++)
	{
		path_length.insert(make_pair(i,subPaths[i].size()));  //将所有的子路径加入到map中进行排序
		printf("Path%d length=%d\n",i,subPaths[i].size());
		/*
		for(int j=0;j<subPaths[i].size();j++)
		{
			cout<<subPaths[i][j]<<endl;
		}
		*/
		//cout<<endl;
	}
	//对路径长度进行排序
	vector<PAIR> path_length_vec(path_length.begin(), path_length.end()); //首先将map转换成vector 
	sort(path_length_vec.begin(), path_length_vec.end(), CmpByValue());  //使用stl的排序函数对vec进行排序
	cout<<"Print sorted sub paths...\n";
	
	for(int i=0;i<path_length_vec.size();i++)//并输出排序之后的路径长度
	{

		cout<<path_length_vec[i]<<endl;
		if(path_length_vec[i].second>TREE_THRE_VALUE)//下面将过滤出来的 点数量 大于50 的路径画出来
		{
			int index=path_length_vec[i].first;
			cout<<"$$"<< index<<endl;
			//cout<<subPaths[index]<<endl;
			for(int j=0;j<subPaths[index].size();j++)
				//cout<<subPaths[index][j]<<endl;
				showImg.at<uchar>(subPaths[index][j])=0;//debug
		}
	}
	cout<<"Show handled image\n";

	//debug 对于分岔路口 全部断开
	for(int i=0;i<three_around_points.size();i++)
		showImg.at<uchar>(three_around_points[i])=255;//分岔路口全部设置为白色
	//enddebug

	globalImg=showImg;
	imshow("5_Prune",showImg);
	return showImg;
}

void Point_Join(vector<Point> &mainBody,const vector<Point> toAdd,bool reverse=false)
{
	if(!reverse)//正向拼接
	{
		for(int i=0;i<toAdd.size();i++)
		{		
			mainBody.push_back(toAdd[i]);
		}
	}
	else
	{
		for(int i=toAdd.size()-1;i>=0;i--)
		{		
			mainBody.push_back(toAdd[i]);
		}
	}
}
double Get_K(vector<Point> chain)//获取一段链的斜率k 使用最小二乘法
{
	int calc_points_num=chain.size();
	//if(chain.size()/3<calc_points_num)calc_points_num=chain.size()/3;//如果链比较短，那么计算延伸线段斜率时候取点数量也比较少

	//double deltStep=8;//最多延长7个单位
	//vector<Point> result(2);
	double test_x=chain[0].x;
	//这个链不能很长，否则可能无效 因为涉及到大弧度的曲线问题
	/*
	for(int i=0;i<calc_points_num;i++)
		if(test_x!=chain[i].x)
		{
			test_x=chain[i].x;
			break;
		}
	if(test_x==chain[0].x)//说明当前是一条垂直的竖线,直接延伸
	{
		
		if(chain[0].y<chain[1].y)
			result[0]=Point(test_x,chain[0].y-deltStep);
		else
			result[0]=Point(test_x,chain[0].y+deltStep);
	}
	*/
	//else
	//{
		vector<Point2f> toCalc;
		for(int i=0;i<calc_points_num;i++)
			toCalc.push_back(Point2f(chain[i].x,chain[i].y));
		LeastSquare ls(toCalc);
		return ls.a;
	//}

}

double get_delt_theta(double k1,double k2)
{
	#define get_theta(x) atan((x))*180/3.141592657
	double deltTheta1=abs(get_theta(k1)-get_theta(k2));
	if(deltTheta1>90)deltTheta1=180-deltTheta1;
	return deltTheta1;
}

vector<Point > take_chain2(vector<int> match_index,vector<vector<Point> >chains,vector<int> &not_walked,int points_size,int start_index=-1)
{
	printf("开始串联端点\n");
	int current_index=0;


	vector<Point > result;

	int join_time=chains.size();
	set<int> already_walked;//记录那些遍历过的链 在最后用来确定哪些链条没有被遍历
	printf("%d -> ",start_index);
	if(start_index%2==0)//从它右边出发
	{
		current_index=start_index+1;
		Point_Join(result,chains[start_index/2],false);
	}
	else
	{
		current_index=start_index-1;//从它左边出发
		Point_Join(result,chains[start_index/2],true);
		
	}
	printf("%d -> ",current_index);
	already_walked.insert(start_index);
	already_walked.insert(current_index);//记录

	for(int j_time=1;j_time<join_time;j_time++)//j_time-1次
	{
		if(match_index[match_index[current_index]]==current_index)//当前index对应的下一跳也指向自己
		{
			current_index=match_index[current_index];//进行跳转
			//print cur_index
			printf("%d -> ",current_index);
			already_walked.insert(current_index);//记录
			if(current_index%2==0)
			{
				current_index++;
				Point_Join(result,chains[current_index/2],false);
			}
			else
			{
				current_index--;
				Point_Join(result,chains[current_index/2],true);
			}
			already_walked.insert(current_index);//记录
				//print cur_index
			printf("%d -> ",current_index);
		}
		else
		{
			break;
		}
	}
	//already_walked
	
	cout<<"Detect if there is not walked chain.\n";
	for(int i=0;i<points_size;i++)
	{
		if(already_walked.count(i)==0)
		{
			printf("Chain %d not walked!\n",i);
			not_walked.push_back(i);
		}
	}

	return result;
}
vector<Point> Get_Chain(const Mat & input_raw)//获取链条
{
	Mat input=input_raw.clone();
	//首先是获取每个线条分段
	//理想状态下会有3个线段  不过也有可能会有更多的线段
	Mat showImg(input.size(),CV_8UC1,Scalar(255));

    vector<Point> endian;
    if(input.channels()!=1){cout<<"Error@ Fun (Find_EndPoint):Input image`s channel != 1\n";return vector<Point>();}
	

    endian=Find_Endpoint(input);
    printf("Endian amount:%d\n",endian.size());
    cout<<"Endians:\n";//输出端点
    cout<<endian<<endl;

    set   <int>   visitedPoints;//记录已经经过的点映射集 (x,y) -> x<<12|y
    #define _Point(p) (p).x<<15|(p).y  

    vector<vector<Point> > chains;

    printf("Begin Get Chain!\n");
    
    for(int i=0;i<endian.size();i++)//遍历每一个端点 先测试一个端点   endian.size()
    {
        printf("Handle:%d\n",i);
        //潜在的围墙bug
        visitedPoints.insert(_Point(endian[i]));//首先将端点插入到集合中
        vector<Point> sub_chain;//存放链条 这是要返回的结果
        Point currentPoint=endian[i];
        Point nextPoint;
        Point lastPoint;
        bool end=false;//算法结束标记
        Point D_P_Map[9]={Point(0,0),Point(1,0),Point(1,-1),Point(0,-1),Point(-1,-1),Point(-1,0),Point(-1,1),Point(0,1),Point(1,1)};
                                    //  1           2           3           4           5           6           7           8
        int panic=0;
        
        while(!end)
        {
            panic++;
            if(panic>1000)
            {
                printf("Panic state!Jump out!\n");
                break;//恐慌模式，因为迭代次数过多，进入死循环了，跳出；注意这里end没有设置为true
            }
            visitedPoints.insert(_Point(currentPoint));//将当前点插入到已经访问过的点集中
            sub_chain.push_back(currentPoint);//将当前点作为链条中的点加入到chain中
			//cout<<currentPoint<<endl;
            vector<int> foundBlack;
            foundBlack.clear();
            //检测周围8个点哪些是黑点
            for(int D_Walker=1;D_Walker<=8;D_Walker++)
            {
                if(input.at<uchar>(currentPoint+D_P_Map[D_Walker])==0)
                    foundBlack.push_back(D_Walker);
            }

            //下面决定下一步往哪里走
            if(foundBlack.size()==1)//周围只有1个黑点，说明当前是端点
            {
                nextPoint=currentPoint+D_P_Map[ foundBlack[0] ];//nextPoind中存放的是下个点的位置
                //需要判断这个点之前有没有经过
                if(visitedPoints.count(_Point(nextPoint))==1)//之前遇到过这个点，说明我走到了尽头 lastPoint==nextPoint
                {
                    end=true;//跳出循环
                }
                lastPoint=currentPoint;
                currentPoint=nextPoint;
            }
            else if(foundBlack.size()==2)//周围有两个黑点，说明当前点是处于线段的中间的点
            {
                Point p0,p1;
                p0=currentPoint+D_P_Map[ foundBlack[0] ];
                p1=currentPoint+D_P_Map[ foundBlack[1] ];
                if(visitedPoints.count(_Point(p0))==1)//p1是新找到的点 需要判断这个点之前有没有经过
                {
                    nextPoint=currentPoint+D_P_Map[ foundBlack[1] ];
                }
                else//p0是新找到的点
                {
                    nextPoint=currentPoint+D_P_Map[ foundBlack[0] ];
                }
                lastPoint=currentPoint;
                currentPoint=nextPoint;
            }
            else
            {
                //如果周围不止2个黑点，那么进入错误恢复模式
                cout<<"We run into a error state!\n";
                //debug这里需要增加处理函数
				 Point p0,p1;
                p0=currentPoint+D_P_Map[ foundBlack[0] ];
                p1=currentPoint+D_P_Map[ foundBlack[1] ];
                if(visitedPoints.count(_Point(p0))==1)//p1是新找到的点 需要判断这个点之前有没有经过
                {
                    nextPoint=currentPoint+D_P_Map[ foundBlack[1] ];
                }
                else//p0是新找到的点
                {
                    nextPoint=currentPoint+D_P_Map[ foundBlack[0] ];
                }
                lastPoint=currentPoint;
                currentPoint=nextPoint;

            }
           
        }
        chains.push_back(sub_chain); 
    }
	//过滤掉长度小于10的链条
	vector<vector<Point > > chains2;
	for(int i=0;i<chains.size();i++)//过滤掉那些长度非常小的线段
	{
		if(chains[i].size()>5)
			chains2.push_back(chains[i]);
	}
	chains=chains2;//交换 ,chains中存放最终的链
	vector<Point> points;
    cout<<"Print chains\n";
    for(int i=0;i<chains.size();i++)
    {
        printf("Chain %d: Length:%d\n",i,chains[i].size());
		char showWords[10];
		sprintf(showWords,"%d",i);
		//putText(showImg,showWords,chains[i][chains[i].size()/2],CV_FONT_BLACK,0.5,Scalar(0));

		//debug 将每一条线段在末端进行延伸
		//首先尝试只将延伸的点作为逻辑端点加入到points中
		/*
		vector<Point> extendP=Extend_Chain(chains[i]);
		if(extendP[0].x<0)extendP[0].x=0;
		if(extendP[0].x>showImg.cols)extendP[0].x=showImg.cols-1;
		if(extendP[0].y<0)extendP[0].y=0;
		if(extendP[0].y>showImg.rows)extendP[0].y=showImg.rows-1;

		if(extendP[1].x<0)extendP[1].x=0;
		if(extendP[1].x>showImg.cols)extendP[1].x=showImg.cols-1;
		if(extendP[1].y<0)extendP[1].y=0;
		if(extendP[1].y>showImg.rows)extendP[1].y=showImg.rows-1;
		*/
		//circle(showImg,extendP[0],4,Scalar(0));
		//circle(showImg,extendP[1],4,Scalar(0));

		points.push_back(chains[i][0]);
		points.push_back(chains[i][chains[i].size()-1]);	
		//points.push_back(extendP[0]);
		//points.push_back(extendP[1]);
    }
	
	 cout<<"Begin\n";
	 //使用跳链数组进行首尾匹配
	 vector<int> match_index(points.size());//用来存放匹配后的点下标
	 for(int i=0;i<points.size();i+=1)
	 {
		 char showWords[10];
	     sprintf(showWords,"%d",i);
		 putText(showImg,showWords,points[i],CV_FONT_BLACK,0.5,Scalar(0));
		 Point p0=points[i];
		 int skipNum=0;
		 if(i%2==0)skipNum=i+1;
		 else skipNum=i-1;

		 int minVal=10000000;
		 int minIndex=-1;
		 //printf("Point%d:\n",i);
		 for(int j=0;j<points.size();j++)
		 {
			 if(j==skipNum || j==i)continue;//不与自己比较
			 Point VecP=p0-points[j];
			 int dist=VecP.x*VecP.x+VecP.y*VecP.y;
			 ///printf("	距离 Point%d =%d\n",j,dist);
			 if(dist < minVal)
			 {
				 minVal=dist;
				 minIndex=j;
			 }
		 }
		 match_index[i]=minIndex;
		 //printf("	minIndex=%d\n",minIndex);
	 }

	cout<<"Print match index:\n";
	for(int i=0;i<match_index.size();i++)
	{
		cout<<match_index[i]<<endl;
		if(match_index[i]==-1)
		{
			cout<<"Error match index !Exit\n";
			return vector<Point>();
		}
	}

	int start_index=-1;
	for(int i=0;i<match_index.size();i++)
	{
		int jump_to=match_index[i];
		if(match_index[jump_to]!=i)//找到开始或者结束的点
		{
			start_index=i;
			break;
		}
	}
	 if(start_index==-1)//三者成环状连接，需要指定一个出发点
	 {
		 start_index=0;
	 }
	printf("开始串联端点\n");
	int current_index=0;


	vector<Point > result;

	int join_time=chains.size();
	set<int> already_walked;//记录那些遍历过的链 在最后用来确定哪些链条没有被遍历
	printf("%d -> ",start_index);
	if(start_index%2==0)//从它右边出发
	{
		current_index=start_index+1;
		Point_Join(result,chains[start_index/2],false);
	}
	else
	{
		current_index=start_index-1;//从它左边出发
		Point_Join(result,chains[start_index/2],true);
		
	}
	printf("%d -> ",current_index);
	already_walked.insert(start_index);
	already_walked.insert(current_index);//记录

	for(int j_time=1;j_time<join_time;j_time++)//j_time-1次
	{
		if(match_index[match_index[current_index]]==current_index)//当前index对应的下一跳也指向自己
		{
			current_index=match_index[current_index];//进行跳转
			//print cur_index
			printf("%d -> ",current_index);
			already_walked.insert(current_index);//记录
			if(current_index%2==0)
			{
				current_index++;
				Point_Join(result,chains[current_index/2],false);
			}
			else
			{
				current_index--;
				Point_Join(result,chains[current_index/2],true);
			}
			already_walked.insert(current_index);//记录
				//print cur_index
			printf("%d -> ",current_index);
		}
		else
		{
			break;
		}
	}
	//already_walked
	vector<int> not_walked;
	bool success_=true;
	cout<<"Detect if there is not walked chain.\n";
	for(int i=0;i<points.size();i++)
	{
		if(already_walked.count(i)==0)
		{
			printf("Chain %d not walked!\n",i);
			not_walked.push_back(i);
			success_=false;
		}
	}
	if(!success_)//使用常规方法串联失败  使用升级的方法
	{
		printf("!_success_\n");
		vector<vector<int> > match_index_list;//存储每个端点到其他端点的距离长短排序，存储的端点序号，长度从最小到最大
		vector<vector<int> > match_index_distance;//存储上面端点中对应的距离
		vector<double>      match_index_k;//存储每个端点附近的斜率
		
		for(int i=0;i<points.size();i+=1)//开始进行距离测算
		{
			map<int,int> distance_;

			Point p0=points[i];
			int skipNum=0;
			if(i%2==0)skipNum=i+1;
			else skipNum=i-1;

			for(int j=0;j<points.size();j++)
			{
				if(j==skipNum || j==i)continue;//不与自己比较
				Point VecP=p0-points[j];
				int dist=sqrt(VecP.x*VecP.x+VecP.y*VecP.y);
				 distance_[j]=dist;
			}
			vector<PAIR> vec1(distance_.begin(),distance_.end());
			std::sort(vec1.begin(),vec1.end(),CmpByValueSmallTop());
			vector<int> tmp1_index;
			vector<int> tmp1_distance;
			for(int tt=0;tt<vec1.size();tt++)
			{
				tmp1_index.push_back(vec1[tt].first);
				tmp1_distance.push_back(vec1[tt].second);
			}
			
			
			vector<Point> points_get_k;
			int wants_points=20;
			if(wants_points>chains[i/2].size())
			{//可以使用的点的数量很少
				points_get_k=chains[i/2];
			}
			else
			{//可以使用的点的数量足够
				if(i%2==0)//链开始的地方
				{
					for(int kk=0;kk<wants_points;kk++)
						points_get_k.push_back(chains[i/2][kk]);
				}
				else//链结束的地方
				{
					for(int kk=chains[i/2].size()-wants_points;kk<chains[i/2].size();kk++)
						points_get_k.push_back(chains[i/2][kk]);
				}
			}
			double my_k=Get_K(points_get_k);

			match_index_list.push_back(tmp1_index);
			match_index_distance.push_back(tmp1_distance);
			match_index_k.push_back(my_k);
		 }

		printf("Print match_index_list and distance\n");
		for(int i=0;i<match_index_list.size();i++)
		{
			printf("[%d] k=%f ",i,match_index_k[i]);
			for(int j=0;j<match_index_list[i].size();j++)
			{
				printf("%d:%d\t",match_index_list[i][j],match_index_distance[i][j]);
			}
			printf("\n");
		}
		//根据上面上个序列进行匹配
		//首先检测某个端点对应的最短距离的点数量
		//其次在特定情况下检测k值
#define THRESH_DISTANCE_VALUE 70

		vector<int> level_0_p;
		
		vector<int> match_group(points.size());//
		printf("Print new match group:\n");
		for(int i=0;i<match_index_list.size();i++)
		{
			int good_num=0;
			//首先是获取小于THRE_DIS_VAL的点的数量
			for(int j=0;j<match_index_distance[i].size();j++)
			{
				if(match_index_distance[i][j]<THRESH_DISTANCE_VALUE)
					good_num++;
			}
			match_group[i]=0;
			if(good_num==0)
				level_0_p.push_back(i);//level 0 点，最外围
			else if(good_num==1)
			{
				match_group[i]=match_index_list[i][0];
			}
			else if(good_num>=2)//进入斜率匹配阶段
			{
				int min_k_index=-1;
				int min_delt_theta=900;
				for(int kk=0;kk<good_num;kk++)
				{
					
					double delt_theta_=get_delt_theta(match_index_k[i],match_index_k[match_index_list[i][kk]]);
					if(delt_theta_<min_delt_theta)
					{
						min_k_index=kk;
						min_delt_theta=delt_theta_;
					}
				}
				match_group[i]=match_index_list[i][min_k_index];
			}
			printf("%d:%d  ",i,match_group[i]);
		}
		vector<int> other_to_go;
		if(level_0_p.size()!=0)
			result=take_chain2(match_group,chains,other_to_go,points.size(),level_0_p[0]);
		else
			result=take_chain2(match_group,chains,other_to_go,points.size(),0);

		if(other_to_go.size()!=0)
		{
			for(int kk=0;kk<other_to_go.size();kk+=2)
			{
				//可以将剩余没有走完的线段先进行拼接 然后再找一个端点连接到当前的result，但是我太累了，所以不想写了
				Point_Join(result,chains[other_to_go[kk]/2],false);
			}
		}
		printf("We are going to use new result!\n");
	}
	

	for(int j=0;j<result.size();j++)
	{
		showImg.at<uchar>(result[j])=0;
		imshow("6_GetChain",showImg);
		waitKey(3);
	}
	return result;	
}

vector<Point2f > Remap_Coord(vector<Point> chains)
{
	vector<Point2f> ret;
	if(chains.size()<10){cout<<"Chains size too small\n";return ret;}
	//首先将chains整理为下方的点为出发点
	Point firstP=chains[0];
	Point lastP=chains[chains.size()-1];
	vector<Point> ret_int;
	if(firstP.y>lastP.y)//当前的出发点在上方，反转chains
	{
		ret_int.clear();
		Point_Join(ret_int,chains,true);//反转链条
	}
	else
	{
		ret_int=chains;
	}
	for(int i=0;i<ret_int.size();i++)
	{
		float X0=ret_int[i].x;
		float Y0=ret_int[i].y;
		float XT=X0/5;		//转换x坐标
		float YT=100-Y0/5;	//转换y坐标
		ret.push_back(Point2f(XT,YT));
	}
	return ret;
}


void Serial_Transfer(vector<Point2f> chains)
{

    String toSend;
    char tmp_buff[40];
    toSend+="<S,0,0>";
    for(int i=0;i<chains.size();i++)
    {
        sprintf(tmp_buff,"<P,%.1f,%.1f>",chains[i].x,chains[i].y);
        toSend+=tmp_buff;
    }
    toSend+="<E,0,0>";
    cout<<"Command to send by serial:"<<toSend<<endl;

    #ifdef use_serial
    Serial_begin("/dev/ttyUSB0",115200);
    Serial_print(toSend);
    #endif
}

bool notClose=true;
vector<Point2f> corners(4);
Mat imgToMap(500,400,CV_8UC3);//校正后的矩形map
int cIndex=0;
void cvMouseCallback_(int mouseEvent,int x,int y,int flags,void* param)
{
  switch(mouseEvent)
  {
	case CV_EVENT_LBUTTONUP:

		corners[cIndex].x=x;
		corners[cIndex].y=y;
		printf("corner%d (%d,%d)\n",cIndex,x,y);
		if(cIndex==3)
		{
			notClose=false;
		}
		cIndex=(cIndex+1)%4;
    break;

	case CV_EVENT_RBUTTONUP:
		cIndex=0;
	break;
  }
}


Mat Get_Standard_Img(String path="standard.jpg")
{
	Mat rawImg;
	//ofstream param_file("config.txt",ios::in|ios::out);
	//int tmp;
	//param_file>>tmp;
	ifstream read_file("config.txt");
	int x=0,y=0;
	read_file>>x>>y;
	corners[0]=Point2f(x,y);
	read_file>>x>>y;
	corners[1]=Point2f(x,y);
	read_file>>x>>y;
	corners[2]=Point2f(x,y);
	read_file>>x>>y;
	corners[3]=Point2f(x,y);
	read_file.close();

#ifdef use_pi
	VideoCapture cap;
#endif
	cvNamedWindow("video",0);
	setMouseCallback("video",cvMouseCallback_);
#ifndef use_pi
	rawImg=imread(path);
#endif

#ifdef use_pi

	try{
		cap.open(0);
		if (!cap.isOpened())
		{
			cout << "***Could not cap0..***\n";
			cap.open(1);
			cout<<"Change to cap1\n";
		}
		cap>>rawImg;
	}
	catch(exception e)
	{
		cap.open(1);
		cout<<"Change to cap1\n";
		cap>>rawImg;
	}	
#endif

	while(notClose)
	{
		imshow("video",rawImg);
		if(waitKey(30)==27)
			notClose=false;
	}
	cvDestroyWindow("video");

	ofstream set_file("config.txt",ios::out);
	set_file<<(int)corners[0].x<<endl<<(int)corners[0].y<<endl;
	set_file<<(int)corners[1].x<<endl<<(int)corners[1].y<<endl;
	set_file<<(int)corners[2].x<<endl<<(int)corners[2].y<<endl;
	set_file<<(int)corners[3].x<<endl<<(int)corners[3].y<<endl;
	set_file.close();


	vector<Point2f> corners_trans(4);  
    corners_trans[0] = Point2f(0,0);  //获取矩形四个角的点(这里需要自动化实现)
    corners_trans[1] = Point2f(imgToMap.cols-1,0);  
    corners_trans[2] = Point2f(imgToMap.cols-1,imgToMap.rows-1);  
    corners_trans[3] = Point2f(0,imgToMap.rows-1);  

	Mat transform = getPerspectiveTransform(corners,corners_trans);  
    cout<<transform<<endl;  
    warpPerspective(rawImg, imgToMap, transform, imgToMap.size());//进行仿射变换，将不规则四边形转变成矩形

	return imgToMap;
}

#define use_up
#ifdef use_up

int main()
{
	//cvNamedWindow("1",1);
	cvNamedWindow("2_threImg",0);
	//cvNamedWindow("3",1);
	cvNamedWindow("5_Prune",1);
	setMouseCallback("5_Prune",cvMouseCallback);//globalImg
	createTrackbar( "Thre", "2_threImg", &thresholdValue, 255, trackbar);
	
	Mat image=Get_Standard_Img("19.jpg");//imread("standard.jpg");
	Mat grayImage;
	Mat threImage;
	cvtColor(image,grayImage,CV_BGR2GRAY);
	//imshow("1",image);
	waitKey(500);

	//thresholdValue=95;
	while(waitKey(30)!=27)
	{
		threshold(grayImage,threImage,thresholdValue,255,CV_THRESH_BINARY);
		imshow("2_threImg",threImage);
	}
	Mat processImg;
	Mat element=getStructuringElement(MORPH_CROSS,Size(3,3));
	dilate(threImage,processImg,element);
	erode(processImg,processImg,element);//进行腐蚀和膨胀
	erode(processImg,processImg,element);//进行腐蚀和膨胀
	threshold(processImg,threImage,thresholdValue,255,CV_THRESH_BINARY);
	imshow("2_threImg",threImage);

	printf("Enter diffuse func\n");//debug 这里是新增加的漫水骨架提取算法
	//threImage=Diffuse(threImage);
	printf("Enter sklen detect func\n");
	printf("Channels=%d\n",threImage.channels());
	Mat sklenImage=Sklen(threImage,array);//获取骨架
	//vector<Point> endian=Find_Endpoint(sklenImage);

	globalImg=sklenImage.clone();
	printf("Show sklen Image\n");
	//for(int i=0;i<endian.size();i++)
	//	circle(sklenImage,endian[i],5,Scalar(0));
	imshow("3_Sklen",sklenImage);
	//waitKey(100);
	cout<<"Get Chain...\n";
	Mat pruneImg=Prune(sklenImage);//剪枝
	vector<Point> chains=Get_Chain(pruneImg);
	vector<Point2f> Origin_Result=Remap_Coord(chains);//转换坐标系，使用浮点数存储 注意map尺寸是400*500
	printf("Done!\n");
	printf("Below is what is going to transmit by serial \n");
	vector<Point2f> Final_Result;//Origin Result 尺寸可能过大，放缩到100以内
	if(Origin_Result.size()>100)
	{
		float ratio_=(float)Origin_Result.size()/100;
		for(int i=0;i<100;i++)
		{
			Final_Result.push_back(Origin_Result[(unsigned int)((float)i*ratio_)]);
		}
	}
	else
		Final_Result=Origin_Result;

	cout<<Final_Result;
	cout<<"size of Final_Result:"<<Final_Result.size()<<endl;
	//waitKey(0);
	Serial_Transfer(Final_Result);
	return 0;
}
#endif



#ifndef use_up



int main()
{
		double k1=1;
		double k2=-1;
		double k3=5;
		double k4=-9;
		


		cout<<get_theta(k1)<<endl;
		cout<<get_theta(k2)<<endl;
		cout<<get_theta(k3)<<endl;
		cout<<get_theta(k4)<<endl<<endl;

		double deltTheta1=abs(get_theta(k1)-get_theta(k4));
		if(deltTheta1>90)deltTheta1=180-deltTheta1;
		cout<<deltTheta1<<endl;

	return 0;
}
#endif