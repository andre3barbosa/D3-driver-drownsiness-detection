#include <unistd.h>
#include <fcntl.h>
#include <iostream>

int main (void){

    char LedOn = '1';
    char LedOff = '0';

    system("insmod led.ko");

    int file_descriptor = open("/dev/led0", O_WRONLY);

  //  while (1){

        write( file_descriptor , &LedOn, 1);

        sleep (1);

        write( file_descriptor , &LedOff, 1);

        sleep (1);
   // }
    
    close ( file_descriptor );
    system("rmmod led.ko");

    return 0;
}