#ifndef __C_DevSound_H__
#define __C_DevSound_H__
 
#include <string>
 
class C_DevSound
{
public:
    C_DevSound();
    ~C_DevSound();
 
    void setAlarm(string);
  
private:
    
    // ver melhor como reproduzir este alarm em string
    string alarmSound();
   
};
// End C_DevSound class definition
 
#endif // !__C_DevSound_H__