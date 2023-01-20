#ifndef __CBluetoothCom_H__
#define __CBluetoothCom_H__
 
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>


class CBluetoothCom
{
public:
    CBluetoothCom();
    ~CBluetoothCom();
 

    bool init();
    bool init(int);
    bool init(char *,int);
    int exit();

    bool listenRemote(char *);
    int connectToRemote();

    int sendToRemote(char* buf, int size);
    int readFromRemote(char *,int);
    
private:
    
    //FILE 	*fp;    //associated to the read rfcomm
    int m_socket;
    int m_portNumber;
    char m_dest[18];  //destination addres
    struct sockaddr_rc m_addr;
    bdaddr_t m_bdaddr_any;
    int m_client;
    socklen_t m_opt;
    //BlueTarget m_targetAddress;
    //BlueTarget m_nameAddress;

    
   
};
// End C_BluetoothCom class definition
 
#endif // !__C_BluetoothCom_H__