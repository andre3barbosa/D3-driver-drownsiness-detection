#include "CBluetoothCom.h"

#include <iostream>
#include <unistd.h>
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
    m_socket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);   //create bluetooth socket
}

CBluetoothCom::~CBluetoothCom()
{

}

/* Overloading of init functions*/

/*init function to create an SP service in the bluetooth stack protocol*/
void CBluetoothCom::init()
{
     system("sdptool add SP");   //add SP protocol

}

/*To initialize store the mac address and channel to communicate*/
void CBluetoothCom::init(char *destAux,int channel)
{
    
    strcpy(this->m_dest,destAux);
    str2ba(this->m_dest, &m_addr.rc_bdaddr);
    m_addr.rc_channel = (uint8_t) channel;
}

/*To initialize store the channel to communicate*/
void CBluetoothCom::init(int channel)
{
    m_addr.rc_channel = (uint8_t) channel;
    
}
/*Close the bluetooth connection*/
int CBluetoothCom::exit()
{
    return close(this->m_socket);
}

/*Listen for Bluetoth connetion*/
void CBluetoothCom::listenRemote(char *targetAddr)
{
    struct sockaddr_rc rem_addr = { 0 };
    //wait for connection
    bind(this->m_socket, (struct sockaddr *)&this->m_addr, sizeof(this->m_addr));

    // put socket into listening mode
    // listen for connections on a socket
    listen(this->m_socket, 1);

    // accept one connection
    this->m_client = accept(this->m_socket, (struct sockaddr *)&rem_addr, &this->m_opt);
    
    this->m_addr = rem_addr;
    ba2str( &this->m_addr.rc_bdaddr, targetAddr);   //convert to mac address
    //debug << "[CBluetoothCom] Target:" << targetAddr << endl;
}
/*Start connection to target address*/
int CBluetoothCom::connectToRemote()
{
    return connect(this->m_socket, (struct sockaddr *)&this->m_addr, sizeof(this->m_addr));
}
/*Send characters to remote*/
int CBluetoothCom::sendToRemote(char* buf, int size)
{
    cout << "Write completed!\n";

    return write(this->m_socket, buf, size);
}
/*Read characters from remote*/
int CBluetoothCom::readFromRemote(char* buf, int size)
{
    //Read the bytes from android
    return read(this->m_client, buf, size);
}