#include "arduino_secrets.h"

#include "Wire.h"  // Faz a comunicaÃ§Ã£o I2C
#include <MPU6050_light.h>
#include "MPU6050_data.h"

MPU6050 mpu(Wire);  // Cria um objeto MPU6050 com o parÃ¢metro Wire (comunicaÃ§Ã£o I2C)
unsigned long timer = 0;

void setup() {
  Serial.begin(9600);  // Inicia a comunicaÃ§Ã£o serial com o computador (9600 Ã© a velocidade)
  
  mpu_setCommunication();
  mpu_calibration();
}

void loop() {
  mpu_read();
}