#include <iostream>
#include <string>
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <ctime>
using namespace cv;
using namespace std;

void buildHistogram(Mat const& image, int histogramAr[], const char* title)
{
    //calculate
    for(size_t i = 0; i<256; i++)
    {
        histogramAr[i] = 0;
    }
    int maxV = 0;
    for(int i = 0; i<image.rows*image.cols; i++)
    {
        uchar ch = image.data[i];
        histogramAr[ch]++;
        if(histogramAr[ch]>maxV)
        {
            maxV = histogramAr[ch];
        }
    }
    //draw
    Mat histIm(256,256,CV_8UC1,Scalar(0));
    for(int i = 0; i<256; i++)
    {
        int height = cvRound(histogramAr[i]*256 / maxV);
        line(histIm, Point(i,256 - height), Point(i, 256), 255);
    }
    imshow(title,histIm);
}

int optimalThresholding(int* histogramAr, Mat& source)
{
    int resT=0;
    int    t=0;
    for(int i=0; i<source.cols*source.rows; i++)
    {
        t+=source.data[i];
    }
    t/=source.cols*source.rows;
    int t1=t;
    while(true)
    {
        int  n1=1;
        int n2=1;
        int sum1=0;
        int sum2=0;
        for (int i=0; i<(t+1); i++)
        {
            sum1+=i*histogramAr[i];
            sum2+=histogramAr[i];
        }
        n1=sum1/sum2;
        sum1=0;
        sum2=0;
        for (int i=t+1; i<256; i++)
        {
            sum1+=i*histogramAr[i];
            sum2+=histogramAr[i];
        }
        n2=sum1/sum2;
        resT=(n1+n2)/2;
        if(t1==t)
            break;
        t=t1;
    }
    return resT;
}

Mat makeBinary(Mat source, int threshold)
{
    for(long int i=0; i<source.cols*source.rows; i++)
    {
        if(source.data[i]<threshold)
        {
            source.data[i]=0;
        }
        else
        {
            source.data[i]=255;
        }
    }
    imshow("Binary", source);
    return source;
}

int main()
{
    string str = "Image";
    string imageName ("C:/DEV/Pictures/Tif/dog.jpg");
    Mat image;
    image= cv::imread(imageName.c_str(), cv::IMREAD_GRAYSCALE );
    if (!image.data)
    {
        std:: cout << "Image not found.";
        return 1;
    }
    imshow("Original", image);
    int histogramAr[256];
    buildHistogram(image,histogramAr,"Original Histogramm");
    int res = optimalThresholding(histogramAr,image);
    cout<<"optimalThresholding - "<<res<<endl;
    makeBinary(image, res);

    waitKey();
    return 0;
}
