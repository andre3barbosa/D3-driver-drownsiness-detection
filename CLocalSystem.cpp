#include "CLocalSystem.h"

#include <signal.h>
#include <iostream> //for debug purpose

using namespace std;


CLocalSystem* CLocalSystem::myPtr = NULL;

//constructor
CLocalSystem::CLocalSystem()  //member-initializer list
    : m_speaker(),
    m_remoteConnection()
{
    myPtr = this;
}
//
CLocalSystem::~CLocalSystem()
{
    //TODO -> close the msg queue
    //maybe the destructor will be called in sigkill handler?

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
 
void CLocalSystem::run()
{
 
    // set signal handler for sensors signal
    signal(SIGUSR1, signal_Handler);    //camera alert signal
    signal(SIGUSR2, signal_Handler);    //secondary sensors advice signal
 
    //pthread_join(T_BluetTransmission_id, NULL);
    pthread_join(T_Alert_id, NULL);
}

 
void CLocalSystem::signal_Handler(int sig)
{
    
    
    switch(sig)
    {
        case SIGUSR1:      //secondary sensors signal
            pthread_mutex_lock(&myPtr->mutexSoundMsg);        
            myPtr->soundMsg = 1;
            //cout << "SIGUSR1 received!" << endl;

            //pthread_mutex_lock(&myPtr->mutexAlert);
            pthread_cond_signal(&myPtr->condSoundMsg);
            //pthread_cond_signal(&thisPtr->condRecvSensors);
            pthread_mutex_unlock(&myPtr->mutexSoundMsg); 
            //pthread_mutex_unlock(&myPtr->mutexAlert);*/
            cout << "SIGUSR1 received!" << endl;
            break;
 
        case SIGUSR2:      //camera signal
            pthread_mutex_lock(&myPtr->mutexSoundMsg); 
            myPtr->soundMsg = 2;
            //pthread_mutex_lock(&myPtr->mutexSoundMsg); 
            //cout << "SIGUSR2 received!" << endl;

            //pthread_mutex_lock(&myPtr->mutexAlert);
            pthread_cond_signal(&myPtr->condSoundMsg);
            //pthread_mutex_unlock(&myPtr->mutexAlert);
            pthread_mutex_unlock(&myPtr->mutexSoundMsg); 
            cout << "SIGUSR2 received!" << endl;
            break;
            //exit(0);
        case SIGINT:
            //close msg queue
            exit(0);
        default:
        break;
    }
    //pthread_mutex_unlock(&myPtr->mutexSoundMsg);
}
  

void* CLocalSystem::BluetTransmission(void *arg)
{
    CLocalSystem *ptr = reinterpret_cast<CLocalSystem*>(arg);
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
    CLocalSystem *ptr = reinterpret_cast<CLocalSystem*>(arg);
    while(ptr)  //while CLocalSystem object exists
    {
        
        //mutex associated to the condition variable to avoid race conditions
        //pthread_mutex_lock(&ptr->mutexAlert);
        pthread_mutex_lock(&ptr->mutexSoundMsg);   //mutex associated to soundMsg var
        pthread_cond_wait(&ptr->condSoundMsg,&ptr->mutexSoundMsg);
        cout << "ALERT received!" << endl;
        //unclock the mutex associated to the condition variable to avoid race conditions
        ptr->m_speaker.setAlarm(ptr->soundMsg);  //breaks when sound msg is finisg
        pthread_mutex_unlock(&ptr->mutexSoundMsg);
        //pthread_mutex_unlock(&ptr->mutexAlert);
    }
    return 0;
}
 
