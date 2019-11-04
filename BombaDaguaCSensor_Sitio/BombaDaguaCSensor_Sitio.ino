/* 
  Sensor para Bomba Dagua
 ---------------------------------------------------------------
   Criado em 2013 (V1.0)
   por Joao R. Perin
  --------------------------------------------------------------
   Revisoes:
   - 04/12/2017
   - Joao R. Perin (V2.0)
   - Simplificacao do codigo para ter apenas 1 sensor.
   
   - 03/11/2019
   - Joao R. Perin (V2.0.1)
   - Bomba estava ligando automaticamente, acredito ser
     por falta de PULL_UP no Switch Button e no Sensor
  --------------------------------------------------------------
   Link projeto:
   https://github.com/jrperin/BombaCaixaDaguaComSensor
  --------------------------------------------------------------

    Resumo:
  ----------------------
    Pins usados:
    D13 - Acionamento LEDs - Caixa Cheia
    D12 - Liga/Desliga Rele que aciona a Bomba De Agua
    D11 - Botao de Liga/Desliga Bomba de Agua
    D10 - Sensor de nivel maximo de agua
  
    Funcionamento:
    D10 vai para GND se caixa dagua cheia
    D10 em GND, D13 deverá ir para HIGH para acender os LEDs.
    D10 em +5V (PULL-UP), D13 deverá estar em +5v para desligar os LEDs
  
    D11 (PULL-UP), se for para GND verifica status da bomba:
       - Ligada    - Efetua o desligamento da bomba - D12 vai para GND
       - Desligada - Verifica status de D10:
                     - D10 em GND nao permite o acionamento.
                     - D10 em +5v (pull-up), LIGA a Bomba D12 vai para +5v
                           verifica sempre o Status de D10, para desligar a
                           bomba quando a caixa dagua estiver cheia (D12 = GND)
*/


/*
 * Constantes:
 */
const long    protectionInterval =  2000; // Default system verification
const long    btInterval         =  1000; // Default Button push interval
const long    s1Interval         =  700;  // Default Sensor closed interval

/*
 * Pins:
 */
const int s1       = 10;                  // Sensor nivel maximo dagua
const int l1       = 13;                  // Led indicador de nivel máximo
const int rele     = 12;                  // Rele - Liga/Desliga a bomba
const int pushBt   = 11;                  // Botao (push) de liga/desliga Bomba     

/*
 * Variaveis:
 */
 
/* Variaveis da Bomba                     */
bool           isPumpOn     =  false;  // false = desligada / true = ligada
unsigned long  protectionTime  =  0;  

/* Variaveis do Switch Button             */
int            btReaded     = HIGH;       // HIGH  = nao acionado
bool           btStatus     = false;      // false = nao acionado / true = acionado
bool           btStatusChanged = false;   // true  = process button command
unsigned long  prevBtTime   = 0;  

/* Variaveis do Sensor                    */
int            s1Readed     = HIGH;       //HIGH = nao acionado
bool           s1Status     = false;      // false = nao esta cheio / true = esta cheio
unsigned long  prevS1Time   = 0;  


void setup() {
  pinMode(s1,     INPUT_PULLUP);          // Sensor Nivel Agua
  pinMode(pushBt, INPUT_PULLUP);          // Push button - Liga/Desliga bomba
  
  pinMode(l1,     OUTPUT);                // Acioana LEDs status cheio
  pinMode(rele,   OUTPUT);                // Rele de liga/Desliga bomba

  /* Garantir que os Leds e o Rele estejam desligados */
  digitalWrite(l1, LOW);                
  digitalWrite(rele, LOW);
  

  Serial.begin(115200);
  
  Serial.println("Sensor Bomba Dagua");
  Serial.println("Criado por: Joao R. Perin");
  Serial.println("Versao 2.0.1 - 03/11/2019");
  Serial.println();
  
}

void loop()
{

  
  /*   
   *  Verifica se botao de Ligar/Desligar
   *  a bomba foi precionado
   */
  checkPressedButton();
  
  /*
   * Verifica o status do sensor no deposito
   */
  checkSensorStatus();
  
  /*
   * Verifica se o deposito esta cheio
   *   desliga bomba
   */
  if(s1Status){
    if(isPumpOn){
      turnPumpOff();  
    }
  }
  
  /*
   * Se o botao de ligar/desligar foi acionado (btStatusChanged)
   *   Se status de ligar a bomba
   *     Verifica se o deposito nao está cheio !s1Status == True
   *       liga a bomba
   *   Senao
   *       desliga a bomba
   */
   
  if (btStatusChanged){
    if(btStatus){
      if(!s1Status){
        Serial.print("s1Status ao ligar a bomba: ");
        Serial.println(s1Status);
        turnPumpOn();
      }else{
        Serial.println("Deposito cheio, comando de ligar a bomba ignorado ...");
        btStatus = false;
        blinkLeds(12, 40);
      }
    }else{
      turnPumpOff();
    }
    btStatusChanged = false;
  }

}


void turnPumpOn(){
  digitalWrite(rele, HIGH);
  isPumpOn = true;
  btStatus = true; // Precisa atualizar o status do botao
  Serial.println("Bomba ligada\n");
}


void turnPumpOff(){
  digitalWrite(rele, LOW);
  isPumpOn = false;
  btStatus = false; // Precisa atualizar o status do botao
  Serial.println("Bomba desligada\n");
}

void checkPressedButton(){
  
  btReaded  = digitalRead(pushBt);
  
  unsigned long currentMillis = millis();
  
  // LOW = Botao pressionado
  if (btReaded == LOW){
    if (currentMillis - prevBtTime > btInterval){
      prevBtTime      = currentMillis;
      btStatus        = !btStatus;
      btStatusChanged = true;
      String btStatusTxt = "desligado";
      if (btStatus){
        btStatusTxt = "ligado";
      }
      Serial.print("btStatus = ");
      Serial.print(btStatus);
      Serial.println("(" + btStatusTxt + ")");
    }      
  }else{
    // Precisa resetar o prevBtTime
    prevBtTime = currentMillis;
  }
  
}

void checkSensorStatus(){
  
  s1Readed  = digitalRead(s1);
  
  unsigned long currentMillis = millis();
  
  // LOW = Sensor fechado
  if (s1Readed == LOW){
    if (currentMillis - prevS1Time > s1Interval){
      prevS1Time  = currentMillis;
      s1Status    = true;
      String s1StatusTxt = "vazio";
      if (s1Status){
        s1StatusTxt = "cheio";
      }
      Serial.print("s1Status = ");
      Serial.print(s1Status);
      Serial.println(" (" + s1StatusTxt + ")");
    }      
  }else{
    // Precisa resetar o prevBtTime
    prevS1Time = currentMillis;
    s1Status = false;
  }

  if(s1Status){
    digitalWrite(l1, HIGH);
  }else{
    digitalWrite(l1, LOW);
  }

}

void blinkLeds(int times, int delayInMiliseconds){

  bool ledStatus = false;

  for (int i = 0; i < times; i ++){
    digitalWrite(l1, ledStatus);
    ledStatus = !ledStatus;
    delay(delayInMiliseconds);
  }

  // Restore LEDs according to sensor status
  if(s1Status){
    digitalWrite(l1, HIGH);
  }else{
    digitalWrite(l1, LOW);
  }
  
}
