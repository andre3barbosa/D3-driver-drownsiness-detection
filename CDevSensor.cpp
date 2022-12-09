#include "CDevSensor.h"

#include <fcntl.h>
//#include <sys/ioctl.h>
#include <unistd.h>

CDevSensor::CDevSensor(std::string name)
{
    this->devName = "/dev/" + name; //the complete path to the device
}
CDevSensor::~CDevSensor()
{
    //close(dev);
}

bool CDevSensor::open()
{
    //this->dev = open(this->devName,O_RDWR);
}
bool CDevSensor::close()
{

}