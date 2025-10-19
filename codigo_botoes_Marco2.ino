#include <BLEMIDI_Transport.h>
#include <hardware/BLEMIDI_ESP32.h>

BLEMIDI_CREATE_DEFAULT_INSTANCE()



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

//---------------------------------------------------//

unsigned long lastChangeTime = 0; // Quando o estado "cru" mudou pela última vez
const long DEBOUNCE_DELAY = 20;   // 20ms para estabilizar
int notaTocandoAgora = 0;         // A nota que está ativamente tocando via MIDI
int lastRawNote = 0;           // A última leitura "crua" dos botões
int notaEstavel = 0;              // A nota que está estável (depois do debounce)

//---------------------------------------------------//

//valores das notas (parâmetros para a função sendNote())

const int Do = 60;
const int Re = 62;
const int Mi = 64;
const int Fa = 65;
const int Sol = 67;
const int La = 69;
const int Si = 71;

//booleano para detectar conexão/desconexão

bool isConnected = false;


void setup()
{
  Serial.begin(115200);
  pinMode(btnPin1, INPUT_PULLUP);
  pinMode(btnPin2, INPUT_PULLUP);
  pinMode(btnPin3, INPUT_PULLUP);
  pinMode(btnPin4, INPUT_PULLUP);

  //inicia para fazer conexão:

  MIDI.begin();

  //para quando o esp32 se conecta via bluetooth com sucesso...:

  BLEMIDI.setHandleConnected([]() {
    isConnected = true;
  });

  // e para quando se desconecta...:

  BLEMIDI.setHandleDisconnected([]() {
    isConnected = false;
  });

  //Os dois eventos abaixo são para leitura de notas que são tocadas pelo usuário...

  MIDI.setHandleNoteOn([](byte channel, byte note, byte velocity) {
    
  });
  MIDI.setHandleNoteOff([](byte channel, byte note, byte velocity) {
    
  });
}

// ******************************************************************************************************* //

void loop()
{
  if (isConnected)
  {
    btnState1 = digitalRead(btnPin1);
    btnState2 = digitalRead(btnPin2);
    btnState3 = digitalRead(btnPin3);
    btnState4 = digitalRead(btnPin4);

    int rawNote = 0; // Nota "crua" lida neste exato ciclo. 0 = silêncio

    if (btnState1 == 0 && btnState2 == 0)
      rawNote = Sol;

    else if (btnState2 == 0 && btnState3 == 0)
      rawNote = La;

    else if (btnState3 == 0 && btnState4 == 0)
    {
      rawNote = Si;
    }

    else if (btnState1 == 0)
      rawNote = Do;

    else if (btnState2 == 0)
      rawNote = Re;

    else if (btnState3 == 0)
      rawNote = Mi;

    else if (btnState4 == 0)
      rawNote = Fa;


    // -> lógica do debounce:

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

  else // se desconectado
  {
    Serial.println("Desconectado.");

    // garantindo que qualquer nota tocando seja interrompida
    if (notaTocandoAgora != 0) { // Use notaTocandoAgora
      MIDI.sendNoteOff(notaTocandoAgora, 0, 1);
      notaTocandoAgora = 0;
    }
    
    // Resete os estados de debounce também
    lastRawNote = 0;
    notaEstavel = 0;
  }
}
