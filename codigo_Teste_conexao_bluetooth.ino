// ** Código retirado dos exemplos de uso da biblioteca "BLE-MIDI"
//    (por "lathoub", autor da biblioteca, no GitHub) ** //

#include <BLEMIDI_Transport.h>
#include <hardware/BLEMIDI_ESP32.h>

BLEMIDI_CREATE_DEFAULT_INSTANCE()

unsigned long t0 = millis();
bool isConnected = false;

//Definição da porta p/ acender o LED
#define LED_BUILTIN 22

void setup()
{

  //inicia para fazer conexão:

  MIDI.begin();

  //LED como output e começa desligado:

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  //para quando o esp32 se conecta via bluetooth com sucesso...:

  BLEMIDI.setHandleConnected([]() {
    isConnected = true;
    digitalWrite(LED_BUILTIN, HIGH); //-> liga o LED!
  });

  // e para quando se desconecta...:

  BLEMIDI.setHandleDisconnected([]() {
    isConnected = false;
    digitalWrite(LED_BUILTIN, LOW); // -> desliga o LED!
  });

  //Os dois eventos abaixo são para leitura de notas que são tocadas pelo usuário...

  MIDI.setHandleNoteOn([](byte channel, byte note, byte velocity) {
    digitalWrite(LED_BUILTIN, LOW);
  });
  MIDI.setHandleNoteOff([](byte channel, byte note, byte velocity) {
    digitalWrite(LED_BUILTIN, HIGH);
  });
}

// ******************************************************************************************************* //

void loop()
{
  MIDI.read(); // vê se alguma nota é tocada pelo usuário

  if (isConnected && (millis() - t0) > 1000)
  {
    t0 = millis();

    MIDI.sendNoteOn (60, 100, 1); // toca um Dó continuamente enquanto estiver conectado ao bluetooth...
  }
}