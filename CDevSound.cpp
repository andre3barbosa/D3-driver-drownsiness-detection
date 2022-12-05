#include "C_DevSound.h"

//debug
//#include <unistd.h>
//#include "a.out.h"
//#include "defs.h"
//#include "debug.h"
 
using namespace std;
 

 C_DevSound::C_DevSound():

 {

 }

 C_DevSound::~C_DevSound():

 {

 }

 int C_DevSound::setAlarm(int SoundMsg):
 {
    switch(SoundMsg)
    {
        case '1':
            system("aplay Alert.wav >/dev/null 2>&1");  //play alert msg
            break;
        case '2':
            system("aplay Alert.wav >/dev/null 2>&1");  //play advice msg
            break;
        default:
            //inform error
        break;
    }

 }