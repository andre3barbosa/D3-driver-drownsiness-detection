#ifndef __C_LOCALSYSTEM_H__
#define __C_LOCALSYSTEM_H__
 
 
#include <pthread.h>
#include <mqueue.h> 

#include "C_DevSound.h"
#include "C_BlueTransmission.h"

 
class C_LocalSystem 
{
public:
    C_LocalSystem();
    ~C_LocalSystem();
 
    void init();
    void run();
 
private:
    
    static void *T_RecvSensors(void*);
    static void *T_BluetTransmission(void*);
    static void *T_Alert(void*);
 
 private:
    
    static void Signal_Handler(int sig);
 
 private:

    C_DevSound m_speaker;
    C_BlueTransmission m_remoteConnection;

    pthread_t T_RecvSensors_id;
    pthread_t T_BluetTransmission_id;
    pthread_t T_Alert_id;
 
    pthread_mutex_t mutexRecvSensors;
    pthread_cond_t condRecvSensors;


    mqd_t msgqSensors;

  
};
// End C_LocalSystem class definition
 
#endif // !__C_LOCALSYSTEM_H__