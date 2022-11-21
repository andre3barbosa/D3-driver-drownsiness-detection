#ifndef __C_BluetoothCom_H__
#define __C_BluetoothCom_H__
 
 
class C_BluetoothCom
{
public:
    C_BluetoothCom();
    ~C_BluetoothCom();
 

    bool init();
    bool exit();

    bool discoverDevices();
    bool connectToRemote();

    bool sendToRemote(void*);
    bool readFromRemote(void*);
    
private:
    
    int m_portNumber;
    BlueTarget m_targetAddress;
    BlueTarget m_nameAddress;

    
   
};
// End C_BluetoothCom class definition
 
#endif // !__C_BluetoothCom_H__