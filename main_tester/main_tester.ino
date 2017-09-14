#include <FastGPIO.h>
//IC Tester
//MCbx 2017
//GNU GPL

#define patsNo 16
const PROGMEM byte patterns[]={0,255,85,170,204,51,153,102, 128, 64, 32, 16,8,4,2,1}; //patterns tested on chip
//85 - 1010, 170 - 0101, 204 - 0011, 51 - 1100, 153 - 1001, 102 - 0110

//chip definitions, sample values not used.
int rows=512;
int cols=512;
unsigned int wait=3000;
long words=262144;
byte addressBits=9;
byte bitsPerWord=1; 

//int - terrible hack
const PROGMEM unsigned int chips[]={4116, 4164, 41256, 4416, 4464, 44256, 21010, 74};
byte menuptr=0;
byte testNo=0;

byte (*read_word)(int, int);   //FILL THESE IN CHIP DEF!
void (*write_word)(byte, int, int);
void (*initialize_memory)();
void (*putAddress)(int);

inline void powerOn()
{
  Serial.print("~");
}

inline void powerOff()
{
  Serial.print("`");
}


//prepares program for chosen chip
void prepare(int chipNo)
{
  if (chipNo==0) //4116
  {
    rows=128;
    cols=128;
    words=16384;
    addressBits=7;
    bitsPerWord=1;
    read_word=&read_word1;
    write_word=&write_word1;
    initialize_memory=&initialize_memory1;
    putAddress=&putAddress1;
  }
  if (chipNo==1) //4164 OK
  {
    rows=256;
    cols=256;
    words=65536;
    addressBits=8;
    bitsPerWord=1;
    read_word=&read_word1;
    write_word=&write_word1;
    initialize_memory=&initialize_memory1;
    putAddress=&putAddress1;
  }
  if (chipNo==2)  //41256 OK
  {
    rows=512;
    cols=512;
    words=262144;
    addressBits=9;
    bitsPerWord=1;
    read_word=&read_word1;
    write_word=&write_word1;
    initialize_memory=&initialize_memory1;
    putAddress=&putAddress1;
  }
  if (chipNo==3)
  {    //4416 OK?
    rows=256;
    cols=64;
    words=16384;
    addressBits=7;
    bitsPerWord=4;
    read_word=&read_word4;
    write_word=&write_word4;
    initialize_memory=&initialize_memory1;
    putAddress=&putAddress4;
  }
  if (chipNo==4)
  {    //4464 OK
    rows=256;
    cols=256;
    words=65536;
    addressBits=8;
    bitsPerWord=4;
    read_word=&read_word4;
    write_word=&write_word4;
    initialize_memory=&initialize_memory1;
    putAddress=&putAddress4;
  }
  if (chipNo==5)
  {     //44256
    rows=512;
    cols=512;
    words=262144;
    addressBits=9;
    bitsPerWord=4;
    read_word=&read_word5;
    write_word=&write_word5;
    initialize_memory=&initialize_memory2;
    putAddress=&putAddress5;
  }
  if (chipNo==6)
  {    //21010 - ok
    rows=1024;
    cols=1024;
    words=1048576;
    addressBits=9;
    bitsPerWord=1;
    read_word=&read_word2;
    write_word=&write_word2;
    initialize_memory=&initialize_memory2;
    putAddress=&putAddress2;
  }
}

//displays menu to prepare tester for chips
void mainMenu()
{ 
  byte k='o';
  while (k!='s')
  {
    Serial.println("   DRAM  TEST");
    prepare(menuptr);
    Serial.print("   < ");
    Serial.print(pgm_read_word_near(chips+menuptr));
    Serial.println(" >");
    while (Serial.available()==0);
    k=Serial.read();
    if (k=='a')
    {
        menuptr--;
        if (menuptr==255)
            menuptr=7;
    }
    if (k=='d') //forward
    {
        menuptr++;
        if (menuptr==8)
            menuptr=0;
    }
  }
  
  if (menuptr==7) //TTL test bootstrap
  {
    BootTTLTester();
  }

  //DRAM STEP 2
  k='o'; //one more time ask user to configure switches and put chip
  while(k!='s')
  {
    Serial.println("CONFIG, PUT CHIP");
    if (menuptr==0)
    {
      Serial.println("SW 1, 2, 3, 4 UP");
    }
    if ((menuptr==1)||(menuptr==2))
    {
      Serial.println("    SW 4 UP");
    }
    if ((menuptr==3)||(menuptr==4)||(menuptr==6))
    {
      Serial.println("    SW 5 UP");
    }
    if (menuptr==5)
    {
      Serial.println("   ALL DOWN");
    }
   // Serial.println(" PUT CHIP IN");
    while (Serial.available()==0);
    k=Serial.read();
  }
}


