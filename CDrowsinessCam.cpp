#include "CDrowsinessCam.h"

//constructor
CDrowsinessCam::CDrowsinessCam()
{
    //reserve 15 elements to store EAR values
    this->classInput.reserve(15);
    this->classInput.clear();
    this->alertNumber = 0;
}

//destructor
CDrowsinessCam::~CDrowsinessCam()
{

}   

//Get the EAR value and store in the 15 vector
bool CDrowsinessCam::processParameter(/*MAT Lframe*/)
{
    float earVal = 0.0;

    
    
    



    // //TO-DO insert in pos1 or 0?
    // this->classInput.emplace(this->classInput.begin(), 1, earVal);
    // int i=5;
    // if((i--) == 0 && this->classInput.size()==15)
    // {

    //     i = 5;
    // }


}


int CDrowsinessCam::EARcalculation()
{

}


//Reads the 15 values stored in global variable
//Do the classification of the 15 EAR input
//Store the result in a global variable
bool CDrowsinessCam::checkDrowState()
{
    //only enters when the first 15 EAR values were captured
    

    /*if()    //if the 
    {

    }
    if()
    {

    }*/
    //increment alert number
}

int CDrowsinessCam::getAlertNumber() const
{
    return this->alertNumber;
}
