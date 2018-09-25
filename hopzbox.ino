#include "animation.h"

 ///static const uint8_t D0   = 16;
 ///static const uint8_t D1   = 5;
 ///static const uint8_t D2   = 4;
 ///static const uint8_t D3   = 0;
 ///static const uint8_t D4   = 2;
 ///static const uint8_t D5   = 14;
 ///static const uint8_t D6   = 12;
 ///static const uint8_t D7   = 13;
 ///static const uint8_t D8   = 15;
 ///static const uint8_t D9   = 3;
 ///static const uint8_t D10  = 1;


//Pin connected to ST_CP of 74HC595(Pin 12)
int latchPin = 5;
//Pin connected to SH_CP of 74HC595(Pin 11)
int clockPin = 4;
////Pin connected to DS of 74HC595(Pin 14)
int dataPin = 16;
///sound sensor pin
int sndPin = 0; ///D3

// A number indicating when to advance to the next frame
unsigned long nextImage = 0;
// A counter to know what frame we're showing
int animationIndex = 0;
// 8x8 Point temporary array
byte brightnesses[64];
// Matrix image frame for 8x8 LED
int M[8];
//sensor last status
byte sensorlaststatus = 0;


//------------------------------------------------------------------------

void setup() {
  //set pins to output so you can control the shift register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT); 
  pinMode(sndPin, INPUT);
}



void loop() {
  int SensorData=digitalRead(sndPin);
  if (SensorData == 0){
          animationIndex = 0;
          sensorlaststatus = 1;
  }
  
   
  if (sensorlaststatus == 1){
    
  
    if(animationIndex >= animationFrames) 
    {
      //restart animation index
      animationIndex = 0;
      sensorlaststatus = 0;
    }
    else{
      //load Delay time for Image
      nextImage = animationDelays[animationIndex];

      //load image(converted)
      for(int i=0; i<64; i++) {
        brightnesses[i] = (animation[animationIndex][i/4] >> (i%4*2)) & B00000001;
        M[i/8] |= (brightnesses[i] << (i%8)) ;
      }

      //Update Image
      screenUpdate(nextImage);
      animationIndex ++;

      //clear M[]
      for(int i=0; i<(8); ++i) {
        M[i]=0;
      }
    }
  }

}


//functions

void screenUpdate(unsigned long frametime) 
{ // function to display image

    unsigned long startime=millis();
  while(millis()-startime<frametime)
  {
    byte row = B10000000; // row 1 
    for (byte k = 0; k < 8; k++) 
    {
      digitalWrite(latchPin, LOW); // open latch ready to receive data
      shiftIt(~row); // row binary number
      shiftIt(M[k]); // LED array (inverted) 

      // Close the latch, sending the data in the registers out to the matrix
      digitalWrite(latchPin, HIGH);
      row = row>> 1; // bitshift right
    }
  } 
}

void shiftIt(byte dataOut) {
  // Shift out 8 bits LSB first, on rising edge of clock
  boolean pinState;

  //clear shift register read for sending data
  digitalWrite(dataPin, LOW);
  // for each bit in dataOut send out a bit
  for (int i=0; i<8; i++) {
    //set clockPin to LOW prior to sending bit
    digitalWrite(clockPin, LOW);
    // if the value of DataOut and (logical AND) a bitmask
    // are true, set pinState to 1 (HIGH)
    if ( dataOut & (1<<i) ) {
      pinState = HIGH;
    }
    else {
      pinState = LOW;
    }
    //sets dataPin to HIGH or LOW depending on pinState
    digitalWrite(dataPin, pinState);
    //send bit out on rising edge of clock
    digitalWrite(clockPin, HIGH);
    digitalWrite(dataPin, LOW);
  }
  digitalWrite(clockPin, LOW); //stop shifting
}