void setup() {
  Serial.begin(38400);
  delay(100); //wait for terminal initialization
  
  //POWER TEST!
  byte k='2';
  while (k!='1')
  {
    powerOn();
    delay(150);
    Serial.println("PWR TEST");
    Serial.print("^");
    while (Serial.available()==0);
    k=Serial.read();
    powerOff();
    if (k!='1')
    {
      Serial.print((char)k);
      Serial.println(" -5V POWER BAD!");
      delay(750);
    }
  }
    Serial.println();
  mainMenu();

  //SET PIN MODES
  if (menuptr<3) //4116, 6164, 61256
  {
    FastGPIO::Pin<6>::setOutput(HIGH);
    FastGPIO::Pin<8>::setOutput(HIGH);
    FastGPIO::Pin<7>::setOutput(HIGH);
    FastGPIO::Pin<12>::setOutput(HIGH);
    FastGPIO::Pin<11>::setOutput(HIGH);
    FastGPIO::Pin<10>::setOutput(HIGH);
    FastGPIO::Pin<13>::setOutput(HIGH);
    FastGPIO::Pin<9>::setOutput(HIGH);
    FastGPIO::Pin<2>::setOutput(HIGH);
    FastGPIO::Pin<3>::setOutput(HIGH);
    FastGPIO::Pin<5>::setOutput(HIGH);
    FastGPIO::Pin<15>::setOutput(HIGH);
    FastGPIO::Pin<4>::setOutput(HIGH);
    FastGPIO::Pin<14>::setInput();
  }

  if ((menuptr==3)||(menuptr==4)) //4416, 4464
  {
    FastGPIO::Pin<6>::setOutput(HIGH);
    FastGPIO::Pin<8>::setOutput(HIGH);
    FastGPIO::Pin<7>::setOutput(HIGH);
    FastGPIO::Pin<12>::setOutput(HIGH);
    FastGPIO::Pin<11>::setOutput(HIGH);
    FastGPIO::Pin<10>::setOutput(HIGH);
    FastGPIO::Pin<13>::setOutput(HIGH);
    FastGPIO::Pin<9>::setOutput(HIGH);
    FastGPIO::Pin<5>::setOutput(HIGH);
    FastGPIO::Pin<15>::setOutput(HIGH);
    FastGPIO::Pin<4>::setOutput(HIGH);
    FastGPIO::Pin<16>::setOutput(HIGH);
    FastGPIO::Pin<2>::setInput();
    FastGPIO::Pin<3>::setInput();
    FastGPIO::Pin<14>::setInput();
    FastGPIO::Pin<17>::setInput();
  }
  
  if (menuptr==6) //21010
  {
    FastGPIO::Pin<2>::setOutput(HIGH);
    FastGPIO::Pin<3>::setOutput(HIGH);
    FastGPIO::Pin<4>::setOutput(LOW);
    FastGPIO::Pin<5>::setOutput(HIGH);
    FastGPIO::Pin<6>::setOutput(HIGH);
    FastGPIO::Pin<7>::setOutput(HIGH);
    FastGPIO::Pin<8>::setOutput(HIGH);
    FastGPIO::Pin<9>::setOutput(HIGH);
    FastGPIO::Pin<10>::setOutput(HIGH);
    FastGPIO::Pin<11>::setOutput(HIGH);
    FastGPIO::Pin<12>::setOutput(HIGH);
    FastGPIO::Pin<13>::setOutput(HIGH);
    FastGPIO::Pin<14>::setOutput(HIGH);
    FastGPIO::Pin<15>::setOutput(HIGH);
    FastGPIO::Pin<16>::setOutput(HIGH);    
    FastGPIO::Pin<17>::setInput();
  }

  if (menuptr==5) //44256
  {
    FastGPIO::Pin<2>::setOutput(HIGH);
    FastGPIO::Pin<3>::setOutput(HIGH);
    FastGPIO::Pin<4>::setOutput(LOW);
    FastGPIO::Pin<5>::setOutput(HIGH);
    FastGPIO::Pin<6>::setOutput(HIGH);
    FastGPIO::Pin<7>::setOutput(HIGH);
    FastGPIO::Pin<8>::setOutput(HIGH);
    FastGPIO::Pin<9>::setOutput(HIGH);
    FastGPIO::Pin<10>::setOutput(HIGH);
    FastGPIO::Pin<11>::setOutput(HIGH);
    FastGPIO::Pin<12>::setOutput(HIGH);
    FastGPIO::Pin<13>::setOutput(HIGH);
    FastGPIO::Pin<14>::setOutput(HIGH);
    FastGPIO::Pin<15>::setOutput(HIGH);
    FastGPIO::Pin<16>::setInput();
    FastGPIO::Pin<17>::setInput();
    FastGPIO::Pin<18>::setInput();
    FastGPIO::Pin<19>::setInput();
    
  }
  powerOn();

  //set refreshing interrupt!
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 1024;  //bad value but seems to work. Fails without.
  TCCR1B |= (1 << WGM12); // CTC mode
  TCCR1B |= (1 << CS12); // 256 prescaler
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
  interrupts(); // enable all interrupts
}

