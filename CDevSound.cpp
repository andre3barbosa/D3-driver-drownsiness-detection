#include "CDevSound.h"
#include <unistd.h>

using namespace std;
 

 CDevSound::CDevSound()
{
}

 CDevSound::~CDevSound()
 {
 }

 int CDevSound::setAlarm(int SoundMsg) const
 {

    cout << SoundMsg << endl;
    switch(SoundMsg)
    {
        case 1:
            system("timeout 3 aplay -q DetectDrowsiness.wav");  //play alert msg
            cout << "After system" << endl;

            break;
        
        case 2:
            //system("aplay Alert.wav >/dev/null 2>&1");  //play advice msg
            cout << "Sound msg 2!" << endl;
            break;
        default:
            //inform error
            cout << "Error value!" << endl;
        break;
    }
    return 1;
 }