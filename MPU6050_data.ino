#include "MPU6050_data.h"

void mpu_calibration() {
  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  mpu.calcOffsets(true,true); // giroscópio and acelerômetro
  Serial.println("Done!\n");
}

void mpu_setCommunication() {
  
  Wire.begin();  
  byte status = mpu.begin();  // Inicia o sensor
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);  // Status 0 indica que o sensor foi encontrado
    int tentativas = 0;
  while (status != 0 && tentativas < 10) {  // tenta no máximo 10 vezes
    Serial.println("MPU6050 nao encontrado, tentando novamente...");
    delay(500);  // espera meio segundo
    status = mpu.begin();  // tenta de novo
    tentativas++;
  }

  if (status == 0) {
    Serial.println("MPU6050 encontrado com sucesso!");
  } else {
    Serial.println("Falha ao inicializar MPU6050. Continuando sem sensor..."); // Entra em um loop infinito até que o sensor seja encontrado
  }
}

void mpu_read() {
  mpu.update();  // Lê os dados mais recentes do sensor e os armazena dentro de mpu
  
  // Coleta os dados referentes ao eixo x
  float AccX = mpu.getAccX() * G_TO_MS2;  
  float AngleX = mpu.getAngleX();

  // Movimentos do pulso para mudança de oitava
  if(AngleX > 0) {
    // MIDI.sendNoteOn(60, 100, 1)
    Oitavas oitava = C1;
  }
  else {
    // MIDI.sendNoteOn(120, 100, 1)
    Oitavas oitava = C2;
  }

  // Movimento horizontal para ativação de sustenido
  if(AccX > 5 || AccX < -5) {
    // MIDI.sendNoteOn(30, 100, 1)
    sustenido = 1;
  }
  else {
    sustenido = 0;
  }

  // Soma a oitava e o sustenido (se não tiver, soma 0)
  MIDI.sendNoteOn(notes["C4"] + SUBIR_OITAVA*oitava + sustenido, 100, 1);
}
