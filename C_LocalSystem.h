#ifndef __C_LOCALSYSTEM_H__
#define __C_LOCALSYSTEM_H__
 
 
#include <pthread.h>
#include <mqueue.h> 
 

 
class C_LocalSystem 
{
public:
    C_LocalSystem();
    ~C_LocalSystem();
 
    void init();
    void run();
 
private:
 

  
};
// End C_LocalSystem class definition
 
#endif // !__C_LOCALSYSTEM_H__