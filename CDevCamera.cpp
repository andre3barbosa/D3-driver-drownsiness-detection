#include "CDevCamera.h"
#include<iostream>
#include <stdlib.h>     /* abs */
#include <unistd.h>

#define IMAGE_PATH  ("/etc/frames/frame.jpg") //directory to store a frame

#define FRAME_W 		640
#define FRAME_H 		480

#define CAM_BRIGHTNESS	0.4  //-> 20
#define CAM_CONTRAST	0.50 //-> 0
#define CAM_SATURATION	0.6  //-> 10

#define deviceID 0

using namespace std;
using namespace cv;
using namespace cv::face;




CDevCamera::CDevCamera(string cameraName)
        :camName(cameraName)
{

     /******** Camera Configurations **********/    
    camDev.set(CAP_PROP_FRAME_WIDTH , FRAME_W);
    camDev.set(CAP_PROP_FRAME_HEIGHT, FRAME_H);
    camDev.set(CAP_PROP_BRIGHTNESS, CAM_BRIGHTNESS);
    camDev.set(CAP_PROP_CONTRAST, CAM_CONTRAST);
    camDev.set(CAP_PROP_SATURATION, CAM_SATURATION);

    system("v4l2-ctl --set-ctrl=auto_exposure=0");
    system("v4l2-ctl --set-ctrl=iso_sensitivity=4");
    system("v4l2-ctl --set-ctrl=auto_exposure_bias=18");
    system("v4l2-ctl --set-ctrl=exposure_time_absolute=1000");
    system("v4l2-ctl --set-ctrl=exposure_dynamic_framerate=1");
    system("v4l2-ctl --set-ctrl=white_balance_auto_preset=7");

    system("v4l2-ctl --set-ctrl=sharpness=30");


}

CDevCamera::~CDevCamera()
{
    close();
}


bool CDevCamera::open()
{
    return camDev.open(deviceID, cv::CAP_V4L2);
}

void CDevCamera::close()
{
    camDev.release();
}


// Mat CDevCamera::frameCapture()
Mat CDevCamera::frameCapture()
{
    //Open camera device
    
        this->open();

    //Checks if camera device opened?
        if (!camDev.isOpened())
            ("Camera did not open!\n");


        camDev.read(frame);
        
            if(frame.empty())
               cerr << ("Camera no frames captured.\n");

        imwrite(IMAGE_PATH, frame);

        this->close();
        
        

        return frame; 
      
}