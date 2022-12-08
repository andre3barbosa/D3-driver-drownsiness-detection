#ifndef __CDEVCAMERA_H__
#define __CDEVCAMERA_H__

//opencv library

#include <string.h>
//define frame width and heigth
//define CAM settings
using namespace std;

class CDevCamera
{
public:
    CDevCamera(string);
    ~CDevCamera();

    bool open();
    bool close();

    bool frameCapture();

private:
    string devPath; 
    //VideoCapture m_capture;
    //MAT
};

#endif