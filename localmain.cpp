/* Includes */
#include "C_LocalSystem.h"      
#include <iostream>

using namespace std;
 
 
int main(int argc, char *argv[])
{
    C_LocalSystem localsystem;
 
    localsystem.init();
    localsystem.run();
 
    cout << endl << "C_LocalSystem localsystem: Exited main()" << endl;
    return 0;
}

