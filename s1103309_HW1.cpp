#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>
using namespace std;
using namespace cv;

int main(int argc, char * argv[] )
{
    Mat img = imread(argv[1]), grayScaled, whiteOnly;
    cvtColor(img, grayScaled, COLOR_BGR2GRAY);
    threshold(grayScaled, whiteOnly, 250 , 255, THRESH_BINARY);
    vector< vector<Point> > contours;
    /* vector<Point> is a contour
     * vector< vector<Point> > is a set of contours stored in a vector */
    findContours(whiteOnly, contours, RETR_CCOMP  , CHAIN_APPROX_SIMPLE);
    double maxArea = 0;
    size_t idx = 0;
    for(size_t t = 0; t < contours.size(); t++)
    {
        double tmp = contourArea(contours[t]);
        if(tmp > maxArea)
        {
            idx = t;
            maxArea = tmp;
        }
    }
    Mat mask(grayScaled.size(), grayScaled.type());
    Scalar color(255,255,255);
    drawContours(mask, contours, idx, color ,-1, 4);
    bitwise_and(grayScaled, mask, img);
    RotatedRect ellipse = fitEllipse(contours[idx]);
    double angle = (ellipse.angle<90)?ellipse.angle:ellipse.angle - 180;
    Mat rotationM = getRotationMatrix2D(ellipse.center, angle, 1.0);
    warpAffine(img, img, rotationM, img.size());
    imwrite(argv[2], img);
}
