#include "CDaemon.h"

#include <signal.h>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <fcntl.h>



using namespace std;
using namespace cv;
    
#define READ_CHANNEL 1

#define MSGQ_PID        "/msgQueuePid"
#define MSGQ_BLUET      "/msgQueueBluet"
#define MSGQ_SENSORS    "/msgQueueSensors"

CDaemon* CDaemon::myPtr = NULL;

CDaemon::CDaemon()
    :m_camera("camName"),
    LedOn('1'),
    LedOff('0')
{
    myPtr = this;

    //open bluetooth message queue
    this->msgQueueBluet = mq_open(MSGQ_BLUET, O_WRONLY | O_CREAT | O_EXCL, S_IRWXG | S_IRWXG, NULL);
    if(this->msgQueueBluet == (mqd_t)-1)
    {
        std::cerr << "CLS::CLocalSystem(): Creating bt message queue";
        exit(1);
    }
    //open sensors message queue
    this->msgQueueSensors = mq_open(MSGQ_SENSORS, O_WRONLY | O_CREAT | O_EXCL, S_IRWXG | S_IRWXG, NULL);
    if(this->msgQueueSensors == (mqd_t)-1)
    {
        std::cerr << "CLS::CLocalSystem(): Creating sens message queue";
        exit(1);
    }
    //open pid message queue
    this->msgQueuePid = mq_open(MSGQ_PID, O_RDONLY | O_CREAT | O_EXCL, S_IRWXG | S_IRWXG, NULL);
    if(this->msgQueuePid == (mqd_t)-1)
    {
        std::cerr << "CLS::CLocalSystem(): Creating Pid message queue";
        exit(1);
    }

    //Load the trainned model from svem
    m_drowCam.init();
    
    /*Instantiate and open the led module*/
    system("insmod /root/led.ko");
    file_descriptor = open("/dev/led0", O_WRONLY);
   
   /*Start up values*/
    nextClass = 4;
    for(int i=0;i<15;i++)
        classInput[i]=0.0;
    

    this->tripStatus = false;
    this->localPid = 0;

}
CDaemon::~CDaemon()
{

}

void CDaemon::init()
{

    
    /*Initialize mutex and confitional variables*/
    pthread_mutex_init(&mutexEARclass, NULL);
    pthread_mutex_init(&mutexEARshared, NULL);

    pthread_cond_init(&condEARclass, NULL);

    /*Initialize thread attributes*/
	pthread_attr_init (&thread_attr);
	pthread_attr_getschedpolicy (&thread_attr, &thread_policy);
	pthread_attr_getschedparam (&thread_attr, &thread_param);
	
    SetupThread(25,&thread_attr,&thread_param);
    pthread_create(&T_BluetListening_id, &thread_attr, BluetListening, this);
    //pthread_create(&T_SecondarySensor_id, NULL, SecondarySensor, this);




    /*define the priority and attributes of each thread*/
    SetupThread(25,&thread_attr,&thread_param);
    pthread_create(&T_CamCapture_id, &thread_attr, CamCapture, this);
    SetupThread(25,&thread_attr,&thread_param);
    pthread_create(&T_CamProcess_id, &thread_attr, CamProcess, this);
    

    /*CPU affinity*/
    // cpu_set_t cpuset;
    // CPU_ZERO(&cpuset);
    // CPU_SET(0,&cpuset);
    // pthread_setaffinity_np(T_CamCapture_id,sizeof(cpu_set_t),&cpuset);




    /*Create the SP service*/
    m_listenBlue.init();
}

void CDaemon::run()
{
    //set signal handler
    signal(SIGINT, timer_Handler);
 
    /*Join threads*/
    pthread_join(T_BluetListening_id, NULL);
    pthread_join(T_CamCapture_id, NULL);
    pthread_join(T_CamProcess_id, NULL);
}


