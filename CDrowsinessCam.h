#include <vector>

class CDrowsinessCam
{
private:
    int drowLevel;
    //Global value to store the 15 EAR values
    //that will be the input of the svm classification
    std::vector<float> classInput;

    int EARcalculation();   //calculate the value of EAR

public:
    CDrowsinessCam();
    ~CDrowsinessCam();
    
    bool processParameter(/*MAT Lframe*/);  //Obtain

    bool checkDrowState();  //machine learning algorithm
};  