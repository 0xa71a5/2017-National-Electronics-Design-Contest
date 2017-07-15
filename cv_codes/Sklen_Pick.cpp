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

//���ȴ���TREE_THRE_VALUE����������·������
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
char *dev  = "/dev/ttyUSB0"; //���ڶ�

int set_Parity(int fd,int databits,int stopbits,int parity)
{ 
	struct termios options; 
	if  ( tcgetattr( fd,&options)  !=  0) { 
		perror("SetupSerial 1");     
		return(FALSE);  
	}
	options.c_cflag &= ~CSIZE; 
	switch (databits) /*��������λ��*/
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
		options.c_cflag |= (PARODD | PARENB); /* ����Ϊ��Ч��*/  
		options.c_iflag |= INPCK;             /* Disnable parity checking */ 
		break;  
	case 'e':  
	case 'E':   
		options.c_cflag |= PARENB;     /* Enable parity */    
		options.c_cflag &= ~PARODD;   /* ת��ΪżЧ��*/     
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
/* ����ֹͣλ*/  
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
options.c_cc[VTIME] = 150; /* ���ó�ʱ15 seconds*/   
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


class LeastSquare{  //��С���˷�
   
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

typedef pair<int, int> PAIR;  //�����ֵ��
struct CmpByValue {  //�����ֵ������  �������������
  bool operator()(const PAIR& lhs, const PAIR& rhs) {  
    return lhs.second > rhs.second;  
  }  
};  

struct CmpByValueSmallTop {  //�����ֵ������  С������������
  bool operator()(const PAIR& lhs, const PAIR& rhs) {  
    return lhs.second < rhs.second;  
  }  
};  

ostream& operator<<(ostream& out, const PAIR& p) {  //����cout
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
				else cout<<"0 ";//�����������0  û��������255
		}
		cout<<endl;
	}
}

void cvMouseCallback(int mouseEvent,int x,int y,int flags,void* param)
{
  switch(mouseEvent)
  {
	case CV_EVENT_LBUTTONUP://����������
		printf("(%d,%d)=%d\n",x,y,(unsigned int)globalImg.at<uchar>(y,x));
		poiLeftUp=Point(x,y);
    break;

	case CV_EVENT_RBUTTONUP://����Ҽ�����
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
	case CV_EVENT_LBUTTONUP://����������
		printf("(%d,%d)\n",x,y);
		poiLeftUp1=Point(x,y);
    break;

	case CV_EVENT_RBUTTONUP://����Ҽ�����
		poiRightDown1=Point(x,y);
		printf("POI:");cout<<poiLeftUp1<<" "<<poiRightDown1<<endl;
		globalImg1=Mat(globalImg,Rect(poiLeftUp1,poiRightDown1));
		imshow("2",globalImg1);
	break;
  }
}

void VThin(Mat &image,int array[])//��ֱ�������������ˮ�㷨
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

