

class CDrowsinessCam
{
private:
    int drowLevel;

    bool calculateDrowLevel();

public:
    CDrowsinessCam();
    ~CDrowsinessCam();


    
    bool processParameter(/*MAT Lframe*/);  //Obtain
    int EARcalculation();   //calculate the value of EAR

    bool checkDrowState();  //machine learning algorithm
};