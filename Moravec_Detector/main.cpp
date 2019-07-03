#include <iostream>
#include <cmath>
#include <math.h>
#include "opencv2/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>

using namespace cv;
using namespace std;

/// Moravec corner detector
///Kryvosheienko Yulia

struct myPoint
{
    int _i, _j;
    myPoint(int i, int j ):_i(i), _j(j) {}
};


void detectCandidatePoints (Mat & srcImage)
{
    Mat resIm;
    resIm = srcImage.clone();
    int   kSize = 5;
    int threshold = 20000;
    int r = kSize / 2;
    int h = srcImage.rows;
    int w = srcImage.cols;
    int nCount = 0;
    vector<myPoint> vect;


    for (int i = r; i < h - r; i++)
    {
        for (int j = r; j < w - r; j++)
        {
            int wV1, wV2, wV3, wV4;
            wV1=wV2=wV3=wV4=0;
            for (int k = -r; k <= r; k++)
            {
                for (int m = -r; m <= r; m++)
                {
                    int a1 = i + k;
                    int b1 = j + m + 1;
                    if (b1 < srcImage.cols)
                    {
                        wV1 += pow((srcImage.data[(a1)*w+ j + m + 1] - srcImage.data[(a1)*w+ j + m]),2);

                    }
                    int a2 = i + k + 1;
                    int b2 = j + m;
                    if (a2 < srcImage.rows)
                    {
                        wV2 += pow((srcImage.data[(a2)*w+ b2] - srcImage.data[(i + k)*w+ b2]),2);
                    }
                    int a3 = i + k + 1;
                    int b3 = j + m + 1;
                    if (!(a3 >= srcImage.rows || b3 >= srcImage.cols))
                    {
                        wV3 += pow((srcImage.data[(a3)*w+ b3] - srcImage.data[(i + k)*w +j + m]),2);
                    }

                    int a4 = i + k + 1;
                    int b4 = j + m - 1;
                    if (!(a4 >= srcImage.rows || b4 < 0))
                    {
                        wV4 += pow((srcImage.data[a4*w+ b4] - srcImage.data[(i + k)*w+ j + m]),2);
                    }
                }
            }

            int  minV = min(min(wV1, wV2), min(wV3, wV4));

            if (minV > threshold)
            {
                vect.push_back(myPoint(j, i));
                nCount++;
            }
        }
    }

    for (int i = 0; i < nCount; i++)
    {
        myPoint p =  vect.back();
        Point point(p._i,p._j);
        circle(resIm, point, 1, 200,1);
        vect.pop_back();
    }

    imshow("Res",resIm);
}

int main()
{
    std::string str = "Image";
    std::string imageName("C:/DEV/Pictures/Tif/Geneva.tif");
    Mat Image;
    Image = cv::imread(imageName.c_str(), cv::IMREAD_GRAYSCALE);
    imshow("Original", Image);
    detectCandidatePoints(Image);

    cv::waitKey(0);
    system("Pause");
    return 0;
}
