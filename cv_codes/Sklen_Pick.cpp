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

typedef pair<int, int> PAIR;  //定义键值对
struct CmpByValue {  //定义键值排序函数  大的数字在上面
  bool operator()(const PAIR& lhs, const PAIR& rhs) {  
    return lhs.second > rhs.second;  
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


int thresholdValue=119;

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

Mat Prune(const Mat & input)//对骨架图像进行剪枝
{
	Mat showImg(input.size(),CV_8UC1,Scalar(255));

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
		if(path_length_vec[i].second>40)//下面将过滤出来的 点数量 大于50 的路径画出来
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
	globalImg=showImg;
	imshow("5",showImg);
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

void Get_Chain(const Mat & input)//获取链条
{
	//首先是获取每个线条分段
	//理想状态下会有3个线段  不过也有可能会有更多的线段
	Mat showImg(input.size(),CV_8UC1,Scalar(255));

    vector<Point> endian;
    if(input.channels()!=1){cout<<"Error@ Fun (Find_EndPoint):Input image`s channel != 1\n";return ;}
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
			cout<<currentPoint<<endl;
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
                //如果周围不止1个黑点，那么进入错误恢复模式
                cout<<"We run into a error state!\n";
                //debug这里需要增加处理函数
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
		points.push_back(chains[i][0]);
		points.push_back(chains[i][chains[i].size()-1]);

		
    }
	

 
	 cout<<"Begin\n";
	 //使用跳链数组进行首尾匹配
	 vector<int> match_index(points.size());//用来存放匹配后的点下标
	 for(int i=0;i<points.size();i+=1)
	 {
		 Point p0=points[i];
		 int skipNum=0;
		 if(i%2==0)skipNum=i+1;
		 else skipNum=i-1;

		 int minVal=100000000;
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
		cout<<match_index[i]<<endl;

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

	for(int j_time=1;j_time<join_time;j_time++)//j_time-1次
	{
		if(match_index[match_index[current_index]]==current_index)//当前index对应的下一跳也指向自己
		{
			current_index=match_index[current_index];
			//print cur_index
			printf("%d -> ",current_index);
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
				//print cur_index
			printf("%d -> ",current_index);
		}
		else
		{
			break;
		}
	}

	for(int j=0;j<result.size();j++)
	{
		showImg.at<uchar>(result[j])=0;
		imshow("6",showImg);
		waitKey(3);
	}
	
}
void main()
{
	cvNamedWindow("1",1);
	cvNamedWindow("2",1);
	//cvNamedWindow("3",1);
	cvNamedWindow("5",0);
	setMouseCallback("5",cvMouseCallback);
	createTrackbar( "Thre", "2", &thresholdValue, 255, trackbar);
	
	Mat image=imread("D:\\8.jpg");
	Mat grayImage;
	Mat threImage;
	cvtColor(image,grayImage,CV_BGR2GRAY);
	imshow("1",image);

	while(waitKey(30)!=27)
	{
		threshold(grayImage,threImage,thresholdValue,255,CV_THRESH_BINARY);
		imshow("2",threImage);
	}

	Mat sklenImage=Sklen(threImage,array);//获取骨架
	//vector<Point> endian=Find_Endpoint(sklenImage);

	globalImg=sklenImage.clone();
	
	//for(int i=0;i<endian.size();i++)
	//	circle(sklenImage,endian[i],5,Scalar(0));
	//imshow("3",sklenImage);
	//waitKey(100);
	cout<<"Get Chain...\n";
	Mat pruneImg=Prune(sklenImage);//剪枝
	Get_Chain(pruneImg);
	printf("Done!\n");
	
	waitKey(0);

}



void main_()
{

}

