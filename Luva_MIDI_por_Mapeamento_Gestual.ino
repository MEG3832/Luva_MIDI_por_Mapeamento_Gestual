#include "arduino_secrets.h"

#include "Wire.h"  // Faz a comunicação I2C
#include <MPU6050_light.h>
#include "MPU6050_data.h"

MPU6050 mpu(Wire);  // Cria um objeto MPU6050 com o parâmetro Wire (comunicação I2C)
unsigned long timer = 0;
#define RED_BUILTIN 27
#define GREEN_BUILTIN 32
#define YELLOW_BUILTIN 33

void setup() {
  Serial.begin(115200);  // Inicia a comunicação serial com o computador

  pinMode(RED_BUILTIN, OUTPUT);
  digitalWrite(RED_BUILTIN, LOW);
  pinMode(GREEN_BUILTIN, OUTPUT);
  digitalWrite(GREEN_BUILTIN, LOW);
  pinMode(YELLOW_BUILTIN, OUTPUT);
  digitalWrite(YELLOW_BUILTIN, LOW);

  mpu_setCommunication();
  mpu_calibration();
}

void loop() {
  mpu_read();
}