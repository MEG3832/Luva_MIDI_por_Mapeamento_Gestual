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
  mpu.update();  // Le os dados mais recentes do sensor e os armazena dentro de mpu

  // Leitura dos dados específicos
  float AccX = mpu.getAccX() * G_TO_MS2;
  float AngleX = mpu.getAngleX();

  // mudança da oitava pelo angulo
  if(AngleX > 0) {
    subirOitava = 1;
  }
  else {
    subirOitava = 0;
  }

  // Toca sustenido quando ha uma movimentacao em x
  if(AccX > 5 || AccX < -5) {
    sustenido = 1;
  }
  else {
    sustenido = 0;
  }
}
