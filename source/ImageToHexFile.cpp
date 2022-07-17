#include "opencv2/opencv.hpp"
#include "opencv2/opencv_modules.hpp"
#include <iostream>
#include<iomanip> // for setbase(), works for base 8,10 and 16 only
#include <fstream>

using namespace cv;
using namespace std;

string intToString(const int Val)
{
    stringstream ss;
    ss << setbase(16) << Val ;
    return ss.str();
}

void WriteImageToHexFile(const Mat mSrc, const string strFileName)
{
    ofstream FileIn;
    FileIn.open(strFileName, ios::out);
    Vec3b Pixel(0, 0, 0);

    if (FileIn.is_open())
    {
        int Idx = 0;
        for (size_t iRows = 0; iRows < mSrc.rows; iRows++)
        {
            for (size_t iCols = 0; iCols < mSrc.cols; iCols++)
            {
                Pixel = mSrc.at<Vec3b>(iRows, iCols);

                for (size_t i = 0; i < 3; i++)
                {
                    string HexString = intToString(Pixel[i]);

                    FileIn << HexString << "\t";

                    if (Idx % 16 == 0 && Idx != 0)
                    {
                        FileIn << "\n";
                        Idx = 0;
                    }
                    else
                    {
                        Idx++;
                    }
                }
            }
            //FileIn << "\n";
        }

        FileIn.close();
    }
    else
    {
        printf("[Error] Unable to Create File! \n");
    }
}

void ReadImageFromHexFile(const string strFileName, Mat &mSrc)
{
    ifstream FileIn;
    FileIn.open(strFileName, ios::in);

    if (FileIn.is_open())
    {
        int Val = 0;
        for (size_t iRows = 0; iRows < mSrc.rows; iRows++)
        {
            for (size_t iCols = 0; iCols < mSrc.cols; iCols++)
            {
                Vec3b Pixel(0,0,0);
                for (size_t i = 0; i < 3; i++)
                {
                    FileIn >> std::hex >> Val;
                    Pixel[i] = Val;
                }

                mSrc.at<Vec3b>(iRows, iCols) = Pixel;
            }
        }

        FileIn.close();
    }
    else
    {
        printf("[Error] Unable to Create File! \n");
    }
}

void ReadImageFromHexFile1(const string strFileName, Mat& mSrc)
{
    ifstream FileIn;
    FileIn.open(strFileName, ios::in);

    if (FileIn.is_open())
    {
        int Val = 0;
        for (size_t iRows = 0; iRows < mSrc.rows; iRows++)
        {
            for (size_t iCols = 0; iCols < mSrc.cols; iCols++)
            {
                Vec3b Pixel(0, 0, 0);
                for (size_t i = 0; i < 3; i++)
                {
                    FileIn >> std::hex >> Val;
                    Pixel[i] = Val;
                }

                mSrc.at<Vec3b>(iRows, iCols) = Pixel;
            }
        }

        FileIn.close();
    }
    else
    {
        printf("[Error] Unable to Create File! \n");
    }
}


int main(int argc, char** argv)
{
    Mat mSrc,mDst;

    mSrc = imread("Input RGB Image.png", 1);

    WriteImageToHexFile(mSrc, "TestHex.txt");

    mDst = Mat(mSrc.size(), CV_8UC3);

    ReadImageFromHexFile("TestHex.txt", mDst);

    imshow("Input Image", mDst);

    cv::waitKey();

    return 0;
}
