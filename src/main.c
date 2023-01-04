#include <string.h>

volatile char buffer[8];

void main(void){

    // char buffer[8];
    strcpy(buffer, "Hello");

    while(1);
}