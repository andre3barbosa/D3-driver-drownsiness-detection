#include "C_LocalSystem.h"


using namespace std;
 

 
CLocalSystem::CLocalSystem() :
  
{
   
}
 
CLocalSystem::~CLocalSystem()
{
 
}
 

 
void CLocalSystem::run()
{
    // start camera frame timer
    timCamFrame.start();
 
    // set signal handler for dSensors signal
    signal(SIG_dSENSORS, sigHandler);
    signal(SIGINT, sigHandler);
 
    // join lora threads
    lora.run(RUN_NONBLOCK);
 
    // send CRQ - connection request to the remote system. Awaits its response,
    // giving this local system a "virtual address" to be used in all comms.
    do
    {
        lora.send("CRQ");
        // wait for <id> command using tLoraRecv thread
        // this ID will be used in every communication from that moment o
        bcm2835_delay(2000);
        // try again if ID was not received
    }
    while(IDReceived == false);
 
    pthread_join(tLoraRecv_id, NULL);
    pthread_join(tRecvSensors_id, NULL);
    pthread_join(tParkDetection_id, NULL);
}


void C_LocalSystem::init()
{



}

 
void CLocalSystem::Signal_Handler(int sig)
{
    stringstream debug_msg;
    switch(sig)
    {
        case SIG_dSENSORS:
            // DEBUG_MSG("[CLS::sigHandler] caught SIG_dSENSORS");
            pthread_cond_signal(&thisPtr->condRecvSensors);
            break;
 
        case SIGINT:
            debug_msg.str("");
            debug_msg << "[CLS::sigHandler] caught SIGINT";
            DEBUG_MSG(debug_msg.str().c_str());
 
            // closing the queue
            mq_close(thisPtr->msgqSensors);
 
            // remove msgq from the system
            if (mq_unlink(MSGQ_NAME) == -1)
                panic("Removing queue error");
            
            debug_msg.str("");
            debug_msg << "[CLS::sigHandler] closing...";
            DEBUG_MSG(debug_msg.str().c_str());
 
            exit(0);
        default:
            debug_msg.str("");
            debug_msg << "[CLS::sigHandler] caught unexpected signal";
            DEBUG_MSG(debug_msg.str().c_str());
    }
}
 

void *CLocalSystem::T_RecvSensors(void *arg)
{
   
}
 

void *CLocalSystem::T_BluetTransmission(void *arg)
{
   
   
}
 
void *CLocalSystem::T_Alert(void *arg)
{
   
}
 
