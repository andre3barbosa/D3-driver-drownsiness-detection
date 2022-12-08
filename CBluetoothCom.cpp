#include "CBluetoothCom.h"

//debug
#include <unistd.h>
//#include "utils.h"
//#include "defs.h"
//#include "debug.h"
 
using namespace std;
 

CBluetoothCom::CBluetoothCom()
    //member-initializer list
{

}

CBluetoothCom::~CBluetoothCom()
{

}

bool CBluetoothCom::init()
{
    return true;
}

bool CBluetoothCom::exit()
{
    return true;
}

bool CBluetoothCom::discoverDevices()
{
    return true;
}

bool CBluetoothCom::connectToRemote()
{
    return true;
}

bool CBluetoothCom::sendToRemote(void*)
{
    return true;
}

bool CBluetoothCom::readFromRemote(void*)
{
    return true;
}