//for 4116, 4164, 41256
void putAddress1(int addr) //this doesn't require NoInterrupts as it's called 
{                       //from inside of noInterrupts function.
    FastGPIO::Pin<6>::setOutput(bitRead(addr,0));
    FastGPIO::Pin<8>::setOutput(bitRead(addr,1));
    FastGPIO::Pin<7>::setOutput(bitRead(addr,2));
    FastGPIO::Pin<12>::setOutput(bitRead(addr,3));
    FastGPIO::Pin<11>::setOutput(bitRead(addr,4));
    FastGPIO::Pin<10>::setOutput(bitRead(addr,5));
    FastGPIO::Pin<13>::setOutput(bitRead(addr,6));
    FastGPIO::Pin<9>::setOutput(bitRead(addr,7));
    FastGPIO::Pin<2>::setOutput(bitRead(addr,8));
}

//for 4416, 4464
void putAddress4(int addr) //this doesn't require NoInterrupts as it's called 
{                       //from inside of noInterrupts function.
    FastGPIO::Pin<13>::setOutput(bitRead(addr,0));
    FastGPIO::Pin<12>::setOutput(bitRead(addr,1));
    FastGPIO::Pin<11>::setOutput(bitRead(addr,2));
    FastGPIO::Pin<10>::setOutput(bitRead(addr,3));
    FastGPIO::Pin<8>::setOutput(bitRead(addr,4));
    FastGPIO::Pin<7>::setOutput(bitRead(addr,5));
    FastGPIO::Pin<6>::setOutput(bitRead(addr,6));
    FastGPIO::Pin<9>::setOutput(bitRead(addr,7));
}

//for 44256
void putAddress5(int addr) //this doesn't require NoInterrupts as it's called 
{                       //from inside of noInterrupts function.
    FastGPIO::Pin<5>::setOutput(bitRead(addr,0));
    FastGPIO::Pin<6>::setOutput(bitRead(addr,1));
    FastGPIO::Pin<7>::setOutput(bitRead(addr,2));
    FastGPIO::Pin<8>::setOutput(bitRead(addr,3));
    FastGPIO::Pin<9>::setOutput(bitRead(addr,4));
    FastGPIO::Pin<10>::setOutput(bitRead(addr,5));
    FastGPIO::Pin<11>::setOutput(bitRead(addr,6));
    FastGPIO::Pin<12>::setOutput(bitRead(addr,7));
    FastGPIO::Pin<13>::setOutput(bitRead(addr,8));
}


