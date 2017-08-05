#define use_up

#define use_pi

#ifdef use_pi
	#define use_serial
    #include <wiringPi.h>
#endif

#include <sys/types.h>

#ifdef use_serial
	#include <unistd.h>
	#include <termios.h>
#endif
bool toExit=false;
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


using namespace cv;
using namespace std;

Mat globalImg;
bool toGo=false;
float leftBoundary=0;
float rightBoundary=0;
    



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

int globalX=21;
int globalY=39;
bool showImg=true;
int cIndex=0;
vector<Point> corners(4);
int OutputPin=1;
int thresholdValue=98;

void trackbar(int input,void *u);
void cvMouseCallback(int mouseEvent,int x,int y,int flags,void* param);//鼠标事件响应函数;
void getScanBoundary(Mat &threImg,float &leftPoint,float &rightPoint);
float getBallCenter(Mat &threImg,float leftB,float rightB,int scanRow);

Point track_leftUp;
Point track_rightDown;
float middle_line=0;

void trackbar(int input,void *u)  
{  
    thresholdValue=input;
} 

void cvMouseCallback(int mouseEvent,int x,int y,int flags,void* param)//鼠标事件响应函数
{
  switch(mouseEvent)
  {
    case CV_EVENT_LBUTTONUP:
        corners[0].x=x;
        corners[0].y=y;
        printf("corner%d (%d,%d)\n",0,x,y);
    break;

    case CV_EVENT_RBUTTONUP:
        cIndex=0;
        toGo=true;
        getScanBoundary(globalImg,leftBoundary,rightBoundary);//获取左右两条边界
        track_leftUp=Point(leftBoundary,middle_line-27);
        track_rightDown=Point(rightBoundary,middle_line);
        printf("leftBoundary=%.1f rightBoundary=%.1f\n",leftBoundary,rightBoundary);
    break;
  }
}

void getScanBoundary(Mat &threImg,float &leftPoint,float &rightPoint)
{
    for(int i=corners[0].x;i>=0;i--)//寻找左边界标志
    {
        if(threImg.at<uchar>(corners[0].y,i)==0)
        {
            leftPoint=i;
            break;
        }
    }
    for(int i=corners[0].x;i<threImg.cols;i++)//寻找右边界标志
    {
        if(threImg.at<uchar>(corners[0].y,i)==0)
        {
            rightPoint=i;
            break;
        }
    }
    leftPoint+=1;
    rightPoint-=1;
}

float getBallCenter(Mat &threImg,float leftB,float rightB,int scanRow)
{
    int scanStartCol=leftB;//corners[0].x;
    int scanEndCol=rightB;//corners[1].x+1;
    //int scanRow=corners[0].y;
    float foundPoint1=0;
    float foundPoint2=0;
    for(int i=scanStartCol;i<scanEndCol+5;i++)
    {
        if(threImg.at<uchar>(scanRow,i)==0)//从左向右扫描
        {
            //连续判断3个点  如果画面足够大 不会溢出 否则考虑溢出问题
            if(threImg.at<uchar>(scanRow,i+1)==0&&threImg.at<uchar>(scanRow,i+2)==0&&threImg.at<uchar>(scanRow,i+3)==0&&threImg.at<uchar>(scanRow,i+4)==0)
            {
                foundPoint1=i;
                break;
            }
        }
    }

    for(int i=scanEndCol;i>=scanStartCol-5;i--)
    {
        if(threImg.at<uchar>(scanRow,i)==0)//从右向左扫描
        {
            //连续判断3个点  如果画面足够大 不会溢出 否则考虑溢出问题
            if(threImg.at<uchar>(scanRow,i-1)==0&&threImg.at<uchar>(scanRow,i-2)==0&&threImg.at<uchar>(scanRow,i-3)==0&&threImg.at<uchar>(scanRow,i-4)==0)
            {
                foundPoint2=i;
                break;
            }
        }
    }
    //计算中点位置
    float centerPoint=(foundPoint1+foundPoint2)/2;
    float lengthOfImg=scanEndCol-scanStartCol;
    float lengthOfPhysic=55;
    if(lengthOfImg!=0)
    {
        float lengthOfRight=scanEndCol-centerPoint;
        //lengthOfPhysic=58*lengthOfRight/lengthOfImg;//默认杆子长
        lengthOfPhysic=58.84824519*lengthOfRight/lengthOfImg-1.65454859;
    }
    return lengthOfPhysic;
}

