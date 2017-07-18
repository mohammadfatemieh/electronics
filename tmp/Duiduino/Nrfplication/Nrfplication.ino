/*
 * Exemple de code pour la bibliothèque Mirf – Serveur d'envoi de texte
 */
#include <SPI.h>      // Pour la communication via le port SPI
#include <Mirf.h>     // Pour la gestion de la communication
#include <nRF24L01.h> // Pour les définitions des registres du nRF24L01
#include <MirfHardwareSpiDriver.h> // Pour la communication SPI


void setup() {
    Serial.begin(9600);
    Mirf.cePin = 9; // Broche CE sur D9
    Mirf.csnPin = 10; // Broche CSN sur D10
    Mirf.spi = &MirfHardwareSpi; // On veut utiliser le port SPI hardware
    Mirf.init(); // Initialise la bibliothèque
    Mirf.channel = 1; // Choix du canal de communication (128 canaux disponibles, de 0 à 127)
    Mirf.payload = 4; // Taille d'un message (maximum 32 octets)
    Mirf.configRegister(RF_SETUP, 0x07); // 1 Mb/s et 0 dBm (puissance maximum)
    Mirf.config(); // Sauvegarde la configuration dans le module radio
    Mirf.setTADDR((byte *) "11111"); // Adresse de transmission
    Mirf.setRADDR((byte *) "22222"); // Adresse de réception
    Serial.println("Go !"); 
}

int       pong = 0x4321;              // contient la valeur à envoyer 
byte      valeur_octet[4];         // contient la valeur découpée en octet pour l’envoi
int       mode = 1;
uint32_t  command = 0;
byte      buf[32];
int       len = 0;

void loop() {
  byte        octet[4];
  int32_t    valeur = 0;
  uint8_t     i = 0;
  static int  n;
  char        tab[7] = "HLTl";

  len = Serial.readBytesUntil('\n', buf, 10);
  buf[len] = '\0';
  command = atol((char *)buf);
  if(Mirf.dataReady())
    {
     Mirf.getData(octet); // Réception du paquet
    valeur = octet[0] + (octet[1] << 8); // transforme en int
    if (valeur == 0x1234)
    {
      Serial.print("PING!... ");
      n = 0;
        while (n < 4)
        {
          if (Mirf.dataReady())
          {
            Mirf.getData(octet); // Réception du paquet
            valeur = octet[0] + (octet[1] << 8); // transforme en int
            if (valeur == 0x1234)
                valeur = -1;
            Serial.print(tab[n]);
            Serial.print(":");
            Serial.print(valeur, DEC);
            Serial.print("||");
            delay(10);
            n++;
          }
        }
      Serial.println(" DONE ---- \\(^-^)/");
    }
    else
    {
      Serial.print("Val : ");
      Serial.print(valeur);
    }
    }
  if (command)
  {
    Serial.print("0x");
    Serial.println(command, HEX);
      valeur_octet[0] = command & 0xFF; // 1er octet 
      valeur_octet[1] = (command & 0xFF00) >> 8; // 2ème octet
      valeur_octet[2] = (command & 0xFF0000) >> 16;
      valeur_octet[3] = (command & 0xFF000000) >> 24;
      Mirf.send(valeur_octet); 
      delay(100);
      while(Mirf.isSending())
      {
        delay(5);
      }
      command = 0; 
  }
}
