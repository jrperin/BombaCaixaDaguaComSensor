/* 
  Water pump with sensors 
 
  created 2013
  by Joao R. Perin
*/

const boolean debugMode = false;

// constants won't change. Used here to 
// set pin numbers:
// INPUT Pins
const int s1 =          2;       // water sensor level 1
const int s2 =          3;       // water sensor level 2
const int s3 =          4;       // water sensor level 3
const int s4 =          5;       // water sensor level 4
const int sWaterIn =    6;       // water in sensor
const int sAux1   =     7;       // auxiliar input
const int OnNowButton = 9;       // button to turn ON water pump

// OUTPUT Pins
const int pump  =   8;       // Turn on/off water pump engine

// Variables will change:
int  pumpSt          = LOW;         // pumpState used to set the pump
long pumpTimePrev    = 0;
long previousMillis  = 0;           // will store last time LED was updated

int level1           = LOW;
int level2           = LOW;
int level3           = LOW;
int level4           = LOW;
int waterInSt        = LOW;
int sAux1St          = LOW;
int OnNowButtonSt    = LOW;

int turnOnNow        = 2; //2 = Normal OFF / 1 = Forced ON / 0 = Forced OFF


// the follow variables is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
const long defaultInterval = 1000; 
long interval              = 1000;  // interval about main check
long pumpInterval          = 1000;  // interval of pump on cicle

long chkBtnTimePrev = 0;


void setup() {
  // set the digital pin as input or output:
  pinMode(pump,        OUTPUT);
  
  pinMode(s1,          INPUT );
  pinMode(s2,          INPUT );
  pinMode(s3,          INPUT );
  pinMode(s4,          INPUT );
  pinMode(sWaterIn,    INPUT );
  pinMode(sAux1,       INPUT );
  pinMode(OnNowButton, INPUT );
  
  Serial.begin(9600);
}

void loop()
{
  checkButtons();
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > interval || turnOnNow == 1) {
    interval = defaultInterval;
    printTime();
    // save the last time you blinked the LED 
    previousMillis = currentMillis;   
    getStatus();
    if (debugMode) {printStatus();}
    
    if ((level1 == LOW && level2 == LOW && level3 == LOW && level4 == LOW) || (turnOnNow == 1)) {
      // Turn ON the water pump
      pumpSt = HIGH;
      digitalWrite(pump, pumpSt);
      printStatus();
      printTime();
      Serial.println("Ligando a bomba!");
      Serial.println("   ---   w a t e r   P u m p   O N     ---");
      while (pumpSt == HIGH){
        waterPumpON();
      }
      printTime();
      Serial.println("   ---   w a t e r   P u m p   O F F   ---");
    } else {
      pumpSt = LOW;
      digitalWrite(pump, pumpSt);
      if(debugMode) {Serial.println("Mantendo a bomba desligada!");}
    }
  }
}

void waterPumpON() {
    checkButtons();

    if (turnOnNow == 0){
      turnOnNow = 2;  // Setting to Normal Status OFF
      pumpSt = LOW;
      digitalWrite(pump, pumpSt);
      Serial.println("Desligamento forcado da bomba");
      return;
    }

    unsigned long currentMillis = millis();
    if(currentMillis - pumpTimePrev > pumpInterval){
      pumpTimePrev = currentMillis;
      printTime();
      // check if water is entering else turn pump off
      getStatus();
      if (debugMode){printStatus();}
      if (waterInSt != HIGH){
        printStatus();
        Serial.println("CHK1 - Nao esta entrando agua! Risco de dano na bomba!");
        delay(10000);  //wait for next check
        getStatus();
        if (waterInSt != HIGH){
          printStatus();
          Serial.println("CHK2 - Continua nao entrando agua! Risco de dano na bomba!");
          pumpSt = LOW;
          turnOnNow = 2;
          digitalWrite(pump, pumpSt); //Water not entering, turn off the pump!
          printTime();
          Serial.println("Apos CHK2 a bomba foi desligada");
          Serial.println("Configurando tempo de checagem para daqui a 15 minutos");
          previousMillis = millis();
          interval = 900000; //Wait 15 minutes for check again! Preventing turn on excessively
          //interval = 60000; //For testing waiting 1 minute.
          return;
        } else {
          printStatus();
          Serial.println("CHK2 - Voltou a entrar agua!");
        }
      }
      if (level1 == HIGH){
        Serial.println("Caixa dagua cheia");
        turnOnNow = 2; //Set to normal turn OFF 
        pumpSt = LOW;
        printStatus();
        digitalWrite(pump, pumpSt); //Water box is full, turn off the pump!
        Serial.println("Bomda desligada por caixa dagua cheia - level1 atingido");
      }
    }
}

void checkButtons(){
    OnNowButtonSt  = digitalRead(OnNowButton);
    unsigned long currentMillis = millis();
  if (OnNowButtonSt == HIGH){
      if (currentMillis - chkBtnTimePrev > 1500){
        chkBtnTimePrev = currentMillis;
        Serial.println("OnNowButtonSt = HIGH");
        if (pumpSt){
            turnOnNow = 0; // Force turn OFF
        }else{    
            getStatus();
            if (level1 != HIGH){
              printStatus();
              turnOnNow = 1;  // Force turn ON
            } else {
              printStatus();
              Serial.println("Caixa dagua cheia, nao deve ligar a bomba!");
            }
        }
      }
  }
}

void getStatus(){
  level1    = digitalRead(s1);
  level2    = digitalRead(s2);
  level3    = digitalRead(s3);
  level4    = digitalRead(s4);
  waterInSt = digitalRead(sWaterIn);
  sAux1St   = digitalRead(sAux1);
}

void printStatus(){
  String strAux, strOut;
  strAux = "level1 = ";
  strOut = strAux + level1; 
  Serial.print(strOut);
  
  strAux = " level2 = ";
  strOut = strAux + level2; 
  Serial.print(strOut);
  
  strAux = " level3 = ";
  strOut = strAux + level3; 
  Serial.print(strOut);

  strAux = " level4 = ";
  strOut = strAux + level4; 
  Serial.print(strOut);
  
  strAux = " waterInSt = ";
  strOut = strAux + waterInSt; 
  Serial.print(strOut);
  
  strAux = " sAux1St = ";
  strOut = strAux + sAux1St; 
  Serial.print(strOut);
  
  strAux = " turnOnNow = ";
  strOut = strAux + turnOnNow; 
  Serial.print(strOut);
  
  Serial.println();

}

void printTime(){
  String strAux, strOut;
  unsigned long nowTime = millis();
  nowTime = nowTime / 1000;
  strAux = "  -Time: ";
  strOut = strAux + nowTime;
  Serial.println(strOut);
  
}
