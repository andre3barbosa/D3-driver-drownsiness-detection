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

    /*Get the sound message to play*/
    switch (SoundMsg)
    {
    case 1: //play alert message
        system("timeout 3 aplay -q /root/DetectDrowsiness.wav"); // play alert msg
        break;

    case 2: //play welcome message
        system("timeout 4 aplay -q /root/Welcome.wav"); // play advice msg
        break;
    default:
        // inform error
        cout << "Error valuein message sound!" << endl;
        break;
    }
    return 1;
}