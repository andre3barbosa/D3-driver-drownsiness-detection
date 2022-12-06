/* Includes */
#include "CLocalSystem.h"      
#include <iostream>

using namespace std;
 

int main(int argc, char *argv[])
{
    CLocalSystem localSystem;
 
    localSystem.init();
    localSystem.run();
 
    cout << endl << "C_LocalSystem localsystem: Exited main()" << endl;
    return 0;
}

