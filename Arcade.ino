
#include <SPI.h>

int P1_VERT = 14;
int P1_HORIZ = 15;
int P2_VERT = 16;
int P2_HORIZ = 17;

int P1_B1 = 1; 
int P1_B2 = 2; 
int P1_B3 = 0; 

int P2_B1 = 5; 
int P2_B2 = 6; 
int P2_B3 = 4; 

int P1_START = 3;
int P2_START = 7;

int COIN = 9;


boolean p1Change = true;
boolean p2Change = true;
boolean auxChange = true;

unsigned char p1BitFieldState=0x00;
unsigned char p2BitFieldState=0x00;
unsigned char auxBitFieldState=0x00;
  
  
char buf [100];
volatile byte pos;
volatile boolean process_it_p1;
volatile boolean process_it_p2;
byte c;

void setup(){
  //start serial connection
//  Serial.begin(9600);
  SPI.begin();
  //configure pin2 as an input and enable the internal pull-up resistor
  pinMode(P1_B1, INPUT_PULLUP);
  pinMode(P1_B2, INPUT_PULLUP);
  pinMode(P1_B3, INPUT_PULLUP);
 
  pinMode(P2_B1, INPUT_PULLUP);
  pinMode(P2_B2, INPUT_PULLUP);
  pinMode(P2_B3, INPUT_PULLUP);
  
  pinMode(P1_START, INPUT_PULLUP);
  pinMode(P2_START, INPUT_PULLUP);
  pinMode(COIN, INPUT);
 

  // have to send on master in, *slave out*
  pinMode(MISO, OUTPUT);
  pinMode(SCK, INPUT);
  pinMode(MOSI, INPUT);
 pinMode(SS, INPUT);

 SPI.setClockDivider(SPI_CLOCK_DIV8);

  
  // turn on SPI in slave mode
  SPCR |= _BV(SPE);
  //  SPCR != _BV(SPIE);

  // get ready for an interrupt 
  pos = 0;   // buffer empty
  process_it_p1 = false;
  process_it_p2 = false;
 

  
  // now turn on interrupts
  SPI.attachInterrupt();
}


// SPI interrupt routine
ISR (SPI_STC_vect)
{
   c = SPDR;  // grab byte from SPI Data Register
   if (c == 0xFF) {
       SPI.transfer((unsigned char)(p1BitFieldState));
       SPI.transfer((unsigned char)(p2BitFieldState));
       SPI.transfer((unsigned char)(auxBitFieldState));
    }   
}


int checkPlayer(int * buttons,int bitField) {
  int anIn = analogRead(buttons[0]); 
  if(anIn < 250) {
      bitField = bitField | 0x01;
  } else if(anIn < 750) {
      bitField = bitField | 0x02;
  }
  anIn = analogRead(buttons[1]); 
  if(anIn < 250) {
      bitField = bitField | 0x04;
  } else if(anIn < 750) {
      bitField = bitField | 0x08;
  }
  int buttonState = digitalRead(buttons[2]);  
  if(buttonState == LOW) {
      bitField = bitField | 0x10;
  }
  buttonState = digitalRead(buttons[3]);  
  if(buttonState == LOW) {
      bitField = bitField | 0x20;
  }
   buttonState = digitalRead(buttons[4]);  
  if(buttonState == LOW) {
      bitField = bitField | 0x40;
  }
  
  return bitField;

  
}

void loop() {

  int p1b[] = {P1_VERT,P1_HORIZ,P1_B1,P1_B2,P1_B3};
  int p2b[] = {P2_VERT,P2_HORIZ,P2_B1,P2_B2,P2_B3};
  
  unsigned int p1BitField=0x00;
  unsigned int p2BitField=0x00;
  unsigned int auxBitField=0x00;


  p1BitField=checkPlayer(p1b,p1BitField);
  p2BitField=checkPlayer(p2b,p2BitField);
  
  int p1s = digitalRead(P1_START);  
  int p2s = digitalRead(P1_START);

  if((p1s == LOW) && (p2s == LOW)) {
        auxBitField = auxBitField | 0x80;
  }
  else if(p1s == LOW) {
      auxBitField = auxBitField | 0x01;
  }  
  else if(p2s == LOW) {
      auxBitField = auxBitField | 0x02;
  }
  
  buttonState = digitalRead(COIN);  
  if(buttonState == LOW) {
      auxBitField = auxBitField | 0x04;
  }
  p1BitFieldState=p1BitField;
  p2BitFieldState=p2BitField;
  auxBitFieldState=auxBitField;
}

