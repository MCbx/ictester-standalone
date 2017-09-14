#include <LiquidCrystal.h>

//TERMINAL PART
//MCbx, 2017
//GNU GPL

//LCD
#define LCD_D7 2
#define LCD_D6 3
#define LCD_D5 4
#define LCD_D4 5
#define LCD_EN 6
#define LCD_RS 7

//Buttons
#define BTN_LT 15
#define BTN_RT 16
#define BTN_OK 17

//power sense
#define PWR_ON 18
#define PWR_GD 12

//baudrate sense
#define BAUD 14

//tester reset
#define REST_PIN 13

#define debounce_delay 65534


String line1;
String line2;
byte cursor=0;
LiquidCrystal lcd(LCD_RS,LCD_EN,LCD_D4,LCD_D5,LCD_D6,LCD_D7);


void refresh()
{
  lcd.clear();
  lcd.print(line1);
  lcd.setCursor(0,1);
  lcd.print(line2);
}

void lcdprintln(String a)
{
  line1=line2;
  line2=a;
  refresh();
}

void lcdprint(String a)
{
  line2 = line2 + a;
  refresh();
}

void powerOn()
{
  digitalWrite(PWR_ON,HIGH);
}

void powerOff()
{
  digitalWrite(PWR_ON,LOW);
}

void powerGood()
{
  if ( (!digitalRead(BTN_LT)) && (!digitalRead(BTN_RT)) )
  {
    Serial.print("1");
  }
  
  if (digitalRead(PWR_GD))
  {
    Serial.print("0");
  }
  else
  {
    Serial.print("1");
  }
}

void setup() {
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  pinMode(BAUD,INPUT_PULLUP);
  if (digitalRead(BAUD))
  {
    Serial.begin(57600);
    lcdprintln("57600 OK");
  }
  else
  {
    Serial.begin(38400);
    lcdprintln("38400 OK");
  }

  pinMode(BTN_LT, INPUT_PULLUP);
  pinMode(BTN_RT, INPUT_PULLUP);
  pinMode(BTN_OK, INPUT_PULLUP);
  
  pinMode(PWR_GD, INPUT_PULLUP);
  pinMode(PWR_ON, OUTPUT);  
  lcdprintln("");
  powerOff();

  pinMode(REST_PIN,OUTPUT);
  digitalWrite(REST_PIN,LOW);
  delay(10);
  digitalWrite(REST_PIN,HIGH);
}

unsigned int debounce = 0;
void loop() {

  //Visualize things from serial port
  if (Serial.available())
  {
    byte k=Serial.read();
    if (k=='\r')
    {
      return;
    }
    if (k=='~')     //power on
    {
      powerOn();
      return;
    }
    if (k=='`')    //power off
    {
      powerOff();
      return;
    }
    if (k=='^')   //power check
    {
      powerGood();
      return;
    }
    if (k=='\n')  //newline
    {
      cursor=1;
//      lcdprintln("");
      return;
    }
    
    if (cursor==1)
    {
      cursor=0;
      lcdprintln("");
    }
    lcdprint(String(char(k)));  //show!
  }

  
  //keyboard
  if (debounce>0)
  {
    debounce--;
    return;     //abort iteration
  }
  if (!digitalRead(BTN_LT))
  {
    Serial.print("a");
    debounce=debounce_delay;
  }
  if (!digitalRead(BTN_RT))
  {
    Serial.print("d");
    debounce=debounce_delay;
  }
  if (!digitalRead(BTN_OK))
  {
    Serial.print("s");
    debounce=debounce_delay;
  }

}
