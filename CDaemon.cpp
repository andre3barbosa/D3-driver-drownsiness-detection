#include "CDaemon.h"

#include <signal.h>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#define READ_CHANNEL 1

using namespace std;

//Timer configuration
#define TIM_INTV_SEC    60*5  //5 minutes period
#define TIM_INTV_US     0
#define TIM_VAL_SEC     1
#define TIM_VAL_US      0

#define MSGQ_BLUET      "/msgQueueBluet"
#define MSGQ_SENSORS    "/msgQueueSensors"

CDaemon* CDaemon::myPtr = NULL;

CDaemon::CDaemon()
    :m_camera("devCam"),
    m_temperature("devTemp"),
    m_heartRate("devHR"),
    m_listenBlue(),
    m_drowCam()
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
    pthread_create(&T_BluetListening_id, NULL, BluetListening, this);

    //this->m_listenBlue.init();
}

void CDaemon::run()
{
    //set timer
    setitimer(ITIMER_REAL, &itv, NULL);
    // set signal handler for periodic sensors reading
    signal(SIGALRM, timer_Handler);
    signal(SIGINT, timer_Handler);
    //pthread_join(T_BluetTransmission_id, NULL);
    //pthread_join(T_SecondarySensor_id, NULL);
    //pthread_join(T_CamCapture_id, NULL);
    pthread_join(T_BluetListening_id, NULL);
}


/*Thread workers*/
void* CDaemon::CamProcess(void* arg)
{    
    CDaemon *ptr = reinterpret_cast<CDaemon*>(arg);
    //boolean variable assume true if drwosiness
    //is detected and an signal needs to sent
    bool drowDetected = 0;

    while(1)
    {
        pthread_mutex_lock(&ptr->mutexEARclass);                    //mutex associated to soundMsg var
        pthread_cond_wait(&ptr->condEARclass,&ptr->mutexEARclass); 

        drowDetected = ptr->m_drowCam.checkDrowState();
        //ptr->m_drowCam.
        //TO-DO -> how to implement the rule number 1? with timer?
        int pid;
        if(drowDetected)
        {
            //emit a signal to the process PID xx
            //kill(pid,SIGUSR1);
            drowDetected = 0;
        }

        pthread_mutex_unlock(&ptr->mutexEARclass);
    }
}
// This thread is responsable to capture a camera
//frame

void* CDaemon::CamCapture(void* arg)
{
    CDaemon *ptr = reinterpret_cast<CDaemon*>(arg);

    //MAT frame = ptr->m_camera.frameCapture();

    //ptr->m_drowCam.processParameter(frame);
    
    //ptr->m_drowCam.EARcalculation();

    //m_camera.frameCapture();


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
    char dataSample[] = "50,8";
    // socklen_t opt = sizeof(rem_addr);

    // int l;

    int error = 0;
    //if char received -> sinalize
    ptr->m_listenBlue.init(READ_CHANNEL);

    ptr->m_listenBlue.listenRemote(targetAddr);
    cout << "Target address: " << targetAddr << endl;

    //Format: "duration,alerts"
    
    // allocate socket
    //ptr->s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // bind socket to port 1 of the first available 
    // local bluetooth adapter
    // loc_addr.rc_family = AF_BLUETOOTH;
    // loc_addr.rc_bdaddr = my_bdaddr_any;
    // loc_addr.rc_channel = (uint8_t) 1;
    // bind(ptr->s, (struct sockaddr *)&loc_addr, sizeof(loc_addr));

    // put socket into listening mode
    // listen for connections on a socket
    // listen(ptr->s, 1);

    // // accept one connection
    // ptr->client = accept(ptr->s, (struct sockaddr *)&rem_addr, &opt);
    
    // // Send data to the android device
    // //write(ptr->s, "Hello Android!", 14);
    // ba2str( &rem_addr.rc_bdaddr, targetAddr );
    // cerr << "accepted connection from" << targetAddr <<endl;


    while(1)
    {
        memset(buf, 0, sizeof(buf));

        // read data from the client
        bytes_read = ptr->m_listenBlue.readFromRemote(buf,sizeof(buf));
        if( bytes_read > 0 ) 
        {
            //printf("received [%s]\n", buf);
            //cout << "received [" << targetAddr << "]" << endl;
            //cout << "value [" << buf << "]" << endl;

            switch(buf[0])
            {
                case 'S':   //start trip
                    do
                    {
                        //TO-DO Led on

                        
                        error = mq_send(ptr->msgQueueBluet, targetAddr, sizeof(targetAddr)/sizeof(targetAddr[0])+1, 1);
                        if(error == -1)
                        { 
                            error = errno;
                            if(error != EAGAIN)
                                std::cerr << "In mq_send()";
                        }
                    } while(error == EAGAIN);
                        // close(ptr->client);
                        // close(ptr->s);
                        // ptr->s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
                        // connect(ptr->s, (struct sockaddr *)&rem_addr, sizeof(rem_addr));
                        // cout << "[recvBlut] I command receiveid" << endl;
                        // l = write(ptr->s, "hello!", 6);
                        // cout << "Hello sent! write[" << l <<"]" <<endl;
                         cout << "Value: [" << buf <<"]" <<endl;
                        // close(ptr->s);
                        //exit(1);
                    break;
                case 'T':
                    /*Get the data stored during the trip*/
                    
                    /*Send to bluetooth message queue*/
                    do
                    {
                        //cout << "[recvBlut]: Buf value " << buf << endl;
                        error = mq_send(ptr->msgQueueSensors, dataSample, 4, 1);
                        if(error == -1)
                        { 
                            error = errno;
                            if(error != EAGAIN)
                                std::cerr << "In mq_send()";
                        }
                    } while(error == EAGAIN);

                    //TO-DO led off

                    
                    cout << "[recvBlut] T command receiveid" << endl;
                    break;
                case 'C':   //cancel 
                    cout << "[recvBlut] C command receiveid" << endl;
                    break;
                default:
                    cout << "[recvBlut] False command receiveid" << endl;
            }

        }
        //close(ptr->client);
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
        //cout << "ALARM received!" << endl;
    }
    else if(sig == SIGINT)   //the execution of program is interrupted
    {
        //close the bluetooth message queue
        //mq_close(myPtr->msgQueueBluet);
        // remove msgq from the system
        //if (mq_unlink(MSGQ_BLUET) == -1)
        //    cerr << "Removing bt queue error" << endl;
        //close sensors message queue
        //mq_close(myPtr->msgQueueSensors);
        // if (mq_unlink(MSGQ_SENSORS) == -1)
        //     cerr << "Removing sensors queue error" << endl;
        //myPtr->m_listenBlue.exit();
        
        //successful termination
        exit(0);
    }
}

