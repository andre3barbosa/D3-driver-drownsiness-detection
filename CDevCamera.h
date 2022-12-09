#ifndef __CDEVCAMERA_H__
#define __CDEVCAMERA_H__

//opencv library
//define frame width and heigth
//define CAM settings
//using namespace std;
#include<string>


class CDevCamera
{
public:
    CDevCamera(std::string name);
    ~CDevCamera();

    bool open();
    bool close();

    bool frameCapture();

private:
    std::string devName; 
    //VideoCapture xx;
    //MAT
};

#endif