#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;
using namespace std;

void makeErosion(uchar* source, uchar* res, int width,int height)
{
    for(int i=1; i<width-1; i++)
    {
        for(int j=1; j<height-1; j++)
        {
            if(source[(i-1)*width+j]==0||source[(i-1)*width+j+1]==0
                    ||source[(i-1)*width+j-1]==0||source[(i+1)*width+j-1]==0||source[(i+1)*width+j]==0
                    ||source[(i+1)*width+j+1]==0||source[(i)*width+j-1]==0||source[(i)*width+j+1]==0)

                res[i*width+j]=0;
            else
                res[i*width+j]=255;
        }
    }
}

void detectEdges(uchar* source, uchar* res,  int width,int height)
{
    for(int i=1; i<width-1; i++)
    {
        for(int j=1; j<height-1; j++)
        {
            if(source[(i)*width+j]== res[i*width+j])
                res[i*width+j]=0;
            else
                res[i*width+j]=255;
        }
    }
}



int main()
{
    Mat image;
    image= cv::imread("C:/DEV/Pictures/TIF/contour(1).tif", IMREAD_GRAYSCALE );
    if (image.data == 0)
    {
        cout << "Could not open or find the image" <<endl;
        return -1;
    }

    int height = image.rows;
    int width = image.cols;
    uchar*  origData = image.data;

    Mat resIm(height, width, CV_8UC1, Scalar(0));

    makeErosion(origData, resIm.data,width, height);
    detectEdges(origData, resIm.data, width, height);

    imshow( "Original im", image);
    imshow("Result im", resIm);

    waitKey(0);
    return 0;
}
