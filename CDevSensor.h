#ifndef __CDEVSENSOR_H__
#define __CDEVSENSOR_H__

#include<string>

class CDevSensor
{
public:
    CDevSensor(std::string name);
    ~CDevSensor();

    bool open();
    bool close();

private:
    int dev;                //Major number associated to the device
    std::string devName;    //name of the device
    int m_dataValue;        //last value readed

    //bool enable();
    //bool disable();



};

#endif