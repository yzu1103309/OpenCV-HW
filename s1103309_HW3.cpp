#include <opencv2/opencv.hpp>
#include <vector>
#include <algorithm>
#include <string>
using namespace std;
using namespace cv;

#define RED Scalar(0,0,255)
#define WHITE Scalar(255,255,255)

int main(int argc, char * argv[])
{
    string path (argv[1]);
    Mat img = imread(path), HSV_img, tmp[2], redMask, blueMask, yellowMask, totalMask, canny, debug;
    cvtColor(img, HSV_img, COLOR_BGR2HSV);

    /* Create Red Mask */
    inRange(HSV_img,Scalar(165,100,60),Scalar(180,255,255),tmp[0]);
    inRange(HSV_img,Scalar(0,100,60),Scalar(9,255,255),tmp[1]);
    redMask = tmp[0] | tmp[1];

    /* Create Blue Mask */
    inRange(HSV_img,Scalar(105,100,60),Scalar(121,255,255),blueMask);

    /* Create Yellow Mask */
    inRange(HSV_img,Scalar(19,200,200),Scalar(23,255,255),yellowMask);

    totalMask = redMask | blueMask;
    totalMask = totalMask | yellowMask;

    /* temp mask for debug */
    tmp[0] = totalMask;
    cvtColor(tmp[0], tmp[0], COLOR_GRAY2BGR);
    bitwise_xor(tmp[0], img, debug);

    vector<vector<Point>> contours;
    findContours(totalMask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    vector<Rect> R;
    for(size_t t = 0; t < contours.size(); ++t)
    {
        double a = contourArea(contours[t]);
        if( a > 300 )
        {
            Rect temp = boundingRect(contours[t]);
            //double Cx = 0.0, Cy = 0.0; //Magnification coefficient
            //int xTrans = temp.width*Cx/2, yTrans = temp.height*Cy/2;
            //temp += Size(temp.width*Cx, temp.height*Cy);
            //temp -= Point(xTrans, yTrans
            rectangle(img, temp, RED,2);
            R.push_back(temp);
        }
    }

    sort(R.begin(), R.end(), [](Rect a, Rect b){
        if(a.tl().x < b.tl().x)
            return true;
        else if(a.tl().x == b.tl().x)
        {
            if(a.tl().y < b.tl().y)
                return true;
            else
                return false;
        }
        else return false;
    });

    for(auto it = R.begin(); it != R.end(); ++it)
    {
        cout << it->tl().x << ' ' << it->tl().y << ' ' << it->br().x << ' ' << it->br().y << endl;
    }

    //namedWindow("win", 0);

    /**/
    while(1)
    {
        //namedWindow("win", 0);
        imshow("win", img);
        waitKey(0);
        imshow("win", totalMask);
        waitKey(0);
        imshow("win", debug);
        waitKey(0);
        imshow("win", redMask);
        waitKey(0);
        imshow("win", blueMask);
        waitKey(0);
        imshow("win", yellowMask);
        waitKey(0);
        imshow("win", HSV_img);
        waitKey(0);
    }

}