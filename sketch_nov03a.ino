/* Include DigitLedDisplay Library */
#include "DigitLedDisplay.h"



/* Arduino Pin to Display Pin
   5 to DIN,
   6 to CS,
   7 to CLK */
DigitLedDisplay ld = DigitLedDisplay(5, 6, 7);


 // Rotary Encoder Inputs
 #define inputCLK 2
 #define inputDT 4
 #define inputSW 3

 //storage variables
 int per16 = 0;
 int counter = 0; 
 int previousStateCLK;
 int currentStateCLK;
 boolean toggle = 0;



 void setup() { 
    // Setup Serial Monitor
    Serial.begin (9600);
    /* Set the brightness min:1, max:15 */
    ld.setBright(10);
    /* Set the digit count */
    ld.setDigitLimit(8);

   // Set encoder pins as inputs  
   pinMode (inputCLK,INPUT_PULLUP);
   pinMode (inputDT,INPUT_PULLUP);
   pinMode (inputSW,INPUT_PULLUP);
   pinMode(8, OUTPUT);
   pinMode(13, OUTPUT);

   // Setup Timer0 to 4KHz 
   TIMER0_SETUP_4KHz();
   // Setup Timer0 to 4KHz 
   TIMER1_SETUP_16Hz();
   
   // Read the initial state of inputCLK
   // Assign to previousStateCLK variable
   previousStateCLK = digitalRead(inputCLK);

   attachInterrupt(digitalPinToInterrupt(inputSW), button_released, RISING);
} 

 void loop() { 
   /* Prints data to the display */
    ld.printDigit(counter);
 }

  
void button_released(){
  counter=0;
}

void TIMER0_SETUP_4KHz(){
  //set timer0 interrupt at 2kHz
  TCCR0A = 0;// set entire TCCR2A register to 0
  TCCR0B = 0;// same for TCCR2B
  TCNT0  = 0;//initialize counter value to 0
  // set compare match register for 2khz increments
  OCR0A = 62;// = (16*10^6) / (2000*64) - 1 (must be <256)
  // turn on CTC mode
  TCCR0A |= (1 << WGM01);
  // Set CS01 and CS00 bits for 64 prescaler
  TCCR0B |= (1 << CS01) | (1 << CS00);   
  // enable timer compare interrupt
  TIMSK0 |= (1 << OCIE0A);
};


ISR(TIMER0_COMPA_vect){//timer0 interrupt 2kHz toggles pin 8
  currentStateCLK = digitalRead(inputCLK);
  // falling edge
  if (!currentStateCLK & previousStateCLK){
    digitalWrite(8,HIGH);
  }

  // raising edge
  if (currentStateCLK & !previousStateCLK){
    digitalWrite(8,LOW);

    if( !digitalRead(inputDT)){
      counter-=per16;

    }else{
      counter+=per16;
    }
    
    per16++;
  }
  previousStateCLK = currentStateCLK; 

//    if (toggle){
//    digitalWrite(13,HIGH);
//    toggle = 0;
//  }
//  else{
//    digitalWrite(13,LOW);
//    toggle = 1;
//  }
}


void TIMER1_SETUP_16Hz(){
  //set timer1 interrupt at 16Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 976;// = (16*10^6) / (16*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
}

ISR(TIMER1_COMPA_vect){
  Serial.print(counter);
  Serial.print(",");
  Serial.println(per16);
  
  per16 = 1;
  
//  if (toggle){
//    digitalWrite(13,HIGH);
//    toggle = 0;
//  }
//  else{
//    digitalWrite(13,LOW);
//    toggle = 1;
//  }
}
