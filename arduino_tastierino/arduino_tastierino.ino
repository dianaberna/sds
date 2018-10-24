/*
  Autore: Diana Bernabei (bernabei.d@gmail.com)
  Data: 03.06.13
  Descrizione: controlla tutti i sensori e i led del modellino per poi inviare i dati alla pagina db_update.php contenuta nel server Raspberry PI
*/
#include <Keypad.h>

int redPin = 11; //pin per il led rosso
int greenPin = 13; //pin per il led verde
int bluePin = 12; //pin per il led blu

int accesso = 1; //pin di collegamento con l'altro arduino

char* secretCode = "0211*94";
int position = 0;

const byte rows = 4;
const byte cols = 3;
char keys[rows][cols] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte rowPins[rows] = {10,9,8,7};
byte colPins[cols] = {6,5,4};

Keypad keypad = Keypad(makeKeymap(keys), 
                       rowPins, colPins,
                       rows, cols);

void setup()
{
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(accesso, OUTPUT);
  setLocked(true);
}

void loop()
{
  char key = keypad.getKey();
  if (key == '#') {
    position = 0;
    setLocked(true);
  }
  
  if (key == secretCode[position]) {
    position++;
  }
  
  if (position == 7) {
    setLocked(false);
  }
  delay(50);
}

void setLocked(int locked)
{
  if (locked) {
    analogWrite(redPin, 255);
    analogWrite(greenPin, 55);
    analogWrite(bluePin, 155); 
    digitalWrite(accesso,LOW);
  }
  else {
    analogWrite(redPin, 55);
    analogWrite(greenPin, 255);
    analogWrite(bluePin, 155);
    digitalWrite(accesso,HIGH);
  }
}







