#include "CBluetoothCom.h"

//debug
#include <iostream>
#include <unistd.h>
//#include "utils.h"
//#include "defs.h"
//#include "debug.h"



using namespace std;
 

CBluetoothCom::CBluetoothCom()
    //member-initializer list
{
    m_bdaddr_any = { 0 };
    m_client = 0;
    m_addr.rc_family = AF_BLUETOOTH;
    m_addr.rc_bdaddr = m_bdaddr_any;
    m_opt = sizeof(this->m_addr);
    m_socket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
}

CBluetoothCom::~CBluetoothCom()
{

}

bool CBluetoothCom::init()
{
    //power on the bluetooth controller
    //system("bluetoothctl power on");
    //set the controller to discoverable
    //system("bluetoothctl discoverable on");
    //set the controller to pairable
    //system("bluetoothctl pairable on");
    // system("rfkill block bluetooth");
    // sleep(1);
    // system("rfkill unblock bluetooth");
    // sleep(1);
    system("sdptool add SP");   //add SP protocol
    system("bluetoothctl power on");
    system("bluetoothctl pairable on");
    system("bluetoothctl discoverable-timeout 0");
    system("bluetoothctl discoverable on");

    return true;
}
bool CBluetoothCom::init(char *destAux,int channel)
{
    
    strcpy(this->m_dest,destAux);
    str2ba(this->m_dest, &m_addr.rc_bdaddr);
    m_addr.rc_channel = (uint8_t) channel;
    
    return true;
}

bool CBluetoothCom::init(int channel)
{
    // str2ba(destAux, &m_addr.rc_bdaddr);
    // strcpy(this->m_dest,destAux);
    m_addr.rc_channel = (uint8_t) channel;
    
    return true;
}

int CBluetoothCom::exit()
{
    //close(this->client);
    return close(this->m_socket);
}
//wait for a device to connect
bool CBluetoothCom::listenRemote(char *targetAddr)
{
    struct sockaddr_rc rem_addr = { 0 };
    //wait for connection
    //system("bluetoothctl scan on");
    bind(this->m_socket, (struct sockaddr *)&this->m_addr, sizeof(this->m_addr));

    // put socket into listening mode
    // listen for connections on a socket
    listen(this->m_socket, 1);

    // accept one connection
    this->m_client = accept(this->m_socket, (struct sockaddr *)&rem_addr, &this->m_opt);
    
    // Send data to the android device
    //write(ptr->s, "Hello Android!", 14);
    this->m_addr = rem_addr;
    ba2str( &this->m_addr.rc_bdaddr, targetAddr);
    cout << "[CBluetoothCom] Target:" << targetAddr << endl;
    return true;
}

int CBluetoothCom::connectToRemote()
{
    return connect(this->m_socket, (struct sockaddr *)&this->m_addr, sizeof(this->m_addr));
}

int CBluetoothCom::sendToRemote(char* buf, int size)
{
    cout << "Write completed!\n";

    return write(this->m_socket, buf, size);
}

int CBluetoothCom::readFromRemote(char* buf, int size)
{
    //Read the bytes from android
    return read(this->m_client, buf, size);
}