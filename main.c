#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sam.h>


#include "setup.h"
#include "aes-lib.h"
#include "sha256-lib.h"
#include "symetric.h"
#include "usart_sercom2.h"
#include "usart_sercom1.h"
#include "color.h"

#define BAUDRATE 9600

void HardFault_Handler(void)

{
  #define HARDFAULT_MESSAGE RED_F"\r\nHARD FAULT Detected\r\n"DEFAULT_COLOR
  sercom2_write(HARDFAULT_MESSAGE,sizeof(HARDFAULT_MESSAGE));
  while(1) {
    __WFI();
  }
}
/******************************************************************************
GCLK0 on PB14 : 120Mhz     from DPLL48
GCLK1 on PB15 : 32,768Khz  from External 32.768kHz Crystal (PA00 / PA01) XOSC32K
GCLK2 on PB16 : 12Mhz      External 12MHz Crystal (PB22 / PB23) XOSC1
GCLK3 on PB17 : 8Mhz       120Mhz/15
GCLK4 on PB18 : 1024Hz     from External 32.768kHz Crystal (PA00 / PA01) XOSC32K
GCLK5 on PB19 : 1Mhz       12Mhz/12
******************************************************************************/


/** initialisation(void) *****************************************************/
static void initialisation(void); 

/** main() ********************************************************************/
int main(void)
{
    char message_sercom1[USART_SERCOM1_SIZE];
    char message_sercom2[USART_SERCOM2_SIZE];
    int numChar;

    initialisation();

    sercom2_write(USART_SERCOM2_STARTUP_MESSAGE,sizeof(USART_SERCOM2_STARTUP_MESSAGE));
    printf(">Programme started\r\n");
    while(1) {
        
        // Gestion des messages de Sercom 0
        // On test pour savoir s'il y a un nouveau message
        if (sercom2_isoverflow()!= 0) {
            // On lits le message dans dans un Buffers
            numChar=sercom2_read(message_sercom2);
            // On crit le message sur la console avec le nombre de caratres lut
            printf(">Overflow, Flush\r\n");
        }
        if (sercom2_ismessage() != 0) {
            // On lits le message dans dans un Buffers
            numChar=sercom2_read(message_sercom2);
            // On crit le message sur la console avec le nombre de caratres lut
            if((message_sercom2[0]=='t')||(message_sercom2[0]=='T')) {
              send_time();
            }
            // This is a Key Challange
            else if((message_sercom2[0]=='k')||(message_sercom2[0]=='K')) {
              send_key_challenge();
            }
            //This is a Cypher Message
            else if((message_sercom2[0]=='c')||(message_sercom2[0]=='C')) {
              send_cypher_text_challenge();
              send_cypher_text(&message_sercom2[1]);
            }
            //This is a Plain text Message
            else if((message_sercom2[0]=='p')||(message_sercom2[0]=='P')) {
              send_plain_text(message_sercom2);
              send_auth(&message_sercom2[1]);
            }
            else {
              //Commande de Type inconue
              printf(">%d:%s\r\n", numChar, message_sercom2);
            }

        }



        // Gestion des messages de Sercom 0
        // On test pour savoir s'il y a un nouveau message
        if (sercom1_isoverflow()!= 0) {
            // On lits le message dans dans un Buffers
            numChar=sercom2_read(message_sercom1);
            // On crit le message sur la console avec le nombre de caratres lut
            printf(PURPLE_U">Overflow, Flush\r\n");
        }
        if (sercom1_ismessage() != 0) {
            // On lits le message dans dans un Buffers
            numChar=sercom1_read(message_sercom1);
            // On crit le message sur la console avec le nombre de caratres lut
            printf(PURPLE">%d:%s\r\n"DEFAULT_COLOR, numChar, message_sercom1);
            // decodage du Sercom1 1   
            numChar=sercom1_decode(message_sercom1);

            if((message_sercom1[0]=='t')||(message_sercom1[0]=='T')) {
              get_time();
            }
            // This is a Key Challange
            else if((message_sercom1[0]=='k')||(message_sercom1[0]=='K')) {
                get_challenge(&message_sercom1[1]);
            }
            //This is a Cypher Message
            else if((message_sercom1[0]=='c')||(message_sercom1[0]=='C')) {
                get_cypher_text_challenge(&message_sercom1[1]);
            }
            else if((message_sercom1[0]=='x')||(message_sercom1[0]=='X')) {
                get_cypher_text(&message_sercom1[1]);
            }
            //This is a Plain text Message
            else if((message_sercom1[0]=='p')||(message_sercom1[0]=='P')) {
                get_plain_text(&message_sercom1[1]);
            }
            //This is a Plain text Message
            else if((message_sercom1[0]=='a')||(message_sercom1[0]=='A')) {
                get_auth(&message_sercom1[1]);
            }
            //This is a a Digest
            else if((message_sercom1[0]=='d')||(message_sercom1[0]=='D')) {
                get_digest(&message_sercom1[1]);
            }
            //This is a Crypto hash
            else if((message_sercom1[0]=='h')||(message_sercom1[0]=='H')) {
                get_digest(&message_sercom1[1]);
            }
            else {
                //Commande de Type inconue
                printf(GREEN_F">%d:%s\r\n"DEFAULT_COLOR, numChar, message_sercom1);
            }
        }
    }
    return 0;
    
} // main()

static void initialisation(void)
{
  setup();
  
  // Init RTC
  initRTC();
  
  // Init TRNG
  initTRNG();
  
  // Init Sercom 1
  sercom1_init(BAUDRATE);

  // Init Sercom 2
  sercom2_init(BAUDRATE);
}