void HThin(Mat &image,int array[])//ˮƽ�������������ˮ�㷨
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


	float radio=0.95;//Ϳ�ױ�Ե
	int start_paint_row=midlle_row-all_rows*radio/2;
	int end_paint_row=midlle_row+all_rows*radio/2;

	int start_paint_col=middle_col-all_cols*radio/2;
	int end_paint_col=middle_col+all_cols*radio/2;

	//���Ƚ����б�ԵͿ��
	for(int i=0;i<start_paint_row;i++)//�����ϱ���Ϳ��
		for(int j=0;j<all_cols;j++)
			iXihua.at<uchar>(i,j)=255;
	for(int i=end_paint_row;i<all_rows;i++)//�����±���Ϳ��
		for(int j=0;j<all_cols;j++)
			iXihua.at<uchar>(i,j)=255;
	for(int i=0;i<all_rows;i++)
		for(int j=0;j<start_paint_col;j++)//���������Ϳ��
			iXihua.at<uchar>(i,j)=255;
	for(int i=0;i<all_rows;i++)
		for(int j=end_paint_col;j<all_cols;j++)//���������Ϳ��
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
	//���ϵ��� ������ ɨ���ֵͼ�����ݾŹ��񷽷��ж�һ�����Ƿ��Ƕ˵�
	//������߶� ��ֵͼ��Ϊ0  �հ������÷�0 ��ʾ
	//֮����Ҫ���� ���μ�� ��ֹ������
	vector<Point> result;
	if(input.channels()!=1){cout<<"Error@ Fun (Find_EndPoint):Input image`s channel != 1\n";return result;}
	for(int i=1;i<input.rows-1;i++)//ɨ���ʱ��ܿ�������ı߿�
	{
		for(int j=1;j<input.cols-1;j++)//ɨ���ʱ��ܿ�������ı߿�
		{
			if(input.at<uchar>(i,j)==0)//������һ���ڵ㣬��ʼ�������Χ�м����ڵ�
			{
				//�����Χ8������ֻ���ҵ�һ���ڵ㣬��ô���õ������Ϊ�Ƕ˵㣬���뵽�������
				//����Ǻڵ�Ϊ255
				#define BlankValue 255 
				int sum=input.at<uchar>(i,j-1)+input.at<uchar>(i-1,j-1)+input.at<uchar>(i-1,j)+input.at<uchar>(i-1,j+1)+input.at<uchar>(i,j+1)+input.at<uchar>(i+1,j+1)+input.at<uchar>(i+1,j)+input.at<uchar>(i+1,j-1);
				if(sum==BlankValue*7)//��Χ��7����ɫ�㣬Ҳ����ֻ��1����ɫ��
					result.push_back(Point(j,i));
			}
		}
	}
	return result;
}

Mat Diffuse(Mat input)//��ͨ������ֵ�����ͼ��  
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


