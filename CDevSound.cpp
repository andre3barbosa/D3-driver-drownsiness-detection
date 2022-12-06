#include "CDevSound.h"

//debug
//#include <unistd.h>
//#include "a.out.h"
//#include "defs.h"
//#include "debug.h"
 
using namespace std;
 

 CDevSound::CDevSound()
 {

 }

 CDevSound::~CDevSound()
 {

 }

 int CDevSound::setAlarm(int SoundMsg) const
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