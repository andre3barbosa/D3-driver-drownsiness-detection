#include "CDevSound.h"
//sasas
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
        case 1:
            system("aplay ../Alert.wav >/dev/null 2>&1");  //play alert msg
            cout << "Sound msg 1!" << endl;
            break;
        case 2:
            system("aplay ../Alert.wav >/dev/null 2>&1");  //play advice msg
            cout << "Sound msg 2!" << endl;
            break;
        default:
            //inform error
            cout << "Error value!" << endl;
        break;
    }
    return 1;
 }