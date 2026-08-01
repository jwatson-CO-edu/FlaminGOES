////////// INIT ////////////////////////////////////////////////////////////////////////////////////

///// Includes //////////////////////////////////////////////////////////// 
#include <Arduino.h>

///// Constants ///////////////////////////////////////////////////////////
const byte _ODD_MASK = 1;
const int  _DATA_PIN = 0;

// put function declarations here:
inline bool parity_even_bit( byte x ){
    return !( x && _ODD_MASK ); 
}


void write0(){                      // write a '0' bit toneburst
    for (int i=1; i<=22; i++){         // 1 ms of 22 kHz (22 cycles)
        digitalWrite(_DATA_PIN,HIGH);
        delayMicroseconds(16);
        digitalWrite(_DATA_PIN,LOW);
        delayMicroseconds(17);
    }
    delayMicroseconds(500);             // 0.5 ms of silence
}


void write1(){                      // write a '1' bit toneburst
    for (int i=1; i<=11; i++){         // 0.5 ms of 22 kHz (11 cycles)
        digitalWrite(_DATA_PIN,HIGH);
        delayMicroseconds(16);
        digitalWrite(_DATA_PIN,LOW);
        delayMicroseconds(17);
    }
    delayMicroseconds(1000);            // 1 ms of silence
}


// write out a byte (as a toneburst)
// high bit first (ie as if reading from the left)
void write_byte( byte x ){
    for (int j=7; j>=0; j--){
        if (x & (1<<j)) write1(); else write0();
    }
}


void write_parity(byte x){
    if (parity_even_bit(x)) write0(); else write1();
}


// write out a byte with parity attached (as a toneburst)
void write_byte_with_parity(byte x){
    write_byte(x);
    write_parity(x);
}


void goto_angle( float a ){
/*
Note the diseqc "goto x.x" command is not well documented.  
*/
    float fa16;
    byte n1,n2,n3,n4,d1,d2;
    int a16;
    // get the angle in range +/- 75 degrees.  Sit at these limits and switch
    // over at ~ midnight unless otherwise instructed.

    if (a <-75.0) {a=-75;}
    if (a > 75.0) {a= 75;}

    // set the sign nibble in n1 to E (east) or D (west). 
    if (a<0) { n1=0xE0;} else {n1=0xD0;}
    // shift everything up so the fraction (1/16) nibble is in the 
    //integer, and round to the nearest integer:
    a16 =  (int) (16.0*abs(a)+0.5); 
    // n2 is the top nibble of the three-nibble number a16:
    n2 = (a16 & 0xF00)>>8;
    // the second data byte is the bottom two nibbles:
    d2 = a16 & 0xFF;
    //the first data byte is
    d1 = n1 | n2;
    // send the command to the positioner
    noInterrupts();
    write_byte_with_parity(0xE0);
    write_byte_with_parity(0x31);
    write_byte_with_parity(0x6E);
    write_byte_with_parity(d1);
    write_byte_with_parity(d2);
    interrupts();
}


void setup() {
  // put your setup code here, to run once:

}

void loop() {
    // put your main code here, to run repeatedly:
    goto_angle( -75.0f );
    sleep( 30 );
    goto_angle( 0.0f );
    sleep( 30 );
    goto_angle( 75.0f );
    sleep( 30 );
}

