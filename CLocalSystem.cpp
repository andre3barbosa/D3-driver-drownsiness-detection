#include "CLocalSystem.h"


using namespace std;
//constructor
CLocalSystem::CLocalSystem(const CDevSound& soundDev,const CBluetoothCom& blueCom)  //member-initializer list
    : m_speaker(soundDev),
      m_remoteConnection(blueCom)
{
    this->init();
    this->run();
}
//
CLocalSystem::~CLocalSystem()
{
    //TODO -> close the msg queue
    //maybe the destructor will be called in sigkill handler?

}
 

 
void CLocalSystem::run()
{
 
    // set signal handler for sensors signal
    signal(SIGUSR1, signal_Handler);    //camera alert signal
    signal(SIGUSR2, signal_Handler);    //secondary sensors advice signal
 
    pthread_join(T_BluetTransmission_id, NULL);
    pthread_join(T_Alert_id, NULL);
}


void CLocalSystem::init()
{
//define the priority and attributes of each thread
// open the msg queue
    //mq_open(&msgQueueSensors);

}

 
void CLocalSystem::signal_Handler(int sig)
{
    //stringstream debug_msg;
    pthread_mutex_lock(&mutexSoundMsg);
    switch(sig)
    {
        case SIGUSR1:      //secondary sensors signal
            soundMsg = 1;
            pthread_cond_signal(condSoundMsg);
            break;
 
        case SIGUSR2:      //camera signal
            soundMsg = 2;
            // remove msgq from the system
            /*if (mq_unlink(MSGQ_NAME) == -1)
                panic("Removing queue error");
            
            debug_msg.str("");
            debug_msg << "[CLS::sigHandler] closing...";
            DEBUG_MSG(debug_msg.str().c_str());*/
 
            //exit(0);
        default:
            /*debug_msg.str("");
            debug_msg << "[CLS::sigHandler] caught unexpected signal";
            DEBUG_MSG(debug_msg.str().c_str());*/
    }
    pthread_mutex_unlock(&this->condSoundMsg);
}
  

void* CLocalSystem::BluetTransmission(void *arg)
{
    //listen for devices
    //idenity the address that the system must connect

    //connect to remote system after found the device to connect

    //read from the queue
    //mq_read(thisPtr->msgQueueSensors);

    //pack the data in a standard way

    //send the package to the remote system
    //while()

}
 
void* CLocalSystem::Alert(void *arg)
{
    //mutex associated to the condition variable to avoid race conditions
    pthread_cond_wait(&this->condSoundMsg,&this->mutexSoundMsg);
    //unclock the mutex associated to the condition variable to avoid race conditions
    pthread_mutex_lock(&this->mutexSoundMsg);   //mutex associated to soundMsg var
    this->m_speaker.setAlarm(soundMsg);       //breaks when sound msg is finisg
    pthread_mutex_unlock(&this->mutexSoundMsg);

}
 