//for 21010
void putAddress2(int addr) //this doesn't require NoInterrupts as it's called 
{                       //from inside of noInterrupts function.
    FastGPIO::Pin<5>::setOutput(bitRead(addr,0));
    FastGPIO::Pin<6>::setOutput(bitRead(addr,1));
    FastGPIO::Pin<7>::setOutput(bitRead(addr,2));
    FastGPIO::Pin<8>::setOutput(bitRead(addr,3));
    FastGPIO::Pin<9>::setOutput(bitRead(addr,4));
    FastGPIO::Pin<10>::setOutput(bitRead(addr,5));
    FastGPIO::Pin<11>::setOutput(bitRead(addr,6));
    FastGPIO::Pin<12>::setOutput(bitRead(addr,7));
    FastGPIO::Pin<13>::setOutput(bitRead(addr,8));
    FastGPIO::Pin<14>::setOutput(bitRead(addr,9));
}

//Refreshing. this has to be fired regurarly 
ISR(TIMER1_COMPA_vect)        // interrupt service routine 
{  
    if(menuptr==6)
    {
      for (int i=0;i<rows;i++)
      {
        (*putAddress)(i);
        FastGPIO::Pin<3>::setOutput(LOW);
        FastGPIO::Pin<3>::setOutput(HIGH); 
      }
      return;
    }
    for (int i=0;i<rows;i++)
    {
      (*putAddress)(i);
      FastGPIO::Pin<5>::setOutput(LOW);
      FastGPIO::Pin<5>::setOutput(HIGH); 
    }
} 

//write bit to memory, 4116, 4164, 41256
void write_word1(byte data, int row, int col)
{
  FastGPIO::Pin<3>::setOutput(bitRead(data,0)); //data
  //row addr
  putAddress1(row);
  FastGPIO::Pin<5>::setOutput(LOW);
  //we
  FastGPIO::Pin<4>::setOutput(LOW);
  //col addr
  putAddress1(col);
  FastGPIO::Pin<15>::setOutput(LOW);

  FastGPIO::Pin<5>::setOutput(HIGH);
  FastGPIO::Pin<4>::setOutput(HIGH);
  FastGPIO::Pin<15>::setOutput(HIGH);
}

//read bit from memory, 4116, 4164, 41256
byte read_word1(int row, int col)
{
  byte a=0;
  //row addr
  putAddress1(row);
   FastGPIO::Pin<5>::setOutput(LOW);
  //col addr
  putAddress1(col);
  FastGPIO::Pin<15>::setOutput(LOW);
  bitWrite(a,0,FastGPIO::Pin<14>::isInputHigh());
  
  FastGPIO::Pin<5>::setOutput(HIGH); 
  FastGPIO::Pin<15>::setOutput(HIGH);
  return a;
}

//initialize, 4116, 4164, 41256
void initialize_memory1() //shoot 8 RAS cycles before using.
{   //this is important.
  noInterrupts();
  for (byte i=0;i<8;i++)
  {
    FastGPIO::Pin<5>::setOutput(LOW); 
    FastGPIO::Pin<5>::setOutput(HIGH); 
  }
  interrupts();
}

//for 4416, 4464
void write_word4(byte data, int row, int col)
{
  FastGPIO::Pin<2>::setOutput(bitRead(data,0)); //data
  FastGPIO::Pin<3>::setOutput(bitRead(data,1)); //data
  FastGPIO::Pin<14>::setOutput(bitRead(data,2)); //data
  FastGPIO::Pin<17>::setOutput(bitRead(data,3)); //data
  //row addr
  putAddress4(row);
  FastGPIO::Pin<5>::setOutput(LOW);
  //we
  FastGPIO::Pin<4>::setOutput(LOW);
  //col addr
  if (menuptr==4)
  {
    putAddress4(col);
  }
  else //4416 is strange.
  {
    FastGPIO::Pin<12>::setOutput(bitRead(col,0));
    FastGPIO::Pin<11>::setOutput(bitRead(col,1));
    FastGPIO::Pin<10>::setOutput(bitRead(col,2));
    FastGPIO::Pin<8>::setOutput(bitRead(col,3));
    FastGPIO::Pin<7>::setOutput(bitRead(col,4));
    FastGPIO::Pin<6>::setOutput(bitRead(col,5));
  }
    
  FastGPIO::Pin<15>::setOutput(LOW);
  FastGPIO::Pin<4>::setOutput(HIGH);
  FastGPIO::Pin<5>::setOutput(HIGH);
  FastGPIO::Pin<15>::setOutput(HIGH);

  FastGPIO::Pin<2>::setInput();
  FastGPIO::Pin<3>::setInput();
  FastGPIO::Pin<14>::setInput();
  FastGPIO::Pin<17>::setInput();
}

