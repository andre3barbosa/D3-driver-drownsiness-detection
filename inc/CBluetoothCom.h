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
 

    void init();
    void init(int);
    void init(char *,int);
    int exit();

    void listenRemote(char *);
    int connectToRemote();

    int sendToRemote(char* buf, int size);
    int readFromRemote(char *,int);
    
private:
    
    int m_socket;
    int m_portNumber;
    int m_client;

    char m_dest[18];    //destination addres
    struct sockaddr_rc m_addr;
    bdaddr_t m_bdaddr_any;

    socklen_t m_opt;    
   
};
// End C_BluetoothCom class definition
 
#endif // !__C_BluetoothCom_H__