/*
float getTrackRedCenter(Mat &inputImg,Point leftUp,Point rightDown)
{
    Mat colorBlock=inputImg(Rect(leftUp,rightDown));//172
    Mat hsvImg;
    cvtColor(colorBlock,hsvImg,CV_BGR2HSV_FULL);
    float sumX=0;
    float sumAmount=0;
    for(int i=0;i<hsvImg.rows;i++)
    {
        for(int j=0;j<hsvImg.cols;j++)
        {
            if(hsvImg.at<Vec3b>(i,j)[0]<=5||hsvImg.at<Vec3b>(i,j)[0]>=250)
            {
                if(hsvImg.at<Vec3b>(i,j)[1]>43&&hsvImg.at<Vec3b>(i,j)[2]>46)
                {
                        sumX+=j;
                        sumAmount+=1;
                }
            }
        }
    }
    if(sumAmount!=0)
    {
        sumX/=sumAmount;
    }

    float lengthOfImg=rightDown.x-leftUp.x;
    float lengthOfPhysic=55;
    if(lengthOfImg!=0)
    {
        float lengthOfRight=lengthOfImg-sumX;
        lengthOfPhysic=58*lengthOfRight/lengthOfImg;//默认杆子长
    }

    return lengthOfPhysic;
}
*/

float getTrackRedCenter(Mat &inputImg,Point leftUp,Point rightDown)
{
    Mat colorBlock=inputImg(Rect(leftUp,rightDown));//172
    Mat hsvImg;
    cvtColor(colorBlock,hsvImg,CV_BGR2HSV_FULL);
    Mat showImg(hsvImg.size(),CV_8UC1,Scalar(255));
    
    int hitCount=0;
    vector<float> x_position(800);
    int vector_size=800;
    
    for(int i=0;i<hsvImg.rows;i++)
    {
        for(int j=0;j<hsvImg.cols;j++)
        {
            if(hsvImg.at<Vec3b>(i,j)[0]<=5||hsvImg.at<Vec3b>(i,j)[0]>=250)
            {
                if(hsvImg.at<Vec3b>(i,j)[1]>43&&hsvImg.at<Vec3b>(i,j)[2]>46)
                {
                    x_position[hitCount]=(float)j;  
                    hitCount++;
                    if(hitCount>=vector_size)
                    {
                        vector_size+=800;
                        x_position.resize(vector_size);
                    }
                }
            }
        }
    }
    if(hitCount!=0)
    {
        x_position.resize(hitCount);
        sort(x_position.begin(),x_position.end(),less<int>());
    }
    
    float lengthOfImg=rightDown.x-leftUp.x;
    float lengthOfPhysic=0;

    if(lengthOfImg!=0)
    {
        float lengthOfRight=rightDown.x-x_position[hitCount/2];
        lengthOfPhysic=58.9333516344*lengthOfRight/lengthOfImg-5.35319415313;//默认杆子长60
        //printf("%.1f\n\n",lengthOfPhysic);
    }
    return lengthOfPhysic;
}

void ReadParameter(Point& p)
{
    ifstream read_file("config.txt");
    read_file>>p.x;
    read_file>>p.y;
    read_file.close();
}

void WriteParameter(Point& p)
{
    ofstream write_file("config.txt",ios::out);
    write_file<<p.x<<endl;
    write_file<<p.y<<endl;
    write_file.close();
}



