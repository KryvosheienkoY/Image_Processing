#include <iostream>
#include <vector>
#include <assert.h>
#include <cmath>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

double* getGaussianFilter(int filterHeight, int filterWidth, double sigma)
{
    int filterSize = filterHeight*filterWidth;
    double* kernel = new double[filterSize];
    double sum=0.0;
    for (int i=0 ; i<filterHeight ; i++)
    {
        for (int j=0 ; j<filterWidth ; j++)
        {
            kernel[i*filterWidth+j] = exp(-((i-2)*(i-2)+(j-2)*(j-2))/(2*sigma*sigma))/(2*M_PI*sigma*sigma);
            sum += kernel[i*filterWidth+j];
        }
    }
    for (int i=0 ; i<filterHeight ; i++)
    {
        for (int j=0 ; j<filterWidth ; j++)
        {
            kernel[i*filterWidth+j] /= sum;
        }
    }
    return kernel;
}

Mat applyFilterToImage (double* kernel,int filterHeight, int filterWidth, int height, int width, uchar* imgAr)
{
    int newImageHeight = height-filterHeight+1;
    int newImageWidth = width-filterWidth+1;

    uchar* newImAr = new uchar[newImageHeight*newImageWidth];

    for (int i=0 ; i<newImageHeight ; i++)
    {
        for (int j=0 ; j<newImageWidth ; j++)
        {
            for (int h=i ; h<i+filterHeight ; h++)
            {
                for (int w=j ; w<j+filterWidth ; w++)
                {
                    newImAr[i*newImageWidth+j] += kernel[(h-i)*(filterWidth)+(w-j)]*imgAr[h*(width)+w];
                }
            }
        }
    }
     Mat newImage(newImageHeight, newImageWidth, CV_8UC1, newImAr);
    return newImage;
}

int main()
{
    Mat image = imread("C:/DEV/Pictures/Tif/Geneva.tif", IMREAD_GRAYSCALE);
    // Check for invalid input
    if (image.data == 0)
    {
        std::cout << "Could not open or find the image" << std::endl;
        return -1;
    }

    int size =5;
    double sigma = 4;
    double* kernel= getGaussianFilter(size, size, sigma);
    Mat bluredIm = applyFilterToImage(kernel,size, size,image.rows, image.cols, image.data);

    imshow("Original", image);
    imshow("New IM", bluredIm);

    waitKey();
    return 0;
}
