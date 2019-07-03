#include<iostream>
#include<cmath>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

//count Gx
int xGradient(uchar* ar, int width, int x, int y)
{
    return ar[(y-1)*width+x-1]+2*ar[y*width+x-1]+ar[(y+1)*width+x-1]-ar[(y-1)*width+x+1]- 2*ar[y*width+x+1]-ar[(y+1)*width+x+1];
}
//count Gy
int yGradient(uchar* ar, int width, int x, int y)
{
    return ar[(y-1)*width+x-1]+2*ar[(y-1)*width+x]+ar[(y-1)*width+x+1]-ar[(y+1)*width+x-1]-2*ar[(y+1)*width+x]-ar[(y+1)*width+x+1];
}

//operator Sobol
Mat operatorSobol(uchar* data, int height, int width)
{
    uchar* dataNewAr = new uchar[height*width];
    int gx;
    int gy;
    int sum;
    for(int y = 1; y < height; y++)
    {
        for(int x = 1; x < width; x++)
        {
            gx = xGradient(data, width, x, y);
            gy = yGradient(data, width, x, y);
            // count result - sqrt of sum of gx^2 and gy^2
            sum = sqrt(gx*gx + gy*gy);
            // check if sum is in [0,255]
            sum = sum > 255 ? 255:sum;
            sum = sum < 0 ? 0 : sum;
            dataNewAr[y*width+x] = sum;
        }
    }
    return Mat (height, width, CV_8UC1, dataNewAr);
}

int main()
{
    Mat img;
    // Load an image
    img = imread("C:/DEV/Pictures/Tif/IM1.tif", CV_LOAD_IMAGE_GRAYSCALE);
    if(img.data==0)
    {
        return -1;
    }

    Mat newIM= operatorSobol(img.data, img.rows, img.cols);

    imshow("New", newIM);
    imshow("Original", img);

    waitKey();

    return 0;
}
