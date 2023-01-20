#include "CDaemon.h"

#include <signal.h>
#include <iostream>

using namespace std;
using namespace cv;
    

//Timer configuration
#define TIM_INTV_SEC    2  //5 minutes period
#define TIM_INTV_US     0
#define TIM_VAL_SEC     1
#define TIM_VAL_US      0

#define IMAGE_PATH  ("/etc/frames/frame.jpg")

CDaemon::CDaemon()
    :m_camera("camName"),
    m_temperature("devTemp"),
    m_heartRate("devHR")
{
    this->itv.it_interval.tv_sec = TIM_INTV_SEC;
    this->itv.it_interval.tv_usec = TIM_INTV_US;
    this->itv.it_value.tv_sec = TIM_VAL_SEC;
    this->itv.it_value.tv_usec = TIM_VAL_US;
}
CDaemon::~CDaemon()
{

}

void CDaemon::init()
{
//define the priority and attributes of each thread
// open the msg queue
    //mq_open(&msgQueueSensors);
    pthread_mutex_init(&mutexEARclass, NULL);
    pthread_mutex_init(&mutexEARshared, NULL);
    pthread_mutex_init(&mutexReadSensors, NULL);

    pthread_cond_init(&condEARclass, NULL);
    pthread_cond_init(&condReadSensors, NULL);

    //pthread_create(&T_BluetTransmission_id, NULL, BluetTransmission, this);
    //pthread_create(&T_SecondarySensor_id, NULL, SecondarySensor, this);

    pthread_create(&T_CamCapture_id, NULL, CamCapture, this);

}

void CDaemon::run()
{
    //set timer
        setitimer(ITIMER_REAL, &itv, NULL);
    //set signal handler for periodic sensors reading
        signal(SIGALRM, timer_Handler);
 
    //pthread_join(T_BluetTransmission_id, NULL);
    //pthread_join(T_SecondarySensor_id, NULL);
    pthread_join(T_CamCapture_id, NULL);
}


/*Thread workers*/
void* CDaemon::CamProcess(void*)
{

}
void* CDaemon::CamCapture(void* arg)
{
    CDaemon *ptr = reinterpret_cast<CDaemon*>(arg);
    Mat tempFrame;

    while (1)
    {   
        tempFrame = ptr->m_camera.frameCapture();
        ptr->m_drowCam.processParameter(tempFrame);
        ptr->m_drowCam.EARcalculation();
    
    }
    
}
void* CDaemon::BluetListening(void*)
{

}
void* CDaemon::SecondarySensor(void*)
{
    while(1);
}

/*Signal handler*/
void CDaemon::timer_Handler(int sig)
{
    if(sig == SIGALRM)
    {
        //cout << "ALARM received!" << endl;
    }
}

