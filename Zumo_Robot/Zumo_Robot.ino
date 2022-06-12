// include the library code:
#include <LiquidCrystal.h>
//#define 

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 31, en = 30, d4 = 10, d5 = 11, d6 = 12, d7 = 13;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int ls1 , ls2, cs, rs1, rs2;
int ls,r;
int obs_detect = 0;



// ---------Voltage Sensor---------
const int voltagePin = A0;
float vOUT = 0.0;
float vIN = 0.0;
float R1 = 30000.0;
float R2 = 7500.0;
int value = 0;
float analogValue;
float input_voltage;
String Left = "Left";
// ----------Current Sensor--------
const int currentPin = A1;
int sensitivity = 66;
int adcValue= 0;
int offsetVoltage = 2500;
double adcVoltage = 0;
double currentValue = 0;

volatile int remember = 0;           // the number of pins (i.e. the length of the array
int last_val, c_val; // To avoid clear screen if its path does not change

// Time must be recorded for each direction instead of storing direction over and over again as it will increase the memory
// Or we can simply read each instruction and pass it to the conditions.



//volatile int counter;
int rpm;
int previousMillis = 0;
 

// array for simple Test
//String memory[200];
//int memory_count=0;

// SD Card
#include <SPI.h>
#include <SD.h>

File myFile, myFilee;

void setup() {
  // set up the LCD's number of columns and rows:
   
//   Serial.begin(9600);     //  opens serial port, sets data rate to 9600 bps
   
  lcd.begin(16, 2);       //// set up the LCD's number of columns and rows: 
  pinMode(A4,INPUT);
   
  pinMode(7,OUTPUT); // left motor
  pinMode(8,OUTPUT);
  pinMode(9,OUTPUT); // right motor
  pinMode(15,OUTPUT);
  
//  pinMode(3,INPUT);
  pinMode(4,INPUT); //Left
  pinMode(5,INPUT); //Left
  pinMode(6,INPUT); // Center
  pinMode(16,INPUT);  // Right
  pinMode(17,INPUT);  // Right
// No need for pin 6 as we connected the front sensor for obstacle to interrupt0 at pin 2.

// Interrupts
  attachInterrupt(digitalPinToInterrupt(2),Obs_detect,RISING);
  attachInterrupt(digitalPinToInterrupt(3),countpulse,RISING);
  // Open serial communications and wait for port to open:
  lcd.print("Serial Check");
  delay(2000);
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  if (!SD.begin(53)) {
    while (1);
  }
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.

  // Removing previous files
  SD.remove("Path.txt");
  SD.remove("check.txt");
  myFile = SD.open("Path.txt", FILE_WRITE);
//  myFile.println("Start");
  myFile.close();

}

void countpulse(){
        counter++;
}

void TurnLeft(){
  digitalWrite(7,0);
  digitalWrite(8,0);
  digitalWrite(9,0);
  digitalWrite(15,1);
  }
void TurnRight(){
  digitalWrite(7,0);
  digitalWrite(8,1);
  digitalWrite(9,0);
  digitalWrite(15,0);
  }
void Forward(){
  digitalWrite(7,0);
  digitalWrite(8,1);
  digitalWrite(9,0);
  digitalWrite(15,1);
  }
void Stop(){
  digitalWrite(7,0);
  digitalWrite(8,0);
  digitalWrite(9,0);
  digitalWrite(15,0);
  }

  // will rotate about the center
void Rotate(){
  digitalWrite(7,1);
  digitalWrite(8,0);
  digitalWrite(9,0);
  digitalWrite(15,1);
  }


