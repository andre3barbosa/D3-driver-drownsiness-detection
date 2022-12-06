#include "CLocalSystem.h"


using namespace std;


CLocalSystem* CLocalSystem::myPtr = NULL;

//constructor
CLocalSystem::CLocalSystem()  //member-initializer list
    : m_speaker(),
      m_remoteConnection()
{
    this->init();
    this->run();

    myPtr = this;
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
    pthread_mutex_init(&mutexSoundMsg, NULL);
    pthread_cond_init(&condSoundMsg, NULL);

    pthread_create(&T_BluetTransmission_id, NULL, BluetTransmission, this);
    pthread_create(&T_Alert_id, NULL, Alert, this);
}

 
void CLocalSystem::signal_Handler(int sig)
{
    //stringstream debug_msg;
    pthread_mutex_lock(&myPtr->mutexSoundMsg);
    switch(sig)
    {
        case SIGUSR1:      //secondary sensors signal
            myPtr->soundMsg = 1;
            cout << "SIGUSR1 received!" << endl;
            pthread_cond_signal(&myPtr->condSoundMsg);
            break;
 
        case SIGUSR2:      //camera signal
            myPtr->soundMsg = 2;
            cout << "SIGUSR2 received!" << endl;
            pthread_cond_signal(&myPtr->condSoundMsg);
            // remove msgq from the system
            /*if (mq_unlink(MSGQ_NAME) == -1)
                panic("Removing queue error");
            
            debug_msg.str("");
            debug_msg << "[CLS::sigHandler] closing...";
            DEBUG_MSG(debug_msg.str().c_str());*/
            break;
            //exit(0);
        default:
            /*debug_msg.str("");
            debug_msg << "[CLS::sigHandler] caught unexpected signal";
            DEBUG_MSG(debug_msg.str().c_str());*/
    }
    pthread_mutex_unlock(&myPtr->mutexSoundMsg);
}
  

void* CLocalSystem::BluetTransmission(void *arg)
{

    while(1)
    {

    }
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
    while(1)
    {
        
        //mutex associated to the condition variable to avoid race conditions
        pthread_mutex_lock(&myPtr->mutexSoundMsg);   //mutex associated to soundMsg var
        
        pthread_cond_wait(&myPtr->condSoundMsg,&myPtr->mutexSoundMsg);
        //unclock the mutex associated to the condition variable to avoid race conditions
        myPtr->m_speaker.setAlarm(myPtr->soundMsg);  //breaks when sound msg is finisg
        pthread_mutex_unlock(&myPtr->mutexSoundMsg);
    }

}
 
