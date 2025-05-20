#include "arduino_secrets.h"
/* 
TCC - Smart Plant - Central de automatização de Hortas Residenciais
Nome: Douglas Aparecido dos Santos Melo
Curso: Engenharia de Computação - UNICEP
*/

// Bibliotecas
#include "thingProperties.h"
#include <DHT.h>
#include <DHT_U.h>
#include <LiquidCrystal_I2C.h>

// Definições de pinos de acionamentos e leituras de sensores
const int pinSensorSolo = A0; //Pino do sensor higrômetro (Umidade do solo)
#define pinBomba D5           //Pino relé
#define pinDHT D4             //Pino DHT11
#define DHTTYPE DHT11         //Definição do tipo do sensor de leitura de temperatura e umidade ambiente

// Inicializa o sensor DHT11
DHT dht(pinDHT, DHTTYPE);

// Inicializa o Display
LiquidCrystal_I2C lcd(0x3F, 16, 2); //FUNÇÃO DO TIPO "LiquidCrystal_I2C"D
 

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(pinSensorSolo, INPUT);
  pinMode(pinBomba, OUTPUT);
  
  delay(1500); 

  lcd.init();   // INICIALIZA O DISPLAY LCD
  lcd.backlight(); // HABILITA O BACKLIGHT (LUZ DE FUNDO) 

  initProperties();

  // Conecta ao Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
}

void loop() {
  ArduinoCloud.update(); 

  // Faz a leitura do valor bruto do sensor de umidade do solo
  int valorBrutoSolo = analogRead(pinSensorSolo); 
  // Imprimi no monitor serial o valor bruto de leitura
  Serial.print("Valor bruto da umidade do solo: "); 
  Serial.println(valorBrutoSolo);

  // Converte a leitura analógica (Que vai de 0 - 1023) em uma porcentagem (0 - 100)
  int porcValorSolo = map(valorBrutoSolo, 0, 1023, 100, 0); 
  // Imprimi no monitor Serial o valor transformado em porcentagem
  Serial.print("Valor em porcentagem: ");
  Serial.print(porcValorSolo);
  Serial.println(" %");

  // Atualiza o valor na nuvem
  umidadeSolo = porcValorSolo;

    // Controle da bomba com base no modo
  if (modoManual) {                  // Modo Manual: Controla a bomba com base na atualização da variável "ControleManualBomba"
      
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print("Modo Manual");
      lcd.setCursor(4, 1);
      lcd.print(" Ativado ");
    
    if (controleManualBomba) {
      digitalWrite(pinBomba, HIGH);    // Ligar a bomba
      estadoBomba = true;
    } else {
      digitalWrite(pinBomba, LOW);   // Desligar a bomba
      estadoBomba = false;
    }
  } else {
    if (porcValorSolo < 50) {         // Modo Automático: Controla a bomba com base na atualização da variavel "porcValorSolo"
      
      Serial.println("**** Solo Seco ***********");

      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print("Solo Seco!");
      lcd.setCursor(1, 1);
      lcd.print(" Bomba Ativada ");
      
      digitalWrite(pinBomba, HIGH);    // Ligar a bomba
      estadoBomba = true;
    } else {
      digitalWrite(pinBomba, LOW);  // Desligar a bomba
      estadoBomba = false;
      
      Serial.println("**** Solo Molhado ***********");

      // Lê os valores de temperatura e umidade do DHT11
      float umiAmbiente = dht.readHumidity();
      float tempAmbiente = dht.readTemperature();
      
      lcd.clear();
      lcd.setCursor(0, 0); // (C , L)
      lcd.print("Temp: ");
      lcd.print(tempAmbiente);
      lcd.print(" C");
    
      lcd.setCursor(0, 1);
      lcd.print("Hum: ");
      lcd.print(umiAmbiente);
      lcd.print(" %");
    }
  }

  // Lê os valores de temperatura e umidade do DHT11
  float umiAmbiente = dht.readHumidity();
  float tempAmbiente = dht.readTemperature();
  // Imprimi no monitor serial o valor da temperatura ambiente
  Serial.print("Temperatura ambiente: ");
  Serial.print(tempAmbiente);
  Serial.println(" %");
  // Imprimi no monitor serial o valor da umidade ambiente
  Serial.print("Umidade ambiente: ");
  Serial.print(umiAmbiente);
  Serial.println(" %");

  /* Verifica se a leitura foi bem-sucedida
  if (isnan(umiAmbiente) || isnan(tempAmbiente)) {
  Serial.println("Failed to read from DHT sensor!");
  return;
  } */
  
  // Atualiza o valor na nuvem
  temperaturaAmbiente = tempAmbiente;
  umidadeAmbiente = umiAmbiente;

  delay(1000);
}


void onUmidadeSoloChange()  { // Variavel de atualização na nuvem
}

void onUmidadeAmbienteChange()  { // Variavel de atualização na nuvem
}

void onEstadoBombaChange()  { // Variavel de atualização na nuvem
}

void onModoManualChange()  { // Variavel de atualização na nuvem
}

void onControleManualBombaChange()  {
  modoManual = true; // Atualiza a variavel modoManual = verdadeiro
}

void onTemperaturaAmbienteChange()  { // Variavel de atualização na nuvem
}
