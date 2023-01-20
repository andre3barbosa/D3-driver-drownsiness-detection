#include "CLocalSystem.h"

#include <signal.h>
#include <iostream> //for debug purpose

#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

using namespace std;

#define MSGQ_BLUET      "/msgQueueBluet"
#define MSGQ_SENSORS    "/msgQueueSensors"

#define WRITE_CHANNEL 4

CLocalSystem* CLocalSystem::myPtr = NULL;



//constructor
CLocalSystem::CLocalSystem()  //member-initializer list
    : m_speaker(),
    m_sendBlue()
{
    myPtr = this;
    //open bluetooth message queue in the x mode
    //and with null attributes
    this->msgQueueBluet = mq_open(MSGQ_BLUET, O_RDONLY, NULL);
    if(this->msgQueueBluet == (mqd_t)-1)
    {
        std::cerr << "[CLocalSystem]: Creating bluetooth message queue\n";
        exit(1);
    }
    this->msgQueueSensors = mq_open(MSGQ_SENSORS, O_RDONLY, NULL);
    if(this->msgQueueSensors == (mqd_t)-1)
    {
        std::cerr << "[CLocalSystem]: Creating sensors message queue\n";
        exit(1);
    }  
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
    signal(SIGINT, signal_Handler);
    pthread_join(T_BluetTransmission_id, NULL);
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
            //close the bluetooth message queue
            //cout << "hi" << endl;
            myPtr->m_sendBlue.exit();
            mq_close(myPtr->msgQueueBluet);
            mq_close(myPtr->msgQueueSensors);
            // remove msgq from the system
            if (mq_unlink(MSGQ_BLUET) == -1)
                cerr << "Removing bt queue error" << endl;
            if (mq_unlink(MSGQ_SENSORS) == -1)
                cerr << "Removing sensors queue error" << endl;
            exit(0);
        default:
        break;
    }
    //pthread_mutex_unlock(&myPtr->mutexSoundMsg);
}
  

void* CLocalSystem::BluetTransmission(void *arg)
{
    CLocalSystem *ptr = reinterpret_cast<CLocalSystem*>(arg);
    char targetAddr[10000];
    char dataFromSensors[10000];
    int error = 0;
    char dest[18];
    // struct sockaddr_rc addr = { 0 };
    int status;
    // cout << "Aqui2" << endl;
    do
    {
        if( mq_getattr(ptr->msgQueueBluet, &ptr->msgq_attr_blue) == -1)
            cerr << "In mq_getattr() of Bluetooth" << endl;
    }while(ptr->msgq_attr_blue.mq_curmsgs != 0);
    
    do
    {
        if( mq_getattr(ptr->msgQueueSensors, &ptr->msgq_attr_sensors) == -1)
            cerr << "In mq_getattr() of Sensors" << endl;
    }while(ptr->msgq_attr_sensors.mq_curmsgs != 0);

    // TO -DO get the values from other queue
    //and send it to remote

    error = mq_receive(ptr->msgQueueBluet, targetAddr, 10000, NULL);
    if(error == -1)
    {
        error = errno;
        // error not expected
        if(error != EAGAIN)
            cerr << "In mq_receive Bluetooth\n";
        exit(1);
    }
    else
    {
        cout << "[MSGQUEUEBLUET] Msg received:" << targetAddr <<endl;
        strcpy(dest,targetAddr);
        ptr->m_sendBlue.init(dest,WRITE_CHANNEL);
    }

    while(1)
    {
        /*Always that a new bluetooth connection is established*/
        // if(n_block--==1)
        // {
        // error = mq_receive(ptr->msgQueueBluet, msg, 10000, NULL);
        //     if(error == -1)
        //     {
        //         error = errno;
        //         // error not expected
        //         if(error != EAGAIN)
        //             cerr << "In mq_receive()";
        //     }

        /*Always that a data is pushed to queue*/
        //Read from message queue - block mode
        //Will have all the data stored in the trip
        error = mq_receive(ptr->msgQueueSensors, dataFromSensors, 10000, NULL);
        if(error == -1)
        {
            error = errno;
            // error not expected
            if(error != EAGAIN)
                cerr << "In mq_receive Sensors\n";
        }
        else    //if msg queue read properly
        {

            //start a conection with the remote system
            status = ptr->m_sendBlue.connectToRemote();
            cout << "[Transmit] Conected\n";
            //if connection established properly
            //send sensors data to remote
            if( status == 0 ) {
                    status = ptr->m_sendBlue.sendToRemote(dataFromSensors,4);
                    cout << "Write completed!\n";
                    cout << "Size of: " << sizeof(dataFromSensors)/sizeof(dataFromSensors[0]) << endl;
            }
            //if any error happen in the writing
            if( status < 0 ) 
                cerr << "uh oh";    
        }



                //cout << "[msgQueueBluetooth] Received " << dest << endl;

                // bind socket to port 1 of the first available 
                // local bluetooth adapter
                //set the connection parameters (who to connect to)
                // set the connection parameters (who to connect to)
                // struct sockaddr_rc addr = { 0 };
                // addr.rc_family = AF_BLUETOOTH;
                // addr.rc_channel = (uint8_t) 4;
                // str2ba( dest, &addr.rc_bdaddr );

                //-----------------
                //connect to server
                //cout << "Before Send\n" << endl;
                //status = ptr->m_sendBlue.connectToRemote();
                //status = connect(ptr->s, (struct sockaddr *)&addr, sizeof(addr));
                //cout << "After Send\n" << endl;
                //end a message
                // if( status == 0 ) {
                //     status = ptr->m_sendBlue.sendToRemote("Hello\n",7);
                //     cout << "Write completed!\n";
                // }

                // if( status < 0 ) 
                //     cerr << "uh oh";
                //perror("uh oh");
                //close(ptr->s);

        //}
        
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

 
