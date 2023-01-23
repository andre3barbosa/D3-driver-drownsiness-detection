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

#define IMAGE_PATH  ("/root/frame.jpg")

CDaemon::CDaemon()
    :m_camera("camName"),
    m_temperature("devTemp"),
    m_heartRate("devHR")
{
    this->itv.it_interval.tv_sec = TIM_INTV_SEC;
    this->itv.it_interval.tv_usec = TIM_INTV_US;
    this->itv.it_value.tv_sec = TIM_VAL_SEC;
    this->itv.it_value.tv_usec = TIM_VAL_US;

    nextClass = 4;
    for(int i=0;i<15;i++)
        classInput[i]=0.0;


    //file.open("d3.txt",ios_base::app);

    //begin = std::chrono::steady_clock::now();

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

    // cpu_set_t cpuset;
    // CPU_ZERO(&cpuset);
    // CPU_SET(4,&cpuset);

    pthread_create(&T_CamCapture_id, NULL, CamCapture, this);
    pthread_create(&T_CamProcess_id, NULL, CamProcess, this);
    /*int result = pthread_setaffinity_np(T_CamCapture_id,sizeof(cpu_set_t),&cpuset);

    if(!result)
    {
        cout << "Criou" <<endl;
    }*/


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
    pthread_join(T_CamProcess_id, NULL);
}


/*Thread workers*/
void* CDaemon::CamProcess(void* arg)
{


    CDaemon *ptr = reinterpret_cast<CDaemon*>(arg);

    int classTemp;

    while(1){

        pthread_mutex_lock(&ptr->mutexEARshared);
        
        pthread_cond_wait(&ptr->condEARclass, &ptr->mutexEARshared);
        //cout << "[CamProcess] Classification realized"<<endl;
        /* Analyze the EAR values to make a prediction*/
        classTemp = ptr->m_drowCam.checkDrowState(ptr->classInput);



        pthread_mutex_unlock(&ptr->mutexEARshared);
    }
}
void* CDaemon::CamCapture(void* arg)
{
    CDaemon *ptr = reinterpret_cast<CDaemon*>(arg);
    Mat tempFrame;
    

    while (1)
    {   
        // ptr-> end = std::chrono::steady_clock::now();
        // ptr->file << "[Time] Value: "<<std::chrono::duration_cast<std::chrono::microseconds>(ptr->end - ptr->begin).count() << endl;

        // ptr-> begin = std::chrono::steady_clock::now();
        tempFrame = ptr->m_camera.frameCapture();
        ptr->m_drowCam.processParameter(tempFrame);

        pthread_mutex_lock(&ptr->mutexEARshared);
            for(int i = MAX_FEAT - 1; i > 0; i--)
            {
                ptr->classInput[i] = ptr->classInput[i-1];
            }
            ptr->classInput[0] = ptr->m_drowCam.EARcalculation();

        pthread_mutex_unlock(&ptr->mutexEARshared);
        
        if((ptr->nextClass--) == 0)
        {
            pthread_cond_signal(&ptr->condEARclass);

            ptr->nextClass = NUM_NEXT_CLASS;
        }
    
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

