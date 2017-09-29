
#include "opencv2/opencv.hpp"    
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
using namespace cv;
using namespace std;
int main(int argc, char* argv[])
{
	Mat src = imread("right.png");   
	//Mat src = imread("left.png"); 左手图
	Mat h_plane, s_plane, v_plane;
	Mat dst;
	blur(src, src, Size(3, 3), Point(-1, -1));
	cvtColor(src, dst, CV_RGB2HSV);
	
	/*利用hsv二值化*/
	vector<Mat>hsv_planes;
	split(dst, hsv_planes);
	h_plane = hsv_planes[0];
	s_plane = hsv_planes[1];
	v_plane = hsv_planes[2];
	
	inRange(h_plane, 14, 180, hsv_planes[0]);
	inRange(s_plane, 4, 255, hsv_planes[1]);
	inRange(v_plane, 12, 250, hsv_planes[2]);
	
	/*
	左手图的hsv范围
	inRange(h_plane, 10, 250, hsv_planes[0]);
	inRange(s_plane, 28, 255, hsv_planes[1]);
	inRange(v_plane, 8, 255, hsv_planes[2]);
	*/
	
	bitwise_and(hsv_planes[0], hsv_planes[1], dst);
	bitwise_and(hsv_planes[2], dst, dst);
	//imshow("dst", dst);
	dilate(dst, dst, Mat(15, 15, CV_8UC1), Point(-1, -1));    
	erode(dst, dst, Mat(15, 15, CV_8UC1), Point(-1, -1));
	imshow("hsv", dst);
	GaussianBlur(dst, dst, Size(3, 3), 0);
	Mat show_img;
	dst.copyTo(show_img, dst);
	std::vector<vector<cv::Point> > contours;     
    vector<Vec4i> hierarchy;    

	/*找出二值图的轮廓*/
    findContours(show_img, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);   
	int index;
	double area, maxarea(0);
	int t = contours.size();
	for(int i = 0; i < t; i++)
	{
		area = contourArea(Mat(contours[i]));
		if(area > maxarea)
		{
			maxarea = area;
			index = i;
		}
	}
	drawContours(src, contours, index, Scalar(0, 0, 255), 1, 8, hierarchy, 1 );

	/*识别指尖和虎口特征点*/
	vector<Point> couPoint = contours[index];    
    vector<Point> fingerTips; 
    Point p, q, r;    
	Point tmp ,tips[10], jaw;  ;
	int num = 0;
	
    for (int i = 8; (i < (couPoint.size() - 8)) && couPoint.size(); i++)    
    {    
		q = couPoint[i - 8];    
        p = couPoint[i];    
        r = couPoint[i + 8];    
		int dot = (q.x - p.x ) * (q.y - p.y) + (r.x - p.x ) * (r.y - p.y);  
        if (dot < 2 && dot > -3)  
		//if(dot > -16 && dot < 48)    左手图的范围
	    {    
			int cross = (q.x - p.x ) * (r.y - p.y) - (r.x - p.x ) * (q.y - p.y);    
		    if (cross > 0)    
            {    
				fingerTips.push_back(p);    
			    circle(src, p, 15 ,Scalar(125, 60, 40), CV_FILLED); 
				tips[num].x = p.x;
				tips[num].y = p.y;
				num++;
             }
			else{
				fingerTips.push_back(p);    
	            circle(src, p, 15 ,Scalar(100, 155, 0), CV_FILLED);
				jaw = p;
			}
        }   
	}

	/*分辨左右手*/
	int judge = (tips[0].x - jaw.x) * (tips[1].y - jaw.y) - (tips[0].y - jaw.y) * (tips[1].x - jaw.x);
	if(judge > 0)
	{
		cout << "Right";
	}
	else
	{
		cout << "Left";
	}
    imshow("result", src);
	waitKey(0);
	return 0;
}


