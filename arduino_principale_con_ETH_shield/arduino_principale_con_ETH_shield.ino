/*
  Autore: Diana Bernabei (bernabei.d@gmail.com)
  Data: 03.06.13
  Descrizione: controlla tutti i sensori e i led del modellino per poi inviare i dati alla pagina db_update.php contenuta nel server Raspberry PI
*/

// Librerie utilizzate 
#include <SPI.h>
#include <Ethernet.h>

// Dichiarazioni variabili 

// Arduino Ethernet Shield
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0d, 0x3E, 0xDA };
byte ip[] = {192,168,1,3};     //IP Arduino
byte subnet[] = { 255, 255, 255, 0 };
char Data_RX;        // Stringa di appoggio per la lettura
String msg = "";     //Stringa di comunicazione

// Stringa di appoggio per l'invio dei dati 
String app1 = "";    //Stringa di appoggio x la concatenazioni
String app2 = "";    //SubString
String app = "";     //Stringa inviata

int cmd[10] = { 0 }; //Vettore contenente la posizione del pin all'interno di msg
int cmd2[10] = { 0 };

// Dichiarazione dei pin utilizzati
int PIR = 6;
int DOOR_SENSOR_PIN = 5; // choose the Door_Sensor_Pin
int ACCESSO_PIN = 7;

// Variabili per i vari sensori 
int alarmValue = 0;
int door = 0;             // variable for reading the Door_Sensor_Pin status
int val=0;
int buttonPress=0;
int rilevazione;
int pros=0;
int accesso=0;
int app3;

// Indice per cicli
int i, j;

// Variabile per la scelta nel menù 
int scelta;
//1 accesso
//2 led
//3 prossimità
//4 porta

//----------------------------------------------------------
// creao un oggetto server che rimane in ascolto sulla porta
// specificata
EthernetServer ArduinoServer(80);

//prototipi funzioni
void led();
void porta();
void prossimita();
void invia_risultati(EthernetClient pc_client);

void setup(){
  // Inizializza lo shield con il mac e l'ip  
  Ethernet.begin(mac, ip);
  
  // Inizializza l'oggetto server
  ArduinoServer.begin();
  
  // LED
  for(i = 1; i < 4 ; i++){
     pinMode(i, OUTPUT);
  }
  
  // SENSORE PROSSIMITA
  pinMode(PIR, INPUT);
  pros=0;
  
  // SENSORE MAGNETICO PORTA
  pinMode(DOOR_SENSOR_PIN, INPUT); 
  
  // PIN CHE RICEVE IL SEGNALE DAL SECONDO ARDUINO CHE GESTISCE IL TASTIERINO DI ACCESSO
  pinMode(ACCESSO_PIN, INPUT); 

}

void loop(){
  
    // IMPORTANTE pulisco la variabile msg, cioè la stringa di comunicazione
    msg = "";
    
    EthernetClient pc_client = ArduinoServer.available();
      
    // Controllo se pc_client è true
    if (pc_client != false){
        // Controllo continuamente che il client sia connesso
        while (pc_client.connected()){
            // Controllo se ci sono byte disponibili per la lettura
            if (pc_client.available()){
                // Leggo i byte disponibili provenienti dal client
                Data_RX = pc_client.read();
                msg += Data_RX;
                if (Data_RX == '\n'){
                  // Ricostruisco la stringa ricevuta concatenando i singoli byte
                  
                  for(i=1; i<5; i++){
                    app1 = String(i);
                    app2 = "scelta=";   
                    app2 += app1;              
                    app3=msg.indexOf(app2);
                    if(app3>0) scelta=i;
                  }  
                
                switch(scelta){
                  case 1:{
                    accedi();
                    break;
                  }
                  case 2:{
                    led();
                    break;
                  }
                  case 3:{
                    prossimita();
                    break;
                  }
                  case 4:{
                    porta();
                    break;
                  }
                }
                
                invia_risultati(pc_client);
                // Aspetto 1 ms affinche la risposta giunga al browser del client
                delay(1);
                // Esco dal ciclo while una volta completato l'invio della risposta
                break;
                } 
            }// Chiuso if
        }// Chiuso while
        // Chiudo la connessione
        pc_client.stop();
    }// Chiuso if client == true
    delay(2000);
}// Fine loop


// Funzioni

// Verificare accesso tastierino
void accedi()
{
  app="";
  rilevazione = digitalRead(ACCESSO_PIN); 
  if (rilevazione == HIGH){ accesso=1;}
  else if (rilevazione == LOW){ accesso=0;}

  app="accesso=";
  app+=accesso;
  
}

// Accensione e spegnimento dei led
void led()
{ 
app="";
  for(i = 1; i < 4; i++){
     app1 = String(i);
     app2 = String(app1 + "=ON");                       
     cmd[i] = msg.indexOf(app2);
     app1 = String(i);
     app2 = String(app1 + "=OFF");
     cmd2[i] = msg.indexOf(app2);
   }
   app="";
   app1="";
   app2="";
   //Piloto l'uscita e invio lo stato al browser 
   for(j = 1; j < 4; j++){
     if (cmd[j] > 0){
        digitalWrite(j, HIGH);
        app1 = String(j); 
        app2 = String(app1 + "=ON");       
     }
     if (cmd2[j] > 0){
        digitalWrite(j, LOW);
        app1 = String(j);
        app2 = String(app1 + "=OFF");
     }
     app += app2;
     app2="";    
  }//chiuso for
}

// Verifica sensore di prossimità a infrarosso
void prossimita()
{
  app="";
  rilevazione = digitalRead(PIR); 
  if (rilevazione == HIGH){ pros=1;}
  else if(rilevazione == LOW){ pros=0;}

  app="pros=";
  app+=pros;
}

// Verifico se la porta è aperta o chiusa 
// (se il sensore magnetico fa contatto oppure no)
void porta()
{  
  app="";
  buttonPress = digitalRead(DOOR_SENSOR_PIN); 
 if (buttonPress == 1) {
    door=0;
  }else if (buttonPress == 0){
     door=1;
  }
  
  app="door=";
  app+=door;
   
}   
   
void invia_risultati(EthernetClient pc_client){
      // Attendo che tutti i byte siano letti quando Data_RX contiene il carattere di nuova line capisco tutti i byte sono stati letti
      
      // Invio la risposta al client invio lo status code
      pc_client.println("HTTP/1.1 200 OK");
      
      // imposto il data type
      pc_client.println("Content-Type: text/html");
      pc_client.println();
      
      // Vengono spediti i valori di stato al database attraverso una pagina Web update.php         
      pc_client.print("<html><head>"); 
      
      // Cambiare ip di raspberry 
      pc_client.print("<meta http-equiv=\"refresh\" content=\"0; url=http://192.168.1.5/db_update.php?");
      
      pc_client.print("scelta="+String(scelta)+"&"+app);
      pc_client.print("\"> ");
      pc_client.print("</head></html>");  
      
      // Aspetto 1 ms affinche la risposta giunga al browser del clients
      delay(1);
}
