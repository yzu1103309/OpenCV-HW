#include <opencv2/opencv.hpp>
#include <vector>
#include <iomanip>
#include <math.h>
using namespace std;
using namespace cv;

int main(int argc, char * argv[])
{
    string path (argv[1]);
    Mat img = imread(path), grayScaled, whiteOnly, mask, masks[2], canny;
    cvtColor(img, grayScaled, COLOR_BGR2GRAY);
    vector< vector<Point> > contours;
    threshold(grayScaled, whiteOnly, 130, 255, THRESH_BINARY);

    Scalar black (0,0,0), red (0,0,255), blue (255, 0, 0);
    //drawContours(img, contours,idx,red,3,8);
    //GaussianBlur(whiteOnly, whiteOnly, Size(19, 19), 0);
    Canny(whiteOnly,canny,0,255);

    bitwise_not(whiteOnly,mask);
    findContours( mask, contours,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE );

    vector<Point> line[2];
    Vec4d lines[2];
    double theta[2];
    for(size_t t = 0; t < 2; t++)
    {
        masks[t] = Mat(whiteOnly.size(), whiteOnly.type(),(0,0,0));

        double maxArea = 0;
        size_t idx = 0;
        for(size_t a = 0; a < contours.size(); ++a)
        {
            double tmp = contourArea(contours[a]);
            if(tmp > maxArea)
            {
                idx = a;
                maxArea = tmp;
            }
        }

        drawContours(masks[t], contours,idx,Scalar(255,255,255),-1,8);
        contours.erase(contours.begin() + idx);

        vector<vector<Point>> thick;
        findContours( masks[t], thick,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE );
        drawContours(masks[t], thick,0,Scalar(255,255,255),2,8);

        bitwise_and(masks[t], canny, masks[t]);
        findNonZero(masks[t], line[t]);
        fitLine(line[t], lines[t], DIST_L2, 0, 0.01, 0.01);
        double m = lines[t][1] / lines[t][0];
        theta[t] = atan(m);
        /*draw line*/
        int y1 = ((0 - lines[t][2]) * m) + lines[t][3]; //formula: y = m(x-x0) + y0
        int y2 = ((img.cols - lines[t][2]) * m) + lines[t][3]; //formula: y = m(x-x0) + y0
        cv::line(img, Point(0, y1), Point(img.cols, y2), red, 2);
    }

    int cx = (img.cols % 2 == 1)? img.cols/2 + 1 : img.cols/2;
    int cy = (img.rows % 2 == 1)? img.rows/2 + 1 : img.rows/2;
    Point center(cx,cy);

    circle(img,center,2,blue,-1);

    double m = tan(M_PI/2 - (theta[0] + theta[1])/2);
    Vec4d slice = Vec4f(1,m,cx,cy);

    /*draw line*/
    {
        double tmp = slice[1]/slice[0];
        int y1 = ((0 - cx) * tmp) + cy; //formula: y = m(x-x0) + y0
        int y2 = ((img.cols - cx) * tmp) + cy; //formula: y = m(x-x0) + y0
        cv::line(img, Point(0, y1), Point(img.cols, y2), red, 2);
    }

    int x[2], y[2];
    for(size_t t = 0; t < 2; ++t)
    {
        double m2 = slice[1]/slice[0], m1 = lines[t][1]/lines[t][0];
        int x1 = lines[t][2], x2 = slice[2], y1 = lines[t][3], y2 = slice[3];
        x[t] = (m2*x2 - m1*x1 + y1 - y2) / (m2-m1);
        y[t] = (m1*y2 - m1*m2*x2 - m2*y1 + m2*m1*x1) / (m1 - m2);

        circle(img,Point(x[t],y[t]),2,blue,-1);
    }

    double distance = sqrt( pow(x[1] - x[0], 2) + pow(y[1] - y[0], 2) );

    //string command = "curl -X POST -F 'argv="+ path + "' http://140.138.242.143/share/file.php";
    cout << fixed << setprecision(4) << distance - 0.5 << endl;

    //system(command.c_str());

//    imshow("win", img);
//    waitKey(0);

    /*show img*/
    while(1)
    {
        imshow("win", whiteOnly);
        waitKey(0);

        imshow("win", canny);
        waitKey(0);

        imshow("win", mask);
        waitKey(0);

        imshow("win", masks[0]);
        waitKey(0);

        imshow("win", masks[1]);
        waitKey(0);

        imshow("win", img);
        waitKey(0);
    }
}