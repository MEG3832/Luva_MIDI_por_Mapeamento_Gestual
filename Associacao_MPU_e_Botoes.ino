# Código final

#include <BLEMIDI_Transport.h>
#include <hardware/BLEMIDI_ESP32.h>
#include <map>
#include <string>
#include "Wire.h"  // Faz a comunicação I2C
#include <MPU6050_light.h>

BLEMIDI_CREATE_DEFAULT_INSTANCE()

unsigned long timer = 0;
const long PRINT_INTERVAL = 1000; // Intervalo de 1 segundo para o print

//valores das notas
std::map<std::string, int> notas;

//pinos dos botões para as notas básicas:
const int btnPin1 = 27; // Dó
const int btnPin2 = 26; // Ré
const int btnPin3 = 25; // Mi
const int btnPin4 = 33; // Fá

//estados dos botões
int btnState1;
int btnState2;
int btnState3;
int btnState4;

//variaveis que cuidam do debounce
unsigned long lastChangeTime = 0; // Quando o estado "cru" mudou pela última vez
const long DEBOUNCE_DELAY = 20;   // 20ms para estabilizar
int notaTocandoAgora = 0;         // A nota que está ativamente tocando via MIDI
int lastRawNote = 0;          // A última leitura "crua" dos botões
int notaEstavel = 0;              // A nota que está estável (depois do debounce)

//MPU6050
MPU6050 mpu(Wire);
int subirOitava = 0;
int sustenido = 0;

//flag para conexao blutooth
bool isConnected = false;

//gravidade
const float G_TO_MS2 = 9.80665; // Conexao de g para m/s²

// ******************************************************************************************************* //

void inicializar_notas() {
  notas["Do"] = 60;
  notas["Re"] = 62;
  notas["Mi"] = 64;
  notas["Fa"] = 65;
  notas["Sol"] = 67;
  notas["La"] = 69;
  notas["Si"] = 71;
}

void mpu_connection() {
  Wire.begin();
  byte status = mpu.begin();  // Inicia o sensor
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);   // Status 0 indica que o sensor foi encontrado
  while (status != 0) {
    Serial.println("Falha ao iniciar o MPU6050. Verifique as conexões e reinicie.");
  } // Entra em um loop infinito atÃ© que o sensor seja encontrado
}

void mpu_callibration() {
  Serial.println(F("Calibrando, nao mova o mpu"));
  delay(1000);
  mpu.calcOffsets(true, true); // giroscopio and acelerometro
  Serial.println("Calibracao pronta!\n");
  mpu.setAccConfig(2);
  Serial.println("Faixa do acelerômetro configurada para +/- 8g");
}

// Faz as leituras do angulo e da aceleracao e associa as respectivas notas
void mpu_read() {
  mpu.update();   // Le os dados mais recentes do sensor e os armazena dentro de mpu

  // Leitura dos dados específicos
  float AccX = mpu.getAccX() * G_TO_MS2;
  float AngleX = mpu.getAngleX();

  // mudança da oitava pelo angulo
  if (AngleX > 20) {
    subirOitava = 1;
  }
  else if (AngleX < -20) {
    subirOitava = -1;
  }
  else {
    subirOitava = 0;
  }

  // Toca sustenido quando ha uma movimentacao em x
  if (AccX > 3 || AccX < -3) {
    sustenido = 1;
  }
  else {
    sustenido = 0;
  }
}

//função para tocar a nota

void tocaNota ()
{
  btnState1 = digitalRead(btnPin1);
  btnState2 = digitalRead(btnPin2);
  btnState3 = digitalRead(btnPin3);
  btnState4 = digitalRead(btnPin4);

  int rawNote = 0; // Nota "crua" lida neste exato ciclo. 0 = silêncio

  if (btnState1 == 0 && btnState2 == 0)
    rawNote = notas["Sol"] + 12 * subirOitava + sustenido;

  else if (btnState2 == 0 && btnState3 == 0)
    rawNote = notas["La"] + 12 * subirOitava + sustenido;

  else if (btnState3 == 0 && btnState4 == 0)
  {
    rawNote = notas["Si"] + 12 * subirOitava;
  }

  else if (btnState1 == 0)
    rawNote = notas["Do"] + 12 * subirOitava + sustenido;

  else if (btnState2 == 0)
    rawNote = notas["Re"] + 12 * subirOitava + sustenido;

  else if (btnState3 == 0)
    rawNote = notas["Mi"] + 12 * subirOitava;

  else if (btnState4 == 0)
    rawNote = notas["Fa"] + 12 * subirOitava + sustenido;

  // verifica se a leitura "crua" mudou desde a última vez
  if (rawNote != lastRawNote) {
    lastChangeTime = millis(); //zera o cronômetro do debounce...
  }
  lastRawNote = rawNote; // Salve a leitura atual

  // verifica se passaram (20ms) desde a última mudança
  if ((millis() - lastChangeTime) > DEBOUNCE_DELAY)
  {
    notaEstavel = rawNote; //só definimos a nota que vamos tocar se ela ficar mais de 20ms acionada

    if (notaTocandoAgora != notaEstavel)
    {
      if (notaTocandoAgora != 0)
        MIDI.sendNoteOff(notaTocandoAgora, 0, 1);

      if (notaEstavel != 0)
        MIDI.sendNoteOn(notaEstavel, 100, 1);

      notaTocandoAgora = notaEstavel;
    }
  }

}

void interrompe()
{

  //Serial.println("Desconectado.");

  // garantindo que qualquer nota tocando seja interrompida
  if (notaTocandoAgora != 0) { // Use notaTocandoAgora
    MIDI.sendNoteOff(notaTocandoAgora, 0, 1);
    notaTocandoAgora = 0;
  }

  // Resete os estados de debounce também
  lastRawNote = 0;
  notaEstavel = 0;

}

// ******************************************************************************************************* //

void setup()
{
  Serial.begin(115200);

  inicializar_notas();
  pinMode(btnPin1, INPUT_PULLUP);
  pinMode(btnPin2, INPUT_PULLUP);
  pinMode(btnPin3, INPUT_PULLUP);
  pinMode(btnPin4, INPUT_PULLUP);

  //inicia para fazer conexão:

  MIDI.begin();

  //para quando o esp32 se conecta via bluetooth com sucesso...:

  mpu_connection();
  mpu_callibration();

  BLEMIDI.setHandleConnected([]() {
    isConnected = true;
    Serial.println("Conectado!!");
  });

  // e para quando se desconecta...:

  BLEMIDI.setHandleDisconnected([]() {
    isConnected = false;
    Serial.println("Desconectado.");
  });

  //Os dois eventos abaixo são para leitura de notas que são tocadas pelo usuário...

  MIDI.setHandleNoteOn([](byte channel, byte note, byte velocity) {

  });
  MIDI.setHandleNoteOff([](byte channel, byte note, byte velocity) {

  });
}

void loop()
{
  if (isConnected)
  {
    // Mpu6050
    mpu_read();

    // Botão
    tocaNota();

    //imprime a cada 1 segundo
    if (millis() - timer > PRINT_INTERVAL)
    {
      Serial.print("Oitava: ");
      Serial.print(subirOitava);
      Serial.print(" | Sustenido: ");
      Serial.println(sustenido);

      timer = millis(); // Reseta o timer
    }

  }

  else // se desconectado
  {
    //função para parar a nota, caso desconecte e ainda estiver tocando...
    interrompe();
  }
}