/*Thread workers*/
void* CDaemon::CamProcess(void* arg)
{

    
    CDaemon *ptr = reinterpret_cast<CDaemon*>(arg);
    /*Local variables*/
    bool alarmValue = false;
    int error;
    char msg[10000];


    /*receive Pid from main process*/
    error = mq_receive(ptr->msgQueuePid, msg, 10000, NULL);
    if(error == -1)
    {
        error = errno;
        // error not expected
        if(error != EAGAIN)
            cerr << "In mq_receive pid\n";
        exit(1);
    }
    else
    {
        ptr->localPid = stoi(msg);  //to string
    }
    
    while(1){

        pthread_mutex_lock(&ptr->mutexEARshared);
        
        pthread_cond_wait(&ptr->condEARclass, &ptr->mutexEARshared);
        
        /* Analyze the EAR values to make a prediction*/
        alarmValue = ptr->m_drowCam.checkDrowState(ptr->classInput);
        pthread_mutex_unlock(&ptr->mutexEARshared);
        /*if any rule is breaked*/
        if(alarmValue)
        {
            //emit linux signal SIGUSER1
            kill(ptr->localPid,SIGUSR1);
        }
    }
}
void* CDaemon::CamCapture(void* arg)
{
    
    CDaemon *ptr = reinterpret_cast<CDaemon*>(arg);
    /*Local variables*/
    Mat tempFrame;
    

    while (1)
    {   
        /*If trip was started*/
        if(ptr->tripStatus)
        {
            tempFrame = ptr->m_camera.frameCapture();   //capture frame
            ptr->m_drowCam.processParameter(tempFrame); //get the points coordenates to calculate next EAR value

            pthread_mutex_lock(&ptr->mutexEARshared);
                /*shift right every elements
                Last element leaves the array*/
                for(int i = MAX_FEAT - 1; i > 0; i--)
                {
                    ptr->classInput[i] = ptr->classInput[i-1];
                }
                ptr->classInput[0] = ptr->m_drowCam.EARcalculation();   //first element gets the current ear value

            pthread_mutex_unlock(&ptr->mutexEARshared);
            
            /*If number of frames passed since last classification is greatter than 5*/
            if((ptr->nextClass--) == 0)
            {
                pthread_cond_signal(&ptr->condEARclass);    //signalize classification

                ptr->nextClass = NUM_NEXT_CLASS;
            }
        }
    
    }
    
}
void* CDaemon::BluetListening(void* arg)
{
 
    CDaemon *ptr = reinterpret_cast<CDaemon*>(arg);

    /*Local variables*/
    std::chrono::steady_clock::time_point start;
    std::chrono::steady_clock::time_point end;
    char buf[1024] = { 0 };
    char targetAddr[24] = { 0 };
    char dataSample[10];
    int bytes_read;
    int time_dif;
    int error = 0;

    //initialize communication with specific channel
    ptr->m_listenBlue.init(READ_CHANNEL);

    //listen to remote
    ptr->m_listenBlue.listenRemote(targetAddr);

    /*Send the SIGUSR2 signal to main process, i.e,
    plays the welcome voice message*/
    kill(ptr->localPid,SIGUSR2);    

    
    
    while(1)
    {
        memset(buf, 0, sizeof(buf));

        // read data from the client
        bytes_read = ptr->m_listenBlue.readFromRemote(buf,sizeof(buf));
        if( bytes_read > 0 ) 
        {

            switch(buf[0])
            {
                case 'S':   //start trip command
                    do
                    {
                        /*Send to main process the address connected to*/
                        error = mq_send(ptr->msgQueueBluet, targetAddr, sizeof(targetAddr)/sizeof(targetAddr[0])+1, 1);
                        if(error == -1)
                        { 
                            error = errno;

                            cout << " error " << endl;
                            if(error != EAGAIN)
                                std::cerr << "In mq_send()";
                            exit(1);    //error exit
                        }
                    } while(error == EAGAIN);
                        //set trip status to true
                        ptr->tripStatus = true;     //trip on going
;
                        //Turn led on when trip starts
                        write(ptr->file_descriptor , &ptr->LedOn, 1);

                        //start timer reference
                        start = std::chrono::steady_clock::now();

                        break;
                case 'T':   //stop trip command

                    /*calculates the trip time*/
                    end = std::chrono::steady_clock::now();
                    time_dif = std::chrono::duration_cast<std::chrono::minutes>(end - start).count();

                    sprintf(dataSample,"%d",time_dif);
                   
                    do
                    {
                        /*send the data stored during the trip to main process*/
                        error = mq_send(ptr->msgQueueSensors, dataSample, 4, 1);
                        if(error == -1)
                        { 
                            error = errno;
                            if(error != EAGAIN)
                                std::cerr << "In mq_send()";
                        }
                    } while(error == EAGAIN);
                    //set trip status to true
                    ptr->tripStatus = false;

                    //Turn led off when trip stops
                    write(ptr->file_descriptor , &ptr->LedOff, 1);

                    break;
                case 'C':   //cancel trip command
                     
                    //set trip status to true
                    ptr->tripStatus = false;
                    
                    //Turn led off when trip stops
                    write(ptr->file_descriptor , &ptr->LedOff, 1);

                    break;
                default:
                    cerr << "[BluetListening] Invalid command receiveid" << endl;
            }

        }
    }       
}

/*Signal handler*/
void CDaemon::timer_Handler(int sig)
{
    /*When //the execution of program is interrupted*/
    if(sig == SIGINT)
    {
      
        //Removes led device driver
        close (myPtr->file_descriptor );
        system("rmmod /root/led.ko");

        //close msg queues
        mq_close(myPtr->msgQueueBluet);
        mq_close(myPtr->msgQueueSensors);
        mq_close(myPtr->msgQueuePid);

        //successful termination
        exit(0);
    }
}

/*Set priority to thread*/
void CDaemon::SetupThread(int prio,pthread_attr_t *pthread_attr,struct sched_param *pthread_param)
{
	int rr_min_priority, rr_max_priority;

	// Step 2: retrieve min and max priority values for scheduling policy
	rr_min_priority = sched_get_priority_min (SCHED_RR);
	rr_max_priority = sched_get_priority_max (SCHED_RR);

	 // Step 3: calculate and/or assign priority value to sched_param structure
	pthread_param->sched_priority = prio; 

	// Step 4: set attribute object with scheduling parameter
	pthread_attr_setschedparam (pthread_attr, pthread_param);

	// Step 5: set scheduling policy
	pthread_attr_setschedpolicy (pthread_attr, SCHED_RR);
}

