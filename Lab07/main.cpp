#include <iostream>
#include <string>
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <ctime>
using namespace std;
using namespace cv;


double* gausKernel(int filterSize, double sigma)
{
    double* kernel = new double[filterSize*filterSize];
    double sum=0.0;
    for (int i=0 ; i<filterSize ; i++)
    {
        for (int j=0 ; j<filterSize ; j++)
        {
            kernel[i*filterSize+j] = exp(-((i-2)*(i-2)+(j-2)*(j-2))/(2*sigma*sigma))/(2*M_PI*sigma*sigma);
            sum += kernel[i*filterSize+j];
        }
    }
    for (int i=0 ; i<filterSize ; i++)
    {
        for (int j=0 ; j<filterSize ; j++)
        {
            kernel[i*filterSize+j] /= sum;
        }
    }
    return kernel;
}

void blur(Mat source, Mat resultImage, double* kernel, int sizeK)
{
    int width= source.cols;
    int height = source.rows;
    uchar* rdata = source.data;
    //  Mat resultImage(height,width,CV_8UC1,Scalar(255));
    for(int i=0; i<height; i++)
    {
        for(int j=0; j<width; j++)
        {
            if(i<sizeK/2||i>=height-sizeK/2||j<sizeK/2||j>=width-sizeK/2)
            {
                resultImage.data[i*width+j]=rdata[i*width+j];
            }
            else
            {
                double res=0;
                for(int k=-sizeK/2; k<=sizeK/2; k++)
                {
                    for (int l=-sizeK/2; l<=sizeK/2; l++)
                    {
                        res+=rdata[(i+k)*width+j+l]*kernel[(k+2)*(int)sizeK+(l+2)];
                    }
                }
                resultImage.data[i*width+j]=(uchar)res;
            }
        }
    }
    return;
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


int markSegments(Mat source, int* arrayI)
{
    int height = source.rows;
    int width = source.cols;

    bool *elements = new bool[height*width];

    int curSegment = 1;
    for(int i = 0 ; i < height; i++)
    {
        for(int j = 0 ; j < width; j++)
        {
            if (source.data[i*width+j] == 255 && arrayI[i*width+j] == 0)
            {

                vector<int> component;
                component.push_back(i*width+j);
                elements[i*width+j] = true;

                while(component.size()>0)
                {
                    int cur =  component.back();
                    component.pop_back();
                    arrayI[cur] = curSegment;
                    elements[cur] = true;

                    int *neighbor = new int[4];
                    //up
                    neighbor[0] = cur-width;
                    //right
                    neighbor[1] = cur+1;
                    //down
                    neighbor[2] = cur+width;
                    //left
                    neighbor[3] = cur-1;
                    for(int k = 0; k < 4; k++)
                    {
                        if(cur/width>0&&cur/width+1<height&&cur%width>0
                                && cur%width+1<width)
                        {
                            if(source.data[neighbor[k]] == 255
                                    && arrayI[neighbor[k]] == 0
                                    && !elements[neighbor[k]])
                            {
                                component.push_back(neighbor[k]);
                                elements[neighbor[k]] = true;

                            }
                        }
                    }

                    delete [] neighbor;
                }
                curSegment++;
            }
        }
    }
    delete [] elements;
    return curSegment;
}


void markIm(int  *arrayIed_image, uchar* result,
                  int num_components,int height,int width)
{
    for(int i = 1 ; i< num_components; i++)
    {
        if(i<255)
        {
            for(int j = 0; j < height; j++)
            {
                for(int k = 0 ; k < width; k++)
                {
                    if(arrayIed_image[j*width+k] == i)
                    {
                        result[j*width+k] = (uchar)i+50;
                    }
                }
            }
        }
        else
        {
            break;
        }
    }

}

int main()
{
    string str = "Image";
    string imageName ("C:/DEV/Pictures/Tif/going-grayscale-on-Android-and-iOS.png");
    Mat image;
    image= cv::imread(imageName.c_str(), cv::IMREAD_GRAYSCALE );
    if (!image.data)
    {
        std:: cout << "Image not found.";
        return 1;
    }


    int height =image.rows;
    int width = image.cols;

    Mat resultImage(height,width,CV_8UC1,Scalar(255));

    imshow("Original", image);
    double* kernel =gausKernel(5, 1);
    blur(image, resultImage, kernel, 5);

    makeBinary(resultImage, 180);
    int* arrayI = new int[height*width];
    int components=markSegments(resultImage, arrayI);
    markIm(arrayI, resultImage.data, components, height, width);


  for(int i=0; i<height*width;i++)
  {
      if(resultImage.data[i]!=51)
        resultImage.data[i]=0;
else
    resultImage.data[i]=255;
  }
    imshow("Res",resultImage);

    imshow("Res",resultImage);
    waitKey();
    return 0;
}
