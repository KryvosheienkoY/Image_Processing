#include<iostream>
#include<cmath>
#include <assert.h>
#include <cmath>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include "opencv2/imgproc.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

//Повертає маску Гауса
double* getGaussianFilter(int filterSize, double sigma)
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

//Накладає маску Гауса на зображення
Mat blur(Mat source, double* kernel, double sizeK)
{
    int w= source.cols;
    int h = source.rows;
    uchar* rdata = source.data;
    Mat resIm(h,w,CV_8UC1,Scalar(255));
    for(int i=0; i<h; i++)
    {
        for(int j=0; j<w; j++)
        {
            if(i<sizeK/2||i>=h-sizeK/2||j<sizeK/2||j>=w-sizeK/2)
            {
                resIm.data[i*w+j]=rdata[i*w+j];
            }
            else
            {
                double res=0;
                for(int k=-sizeK/2; k<sizeK/2; k++)
                {
                    for (int l=-sizeK/2; l<sizeK/2; l++)
                    {
                        res+=rdata[(i+k)*w+j+l]*kernel[(k+2)*(int)sizeK+(l+2)];
                    }
                }
                resIm.data[i*w+j]=(uchar)res;
            }
        }
    }
    return resIm;
}

//рахує Gx
int xGradient(uchar* ar, int w, int x, int y)
{
    return ar[(y-1)*w+x-1]+2*ar[y*w+x-1]+ar[(y+1)*w+x-1]-ar[(y-1)*w+x+1]- 2*ar[y*w+x+1]-ar[(y+1)*w+x+1];
}
//рахує Gy
int yGradient(uchar* ar, int w, int x, int y)
{
    return ar[(y-1)*w+x-1]+2*ar[(y-1)*w+x]+ar[(y-1)*w+x+1]-ar[(y+1)*w+x-1]-2*ar[(y+1)*w+x]-ar[(y+1)*w+x+1];
}

//Метод накладає соболя на зображеняяі
Mat sobelOperator(Mat source, uchar* anglesAr)
{
    int h = source.rows;
    int w = source.cols;
    uchar* dataNewAr = new uchar[h*w];
    int gx;
    int gy;
    int sum;
    for(int y = 1; y < h; y++)
    {
        for(int x = 1; x < w; x++)
        {
            gx = xGradient(source.data, w, x, y);
            gy = yGradient(source.data, w, x, y);
            // count result - sqrt of sum of gx^2 and gy^2
            sum = sqrt(gx*gx + gy*gy);
            // check if sum is in [0,255]
            sum = sum > 255 ? 255:sum;
            sum = sum < 0 ? 0 : sum;
            dataNewAr[y*w+x] = sum;

            // пошук градієнтів
            // кути квантуються по 45"
            double localanglesAr=(atan2(gx, gy)/M_PI)*180.0;
            if ( ( (localanglesAr < 22.5) && (localanglesAr > -22.5) ) || (localanglesAr > 157.5) || (localanglesAr < -157.5) )
                anglesAr[y*w+x] = 0;
            if ( ( (localanglesAr > 22.5) && (localanglesAr < 67.5) ) || ( (localanglesAr < -112.5) && (localanglesAr > -157.5) ) )
                anglesAr[y*w+x] = 45;
            if ( ( (localanglesAr > 67.5) && (localanglesAr < 112.5) ) || ( (localanglesAr < -67.5) && (localanglesAr > -112.5) ) )
                anglesAr[y*w+x] = 90;
            if ( ( (localanglesAr > 112.5) && (localanglesAr < 157.5) ) || ( (localanglesAr < -22.5) && (localanglesAr > -67.5) ) )
                anglesAr[y*w+x] = 135;
        }

    }
    return Mat(h, w, CV_8UC1, dataNewAr);;
}

