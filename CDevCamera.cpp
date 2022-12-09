#include "CDevCamera.h"


CDevCamera::CDevCamera(std::string name)
    :devName(name)
{
    this->open(); //open the camera
}

CDevCamera::~CDevCamera()
{

}

bool CDevCamera::open()
{
    //return xx.open();
}
bool CDevCamera::close()
{

}

bool CDevCamera::frameCapture()
{
    //device open?

    //read frame

    //verify 

    
}