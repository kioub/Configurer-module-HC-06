//     _____________________________________________
//    |                                             |
//    |          Configurer module HC-06            |
//    |                                             |
//    |              Créé  par Kioub                |
//    |               Le 08/01/2017                 |
//    |_____________________________________________|

#include <SoftwareSerial.h>
SoftwareSerial mySerial(10, 11);  // RX, TX
unsigned long vitesseDispo[] = {1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200};
String nom = "";
int vitesse = 0;
String firmware = "";
String codepin = "";

//___________ Setup ______________________________________________________________________________________________________________________________________________________________________________

void setup() {
  int compteur = 0;

  Serial.begin(9600);
  Serial.print("Le programme essaye de se connecter au module... ");

  for (compteur = 0; compteur < 8; compteur++) {
    mySerial.begin(vitesseDispo[compteur]);
    mySerial.print("AT");
    if (readModule() == "OK") {
      vitesse = compteur;
      Serial.println("connexion reussie.");
      Serial.println();
      Serial.print("vitesse de communication : ");
      Serial.print(vitesseDispo[compteur]);
      Serial.println(" bites par seconde");
      Serial.print("Version du firmware : ");
      mySerial.print("AT+VERSION");
      firmware = readModule();
      Serial.println(firmware);
      Serial.println();
      break;
    }
    else if (compteur == 7){
      Serial.println("impossible d'etablir la connexion.");
      Serial.println();
      Serial.println("Veuillez controler que le module est correctement connecte");
      Serial.println("  ____________        ____________");
      Serial.println(" |          Rx|-3,3V-|11          |");
      Serial.println(" |  HC-06   Tx|------|10  Arduino |");
      Serial.println(" |         gnd|------|gnd         |");
      Serial.println(" |_________ 5V|------|5V__________|");
      Serial.println();
      Serial.println("Pour relancer le programme, appuyer sur une touche...");
      while (Serial.available() != 1 ) {
      }
      asm volatile ("  jmp 0");      
    }
  }
}

//___________ Loop _______________________________________________________________________________________________________________________________________________________________________________

void loop() {
  Serial.println("Entrer le nombre correspondant a votre choix");
  Serial.println(" 1 - Changer le nom");
  Serial.println(" 2 - Changer le code PIN");
  Serial.println(" 3 - Changer la vitesse de communication");
  Serial.println(" 4 - Afficher les informations");
  Serial.println("");

  while (Serial.available() != 1 ) {
  }
  
    switch (Serial.read()) {
      case 49 :
        nameModule();
        break;
      case 50 :
        pinModule();
        break;
      case 51 :
        baudModule();
        break;
      case 52 :
        infoModule();
        break;
      default :
        Serial.println("Vous n'avez pas choisi le bon numero");
      break;
  }
}

//___________ Fonction afficher les informations connues du module _______________________________________________________________________________________________________________________________

void infoModule() {
  Serial.print("Version du firmware : ");
  Serial.println(firmware);
  Serial.print("Vitesse de communication : ");
  Serial.print(vitesseDispo[vitesse]);
  Serial.println(" bites par seconde");
  if (nom != ""){
    Serial.println("Nom : "+nom);  
  }
  if (codepin != ""){
    Serial.println("Code PIN : "+codepin);
  }
  Serial.println();
}

//___________ Fonction changer la vitesse de communication _______________________________________________________________________________________________________________________________________

void baudModule() {
  int numeroBaud;
  String texte = "" ;
  Serial.println("Entrer le nombre correspondant a la vitesse souhaitee");
  Serial.println("    1 -   1200 bites par seconde");
  Serial.println("    2 -   2400 bites par seconde");
  Serial.println("    3 -   4800 bites par seconde");
  Serial.println("    4 -   9600 bites par seconde");
  Serial.println("    5 -  19200 bites par seconde");
  Serial.println("    6 -  38400 bites par seconde");
  Serial.println("    7 -  57600 bites par seconde");
  Serial.println("    8 - 115200 bites par seconde");
  Serial.println("");  
  
  Serial.println("Faites votre choix...");
  while (Serial.available() != 1 ) {
  }
  texte = readPC();
  for (int compteur=1 ; compteur < 9; compteur++) {
    if (texte==String(compteur)){
      numeroBaud = compteur-1;     
    }
  }
  mySerial.print("AT+BAUD" + texte);
  delay(500);
  if (readModule() == ("OK" + String(vitesseDispo[numeroBaud]))){
    vitesse = numeroBaud;
    Serial.print("Le module communique maintenant a la vitesse de : ");
    Serial.print(vitesseDispo[numeroBaud]);
    Serial.println(" bites par secondes");
    mySerial.begin(vitesseDispo[numeroBaud]);
    Serial.println("");
  }
  else {
    Serial.println("Il y a eu une erreur, veuillez recommencer.");
    Serial.println("");
  }
}

//___________ Fonction changer le nom du module __________________________________________________________________________________________________________________________________________________

void nameModule() {
  String texte = "" ;
  Serial.println("Entrer un nouveau nom...");
  while (Serial.available() != 1 ) {
  }
  texte = readPC();
  mySerial.print("AT+NAME" + texte);
  delay(10);
  if (readModule() == "OKsetname"){
    nom = texte;
    Serial.println("Le module se nomme maintenant : " + nom);
    Serial.println("");
  }
  else {
    Serial.println("Il y a eu une erreur, veuillez recommencer.");
    Serial.println("");
  }
}

//___________ Fonction changer le code PIN du module _____________________________________________________________________________________________________________________________________________

void pinModule() {
  String texte = "";
  Serial.println("Entrer un nouveau code PIN...");
  while (Serial.available() != 1 ) {
  }
  texte = readPC();
  mySerial.print("AT+PIN" + texte);
  delay(10);
  if (readModule() == "OKsetPIN"){
    codepin = texte;
    Serial.println("Le code PIN est maintenant : " + codepin);
    Serial.println("");
  }
  else {
    Serial.println("Il y a eu une erreur, veuillez recommencer.");
    Serial.println("");
  }
}

//___________ Fonction lecture du moniteur serie _________________________________________________________________________________________________________________________________________________

String readPC() {
  String command = "";
  while(Serial.available() == -1){
  }
  delay(10);
  while (Serial.available()) {
    delay(10);
    command += (char)Serial.read();
  }
//  Serial.print("readPC : ");
//  Serial.println(command);
  return command;
}

//___________ Fonction lecture du module HC-06 ___________________________________________________________________________________________________________________________________________________

String readModule() {
  String command = "";
  while(mySerial.available()) {
  }
  delay(1500);
  while (mySerial.available()) {
    delay(10);
    command += (char)mySerial.read();
  }
//  Serial.print("readModule : ");
//  Serial.println(command);
  return command;
}

