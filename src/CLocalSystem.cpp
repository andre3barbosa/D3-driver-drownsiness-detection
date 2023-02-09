#include "CLocalSystem.h"
#include "CDrowsinessCam.h"

#include <signal.h>
#include <iostream> //for debug purpose

#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>


using namespace std;

#define MSGQ_PID "/msgQueuePid"
#define MSGQ_BLUET "/msgQueueBluet"
#define MSGQ_SENSORS "/msgQueueSensors"

#define WRITE_CHANNEL 4

CLocalSystem *CLocalSystem::myPtr = NULL;

// constructor
CLocalSystem::CLocalSystem() // member-initializer list
    : m_speaker(),
      m_sendBlue()
{

    myPtr = this;

    // open bluetooth message queue
    this->msgQueueBluet = mq_open(MSGQ_BLUET, O_RDONLY, NULL);
    if (this->msgQueueBluet == (mqd_t)-1)
    {
        std::cerr << "[CLocalSystem]: Creating bluetooth message queue\n";
        exit(1);
    }
    // open sensors message queue
    this->msgQueueSensors = mq_open(MSGQ_SENSORS, O_RDONLY, NULL);
    if (this->msgQueueSensors == (mqd_t)-1)
    {
        std::cerr << "[CLocalSystem]: Creating sensors message queue\n";
        exit(1);
    }
    // open pid message queue
    this->msgQueuePid = mq_open(MSGQ_PID, O_WRONLY, NULL);
    if (this->msgQueuePid == (mqd_t)-1)
    {
        std::cerr << "CLS::CLocalSystem(): Creating Pid message queue";
        exit(1);
    }

    //intialize alert number
    alertNumber = 0;
}

CLocalSystem::~CLocalSystem()
{
}

void CLocalSystem::init()
{
    int error;
    char pid[5];    //get the pid in string

    /*get the pid and convert to string*/
    int pid_aux = getpid();
    sprintf(pid, "%d", pid_aux);

    /*send pid do daemon*/
    error = mq_send(this->msgQueuePid, pid, 6, 1);
    if (error == -1)
    {
        error = errno;
        if (error != EAGAIN)
            std::cerr << "In mq_send()";
    }

    //initialize mutex and condition variable
    pthread_mutex_init(&mutexSoundMsg, NULL);
    pthread_cond_init(&condSoundMsg, NULL);

    // define the priority and attributes of each thread
    pthread_attr_init (&thread_attr);
	pthread_attr_getschedpolicy (&thread_attr, &thread_policy);
	pthread_attr_getschedparam (&thread_attr, &thread_param);
	
    SetupThread(25,&thread_attr,&thread_param);
    pthread_create(&T_BluetTransmission_id, &thread_attr, BluetTransmission, this);
    
    SetupThread(25,&thread_attr,&thread_param);
    pthread_create(&T_Alert_id, &thread_attr, Alert, this);
}

void CLocalSystem::run()
{

    // set signal handler for sensors signal
    signal(SIGUSR1, signal_Handler); // camera alert signal
    signal(SIGUSR2, signal_Handler); // secondary sensors advice signal
    signal(SIGINT, signal_Handler);

    //thread join
    pthread_join(T_BluetTransmission_id, NULL);
    pthread_join(T_Alert_id, NULL);
}

