#include "MPU6050_data.h"

const float G_TO_MS2 = 9.80665; // Conersão de g para m/s²

void mpu_calibration() {
  Serial.println(F("Calibrando, nao mova o mpu"));
  delay(1000);
  mpu.calcOffsets(true,true); // giroscÃ³pio and acelerÃ´metro
  Serial.println("Done!\n");

  mpu.setAccConfig(2);
  Serial.println("Faixa do acelerômetro configurada para +/- 8g");
}

void mpu_setCommunication() { 
  Wire.begin();  
  byte status = mpu.begin();  // Inicia o sensor
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);  // Status 0 indica que o sensor foi encontrado
  Serial.println("Falha ao iniciar o MPU6050. Verifique as conexões e reinicie.");
  while(status!=0){} // Entra em um loop infinito atÃ© que o sensor seja encontrado
}

void mpu_read() {
  mpu.update();  // LÃª os dados mais recentes do sensor e os armazena dentro de mpu

  // Imprime os dados a cada segundo
  if(millis() - timer > 1000){ // milis() retorna o tempo desde o iní­cio da execução do código

    float AccX = mpu.getAccX() * G_TO_MS2;
    Serial.print(F("ACELERACAO  X: "));
    Serial.print(AccX);
    float AccY = mpu.getAccY() * G_TO_MS2;
    Serial.print("\tY: ");
    Serial.print(AccY);
    float AccZ = mpu.getAccZ() * G_TO_MS2;
    Serial.print("\tZ: ");
    Serial.println(AccZ);  // println pula linha
    
    float AngleX = mpu.getAngleX();
    Serial.print(F("ANGULO  X: "));
    Serial.print(AngleX);
    float AngleY = mpu.getAngleY();
    Serial.print("\tY: ");
    Serial.print(AngleY);
    float AngleZ = mpu.getAngleZ();
    Serial.print("\tZ: ");
    Serial.println(AngleZ);
    
    Serial.println(F("=====================================================\n"));

    if(AngleX > 0) {
      digitalWrite(RED_BUILTIN, HIGH);
      digitalWrite(GREEN_BUILTIN, LOW);
    }
    else {
      digitalWrite(GREEN_BUILTIN, HIGH);
      digitalWrite(RED_BUILTIN, LOW);
    }

  if(AccX > 5 || AccX < -5) digitalWrite(YELLOW_BUILTIN, HIGH);

  else digitalWrite(YELLOW_BUILTIN, LOW);

    timer = millis();  // Atualiza o tempo
  }
}