//Подавлення локальних максимумів
uchar* nonmaximumSuppression(int w, int h, uchar* source,  uchar* anglesAr)
{
    uchar * res = new uchar[w*h];
    for (int i = 1; i < h-1; i++)
    {
        for (int j = 1; j < w-1; j++)
        {
            // Порівняємо сусідів пікселя
            // тільки локальні максимуми помічаємо як границі, всі інші - 0
            switch(anglesAr[i*w+j])
            {
            //  якщо кут  дорівнює 0, то точка вважатиметься на краю,
            //якщо величина її градієнта більше, ніж величини в пікселях зліва і справа
            // інакше присвоюємо 0
            case 0:
                if((source[(i-1)*w+j]>source[i*w+j]||source[(i+1)*w+j]>source[i*w+j]))
                {

                    res[i*w+j]=0;
                }
                else
                {
                    res[i*w+j]=source[i*w+j];
                }
                break;
            //  якщо кут дорівнює 45  то точка вважатиметься на краю,
            // якщо величина її градієнта більше, ніж величини в пікселях зверху праворуч і знизу ліворуч
            // інакше присвоюємо 0
            case 45:
                if(source[(i-1)*w+j+1]>source[i*w+j]||source[(i+1)*w+j-1]>source[i*w+j])
                {
                    res[i*w+j]=0;
                }
                else
                {
                    res[i*w+j]=source[i*w+j];
                }
                break;
            //  якщо кут дорівнює 90  то точка вважатиметься на краю,
            // якщо величина її градієнта більше, ніж величини в пікселях зверху і знизу
            // інакше присвоюємо 0
            case 90:
                if(source[(i)*w+j+1]>source[i*w+j]||source[(i)*w+j-1]>source[i*w+j])
                {
                    res[i*w+j]=0;
                }
                else
                {
                    res[i*w+j]=source[i*w+j];
                }
                break;
            //  якщо кут дорівнює 135  то точка вважатиметься на краю,
            // якщо величина її градієнта більше, ніж величини в пікселях зверху зліва і знизу праворуч
            // інакше присвоюємо 0
            case 135:
                if(source[(i-1)*w+j-1]>source[i*w+j]||source[(i+1)*w+j+1]>source[i*w+j])
                {
                    res[i*w+j]=0;
                }
                else
                {
                    res[i*w+j]=source[i*w+j];
                }
                break;
            }
        }
    }
    Mat supr (h, w, CV_8UC1, res);
    imshow("nonmaximumSuppression", supr);
    return res;
}

//Подвійна порогова фільтрація
uchar* doubleThreshold(int l_threshold,int h_threshold,int w,int h,uchar *res, uchar* anglesAr)
{
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            //Якщо значення більше за верхній поріг,тоді присвоюємо 255,
            //Якщо між порогами, тоді - 127
            //Якщо нижчим нижнього – 0
            if (res[i*w+j] > h_threshold)
            {
                anglesAr[i*w+j] = 255;

            }
            else if (res[i*w+j] > l_threshold)
            {

                anglesAr[i*w+j] = 127;
            }
            else
            {
                anglesAr[i*w+j] = 0;
            }
        }
    }
    return anglesAr;
}

//Трасування зони невизначення
uchar* edgeTrackingByHysteresis(int w, int h, uchar* res, uchar* anglesAr)
{
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            // Фінальні границі визначаються придушенням пікселів, які не повязані з сильними сусідами (255)
            if (anglesAr[i*w+j] == 255)
            {
                //сильний  - задаємо 255
                res[i*w+j] = 255;
            }
            else if (anglesAr[i*w+j] == 127)
            {
                // перевіряємо сусідів
                if (anglesAr[i*w+j - 1] == 255 || anglesAr[i*w+j + 1] == 255 ||
                        anglesAr[i*w+j - 1 - w] == 255 ||
                        anglesAr[i*w+j + 1 - w] == 255 || anglesAr[i*w+j + w] == 255 ||
                        anglesAr[i*w+j + w - 1] == 255 ||
                        anglesAr[i*w+j + w + 1] == 255)
                {
                    // сильний сусід знайдено  - задаємо 255
                    res[i*w+j] = 255;
                }
                //немає сильних сусідів - задаємо 0
                else
                {
                    res[i*w+j] = 0;
                }
            }
            else
            {
                res[i*w+j] = 0;
            }
        }
    }
    return res;
}

Mat cannyOperator(Mat source,  uchar* anglesAr,int l_threshold, int h_threshold )
{
    int w = source.cols;
    int h = source.rows;

    //Подавлення локальних максимумів
    uchar *res = nonmaximumSuppression(w,h,source.data,anglesAr);

    //Подвійна порогова фільтрація
    anglesAr = doubleThreshold(l_threshold,h_threshold,w,h,res, anglesAr);

//Трасування зони невизначення
    res = edgeTrackingByHysteresis(h,w,res, anglesAr);
    return Mat (h, w, CV_8UC1, res);
}
//
int main()
{
    Mat image;
    image = imread("C:/DEV/Pictures/Tif/3266372_0.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    if(image.data==0)
    {
        cout<<"Image not found";
        return -1;
    }

    int size =5;
    double sigma = 0.8;
    double* kernel= getGaussianFilter(size, sigma);
    Mat bluredIm = blur(image,kernel,size);
    uchar* edgeDirections = new uchar[image.cols*image.rows];
    Mat sobolAppliedB= sobelOperator(bluredIm, edgeDirections);
    imshow("blured", bluredIm);
    imshow("sobol on blur", sobolAppliedB);
    imshow("Original", image);
    Mat c= cannyOperator(sobolAppliedB, edgeDirections,55,60);
    imshow("Canny",c);

    waitKey();
    return 0;
}
