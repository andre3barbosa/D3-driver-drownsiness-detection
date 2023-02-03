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

//Timer configuration
#define TIM_INTV_SEC    60*5  //5 minutes period
#define TIM_INTV_US     0
#define TIM_VAL_SEC     1
#define TIM_VAL_US      0

#define IMAGE_PATH  ("/root/frame.jpg")

#define MSGQ_PID        "/msgQueuePid"
#define MSGQ_BLUET      "/msgQueueBluet"
#define MSGQ_SENSORS    "/msgQueueSensors"

CDaemon* CDaemon::myPtr = NULL;

CDaemon::CDaemon()
    :m_camera("camName"),
    m_temperature("devTemp"),
    m_heartRate("devHR"),
    LedOn('1'),
    LedOff('0')
{

    this->itv.it_interval.tv_sec = TIM_INTV_SEC;
    this->itv.it_interval.tv_usec = TIM_INTV_US;
    this->itv.it_value.tv_sec = TIM_VAL_SEC;
    this->itv.it_value.tv_usec = TIM_VAL_US;

        //open bluetooth message queue in the x mode
    //and with null attributes
    this->msgQueueBluet = mq_open(MSGQ_BLUET, O_WRONLY | O_CREAT | O_EXCL, S_IRWXG | S_IRWXG, NULL);
    if(this->msgQueueBluet == (mqd_t)-1)
    {
        std::cerr << "CLS::CLocalSystem(): Creating bt message queue";
        exit(1);
    }
    this->msgQueueSensors = mq_open(MSGQ_SENSORS, O_WRONLY | O_CREAT | O_EXCL, S_IRWXG | S_IRWXG, NULL);
    if(this->msgQueueSensors == (mqd_t)-1)
    {
        std::cerr << "CLS::CLocalSystem(): Creating sens message queue";
        exit(1);
    }
    this->msgQueuePid = mq_open(MSGQ_PID, O_RDONLY | O_CREAT | O_EXCL, S_IRWXG | S_IRWXG, NULL);
    if(this->msgQueuePid == (mqd_t)-1)
    {
        std::cerr << "CLS::CLocalSystem(): Creating Pid message queue";
        exit(1);
    }

    m_drowCam.init();

    nextClass = 4;
    for(int i=0;i<15;i++)
        classInput[i]=0.0;


   
    file_descriptor = open("/dev/led0", O_WRONLY);

    this->tripStatus = false;
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

    pthread_create(&T_BluetListening_id, NULL, BluetListening, this);
    //pthread_create(&T_SecondarySensor_id, NULL, SecondarySensor, this);

    // cpu_set_t cpuset;
    // CPU_ZERO(&cpuset);
    // CPU_SET(4,&cpuset);

    pthread_create(&T_CamCapture_id, NULL, CamCapture, this);
    pthread_create(&T_CamProcess_id, NULL, CamProcess, this);
     //int result = pthread_setaffinity_np(T_CamCapture_id,sizeof(cpu_set_t),&cpuset);


}

void CDaemon::run()
{
    //set timer
        setitimer(ITIMER_REAL, &itv, NULL);
    //set signal handler for periodic sensors reading
        signal(SIGALRM, timer_Handler);
        signal(SIGINT, timer_Handler);
 
    pthread_join(T_BluetListening_id, NULL);
    //pthread_join(T_SecondarySensor_id, NULL);
    pthread_join(T_CamCapture_id, NULL);
    pthread_join(T_CamProcess_id, NULL);
}


