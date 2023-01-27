#ifndef __CLOCALSYSTEM_H__
#define __CLOCALSYSTEM_H__
 
 
#include <pthread.h>
#include <mqueue.h> 


#include "CDevSound.h"
#include "CBluetoothCom.h"

#include "/usr/include/opencv2/opencv.hpp"
#include "/usr/include/opencv2/face.hpp"

using namespace cv;
using namespace cv::face;
 
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
    CBluetoothCom m_sendBlue;


    static CLocalSystem* myPtr;
    /*Threads ids*/
    pthread_t T_BluetTransmission_id;
    pthread_t T_Alert_id;
 
    pthread_mutex_t mutexSoundMsg;
    pthread_mutex_t mutexAlert;
    pthread_cond_t condSoundMsg;

    /*Message queue to read the trip store during the trip*/
    mqd_t msgQueueSensors;
    mqd_t msgQueueBluet;
    mqd_t msgQueuePid;
    struct mq_attr msgq_attr_sensors;
    struct mq_attr msgq_attr_blue;
   
   //variable to establish rfcomm protocol
    //int s, client;

    int soundMsg;

    Ptr<Facemark> facemark;


};
// End C_LocalSystem class definition
 
#endif // !__C_LOCALSYSTEM_H__