void *CLocalSystem::BluetTransmission(void *arg)
{
    CLocalSystem *ptr = reinterpret_cast<CLocalSystem *>(arg);
    /*Local variables*/
    char targetAddr[10000];       //msg queue bluetooth 
    char dataFromSensors[10000];  //msg queue sensors    
    int error = 0;
    char dest[18];
    int status;
    
    //get msg queue attributes
    do
    {
        if (mq_getattr(ptr->msgQueueBluet, &ptr->msgq_attr_blue) == -1)
            cerr << "In mq_getattr() of Bluetooth" << endl;
    } while (ptr->msgq_attr_blue.mq_curmsgs != 0);

    do
    {
        if (mq_getattr(ptr->msgQueueSensors, &ptr->msgq_attr_sensors) == -1)
            cerr << "In mq_getattr() of Sensors" << endl;
    } while (ptr->msgq_attr_sensors.mq_curmsgs != 0);


    //recieve mac addres from daemon
    error = mq_receive(ptr->msgQueueBluet, targetAddr, 10000, NULL);
    if (error == -1)
    {
        error = errno;
        // error not expected
        if (error != EAGAIN)
            cerr << "In mq_receive Bluetooth\n";
        exit(1);
    }
    else
    {
        strcpy(dest, targetAddr);
        ptr->m_sendBlue.init(dest, WRITE_CHANNEL); //init communication
    }

    while (1)
    {

        /*Always that a data is pushed to queue*/
        // Read from message queue - block mode
        // Will have all the data stored in the trip
        error = mq_receive(ptr->msgQueueSensors, dataFromSensors, 10000, NULL);
        if (error == -1)
        {
            error = errno;
            // error not expected
            if (error != EAGAIN)
                cerr << "In mq_receive Sensors\n";
        }
        else // if msg queue read properly
        {
            // start a conection with the remote system
            status = ptr->m_sendBlue.connectToRemote();
            if (status == 0)
            {
                /*store all data in string*/
                char dataSend[20];
                sprintf(dataSend, "%s,%d,", dataFromSensors, ptr->alertNumber-1);

                //send the string to remote
                status = ptr->m_sendBlue.sendToRemote(dataSend, 20);

                ptr->alertNumber = 0;   //reset the number of alerts
            }

            // if any error happen in the writing
            if (status < 0)
                cerr << "[BluetTransmission] Error sending message";
                exit(1)
        }

    }
}

/*Alert thread worker to play the sound messages*/
void *CLocalSystem::Alert(void *arg)
{
    CLocalSystem *ptr = reinterpret_cast<CLocalSystem *>(arg);
    while (1) // while CLocalSystem object exists
    {
        // mutex associated to the condition variable to avoid race conditions
        pthread_mutex_lock(&ptr->mutexSoundMsg); // mutex associated to soundMsg var

        pthread_cond_wait(&ptr->condSoundMsg, &ptr->mutexSoundMsg);

        // unclock the mutex associated to the condition variable to avoid race conditions
        ptr->alertNumber++;
        ptr->m_speaker.setAlarm(ptr->soundMsg); // breaks when sound msg is finished

        // unclock the mutex associated to the condition variable to avoid race conditions
        pthread_mutex_unlock(&ptr->mutexSoundMsg);
    }
    return 0;
}

void CLocalSystem::signal_Handler(int sig)
{

    switch (sig)
    {
    case SIGUSR1: // camera alert signal

        myPtr->soundMsg = 1; //alert message

        //signalize the thread Alert
        pthread_cond_signal(&myPtr->condSoundMsg);

        break;

    case SIGUSR2: // welcome signal
        
        myPtr->soundMsg = 2;    //welcome message

        pthread_cond_signal(&myPtr->condSoundMsg);
;
        break;
    case SIGINT:
        
        // close bluetooth connection
        myPtr->m_sendBlue.exit();
        // close message queues
        mq_close(myPtr->msgQueueBluet);
        mq_close(myPtr->msgQueueSensors);
        mq_close(myPtr->msgQueuePid);

        // remove msgq from the system
        if (mq_unlink(MSGQ_BLUET) == -1)
            cerr << "Removing bt queue error" << endl;
        if (mq_unlink(MSGQ_SENSORS) == -1)
            cerr << "Removing sensors queue error" << endl;
        if (mq_unlink(MSGQ_PID) == -1)
            cerr << "Removing PID queue error" << endl;
        exit(0);
    default:
        break;
    }
}

/*Set up thread priority*/
void CLocalSystem::SetupThread(int prio,pthread_attr_t *pthread_attr,struct sched_param *pthread_param)
{
	int rr_min_priority, rr_max_priority;

	// Step 2: retrieve min and max priority values for scheduling policy
	rr_min_priority = sched_get_priority_min (SCHED_RR);
	rr_max_priority = sched_get_priority_max (SCHED_RR);

	 // Step 3: calculate and/or assign priority value to sched_param structure
	pthread_param->sched_priority = prio; 
	
	printf ("SCHED_RR priority range is %d to %d: using %d\n",rr_min_priority,
		rr_max_priority,pthread_param->sched_priority);

	// Step 4: set attribute object with scheduling parameter
	pthread_attr_setschedparam (pthread_attr, pthread_param);

	// Step 5: set scheduling policy
	pthread_attr_setschedpolicy (pthread_attr, SCHED_RR);
}

