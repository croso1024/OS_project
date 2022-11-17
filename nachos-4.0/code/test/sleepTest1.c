#include "syscall.h" 

main() 
{
    int i ; 

    for( i = 0 ; i < 3 ; i++ ) {
        Sleep(5000);
        PrintInt(50) ; 

    }
    return 0 ; 
}