/*Thread workers*/
void* CDaemon::CamProcess(void* arg)
{


    CDaemon *ptr = reinterpret_cast<CDaemon*>(arg);
    bool alarmValue = false;

    int error;
    char msg[10000];
    int localPid;

    cout << "Before att" <<endl;

    // do
    // {
    //     if( mq_getattr(ptr->msgQueuePid, &ptr->msgq_attr_pid) == -1)
    //         cerr << "In mq_getattr() of Bluetooth" << endl;
    // }while(ptr->msgq_attr_pid.mq_curmsgs != 0);

    cout << "Before Recieve" <<endl;
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
        localPid = stoi(msg);
        cout << "[MSGQUEUEPID] Msg received:" << localPid <<endl;
    }
    cout << "After Recieve" <<endl;


    while(1){

        pthread_mutex_lock(&ptr->mutexEARshared);
        
        pthread_cond_wait(&ptr->condEARclass, &ptr->mutexEARshared);
        cout << "[CamProcess] Classification realized"<<endl;
        /* Analyze the EAR values to make a prediction*/
        alarmValue = ptr->m_drowCam.checkDrowState(ptr->classInput);
        pthread_mutex_unlock(&ptr->mutexEARshared);
        if(alarmValue)
        {
            //emit linux signal SIGUSER1
            kill(localPid,SIGUSR1);
        }
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
        if(ptr->tripStatus)
        {
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
                //cout << "if" << endl;
                pthread_cond_signal(&ptr->condEARclass);

                ptr->nextClass = NUM_NEXT_CLASS;
            }
        }
    
    }
    
}
void* CDaemon::BluetListening(void* arg)
{
    //will read from(/dev/rfcomm1)
    //Bluetooth configuration
    CDaemon *ptr = reinterpret_cast<CDaemon*>(arg);

    // bdaddr_t my_bdaddr_any = { 0 }; //any address
    // struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };
    char buf[1024] = { 0 };
    // //string buf;
    char targetAddr[24] = { 0 };
    int bytes_read;


    
    //char dataSample[] = "50,8";
    char dataSample[10];
    // socklen_t opt = sizeof(rem_addr);

    // int l;

    int error = 0;
    //if char received -> sinalize
    ptr->m_listenBlue.init(READ_CHANNEL);

    ptr->m_listenBlue.listenRemote(targetAddr);
    cout << "Target address: " << targetAddr << endl;

    
    while(1)
    {
        memset(buf, 0, sizeof(buf));

        // read data from the client
        bytes_read = ptr->m_listenBlue.readFromRemote(buf,sizeof(buf));
        if( bytes_read > 0 ) 
        {

            switch(buf[0])
            {
                case 'S':   //start trip
                    do
                    {
                        
                        cout << " error 1 " << endl;
                        error = mq_send(ptr->msgQueueBluet, targetAddr, sizeof(targetAddr)/sizeof(targetAddr[0])+1, 1);
                        cout << "error 3" << endl;
                        if(error == -1)
                        { 
                            error = errno;

                            cout << " error 2 " << endl;
                            if(error != EAGAIN)
                                std::cerr << "In mq_send()";
                            exit(1);    //error exit
                        }
                    } while(error == EAGAIN);

                         cout << "Value: [" << buf <<"]" <<endl;

                        ptr->tripStatus = true;
                        cout << "trip" << endl;
                        //Turn led on when trip starts
                        write(ptr->file_descriptor , &ptr->LedOn, 1);

                    break;
                case 'T':
                    /*Get the data stored during the trip*/
                    
                    sprintf(dataSample,"%d,%d",10,ptr->m_drowCam.getAlertNumber());
                    /*Send to bluetooth message queue*/
                    do
                    {
                        error = mq_send(ptr->msgQueueSensors, dataSample, 4, 1);
                        if(error == -1)
                        { 
                            error = errno;
                            if(error != EAGAIN)
                                std::cerr << "In mq_send()";
                        }
                    } while(error == EAGAIN);

                    //Turn led off when trip stops
                    write(ptr->file_descriptor , &ptr->LedOff, 1);
                   
                    ptr->tripStatus = false;
                    
                    cout << "[recvBlut] T command receiveid" << endl;
                    break;
                case 'C':   //cancel 
                    cout << "[recvBlut] Cancel command receiveid" << endl;
                    break;
                default:
                    cout << "[recvBlut] Invalid command receiveid" << endl;
            }

        }
    }       
}
void* CDaemon::SecondarySensor(void*)
{
    while(1);
}

/*Signal handler*/
//Used to perform a periodic read in secondary sensors
void CDaemon::timer_Handler(int sig)
{
    if(sig == SIGALRM)
    {
        
    }
    else if(sig == SIGINT)   //the execution of program is interrupted
    {
      
        //Removes led device driver
        close (myPtr->file_descriptor );
        system("rmmod led.ko");

        //successful termination
        exit(0);
    }
}

