#include <opencv2/opencv.hpp>
#include <vector>
#include <algorithm>
#include <string>
#include <math.h>
using namespace std;
using namespace cv;

int main(int argc, char * argv[])
{
    string path(argv[1]);
    Mat img = imread(path), img_HSV, grayScaled, filtered, debug;
    cvtColor(img, img_HSV, COLOR_BGR2HSV);

    //only take out the 'gray' floor of the parking lot, filter out the cars
    inRange(img_HSV,Scalar(0,0,65),Scalar(255,50,195),filtered);

    /*for debugging
    cvtColor(filtered, debug, COLOR_GRAY2BGR);
    bitwise_xor(img, debug, debug);*/

    //revert the whole filtered picture
    //meaning that the cars will be shown, but not the gray floor
    bitwise_not(filtered, filtered);

    vector<vector<Point>> contours;
    findContours(filtered, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    vector<Rect> BDR;
    int count = 0;
    for(size_t t = 0; t < contours.size(); ++t)
    {
        //use the reverted mask to find contours of the cars
        double area = contourArea(contours[t]);

        //exclude the unwanted particles
        if( area > 2000)
        {
            ++count;
            //find the bounding rect of each car
            Rect tmp = boundingRect(contours[t]);
            BDR.push_back(tmp);

            /*For debugging
            drawContours(img, contours, t, Scalar(0,0,255), -1);
            rectangle(img, BDR.back(), Scalar(0,255,0));
            imshow("win", img);
            waitKey(0);
            */
        }
    }

    int width = 0, height = 0;
    for(size_t t = 0; t < BDR.size(); ++t)
    {
        width += BDR[t].width + 10;
        height += BDR[t].height + 20;
    }

    //use the img size and the average rect size to calculate the parking grids
    if(count != 0)
    {
        int avr_W = width/count, avr_H = height/count;
        int gridX = img.cols/avr_W;
        int gridY = img.rows/avr_H;
        int grids = gridX * gridY;
        //cout << img.cols << ' ' << img.rows << endl;
        cout << count << ' ' << grids - count;
    }
//
//    while(1)
//    {
//        namedWindow("win", 0);
//        imshow("win", img);
//        waitKey(0);
//        imshow("win", filtered);
//        waitKey(0);
//        imshow("win", debug);
//        waitKey(0);
//
//    }

}