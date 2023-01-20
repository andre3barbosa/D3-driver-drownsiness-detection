/**
 * @file 	CCamera.cpp, CCamera.h
 * @author 	AB3 , CS11
 * @date 	21/01/2022
 *
 * @brief	Class for camera device
 */

#ifndef __CDEVCAMERA_H__
#define __CDEVCAMERA_H__


#include<string>
#include "/usr/include/opencv2/opencv.hpp"
#include "/usr/include/opencv2/face.hpp"
   

using namespace std;
using namespace cv;
using namespace cv::face;

class CDevCamera
{
public:
    CDevCamera(std::string camName);
    ~CDevCamera();
    Mat frameCapture();


private:
    bool open();
    void close();

    // Last image captured
        cv::Mat frame;
    
    // Camera device
        cv::VideoCapture camDev;
    
    // Camera name 
        const std::string camName;

    
};
// End CDevCamera class definition
 
#endif // !__C_DevCamera_H__