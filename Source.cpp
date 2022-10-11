
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include "opencv2/videoio.hpp"
#include "opencv2/imgproc.hpp"
#include "Windows.h"
#include <opencv2/core/core.hpp>
#include <random>

using namespace std;
using namespace cv;

const String video_original = "video_original";
const String video_capture_red = "video_capture_red";
const String video_capture_blue = "video_capture_blue";

cv::Scalar bluelow = cv::Scalar(110, 50, 50);
cv::Scalar bluehigh = cv::Scalar(130, 255, 255);
cv::Scalar redlow = cv::Scalar(160, 100, 20);
cv::Scalar redhigh = cv::Scalar(179, 255, 255);


void createCircle(Mat img, int x2, int y2);
bool MyCollision(Point pt1, Point pt2);
bool toIncrement(int t1, int t2);

const int n = 5;
int xc = rand() % 600;
int yc = 500;
int xr = 260;
int yr = 80;
int widthincrred = 30;
int widthincrblue = 30;

Point pt(xc, yc);

int result = 0;
int failed = 0;

void CoordFunc(int event, int x, int y, int flags, void* userdata)
{
    if (event == EVENT_MOUSEMOVE)
    {
        //cout << "Mouvement du curseur - coordonnees (" << x << ", " << y << ")" << endl;
    }
}

int main(int argc, char** argv)
{
    VideoCapture cap(0);
    namedWindow(video_original);
    namedWindow(video_capture_red);
    namedWindow(video_capture_blue);
    srand(time(NULL));

    int nbr_pixel_red, nbr_pixel_blue;

    Mat video, frame_HSV, video_detection1, video_detection2;
    while (true) {
        cap >> video;
        if (video.empty())
        {
            break;
        }
        // BGR au HSV 
        cv::cvtColor(video, frame_HSV, COLOR_BGR2HSV);
        // Detection basee sur le HSV bleu
        inRange(frame_HSV, bluelow, bluehigh, video_detection1);
        // Detection basee sur le HSV rouge
        inRange(frame_HSV, redlow, redhigh, video_detection2);
      

        //set the callback function for any mouse event
        setMouseCallback("video_original", CoordFunc, NULL);

        vector<vector<Point>> contours;

        vector<Vec4i> hierarchy;

        findContours(video_detection1, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE);

        // find moments of the image
        Moments m = moments(video_detection1);
        Point p(m.m10 / m.m00, m.m01 / m.m00);

        // coordinates of centroid
        //cout << Mat(p) << endl;

        // show the image with a point mark at the centroid
        //circle(video_detection1, p, 5, Scalar(128, 0, 0), -1);

        //SetCursorPos(p.x, p.y);
        int tentative = 10;
        //xc = 200;
        
        int xr1 = 583;
        int yr1 = 60;
        int xr2 = 0;
        int yr2 = 60;
        int xrb = 0;
        int yrb = 0;
        int xr1b = 583;
        int yr1b = 60;
        int xr2r = 0;
        int yr2r = 60;

        Point pr(xr, yr);
        cv::Rect rectright(xr1, yr1, 40, 420);
        cv::Rect rectleft(xr2, yr2, 40, 420);
        cv::Rect rectback(xrb, yrb, 1000, 60);
        cv::Rect rectblue(xr1b, yr1b, 40, widthincrblue);
        cv::Rect rectred(xr2r, yr2r, 40, widthincrred);
        int nbr_pixel_blue;
        int nbr_pixel_red;
        

        for (tentative = 10;tentative > 0;tentative--) {

            createCircle(video, xc, yc);

            nbr_pixel_blue = countNonZero(video_detection1);
            nbr_pixel_red = countNonZero(video_detection2);
            //circle
            circle(video, pr, 40, Scalar(0, 255, 255), -1);
            circle(video, pr, 5, Scalar(0, 0, 0), -1);

            //progress bar
            rectangle(video, rectright, Scalar(0, 0, 0),3);
            rectangle(video, rectleft, Scalar(0, 0, 0),3);
            rectangle(video, rectblue, Scalar(255, 0, 0), -1);
            rectangle(video, rectred, Scalar(0,0, 255), -1);
            
            if (toIncrement(nbr_pixel_blue, nbr_pixel_red)) {
                xr -= 1;
                widthincrblue += 1;
                while (widthincrred != 10)
                {
                    widthincrred -= 1;
                }
            }
            else
            {
                xr += 1;
                widthincrred += 1;
                while (widthincrblue!=10)
                {
                    widthincrblue -= 1;
                }
                
            }

            if (MyCollision(pr, Point(xc, yc)))
            {
                cout << "collision" << endl;
                result += 1;
                yc = 520;
                xc = rand() % 600;
            }
            rectangle(video, rectback, Scalar(220,220, 220),-1);
            line(video, cv::Point(0, 60), cv::Point(1000, 60), Scalar(0, 0, 0),
                3, LINE_8);
            cv::putText(video, //target image##
                "Score = " + to_string(result), cv::Point(10, 50), cv::FONT_HERSHEY_DUPLEX, 1.0, Scalar(0, 0, 0), 2);

            if (yc < 40) {
                failed = 1;
            }

        }
        
        if (failed == 1) {
            cv::putText(video, "FAILED", cv::Point(260, 250), cv::FONT_HERSHEY_DUPLEX, 1.0, Scalar(0, 0, 255), 4);
            cv::putText(video, "YOUR SCORE IS : " + to_string(result), cv::Point(160, 300), cv::FONT_HERSHEY_DUPLEX, 1.0, Scalar(0, 0, 255), 4);
        }

        drawContours(video_detection1, contours, -1, Scalar(0, 255, 0), 2);
        drawContours(video_detection2, contours, -1, Scalar(0, 255, 0), 2);
        imshow(video_original, video);
        imshow(video_capture_blue, video_detection1);
        imshow(video_capture_red, video_detection2);

        yc -= 3;


        char key = (int)waitKey(1);
        if (key == 27)
        {
            break;
        }
    }
    return 0;

}

//methode pour creer un cercle
void createCircle(Mat img, int x2, int y2) {
    circle(img, Point(x2, y2), 40, Scalar(0, 0, 255), -1);
}

//methode pour detecter une collision
bool MyCollision(Point pt1, Point pt2)
{
    double res = cv::norm(pt1 - pt2);//Euclidian distance
    if (res <= 40)
    {  //collis
        return true;
    }
    return false;
}
bool toIncrement(int t1,int t2)
{
    if (t1 > t2) {
        return true;
    }
    else if (t1 < t2) {
        return false;
    }
}