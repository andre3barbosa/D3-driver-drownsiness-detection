#ifndef __C_DevSound_H__
#define __C_DevSound_H__
 
#include <string>
#include <iostream>
 
class CDevSound
{
public:
    CDevSound();
    ~CDevSound();
 
    int setAlarm(int) const;

private:
    //define variable with the voice msg path and file name
};
// End C_DevSound class definition
 
#endif // !__C_DevSound_H__