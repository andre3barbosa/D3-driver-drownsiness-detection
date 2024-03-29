#ifndef __CDAEMON_H__
#define __CDAEMON_H__

#include <pthread.h>
#include <mqueue.h>
#include <sys/time.h>


#include "CDevCamera.h"
#include "CDevSensor.h"
#include "CBluetoothCom.h"
#include "CDrowsinessCam.h"

#include <chrono>

#include <signal.h>

#include<fstream>

class CDaemon
{
public:
    CDaemon();
    ~CDaemon();

    void init();
    void run();

private:


    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;

    //objects from others classes
    CDevCamera m_camera;

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


    mqd_t msgQueueSensors;
    //msg queue responsible to share the
    //destination mac address in bluetooth
    //transmission
    mqd_t msgQueueBluet;

    mqd_t msgQueuePid;

    struct mq_attr msgq_attr_pid;


    /*Thread workers*/
    static void* CamProcess(void*);
    static void* CamCapture(void*);
    static void* BluetListening(void*);
    static void* SecondarySensor(void*);

    /*Signal handler*/
    struct itimerval itv;
    static void timer_Handler(int sig); 
    static void SetupThread(int prio,pthread_attr_t *pthread_attr,struct sched_param *pthread_param);

    /*Change the threads prioritys*/
    int thread_policy;
    pthread_attr_t thread_attr;
	struct sched_param thread_param;
   
   
   
    //Shared variable to store the ear values for classification
    float classInput[30];
    ofstream file;
    int nextClass;

    /*if true trip is on going*/
    bool tripStatus;    

    /*Leds variables*/
    int file_descriptor;
    char LedOn;
    char LedOff;

    /*Local system Pid*/
    int localPid;



};

#endif