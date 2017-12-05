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
  --------------------------------------------------------------
   Link projeto:
   https://github.com/jrperin/SensorNivelAguaMtoMtoSimplificado.git
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
    D10 em GND, D13 deverá ir para GND para acender os LEDs.
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
const boolean debugMode     = true;
const long    sysInterval   =  600;  // Default system verification
const long    btInterval = 1500;     // Defaut Button push interval


/*
 * Pins:
 */
const int s1       = 10;       // Sensor nivel maximo dagua
const int l1       = 13;       // Led indicador de nivel máximo
const int rele     = 12;       // Rele - Liga/Desliga a bomba
const int pushBt   = 11;       // Botao (push) de liga/desliga Bomba     

/*
 * Variaveis:
 */
int   statusBomba  = 0;
int   prevStatusBomba =-1;
int   btStatus     = HIGH;
long  prevBtTime   = 0;  
long  prevSysTime  = 0;  
int   s1Status     = 0;        // Status do sensor 1


void setup() {
  pinMode(s1,     INPUT); // Sensor Nivel Agua
  pinMode(pushBt, INPUT); // Push button - Liga/Desliga bomba
  
  pinMode(l1,     OUTPUT); // Acioana LEDs status cheio
  pinMode(rele,   OUTPUT); // Rele de liga/Desliga bomba

  /* Garantir que os Leds e o Rele estejam desligados */
  digitalWrite(l1, HIGH); // Os leds desligaram quando estiverem em HIGH, (sem conducao para GND)
  digitalWrite(rele, LOW);
  

  Serial.begin(9600);
  
  Serial.println("Sensor Bomba Dagua");
  Serial.println("Criado por: Joao R. Perin");
  Serial.println("Versao 2.0 - 04/12/2017");
  Serial.println();
  
}

void loop()
{

  /* 
   *  Verifica se botao de Ligar/Desligar
   *  a bomba foi precionado
   */
  checkPressedButton();

  unsigned long currentMillis = millis();

  /*
   * Verifica o status da bomba por intervalo de tempo
   */
  if(currentMillis - prevSysTime > sysInterval) {
    
    prevSysTime = currentMillis;   
    
    checkBomba();
    
  }
}

void checkBomba(){

  /* 
   * Se o botao tiver mudado o status para 
   * desligado, precisa desligar a bomba
   * imediatamente
   */
  if(statusBomba == 0){
    digitalWrite(rele, LOW);
    Serial.println("Bomba desligada");
  }
   
  /* Verifica o Status do Sensor*/
  getStatus();
    
  if (debugMode) {printStatus();}
  
  /*
   * s1Status(Pullup) = HIGH(Vazio) / LOW(Cheio)
   */
  if (s1Status == LOW && statusBomba == 1){
    /* Se CHEIO - Forca desligar a bomba */
    statusBomba = 0;
    Serial.println("Reservatorio cheio");
    Serial.println("Nao foi possivel ligar a bomba");
  }

  /*
   * Se reservatorio nao estiver cheio,
   * e o status estiver para ligar a bomba
   * efetua o acionamento do Rele
   */
   if(statusBomba == 1){
    digitalWrite(rele, HIGH);
    Serial.println("Bomba ligada");
   }
  
}

void checkPressedButton(){
  
  btStatus  = digitalRead(pushBt);
  
  unsigned long currentMillis = millis();

  if (btStatus == LOW){
    if (currentMillis - prevBtTime > btInterval){
      prevBtTime = currentMillis;
      statusBomba = !statusBomba;
      Serial.println("========================================");
      Serial.print("Status bomba = ");
      Serial.println(statusBomba);
      Serial.println("========================================");
    }      
  }
  
}

void getStatus(){
  s1Status    = digitalRead(s1); // s1Status(Pullup) = HIGH(Vazio) / LOW(Cheio)
  /*
   * Liga/Desliga os LEDs se o reservatorio estiver cheio/vazio respectivamente 
   */
  if(s1Status == LOW){
    digitalWrite(l1, LOW);
  }else{
    digitalWrite(l1, HIGH);
  }
}

void printStatus(){
  String strAux, strOut;
  strAux = "s1Status = ";
  strOut = strAux + s1Status; 
  Serial.print(strOut);  
  Serial.println();

}
