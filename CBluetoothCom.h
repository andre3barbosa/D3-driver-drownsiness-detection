#ifndef __CBluetoothCom_H__
#define __CBluetoothCom_H__
 
 
class CBluetoothCom
{
public:
    CBluetoothCom(int, int, int);
    ~CBluetoothCom();
 

    bool init();
    bool exit();

    bool discoverDevices();
    bool connectToRemote();

    bool sendToRemote(void*);
    bool readFromRemote(void*);
    
private:
    
    int m_portNumber;
    //BlueTarget m_targetAddress;
    //BlueTarget m_nameAddress;

    
   
};
// End C_BluetoothCom class definition
 
#endif // !__C_BluetoothCom_H__