#ifndef __CLOCALSYSTEM_H__
#define __CLOCALSYSTEM_H__
 
 
#include <pthread.h>
#include <mqueue.h> 
#include <signal.h>
#include <iostream> //for debug purpose

#include "CDevSound.h"
#include "CBluetoothCom.h"

 
class CLocalSystem 
{
public:
    /*Public function member*/
    CLocalSystem();
    ~CLocalSystem();
 
    void init();
    void run();
 
private:
    /*Thread workers*/
    static void* BluetTransmission(void*);
    static void* Alert(void*);
    
    /*Signal handler*/
    static void signal_Handler(int sig);
 
    /*private Data member*/
    const CDevSound m_speaker;
    const CBluetoothCom m_remoteConnection;


    static CLocalSystem* myPtr;
    /*Threads ids*/
    pthread_t T_BluetTransmission_id;
    pthread_t T_Alert_id;
 
    pthread_mutex_t mutexSoundMsg;
    pthread_cond_t condSoundMsg;

    /*Message queue to read the trip store during the trip*/
    mqd_t msgQueueSensors;

    int soundMsg;
};
// End C_LocalSystem class definition
 
#endif // !__C_LOCALSYSTEM_H__