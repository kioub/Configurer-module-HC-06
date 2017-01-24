//                           _____________________________________________
//                          |                                             |
//                          |          Configure HC-06 module             |
//                          |                                             |
//                          |            Author : Kioub                  |
//                          |            Create : 08/01/2017              |
//                          |            Update : 24/01/2017              |
//                          |_____________________________________________|
//    ______________________________________________________________________________________________   
//   |                                                                                             |
//   |      Configure the HC-06 Bluetooth module using an Arduino UNO and your serial mobitor.     |
//   |      The sketch find module speed comunication and displays the firmware version.           |
//   |      The Bluetooth module name, PIN  and speed communication can be changed.                |
//   |_____________________________________________________________________________________________|
//    ______________________________________________________________________________________________ 
//   |                                                                                              |
//   |                             ____________        ____________                                 |
//   |                            |          Rx|-3,3V-|11          |                                |
//   |                            |  HC-06   Tx|------|10  Arduino |                                |
//   |                            |         gnd|------|gnd         |                                |
//   |                            |_________ 5V|------|5V__________|                                |
//   |______________________________________________________________________________________________|


//________________________________________________________________________________________________________________________________________________________________________________________________

#include <SoftwareSerial.h>
SoftwareSerial mySerial(10, 11);  // RX, TX (arduino side)
unsigned long baud_list[] = {1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200};
String name = "";
int baud = 0;
String firmware = "";
String pin = "";

//___________ Setup ______________________________________________________________________________________________________________________________________________________________________________

void setup() {
  Serial.begin(9600);
  Serial.print("Le programme essaye de se connecter au module... ");

  for (int baud_nb = 0; baud_nb < 8; baud_nb++) {
    mySerial.begin(baud_list[baud_nb]);
    mySerial.print("AT");
    if (readModule() == "OK") {
      baud = baud_nb;
      Serial.println("connexion reussie.");
      Serial.println();
      Serial.print("vitesse de communication : ");
      Serial.print(baud_list[baud_nb]);
      Serial.println(" bites par seconde");
      Serial.print("Version du firmware : ");
      mySerial.print("AT+VERSION");
      firmware = readModule();
      Serial.println(firmware);
      Serial.println();
      break;
    }
    else if (baud_nb == 7){
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

//___________ Display known module information ___________________________________________________________________________________________________________________________________________________

void infoModule() {
  Serial.print("Version du firmware : ");
  Serial.println(firmware);
  Serial.print("Vitesse de communication : ");
  Serial.print(baud_list[baud]);
  Serial.println(" bites par seconde");
  if (name != ""){
    Serial.println("Nom : "+name);  
  }
  if (pin != ""){
    Serial.println("Code PIN : "+pin);
  }
  Serial.println();
}

//___________ Change HC-06 speed communication ___________________________________________________________________________________________________________________________________________________

void baudModule() {
  int user_choice;
  Serial.println("Entrer le nombre correspondant a la vitesse souhaitee");
  for (int baud_nb=0 ; baud_nb < 8 ; baud_nb++) {
      Serial.print("   ");
      Serial.print(baud_nb + 1);
      Serial.print(" - ");
      Serial.print(baud_list[baud_nb]);
      Serial.println(" bites par seconde");
  } 
  Serial.println("   9 - revenir au menu");  
  Serial.println();
  
  do {
    Serial.println("Faites votre choix...");
    while (Serial.available() != 1 ) {
      }
    user_choice = Serial.read() - 48;
  
    if (user_choice == 9){
      Serial.println();
      return;
    }
  } while ((user_choice < 1) || (user_choice > 8));

  mySerial.print("AT+BAUD" + String(user_choice));
  if (readModule() == ("OK" + String(baud_list[user_choice-1]))){
    baud = user_choice-1;
    Serial.print("Le module communique maintenant a la vitesse de : ");
    Serial.print(baud_list[user_choice-1]);
    Serial.println(" bites par secondes");
    mySerial.begin(baud_list[user_choice-1]);
    Serial.println("");
  }
  else {
    Serial.println("Il y a eu une erreur, veuillez recommencer.");
    Serial.println("");
  }
}

//___________ Change HC-06 name __________________________________________________________________________________________________________________________________________________________________

void nameModule() {
  String user_choice = "" ;
  do {
    Serial.println("Entrer un nouveau nom...");
    while (Serial.available() != 1 ) {
    }
    user_choice = readPC();
    if (user_choice.length() > 10) {
      Serial.println("Le nom du module ne doit pas depasser 20 caracteres !");
      Serial.println();
    }
  } while (user_choice.length() > 10);

  mySerial.print("AT+NAME" + user_choice);
  delay(10);
  if (readModule() == "OKsetname"){
    name = user_choice;
    Serial.println("Le module se nomme maintenant : " + name);
    Serial.println("");
  }
  else {
    Serial.println("Il y a eu une erreur, veuillez recommencer.");
    Serial.println("");
  }
}

//___________ Change HC-06 PIN ___________________________________________________________________________________________________________________________________________________________________

void pinModule() {
  String user_choice = "";
  do {
    Serial.println("Entrer un nouveau code PIN...");
    while (Serial.available() != 1 ) {
    }
    user_choice = readPC();

    if (user_choice.length() != 4 || user_choice.toInt() == 0 && user_choice != "0000") {
      Serial.println("Le code PIN doit etre compose de 4 chiffres");
      Serial.println();
    }
  } while (user_choice.length() != 4 || user_choice.toInt() == 0 && user_choice != "0000");
  
  mySerial.print("AT+PIN" + user_choice);
  delay(10);
  if (readModule() == "OKsetPIN"){
    pin = user_choice;
    Serial.println("Le code PIN est maintenant : " + pin);
    Serial.println();
  }
  else {
    Serial.println("Il y a eu une erreur, veuillez recommencer.");
    Serial.println();
  }
}

//___________ Read serial monitor ________________________________________________________________________________________________________________________________________________________________

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

//___________ Read HC-06 module __________________________________________________________________________________________________________________________________________________________________

String readModule() {
  String command = "";
  while(mySerial.available()) {
  }
  delay(1500);
  while (mySerial.available()) {
//    delay(10);
    command += (char)mySerial.read();
  }
//  Serial.print("readModule : ");
//  Serial.println(command);
  return command;
}

