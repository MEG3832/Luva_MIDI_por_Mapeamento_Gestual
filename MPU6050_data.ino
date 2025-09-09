#include "MPU6050_data.h"

void mpu_calibration() {
  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  mpu.calcOffsets(true,true); // giroscÃ³pio and acelerÃ´metro
  Serial.println("Done!\n");
}

void mpu_setCommunication() {
  
  Wire.begin();  
  byte status = mpu.begin();  // Inicia o sensor
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);  // Status 0 indica que o sensor foi encontrado
  while(status!=0){ } // Entra em um loop infinito atÃ© que o sensor seja encontrado
}

void mpu_read() {
  mpu.update();  // LÃª os dados mais recentes do sensor e os armazena dentro de mpu
  // Imprime os dados a cada segundo
  if(millis() - timer > 1000){ // milis() retorna o tempo desde o inÃ­cio da execuÃ§Ã£o do cÃ³digo
    Serial.print(F("ACELERAÃÃO  X: "));
    Serial.print(mpu.getAccX());
    Serial.print("\tY: ");
    Serial.print(mpu.getAccY());
    Serial.print("\tZ: ");
    Serial.println(mpu.getAccZ());  // println pula linha
    
    Serial.print(F("ANGULO  X: "));
    Serial.print(mpu.getAngleX());
    Serial.print("\tY: ");
    Serial.print(mpu.getAngleY());
    Serial.print("\tZ: ");
    Serial.println(mpu.getAngleZ());
    
    Serial.println(F("=====================================================\n"));

    timer = millis();  // Atualiza o tempo
  }
}