void loop() {

// ---------Voltage Sensor Reading-----------
  value = analogRead(voltagePin);
  vOUT = (value * 5.0) / 1024.0;
  vIN = vOUT / (R2/(R1+R2));
//  lcd.setCursor(0,1);
//  lcd.print("Input =         ");
  lcd.setCursor(10,0);
  lcd.print(vIN);
  lcd.print("V");
//  delay(500);


// --------Current Sensor--------------------
  adcValue = analogRead(currentPin);
  adcVoltage = (adcValue / 1024.0) * 5000;
  currentValue = ((adcVoltage - offsetVoltage) / sensitivity);
  lcd.setCursor(8,1);
  lcd.print(currentValue);
  lcd.print("A");
  delay(200);

// -----------Encoder for RPS---------------
  if (millis() - previousMillis >= 1000) {
            rpm = (counter/20)*60;          
            counter = 0;
            previousMillis += 1000;
  }
 
//  lcd.setCursor(0,1);
//  lcd.print("Speed: ");
//  lcd.setCursor(8,1);
//  lcd.print(rpm);
//  lcd.print(" rps");
//  delay(1);


//delay(1000);
Serial.println("----------------------------------------------------------");
// ------------- IR Sensors Input-----------
  ls2 = digitalRead(4);
  ls1 = digitalRead(5);
  cs = digitalRead(17);
  rs1 = digitalRead(6);
  rs2 = digitalRead(16);
  Serial.println(ls2);
  Serial.println(ls1);
  Serial.println(cs);
  Serial.println(rs1);
  Serial.println(rs2);

// --------------Battery percentage------------
  analogValue = analogRead (A0);
  input_voltage = (analogValue * 5.0) / 1024.0;
  int percntage = (input_voltage*100/5);
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
//  lcd.setCursor(12, 1);
////  lcd.print("Battery= ");
//  lcd.print(percntage);
//  lcd.print("%");




// ------------Normal Operation---------------
  if (obs_detect == 0){
    myFile = SD.open("Path.txt", FILE_WRITE); // open text file
    if (ls2 == 0 && ls1 == 0 && cs == 1 && rs1 == 0 && rs2 == 0){
      c_val = 1;
      
      if (c_val != last_val){
        lcd.clear();
        myFile.println("Forward");
      }
      Forward();
      lcd.setCursor(0, 0);
      lcd.print("FORWARD");
      last_val = 1;
      myFile.flush();
      }
      // for Left, there are total 6 cases
    else if((ls2 == 0 && ls1 == 1 && cs == 0 && rs1 == 0 && rs2 == 0) || (ls2 == 0 && ls1 == 1 && cs == 1 && rs1 == 0 && rs2 == 0) || (ls2 == 1 && ls1 == 0 && cs == 0 && rs1 == 0 && rs2 == 0) || (ls2 == 1 && ls1 == 0 && cs == 1 && rs1 == 0 && rs2 == 0) || (ls2 == 1 && ls1 == 1 && cs == 0 && rs1 == 0 && rs2 == 0) || (ls2 == 1 && ls1 == 1 && cs == 1 && rs1 == 0 && rs2 == 0))
    {
      c_val = 2;
      if (c_val != last_val){
        lcd.clear();
        myFile.println("Left");
      }
      TurnLeft();
      lcd.setCursor(0, 0);
      lcd.print("LEFT");
      last_val = 2;
      myFile.flush();
      }
      // for Right, there are total 6 cases
    else if((ls2 == 0 && ls1 == 0 && cs == 0 && rs1 == 0 && rs2 == 1) || (ls2 == 0 && ls1 == 0 && cs == 0 && rs1 == 1 && rs2 == 0) || (ls2 == 0 && ls1 == 0 && cs == 0 && rs1 == 1 && rs2 == 1) || (ls2 == 0 && ls1 == 0 && cs == 1 && rs1 == 0 && rs2 == 1) || (ls2 == 0 && ls1 == 0 && cs == 1 && rs1 == 1 && rs2 == 0) || (ls2 == 0 && ls1 == 0 && cs == 1 && rs1 == 1 && rs2 == 1))
    {
      c_val = 3;
      if (c_val != last_val){
        lcd.clear();
        myFile.println("Right");
      }
      TurnRight();
      lcd.setCursor(0, 0);
      lcd.print("RIGHT");
      last_val = 3;
      myFile.flush();
      }
      // for Stop, there are total 6 cases
    else
    {
      c_val = 4;
      if (c_val != last_val){
        lcd.clear();
        myFile.println("Stop");
      }
      Stop();
      lcd.setCursor(0, 0);
      lcd.print("STOP");
      last_val = 4;
      myFile.flush();
      }
    myFile.close();
  }



  // -----------Once Obstacle is Detected-----------------
  else if (obs_detect == 1) {
    noInterrupts();
    Rotate();
    while(digitalRead(6) != 0){
    Rotate();
    lcd.print("Rotating to White");
    }
    while(digitalRead(6) != 1){
    Rotate();
    lcd.print("Rotating to Black");
    }
    Stop();
    lcd.clear();
    lcd.print("Done");
  }
//  myFile.close();
}
void Obs_detect(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Halt!!");
//  myFile = SD.open("Path.txt", FILE_WRITE);
//  myFile.println(timer);
//  myFile.println("Obstacle Detected");
//  myFile.flush();
  e_time = millis();
  t_time = e_time - s_time;
  timer_arr[count] = t_time;
  count++;
  obs_detect = 1;
  Stop(); 
  }
