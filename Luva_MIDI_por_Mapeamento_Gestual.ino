#include "Wire.h"  // Faz a comunicação I2C
#include <MPU6050_light.h>
#include "MPU6050_data.h"
#include <BLEMIDI_Transport.h>
#include <hardware/BLEMIDI_ESP32.h>
#include <map>
#include <string.h>

BLEMIDI_CREATE_DEFAULT_INSTANCE()

#define SUBIR_OITAVA 12 // Precisa somar 12 para tocar a mesma nota uma oitava acima

int sustenido = 1;  // Precisa somar 1 para tocar o sustenido da respectiva nota
MPU6050 mpu(Wire);  // Cria um objeto MPU6050 com o parâmetro Wire (comunicação I2C)
std::map<string, int> notes;
bool isConnected = false;

// Denifição das notas base
void creatNotes() {
  notes["C1"] = 24;  // Dó
  notes["D1"] = 26;  // Ré
  notes["E1"] = 28;  // Mi
  notes["F1"] = 29;  // Fá
  notes["G1"] = 31; // Sol
  notes["A1"] = 33; // Lá
  notes["B1"] = 35; // Si
}

// enum das oitavas, que podem servir de multiplicador do valor a ser somado (tirei o C0 por isso)
enum Oitavas {
  C1, // 0
  C2, // 1
  C3, // 2
  C4, // 3
  C5, // 4
  C6, // 5
  C7, // 6
  C8, // 7
  C9  // 8
}
// Exemplo: Oitavas minhaOitava = C4; (minhaOitava = 3)

void setup() {
  Serial.begin(9600);  // Inicia a comunicação serial com o computador (9600 é a velocidade)
  
  createNotes();

  // Inicialização do MIDI
  MIDI.begin();
  BLEMIDI.setHandleConnected([]() {
    isConnected = true;
  });
  BLEMIDI.setHandleDisconnected([]() {
    isConnected = false;
  });

  // Inicialização do MPU
  mpu_setCommunication();
  mpu_calibration();
}

void loop() {
  MIDI.read();
  if (isConnected) {
    mpu_read();
  }
}
