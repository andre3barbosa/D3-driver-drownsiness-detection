#ifndef __CDAEMON_H__
#define __CDAEMON_H__

#include <pthread.h>
#include <mqueue.h>
#include <sys/time.h>

#include "CDevCamera.h"
#include "CDevSensor.h"
#include "CBluetoothCom.h"
#include "CDrowsinessCam.h"



class CDaemon
{
public:
    CDaemon();
    ~CDaemon();

    void init();
    void run();

private:

    //TO-DO ->define priorities
    


    //objects from others classes
    CDevCamera m_camera;
    CDevSensor m_temperature;
    CDevSensor m_heartRate;

    CDrowsinessCam m_drowCam;
    CBluetoothCom m_listenBlue;

    static CDaemon* myPtr; //use in static funciton member tim_handler
    
    //multithreading
    /*Threads ids*/
    pthread_t T_CamProcess_id;
    pthread_t T_CamCapture_id;
    pthread_t T_BluetListening_id;
    pthread_t T_SecondarySensor_id;

    //synchronization objects
    pthread_mutex_t mutexEARshared; //mutex to acess shared variable
    pthread_mutex_t mutexEARclass;  //mutex associated to the condition variable condEARclass
    pthread_cond_t condEARclass;    //when signalized perform a classification of the EAR values

    pthread_mutex_t mutexReadSensors;  //associated to condReadSensors
    pthread_cond_t condReadSensors;    //signalize a sensores reading


    mqd_t msgQueueSensors;
    //msg queue responsible to share the
    //destination mac address in bluetooth
    //transmission
    mqd_t msgQueueBluet;
  

    /*Thread workers*/
    static void* CamProcess(void*);
    static void* CamCapture(void*);
    static void* BluetListening(void*);
    static void* SecondarySensor(void*);

    /*Signal handler*/
    struct itimerval itv;
    static void timer_Handler(int sig); 





};

#endif