Mat Prune(const Mat & input_raw)//�ԹǼ�ͼ����м�֦
{
	Mat input=input_raw.clone();
	Mat showImg(input.size(),CV_8UC1,Scalar(255));

	vector<Point> three_around_points;
	vector<Point> endian;
	if(input.channels()!=1){cout<<"Error@ Fun (Find_EndPoint):Input image`s channel != 1\n";return showImg ;}
	endian=Find_Endpoint(input);
	
	stack <Point> pointStack;//������ȱ������洢��
	set   <int>   visitedPoints;//��¼�Ѿ������ĵ�ӳ�伯 (x,y) -> x<<12|y
	#define _Point(p) (p).x<<15|(p).y  

	//vector<vector<Point> > chains;
	vector<vector<Point> > subPaths;

	printf("Begin Get Chain!\n");
	printf("Endian amount:%d\n",endian.size());
	for(int i=0;i<endian.size();i++)//����ÿһ���˵� �Ȳ���һ���˵�   endian.size()
	{
		printf("Handle:%d\n",i);
		//Ǳ�ڵ�Χǽbug
		visitedPoints.insert(_Point(endian[i]));//���Ƚ��˵���뵽������
		vector<Point> sub_chain;//������� ����Ҫ���صĽ��
		Point currentPoint=endian[i];
		Point nextPoint;
		Point lastPoint;
		bool end=false;//�㷨�������
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
				break;//�ֻ�ģʽ����Ϊ�����������࣬������ѭ���ˣ�������ע������endû������Ϊtrue
			}
			visitedPoints.insert(_Point(currentPoint));//����ǰ����뵽�Ѿ����ʹ��ĵ㼯��
			//sub_chain.push_back(currentPoint);//����ǰ����Ϊ�����еĵ���뵽chain��
			
			subPaths[subPaths.size()-1].push_back(currentPoint);//debug ���뵱ǰ�㵽��·��
			//if(currentPath>=0)pathLength[currentPath]++;//�ֲ�·�����ȼ�¼
			//std::cout<<currentPoint<<endl;

			vector<int> foundBlack;
			foundBlack.clear();
			//�����Χ8������Щ�Ǻڵ�
			for(int D_Walker=1;D_Walker<=8;D_Walker++)
			{
				if(input.at<uchar>(currentPoint+D_P_Map[D_Walker])==0)
					foundBlack.push_back(D_Walker);
			}



			if(foundBlack.size()==1)//��Χֻ��1���ڵ㣬˵����ǰ�Ƕ˵�
			{
				nextPoint=currentPoint+D_P_Map[ foundBlack[0] ];//nextPoind�д�ŵ����¸����λ��
				//��Ҫ�ж������֮ǰ��û�о���
				if(visitedPoints.count(_Point(nextPoint))==1)//֮ǰ����������㣬˵�����ߵ��˾�ͷ lastPoint==nextPoint
				{
					if(pointStack.size()==0)//ת�۵㲻���ڣ�˵�������߶���������
					{
						end=true;//����ѭ��
					}
					else//����ת�۵�,����Ҫ�ص�ת�۵�ĵط�������
					{
						subPaths.push_back(vector<Point>());//debug�����µ���·
						nextPoint=pointStack.top();//�ص�ת�۵�
						pointStack.pop();//��ջһ��ת�۵�
					}
				}
				else//֮ǰû�����������
				{}
				lastPoint=currentPoint;
				currentPoint=nextPoint;
			}
			else if(foundBlack.size()==2)//��Χ�������ڵ㣬˵����ǰ���Ǵ����߶ε��м�ĵ�
			{
				Point p0,p1;
				p0=currentPoint+D_P_Map[ foundBlack[0] ];
				p1=currentPoint+D_P_Map[ foundBlack[1] ];
				if(visitedPoints.count(_Point(p0))==1)//p1�����ҵ��ĵ� ��Ҫ�ж������֮ǰ��û�о���
				{
					nextPoint=currentPoint+D_P_Map[ foundBlack[1] ];
				}
				else//p0�����ҵ��ĵ�
				{
					nextPoint=currentPoint+D_P_Map[ foundBlack[0] ];
				}
				lastPoint=currentPoint;
				currentPoint=nextPoint;
			}
			else if(foundBlack.size()>=3)//��ǰ�ĵ���һ���ֲ�� һ�������Ӧ�������Ϊ3
			{//?
				three_around_points.push_back(currentPoint);//debug���ֲ�·�ڼ���
				subPaths.push_back(vector<Point>());//debug�����µ���·
				if(foundBlack.size()>3)cout<<"##########Found Exception Points############\n";//�����������3  ��Ϊ���쳣״̬
				//����Ĭ�ϰ���ֻ��3������д���
				int kk_b=0;//�����ж��Ƿ��ҵ����µ���һ��
				for(int kk=0;kk<3;kk++)//kk<foundBlack.size() ����3���ڵ㣬�ҵ�û�б�visited����
				{
					nextPoint=currentPoint+D_P_Map[ foundBlack[kk] ];
					if(visitedPoints.count(_Point(nextPoint))==0)
					{
						//�����û�б�visited
						if(kk_b==0)
						{
							lastPoint=currentPoint;
							currentPoint=nextPoint;
						}
						kk_b++;
					}
				}
				if(kk_b==0)//��һ��ѭ����û���ҵ��µĿ��õ� ������
				{
					end=true;
				}
				else if(kk_b>=2)//��һ��ѭ�����ҵ���ֹ1�����õ� ѹջ
				{
					pointStack.push(lastPoint);
					//ѹջ֮��ʼ��¼�ֲ�·�ĳ���
					//cout<<"�µķֲ�·��"<<endl;
					//pathLength.push_back(0);
					//if(currentPath==-1)
					//	currentPath=0;
					//else
					//	currentPath++;

				}
				else//ֻ����һ���ֲ�·û������
				{
					//cout<<"��һ���ֲ�·�ĳ���Ϊ:"<<pathLength[currentPath]<<endl;
					//cout<<"�µķֲ�·��"<<endl;
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
		path_length.insert(make_pair(i,subPaths[i].size()));  //�����е���·�����뵽map�н�������
		printf("Path%d length=%d\n",i,subPaths[i].size());
		/*
		for(int j=0;j<subPaths[i].size();j++)
		{
			cout<<subPaths[i][j]<<endl;
		}
		*/
		//cout<<endl;
	}
	//��·�����Ƚ�������
	vector<PAIR> path_length_vec(path_length.begin(), path_length.end()); //���Ƚ�mapת����vector 
	sort(path_length_vec.begin(), path_length_vec.end(), CmpByValue());  //ʹ��stl����������vec��������
	cout<<"Print sorted sub paths...\n";
	
	for(int i=0;i<path_length_vec.size();i++)//���������֮���·������
	{

		cout<<path_length_vec[i]<<endl;
		if(path_length_vec[i].second>TREE_THRE_VALUE)//���潫���˳����� ������ ����50 ��·��������
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

	//debug ���ڷֲ�·�� ȫ���Ͽ�
	for(int i=0;i<three_around_points.size();i++)
		showImg.at<uchar>(three_around_points[i])=255;//�ֲ�·��ȫ������Ϊ��ɫ
	//enddebug

	globalImg=showImg;
	imshow("5_Prune",showImg);
	return showImg;
}

void Point_Join(vector<Point> &mainBody,const vector<Point> toAdd,bool reverse=false)
{
	if(!reverse)//����ƴ��
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
double Get_K(vector<Point> chain)//��ȡһ������б��k ʹ����С���˷�
{
	int calc_points_num=chain.size();
	//if(chain.size()/3<calc_points_num)calc_points_num=chain.size()/3;//������Ƚ϶̣���ô���������߶�б��ʱ��ȡ������Ҳ�Ƚ���

	//double deltStep=8;//����ӳ�7����λ
	//vector<Point> result(2);
	double test_x=chain[0].x;
	//��������ܺܳ������������Ч ��Ϊ�漰���󻡶ȵ���������
	/*
	for(int i=0;i<calc_points_num;i++)
		if(test_x!=chain[i].x)
		{
			test_x=chain[i].x;
			break;
		}
	if(test_x==chain[0].x)//˵����ǰ��һ����ֱ������,ֱ������
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
	printf("��ʼ�����˵�\n");
	int current_index=0;


	vector<Point > result;

	int join_time=chains.size();
	set<int> already_walked;//��¼��Щ���������� ���������ȷ����Щ����û�б�����
	printf("%d -> ",start_index);
	if(start_index%2==0)//�����ұ߳���
	{
		current_index=start_index+1;
		Point_Join(result,chains[start_index/2],false);
	}
	else
	{
		current_index=start_index-1;//������߳���
		Point_Join(result,chains[start_index/2],true);
		
	}
	printf("%d -> ",current_index);
	already_walked.insert(start_index);
	already_walked.insert(current_index);//��¼

	for(int j_time=1;j_time<join_time;j_time++)//j_time-1��
	{
		if(match_index[match_index[current_index]]==current_index)//��ǰindex��Ӧ����һ��Ҳָ���Լ�
		{
			current_index=match_index[current_index];//������ת
			//print cur_index
			printf("%d -> ",current_index);
			already_walked.insert(current_index);//��¼
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
			already_walked.insert(current_index);//��¼
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
vector<Point> Get_Chain(const Mat & input_raw)//��ȡ����
{
	Mat input=input_raw.clone();
	//�����ǻ�ȡÿ�������ֶ�
	//����״̬�»���3���߶�  ����Ҳ�п��ܻ��и�����߶�
	Mat showImg(input.size(),CV_8UC1,Scalar(255));

    vector<Point> endian;
    if(input.channels()!=1){cout<<"Error@ Fun (Find_EndPoint):Input image`s channel != 1\n";return vector<Point>();}
	

    endian=Find_Endpoint(input);
    printf("Endian amount:%d\n",endian.size());
    cout<<"Endians:\n";//����˵�
    cout<<endian<<endl;

    set   <int>   visitedPoints;//��¼�Ѿ������ĵ�ӳ�伯 (x,y) -> x<<12|y
    #define _Point(p) (p).x<<15|(p).y  

    vector<vector<Point> > chains;

    printf("Begin Get Chain!\n");
    
    for(int i=0;i<endian.size();i++)//����ÿһ���˵� �Ȳ���һ���˵�   endian.size()
    {
        printf("Handle:%d\n",i);
        //Ǳ�ڵ�Χǽbug
        visitedPoints.insert(_Point(endian[i]));//���Ƚ��˵���뵽������
        vector<Point> sub_chain;//������� ����Ҫ���صĽ��
        Point currentPoint=endian[i];
        Point nextPoint;
        Point lastPoint;
        bool end=false;//�㷨�������
        Point D_P_Map[9]={Point(0,0),Point(1,0),Point(1,-1),Point(0,-1),Point(-1,-1),Point(-1,0),Point(-1,1),Point(0,1),Point(1,1)};
                                    //  1           2           3           4           5           6           7           8
        int panic=0;
        
        while(!end)
        {
            panic++;
            if(panic>1000)
            {
                printf("Panic state!Jump out!\n");
                break;//�ֻ�ģʽ����Ϊ�����������࣬������ѭ���ˣ�������ע������endû������Ϊtrue
            }
            visitedPoints.insert(_Point(currentPoint));//����ǰ����뵽�Ѿ����ʹ��ĵ㼯��
            sub_chain.push_back(currentPoint);//����ǰ����Ϊ�����еĵ���뵽chain��
			//cout<<currentPoint<<endl;
            vector<int> foundBlack;
            foundBlack.clear();
            //�����Χ8������Щ�Ǻڵ�
            for(int D_Walker=1;D_Walker<=8;D_Walker++)
            {
                if(input.at<uchar>(currentPoint+D_P_Map[D_Walker])==0)
                    foundBlack.push_back(D_Walker);
            }

            //���������һ����������
            if(foundBlack.size()==1)//��Χֻ��1���ڵ㣬˵����ǰ�Ƕ˵�
            {
                nextPoint=currentPoint+D_P_Map[ foundBlack[0] ];//nextPoind�д�ŵ����¸����λ��
                //��Ҫ�ж������֮ǰ��û�о���
                if(visitedPoints.count(_Point(nextPoint))==1)//֮ǰ����������㣬˵�����ߵ��˾�ͷ lastPoint==nextPoint
                {
                    end=true;//����ѭ��
                }
                lastPoint=currentPoint;
                currentPoint=nextPoint;
            }
            else if(foundBlack.size()==2)//��Χ�������ڵ㣬˵����ǰ���Ǵ����߶ε��м�ĵ�
            {
                Point p0,p1;
                p0=currentPoint+D_P_Map[ foundBlack[0] ];
                p1=currentPoint+D_P_Map[ foundBlack[1] ];
                if(visitedPoints.count(_Point(p0))==1)//p1�����ҵ��ĵ� ��Ҫ�ж������֮ǰ��û�о���
                {
                    nextPoint=currentPoint+D_P_Map[ foundBlack[1] ];
                }
                else//p0�����ҵ��ĵ�
                {
                    nextPoint=currentPoint+D_P_Map[ foundBlack[0] ];
                }
                lastPoint=currentPoint;
                currentPoint=nextPoint;
            }
            else
            {
                //�����Χ��ֹ2���ڵ㣬��ô�������ָ�ģʽ
                cout<<"We run into a error state!\n";
                //debug������Ҫ���Ӵ�����
				 Point p0,p1;
                p0=currentPoint+D_P_Map[ foundBlack[0] ];
                p1=currentPoint+D_P_Map[ foundBlack[1] ];
                if(visitedPoints.count(_Point(p0))==1)//p1�����ҵ��ĵ� ��Ҫ�ж������֮ǰ��û�о���
                {
                    nextPoint=currentPoint+D_P_Map[ foundBlack[1] ];
                }
                else//p0�����ҵ��ĵ�
                {
                    nextPoint=currentPoint+D_P_Map[ foundBlack[0] ];
                }
                lastPoint=currentPoint;
                currentPoint=nextPoint;

            }
           
        }
        chains.push_back(sub_chain); 
    }
	//���˵�����С��10������
	vector<vector<Point > > chains2;
	for(int i=0;i<chains.size();i++)//���˵���Щ���ȷǳ�С���߶�
	{
		if(chains[i].size()>5)
			chains2.push_back(chains[i]);
	}
	chains=chains2;//���� ,chains�д�����յ���
	vector<Point> points;
    cout<<"Print chains\n";
    for(int i=0;i<chains.size();i++)
    {
        printf("Chain %d: Length:%d\n",i,chains[i].size());
		char showWords[10];
		sprintf(showWords,"%d",i);
		//putText(showImg,showWords,chains[i][chains[i].size()/2],CV_FONT_BLACK,0.5,Scalar(0));

		//debug ��ÿһ���߶���ĩ�˽�������
		//���ȳ���ֻ������ĵ���Ϊ�߼��˵���뵽points��
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
	 //ʹ���������������βƥ��
	 vector<int> match_index(points.size());//�������ƥ���ĵ��±�
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
			 if(j==skipNum || j==i)continue;//�����Լ��Ƚ�
			 Point VecP=p0-points[j];
			 int dist=VecP.x*VecP.x+VecP.y*VecP.y;
			 ///printf("	���� Point%d =%d\n",j,dist);
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
		if(match_index[jump_to]!=i)//�ҵ���ʼ���߽����ĵ�
		{
			start_index=i;
			break;
		}
	}
	 if(start_index==-1)//���߳ɻ�״���ӣ���Ҫָ��һ��������
	 {
		 start_index=0;
	 }
	printf("��ʼ�����˵�\n");
	int current_index=0;


	vector<Point > result;

	int join_time=chains.size();
	set<int> already_walked;//��¼��Щ���������� ���������ȷ����Щ����û�б�����
	printf("%d -> ",start_index);
	if(start_index%2==0)//�����ұ߳���
	{
		current_index=start_index+1;
		Point_Join(result,chains[start_index/2],false);
	}
	else
	{
		current_index=start_index-1;//������߳���
		Point_Join(result,chains[start_index/2],true);
		
	}
	printf("%d -> ",current_index);
	already_walked.insert(start_index);
	already_walked.insert(current_index);//��¼

	for(int j_time=1;j_time<join_time;j_time++)//j_time-1��
	{
		if(match_index[match_index[current_index]]==current_index)//��ǰindex��Ӧ����һ��Ҳָ���Լ�
		{
			current_index=match_index[current_index];//������ת
			//print cur_index
			printf("%d -> ",current_index);
			already_walked.insert(current_index);//��¼
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
			already_walked.insert(current_index);//��¼
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
	if(!success_)//ʹ�ó��淽������ʧ��  ʹ�������ķ���
	{
		printf("!_success_\n");
		vector<vector<int> > match_index_list;//�洢ÿ���˵㵽�����˵�ľ��볤�����򣬴洢�Ķ˵���ţ����ȴ���С�����
		vector<vector<int> > match_index_distance;//�洢����˵��ж�Ӧ�ľ���
		vector<double>      match_index_k;//�洢ÿ���˵㸽����б��
		
		for(int i=0;i<points.size();i+=1)//��ʼ���о������
		{
			map<int,int> distance_;

			Point p0=points[i];
			int skipNum=0;
			if(i%2==0)skipNum=i+1;
			else skipNum=i-1;

			for(int j=0;j<points.size();j++)
			{
				if(j==skipNum || j==i)continue;//�����Լ��Ƚ�
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
			{//����ʹ�õĵ����������
				points_get_k=chains[i/2];
			}
			else
			{//����ʹ�õĵ�������㹻
				if(i%2==0)//����ʼ�ĵط�
				{
					for(int kk=0;kk<wants_points;kk++)
						points_get_k.push_back(chains[i/2][kk]);
				}
				else//�������ĵط�
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
		//���������ϸ����н���ƥ��
		//���ȼ��ĳ���˵��Ӧ����̾���ĵ�����
		//������ض�����¼��kֵ
#define THRESH_DISTANCE_VALUE 70

		vector<int> level_0_p;
		
		vector<int> match_group(points.size());//
		printf("Print new match group:\n");
		for(int i=0;i<match_index_list.size();i++)
		{
			int good_num=0;
			//�����ǻ�ȡС��THRE_DIS_VAL�ĵ������
			for(int j=0;j<match_index_distance[i].size();j++)
			{
				if(match_index_distance[i][j]<THRESH_DISTANCE_VALUE)
					good_num++;
			}
			match_group[i]=0;
			if(good_num==0)
				level_0_p.push_back(i);//level 0 �㣬����Χ
			else if(good_num==1)
			{
				match_group[i]=match_index_list[i][0];
			}
			else if(good_num>=2)//����б��ƥ��׶�
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
				//���Խ�ʣ��û��������߶��Ƚ���ƴ�� Ȼ������һ���˵����ӵ���ǰ��result��������̫���ˣ����Բ���д��
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
	//���Ƚ�chains����Ϊ�·��ĵ�Ϊ������
	Point firstP=chains[0];
	Point lastP=chains[chains.size()-1];
	vector<Point> ret_int;
	if(firstP.y>lastP.y)//��ǰ�ĳ��������Ϸ�����תchains
	{
		ret_int.clear();
		Point_Join(ret_int,chains,true);//��ת����
	}
	else
	{
		ret_int=chains;
	}
	for(int i=0;i<ret_int.size();i++)
	{
		float X0=ret_int[i].x;
		float Y0=ret_int[i].y;
		float XT=X0/5;		//ת��x����
		float YT=100-Y0/5;	//ת��y����
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
Mat imgToMap(500,400,CV_8UC3);//У����ľ���map
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
    corners_trans[0] = Point2f(0,0);  //��ȡ�����ĸ��ǵĵ�(������Ҫ�Զ���ʵ��)
    corners_trans[1] = Point2f(imgToMap.cols-1,0);  
    corners_trans[2] = Point2f(imgToMap.cols-1,imgToMap.rows-1);  
    corners_trans[3] = Point2f(0,imgToMap.rows-1);  

	Mat transform = getPerspectiveTransform(corners,corners_trans);  
    cout<<transform<<endl;  
    warpPerspective(rawImg, imgToMap, transform, imgToMap.size());//���з���任�����������ı���ת��ɾ���

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
	erode(processImg,processImg,element);//���и�ʴ������
	erode(processImg,processImg,element);//���и�ʴ������
	threshold(processImg,threImage,thresholdValue,255,CV_THRESH_BINARY);
	imshow("2_threImg",threImage);

	printf("Enter diffuse func\n");//debug �����������ӵ���ˮ�Ǽ���ȡ�㷨
	//threImage=Diffuse(threImage);
	printf("Enter sklen detect func\n");
	printf("Channels=%d\n",threImage.channels());
	Mat sklenImage=Sklen(threImage,array);//��ȡ�Ǽ�
	//vector<Point> endian=Find_Endpoint(sklenImage);

	globalImg=sklenImage.clone();
	printf("Show sklen Image\n");
	//for(int i=0;i<endian.size();i++)
	//	circle(sklenImage,endian[i],5,Scalar(0));
	imshow("3_Sklen",sklenImage);
	//waitKey(100);
	cout<<"Get Chain...\n";
	Mat pruneImg=Prune(sklenImage);//��֦
	vector<Point> chains=Get_Chain(pruneImg);
	vector<Point2f> Origin_Result=Remap_Coord(chains);//ת������ϵ��ʹ�ø������洢 ע��map�ߴ���400*500
	printf("Done!\n");
	printf("Below is what is going to transmit by serial \n");
	vector<Point2f> Final_Result;//Origin Result �ߴ���ܹ��󣬷�����100����
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