//for 4416, 4464
byte read_word4(int row, int col)
{
  byte a=0;
  //row addr
  putAddress4(row);
  FastGPIO::Pin<5>::setOutput(LOW);
  //col addr
    if (menuptr==4)
  {
    putAddress4(col);
  }
  else //4416 is strange. Columns have to be entered different way.
  {
    FastGPIO::Pin<12>::setOutput(bitRead(col,0));
    FastGPIO::Pin<11>::setOutput(bitRead(col,1));
    FastGPIO::Pin<10>::setOutput(bitRead(col,2));
    FastGPIO::Pin<8>::setOutput(bitRead(col,3));
    FastGPIO::Pin<7>::setOutput(bitRead(col,4));
    FastGPIO::Pin<6>::setOutput(bitRead(col,5));
  }
  FastGPIO::Pin<15>::setOutput(LOW);

  //inputs
  FastGPIO::Pin<2>::setInput();
  FastGPIO::Pin<3>::setInput();
  FastGPIO::Pin<14>::setInput();
  FastGPIO::Pin<17>::setInput();

  // /g low
  FastGPIO::Pin<16>::setOutput(LOW);

  //read things
  bitWrite(a,0,FastGPIO::Pin<2>::isInputHigh());
  bitWrite(a,1,FastGPIO::Pin<3>::isInputHigh());
  bitWrite(a,2,FastGPIO::Pin<14>::isInputHigh());
  bitWrite(a,3,FastGPIO::Pin<17>::isInputHigh());

  FastGPIO::Pin<16>::setOutput(HIGH);

  FastGPIO::Pin<5>::setOutput(HIGH); 
  FastGPIO::Pin<15>::setOutput(HIGH);

  return a;
}



//for 44256
void write_word5(byte data, int row, int col)
{
  FastGPIO::Pin<19>::setOutput(bitRead(data,0)); //data
  FastGPIO::Pin<16>::setOutput(bitRead(data,1)); //data
  FastGPIO::Pin<17>::setOutput(bitRead(data,2)); //data
  FastGPIO::Pin<18>::setOutput(bitRead(data,3)); //data
  //row addr
  putAddress5(row);
  FastGPIO::Pin<3>::setOutput(LOW);
  //we
  FastGPIO::Pin<2>::setOutput(LOW);
  //col addr
  putAddress5(col);
  FastGPIO::Pin<15>::setOutput(LOW);
  
  FastGPIO::Pin<3>::setOutput(HIGH);
  FastGPIO::Pin<2>::setOutput(HIGH);
  FastGPIO::Pin<15>::setOutput(HIGH);

  FastGPIO::Pin<19>::setInput();
  FastGPIO::Pin<16>::setInput();
  FastGPIO::Pin<17>::setInput();
  FastGPIO::Pin<18>::setInput();
}

//for 44256
byte read_word5(int row, int col)
{
  byte a=0;
  //row addr
  putAddress5(row);
  FastGPIO::Pin<3>::setOutput(LOW);
  //col addr
  putAddress5(col);
  FastGPIO::Pin<15>::setOutput(LOW);

  //inputs
  FastGPIO::Pin<19>::setInput();
  FastGPIO::Pin<16>::setInput();
  FastGPIO::Pin<17>::setInput();
  FastGPIO::Pin<18>::setInput();

  // /g low
  FastGPIO::Pin<14>::setOutput(LOW);

  //read things
  bitWrite(a,0,FastGPIO::Pin<19>::isInputHigh());
  bitWrite(a,1,FastGPIO::Pin<16>::isInputHigh());
  bitWrite(a,2,FastGPIO::Pin<17>::isInputHigh());
  bitWrite(a,3,FastGPIO::Pin<18>::isInputHigh());

  FastGPIO::Pin<14>::setOutput(HIGH);

  FastGPIO::Pin<3>::setOutput(HIGH); 
  FastGPIO::Pin<15>::setOutput(HIGH);

  return a;
}



