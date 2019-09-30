#include <SPI.h>
#include <LoRa.h>

//declare pins for fueling, venting, and disconnect/reset switches
const int fuelSwitch = 4;
const int ventSwitch = 5;

//VARIABLES FOR IGNITION SAFETY AND BUTTON
const int ignitionLock = 8;
const int ignitionSwitch = 9;
int tOpen = 0;

const int BVSwitch = 10

//set variables for the condition of each switch
//this corresponds to the desired state of the analogous solenoid
String CommandV;
String CommandD;
String CommandF;
String BValve;
String trimKey;

//variable to store list of desired states
String Command;

//variable to hold last ball valve command on button to prevent repetitive signal sending? -> only "switch" commands processed
bool prevBVCommand = false;

void setup() {
  //begin RF communiation
  LoRa.begin(915E6);
  LoRa.setTxPower(2);
  Serial.begin(9600);
  //set all switch pins to receive voltage
  pinMode(ventSwitch, INPUT);
  pinMode(fuelSwitch, INPUT);
  pinMode(ignitionSwitch, INPUT);
  pinMode(ignitionLock, INPUT);
}

void ventCom(){
  if (digitalRead(ventSwitch) == LOW){
    CommandV = "C"; //indicate venting command was sent
  } 
  else{
    CommandV = "O";
}

void fuelCom() {
  if (digitalRead(fuelSwitch) == LOW){
    CommandF = "C"; //indicate fueling variable was sent
  }else if (digitalRead(fuelSwitch) == HIGH){
    CommandF = "O"; //indicate stop fueling varaible was sent
  }
}

void BV() {
  if (Serial.available()) { // The testing of valve via serial input, which can happen EVEN IF IGNITION LOCK IS LOW
    trimKey = Serial.read();
    if (trimKey == "F" or trimKey == "R"){
      BValve = trimKey; //indicate ball valve was driven in the direction determined by the 
    }
  }
  if (ignitionLock != HIGH) {
    if (digitalRead(BVSwitch) == HIGH and prevBVCommand == false){
      BValve = "F";
      prevBVCommand = true;
    }
    else if(digitalRead(BVSwitch) == LOW and prevBVCCommand == true){
      BValve = "R";
      prevBVCommand = false;
  }
}

void ignitionCom() {
  if (digitalRead(ignitionLock) == HIGH and digitalRead(ignitionSwitch) == HIGH){ 
    //only execute ignition function if key switch is turned to ON position
    BValve = "I"; //indicate ignition variable was sent
  }
}

void getCom() { //check states, send commands to open/close valves to rocket
  ventCom();
  fuelCom(); 
  BV();
  ignitionCom();
}

void loop() {
  CommandV = "X"; //set states to default holding state variables
  CommandF = "X";
  BValve = "X";
  trimKey = "X";

  getCom(); //detect desired states
  Command = CommandV + CommandF + BValve; //compile data packet to send to solenoid actuator
  Serial.println(Command); //DEBUG: display what you sent as the command
  LoRa.beginPacket(); //start transmission
  LoRa.print(Command); //just gonna send it, asuhh dudes
  LoRa.endPacket(); //end transmission
}