#ifdef use_up
int main(int argc,char * argv[])
{
    int imageIndex=0;
    wiringPiSetup();
    int cameraIndex=0;
    if(argc==2)
        if(argv[1][0]=='1')
            cameraIndex=1;

    #ifdef use_pi
        pinMode(OutputPin, OUTPUT);
        digitalWrite(OutputPin,0);
    #endif
    VideoCapture cap(cameraIndex);
    Serial_begin("/dev/ttyUSB0",115200);



    Mat rawImg;
    thresholdValue=120;//origin is 144,remember to darken the line.
    cvNamedWindow("raw",1);
    cvNamedWindow("thre",1);
    setMouseCallback("thre",cvMouseCallback);
    createTrackbar( "thre", "thre", &thresholdValue, 255, trackbar);


    //corners[0]=Point(50,184);
    //corners[1]=Point(281,181);
    ReadParameter(corners[0]);
    printf("Running...\n");
    Mat grayImg;
    Mat threImg;
    while(waitKey(1)!=27)
    {
        cap>>rawImg;
        resize(rawImg,rawImg,Size(320,240));//160 120
        cvtColor(rawImg,grayImg,CV_BGR2GRAY);
        threshold(grayImg,threImg,thresholdValue,255,CV_THRESH_BINARY);
        globalImg=threImg;
        imshow("raw",rawImg);
        imshow("thre",threImg);
        if(toGo==true)break;
        middle_line=corners[0].y;
    }
    WriteParameter(corners[0]);


    cvDestroyWindow("raw");
    cvDestroyWindow("thre");
    printf("To Go!\n");

    while(waitKey(1)!=27)
    {
        cap>>rawImg;
        resize(rawImg,rawImg,Size(320,240));//160 120
        cvtColor(rawImg,grayImg,CV_BGR2GRAY);
        threshold(grayImg,threImg,thresholdValue,255,CV_THRESH_BINARY);
        float centerPoint=getBallCenter(threImg,leftBoundary,rightBoundary,corners[0].y);
        float centerTrackPoint=getTrackRedCenter(rawImg,track_leftUp,track_rightDown);
        if(centerPoint<0)centerPoint=0;
        if(centerTrackPoint<0)centerTrackPoint=0;

        printf("%.1f,%.1f\n",centerTrackPoint,centerPoint);
        char toSend[20];
        sprintf(toSend,"<%.1f,%.1f>",centerTrackPoint,centerPoint);
        string toSendString=toSend;
        Serial_print(toSendString);
        //imshow("thre",threImg);
        //imshow("raw",grayImg);
    }
    return 0;
}
#else
bool toCapture2=false;
bool toExit2=false;
void cvMouseCallback2(int mouseEvent,int x,int y,int flags,void* param)//鼠标事件响应函数
{
  switch(mouseEvent)
  {
    case CV_EVENT_LBUTTONUP:
        toCapture2=true;
    break;

    case CV_EVENT_RBUTTONUP:
        toExit2=true;
    break;
  }
}

int main(int argc,char * argv[])
{
    int imageIndex=0;
    wiringPiSetup();
    int cameraIndex=0;
    if(argc==2)
        if(argv[1][0]=='1')
            cameraIndex=1;

    #ifdef use_pi
        pinMode(OutputPin, OUTPUT);
        digitalWrite(OutputPin,0);
    #endif
    VideoCapture cap(cameraIndex);
    Mat rawImg;
    cvNamedWindow("raw",1);
    setMouseCallback("raw",cvMouseCallback2);
    while(waitKey(1)!=27)
    {
        cap>>rawImg;
        resize(rawImg,rawImg,Size(320,240));//160 120
        if(toCapture2)
        {
            toCapture2=false;
            char imgName[10];
            sprintf(imgName,"A%d.jpg",imageIndex);
            string toWriteName=imgName;
            imwrite(toWriteName,rawImg);
            cout<<toWriteName<<endl;
            imageIndex++;
        }
        if(toExit2)
        {
            break;
        }
        imshow("raw",rawImg);
    }
}
#endif












