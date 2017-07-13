#include <fstream> 
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <opencv/cv.h>
#include <opencv/cvaux.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <set>
#include <stack>
using namespace std;
using namespace cv;

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

Point poiLeftUp;
Point poiRightDown;

typedef pair<int, int> PAIR;  //�����ֵ��
struct CmpByValue {  //�����ֵ������  �������������
  bool operator()(const PAIR& lhs, const PAIR& rhs) {  
    return lhs.second > rhs.second;  
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


void VThin(Mat &image,int array[])
{
	int h = image.rows;
	int w = image.cols;
	int NEXT=1;
	for(int i=0;i<h;i++)
	{
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

}

void HThin(Mat &image,int array[])
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
	
	for(int i=0;i<num;i++)
	{
		VThin(iXihua,array1);

		#ifdef debug
		imshow("3",iXihua);
		waitKey(300);
		#endif

		HThin(iXihua,array1);

		#ifdef debug
		imshow("3",iXihua);
		waitKey(300);
		#endif
	}
	return iXihua;
}


int thresholdValue=100;

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

void Get_Chain(const Mat & input)
{
	Mat showImg(input.size(),CV_8UC1,Scalar(255));

	vector<Point> endian;
	if(input.channels()!=1){cout<<"Error@ Fun (Find_EndPoint):Input image`s channel != 1\n";return ;}
	endian=Find_Endpoint(input);
	
	stack <Point> pointStack;//������ȱ������洢��
	set   <int>   visitedPoints;//��¼�Ѿ������ĵ�ӳ�伯 (x,y) -> x<<12|y
	#define _Point(p) (p).x<<15|(p).y  

	vector<vector<Point>> chains;
	vector<vector<Point>> subPaths;

	for(int i=0;i<endian.size();i++)//����ÿһ���˵� �Ȳ���һ���˵�   endian.size()
	{
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
		
		
		while(!end)
		{
			visitedPoints.insert(_Point(currentPoint));//����ǰ����뵽�Ѿ����ʹ��ĵ㼯��
			sub_chain.push_back(currentPoint);//����ǰ����Ϊ�����еĵ���뵽chain��
			
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
		//debug handler subpath
		
		//
		chains.push_back(sub_chain);
		cout<<endl<<endl;
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
		if(path_length_vec[i].second>40)//���潫���˳����� ������ ����50 ��·��������
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
	imshow("5",showImg);
	

	
}


void main()
{
	cvNamedWindow("1",1);
	cvNamedWindow("2",1);
	cvNamedWindow("3",1);

	setMouseCallback("3",cvMouseCallback);
	createTrackbar( "Thre", "2", &thresholdValue, 255, trackbar);
	
	Mat image=imread("D:\\4.jpg");
	Mat grayImage;
	Mat threImage;
	cvtColor(image,grayImage,CV_BGR2GRAY);
	imshow("1",image);

	while(waitKey(30)!=27)
	{
		threshold(grayImage,threImage,thresholdValue,255,CV_THRESH_BINARY);
		imshow("2",threImage);
	}

	Mat sklenImage=Sklen(threImage,array);
	//vector<Point> endian=Find_Endpoint(sklenImage);

	globalImg=sklenImage.clone();
	
	//for(int i=0;i<endian.size();i++)
	//	circle(sklenImage,endian[i],5,Scalar(0));
	imshow("3",sklenImage);

	Get_Chain(sklenImage);
	printf("Done!\n");
	
	waitKey(0);

}



void main_()
{
  map<int, int> name_score_map;  
  name_score_map[1] = 90;  
  name_score_map[2] = 79;  
  name_score_map[3] = 92;  
  name_score_map.insert(make_pair(4,99));  
  name_score_map.insert(make_pair(5,86));  
 //��map��Ԫ��ת�浽vector��   
  vector<PAIR> name_score_vec(name_score_map.begin(), name_score_map.end());  
  sort(name_score_vec.begin(), name_score_vec.end(), CmpByValue());  
 // sort(name_score_vec.begin(), name_score_vec.end(), cmp_by_value);  
  for (int i = 0; i != name_score_vec.size(); ++i) {  
    cout << name_score_vec[i] << endl;  
  }  
  


//	cout<<visitedPoints.count(_Point(Point(10,20)+Point(1,2)))<<endl;
}