//write bit to memory, 21010
void write_word2(byte data, int row, int col)
{
  FastGPIO::Pin<16>::setOutput(bitRead(data,0)); //data
  //row addr
  putAddress2(row);
  FastGPIO::Pin<3>::setOutput(LOW);
  //we
  FastGPIO::Pin<2>::setOutput(LOW);
  //col addr
  putAddress2(col);
  FastGPIO::Pin<15>::setOutput(LOW);

  FastGPIO::Pin<2>::setOutput(HIGH);
  FastGPIO::Pin<15>::setOutput(HIGH);
  FastGPIO::Pin<3>::setOutput(HIGH);
}

//read bit from memory, 21010
byte read_word2(int row, int col)
{
  byte a=0;
  //row addr
  putAddress2(row);
   FastGPIO::Pin<3>::setOutput(LOW);
  //col addr
  putAddress2(col);
  FastGPIO::Pin<15>::setOutput(LOW);
  bitWrite(a,0,FastGPIO::Pin<17>::isInputHigh());
  
  FastGPIO::Pin<3>::setOutput(HIGH); 
  FastGPIO::Pin<15>::setOutput(HIGH);

  return a;
}

//initialize, 21010
void initialize_memory2() //shoot 8 RAS cycles before using.
{   //this is important.
  noInterrupts();
  for (byte i=0;i<8;i++)
  {
    FastGPIO::Pin<3>::setOutput(LOW); 
    FastGPIO::Pin<3>::setOutput(HIGH); 
  }
  interrupts();
}

//test pattern generator.
short pt1=-1;
byte pattern(byte no,byte bits)
{
  byte k=0;
  for (int i=0;i<bits;i++)
  {
    pt1++;
    if (pt1>7)
      pt1=0;
    bitWrite(k,i,bitRead(no,pt1));
  }
  return k;
}

//error halter
void throw_err(int r, int c, int t, int p)
{
   powerOff();
   Serial.println("");
   Serial.print("PASS ");
   Serial.print(t+1);
   Serial.print(" TEST ");
   Serial.print(p);
   Serial.println("");
   Serial.print("ERROR! ");
   Serial.println((long)r*rows+c);
   while(1);
}

void loop() {
  (*initialize_memory)();
  delay(100);
  for (byte curPatt=0;curPatt<patsNo;curPatt++) //patterns loop
  {
    Serial.print("PASS ");
    Serial.print(testNo+1);
    Serial.print(" TEST ");
    Serial.print(curPatt);
    Serial.println("");
    Serial.print("WRITE ");
    Serial.flush();
    pt1=-1;
    noInterrupts();
    //Now by calling rows more frequently, we implicitly refresh memory
    for (int c=0;c<cols;c++)
    {
      for (int r=0;r<rows;r++)
      {
        (*write_word)(pattern(pgm_read_byte_near(patterns+curPatt),bitsPerWord), r,c);
      // Serial.println(pattern(patterns[curPatt],bitsPerWord),BIN); //generator test
      }
    }
    interrupts(); //WAIT: Refreshing interrupt go!
    Serial.print("WAIT ");
    delay(wait);
    pt1=-1;
    Serial.print("READ ");
    Serial.flush();
    noInterrupts();
    for (int c=0;c<cols;c++)
    {
       for (int r=0;r<rows;r++)
       {
         if ((*read_word)(r,c)!=pattern(pgm_read_byte_near(patterns+curPatt),bitsPerWord))
         {
          interrupts();
//          Serial.println((*read_word)(r,c),BIN);
          throw_err(r,c,testNo,curPatt);
         }
       }
    }
    interrupts();
    Serial.println("");
  }
  testNo++;
}
