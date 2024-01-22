#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sam.h>

#include "symetric.h"
#include "aes-lib.h"
#include "sha256-lib.h"
#include "usart_sercom2.h"
#include "usart_sercom1.h"
#include "color.h"

static uint8_t random[16]                = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static uint8_t random_challenge_send[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static uint8_t random_challenge_get[16]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static char plain_text[USART_SERCOM2_SIZE<<1];

// 128bit key = 2b7e151628aed2a6abf7158809cf4f3c
static const uint8_t key[16]        = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
static  uint8_t derived_key_send[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static uint8_t  derived_key_get[16]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static uint8_t iv[]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };


void send_key_challenge(void)
{
  char message[17];
  message[0]='k';
  // Generate Random KEY
  compute_random_challenge(&message[1]);
  
  // send the challenge
  sercom1_write(message,17);
}

void get_challenge(char *challenge)
{
  char message[33];
  uint8_t digest[32];
  for(uint32_t i =0; i<16;i++) {
     random_challenge_get[i]=challenge[i];
  }
  compute_challenge(challenge,digest);

  // Create Digest Message to be sent
  message[0]='d';
  for(uint32_t i =0; i<32;i++) {
    message[i+1]=digest[i];
    
  }

  // send the challenge
  sercom1_write(message,33);
}

void send_time(void)
{
  char message[1]={'T'};
  uint32_t clock = RTC_REGS->MODE2.RTC_CLOCK;

  // TODO : Get value from clock
  uint32_t second = (RTC_MODE2_CLOCK_SECOND_Msk & clock) >> RTC_MODE2_CLOCK_SECOND_Pos;
  uint32_t minute = (RTC_MODE2_CLOCK_MINUTE_Msk & clock) >> RTC_MODE2_CLOCK_MINUTE_Pos;;
  uint32_t hour   = (RTC_MODE2_CLOCK_HOUR_Msk & clock) >> RTC_MODE2_CLOCK_HOUR_Pos;;
  uint32_t day    = (RTC_MODE2_CLOCK_DAY_Msk & clock) >> RTC_MODE2_CLOCK_DAY_Pos;;
  uint32_t month  = (RTC_MODE2_CLOCK_MONTH_Msk & clock) >> RTC_MODE2_CLOCK_MONTH_Pos;;
  uint32_t year   = (RTC_MODE2_CLOCK_YEAR_Msk & clock) >> RTC_MODE2_CLOCK_YEAR_Pos;;

  printf(YELLOW_F">Time: %02d/%02d/%04d  %02dh%02dm%02ds\r\n"DEFAULT_COLOR, day,month,year,hour,minute,second);
  sercom1_write(message,1);
}

void get_time(void)
{
  printf(PURPLE_F">Time: 0x%08X\r\n"DEFAULT_COLOR, RTC_REGS->MODE2.RTC_CLOCK);
}

void send_cypher_text_challenge(void)
{
  char message[17];
  message[0]='c';
  // Generate Random KEY
  compute_random_challenge(&message[1]);
  (random_challenge_send,derived_key_send);
  // send the challenge
  sercom1_write(message,17);
}

void get_cypher_text_challenge(char *challenge)
{
  for(uint32_t i =0; i<16;i++) {
     random_challenge_get[i]=challenge[i];
  }
  compute_derived_key(random_challenge_get,derived_key_get);
}


void send_cypher_text(char *text)
{
  struct AES_ctx ctx;
  SHA256_CTX stx;
  char message[USART_SERCOM1_SIZE];
  uint32_t index = 0;
  uint32_t size = strlen(text);

  // Copie Message et PADDING avec 0
  // La taille du message doit être size % 16 = 0, en d'autres termes, la taille doit être un multiple de 16
  // TODO: 1 Padding du message avec des Zéro

  int padding = 16 - (size % 16);
  memcpy(message + 1, text, size);  // Copie le texte à partir de la deuxième position
  memset(message + size + 1, 0, padding);  // Ajout de zéros pour le bourrage

  // Ajouter des octets de bourrage 0x00 si nécessaire
  memset(message + size + padding + 1, 0, padding);

  // Initialiser le contexte AES avec la clé dérivée
  AES_init_ctx_iv(&ctx, derived_key_send, iv );

  // TODO: 2 Crypter en AES_CBC chaque tranche de 16 Octets du message avec la clé
   AES_CBC_encrypt_buffer(&ctx, message + 1 , 16);

  // Envoyer le texte crypté
  message[0] = 'x';  // Ajouter 'x' au début du tableau
  sercom1_write(message, 16 + 1);
}

void get_cypher_text(char *text)
{
    struct AES_ctx ctx;
    uint32_t index = strlen(text);
    uint32_t size = strlen(text);

    // Initialiser le contexte AES avec la clé dérivée
    AES_init_ctx_iv(&ctx, derived_key_get,iv);

    // TODO: Déchiffrer le message avec la clé dérivée
  
    AES_CBC_decrypt_buffer(&ctx, (uint8_t *)(text), 16);

    printf(PURPLE_F">Decypher Text: %s\r\n"DEFAULT_COLOR, text);
}

void send_plain_text(char *text)
{
  sercom1_write(text,strlen(text));
}

void  send_auth(char *text)
{
  char auth[33];
  uint8_t digest[32];
  compute_auth(text,digest);
  // Create Digest Message to be sent
  auth[0] = 'a';
  for (uint32_t i = 0; i < 32; i++) {
    auth[i + 1] = digest[i];
  }

  // send the challenge
  sercom1_write(auth, 33);
}

void  get_auth(char *auth)
{
  uint8_t digest[32];
  compute_auth(plain_text,digest);
  // Compare the Challange
  if (0 == memcmp(digest, auth, SHA256_BLOCK_SIZE)) {
    printf(GREEN_F"Message Auhentifcation SUCCESS!\r\n"DEFAULT_COLOR);
  } else {
    printf(RED_F"Message  Auhentifcation FAILURE!\r\n"DEFAULT_COLOR);
  }
}

void get_plain_text(char *text)
{
  
  for(uint32_t i =0; (text[i]!='\0'||i<(USART_SERCOM2_SIZE<<1));i++) {
    //Store the Message For Futur Use
    plain_text[i]=text[i];
  }
  printf(PURPLE_F">Plain Text: %s\r\n"DEFAULT_COLOR, plain_text);

}

void get_digest(char *text)
{
   uint8_t digest[32];

   compute_challenge(random_challenge_send,digest);

   // Compare the Challange
   if (0 == memcmp(digest, text, SHA256_BLOCK_SIZE)) {
      printf(GREEN_F"Digest SUCCESS!\r\n"DEFAULT_COLOR);
   } else {
      printf(RED_F"Digest FAILURE!\r\n"DEFAULT_COLOR);
   }
}

void compute_auth(char *plain_text,char *auth)
{
  
  struct AES_ctx ctx;
  SHA256_CTX stx;
  char message[USART_SERCOM1_SIZE];
  uint32_t index = 0;
  uint32_t size = strlen(plain_text);

  // Chiffrer le challenge avec la clé en utilisant AES-ECB
  AES_init_ctx(&ctx, key); // Assurez-vous que 'key' est défini quelque part dans votre code

  // Copie Message and PADDING with 0
  // Message size must be size%16=0 in other word size in a multiple of 16
    
  // TODO: 1 Padding du message avec des Zéro
  int padding = 16 - (size % 16);
  memcpy(message, plain_text, size);
  memset(message + size, 0, padding);
  
  // TODO: 2 Crypter en AEC_ECB chaque trançon de 16 Octets du message avec la clef
  while(index < size + padding){
    AES_ECB_encrypt(&ctx, message + index);
    index = index + 16;
    }
 
  // TODO: 3 Calculer auth en calculant le SHA256 du message
  sha256_init(&stx);
  sha256_update(&stx, message, size + padding); // Utilisez la longueur appropriée
  sha256_final(&stx, auth);
}

void compute_challenge(char *challenge, char *digest)
{
   struct AES_ctx ctx;
   SHA256_CTX stx;

   // 1 - Chiffrer le challenge avec la clé en utilisant AES-ECB
   AES_init_ctx(&ctx, key); // Assurez-vous que 'key' est défini quelque part dans votre code
   AES_ECB_encrypt(&ctx, challenge);

   // 2 - Calculer le SHA-256 du message chiffré pour obtenir le digest
   sha256_init(&stx);
   sha256_update(&stx, (const uint8_t *)challenge, AES_BLOCKLEN); // Utilisez la longueur appropriée
   sha256_final(&stx, digest);
}

void compute_random_challenge(char *random)
{
  for(uint32_t i = 0; i < 16; i++) {
    // Générer un octet aléatoire en utilisant la fonction getRandomByte
    random[i] = getRandomByte();
    random_challenge_send[i]= random[i];
  }
}

void compute_derived_key(char *challenge, char *derived_key)
{
    struct AES_ctx ctx;
    for(uint32_t i =0; i<16;i++) {
      derived_key[i]=challenge[i];
    }
    //TODO : Crypt in AES_ECB challenge with key

    // Initialiser le contexte AES avec la clé principale
    AES_init_ctx(&ctx, derived_key);

    // Crypter le challenge en utilisant AES_ECB
    AES_ECB_encrypt(&ctx, derived_key);

}

void initRTC(void)
{
  // Codage du démrarage du RTC en mode Clock
  // Vérifiez si le RTC n'est pas déjé démarré
  // S'il n'est pas démarré, démarrer le RTC  
  if((RTC_REGS->MODE2.RTC_CTRLA&RTC_MODE2_CTRLA_ENABLE_Msk)!=RTC_MODE2_CTRLA_ENABLE_Msk) {
     OSC32KCTRL_REGS->OSC32KCTRL_OSCULP32K = OSC32KCTRL_OSCULP32K_EN1K_Msk | OSC32KCTRL_OSCULP32K_WRTLOCK_Msk ;
     OSC32KCTRL_REGS->OSC32KCTRL_RTCCTRL   = OSC32KCTRL_RTCCTRL_RTCSEL_ULP1K;
  
     RTC_REGS->MODE2.RTC_CTRLA = RTC_MODE2_CTRLA_ENABLE(1)    |
                                 RTC_MODE2_CTRLA_MODE(2)      |
                                 RTC_MODE2_CTRLA_CLOCKSYNC(1) |
                                 RTC_MODE2_CTRLA_PRESCALER(RTC_MODE2_CTRLA_PRESCALER_DIV1024_Val);
     while(RTC_REGS->MODE2.RTC_SYNCBUSY);
  }
}

void initTRNG(void)
{
  // TODO : Codage du démarrage du TRNG
  // Vérifiez si le TRNG n'est pas déjà démarré
  MCLK_REGS->MCLK_APBCMASK |= MCLK_APBCMASK_TRNG_Msk;
  if ((TRNG_REGS->TRNG_CTRLA & TRNG_CTRLA_ENABLE_Msk) != TRNG_CTRLA_ENABLE_Msk) {
    // S'il n'est pas démarré, démarrer le TRNG
    TRNG_REGS->TRNG_CTRLA |= TRNG_CTRLA_ENABLE_Msk;

    // Attendre que le démarrage soit effectif
    while (!(TRNG_REGS->TRNG_CTRLA & TRNG_CTRLA_ENABLE_Msk));
  }
  
}

uint8_t getRandomByte(void)
{
  // TODO : check if data is ready
  while (!(TRNG_REGS->TRNG_INTFLAG & TRNG_INTFLAG_DATARDY_Msk));
  
  // TODO : return New Data
  return (uint8_t)(TRNG_REGS->TRNG_DATA & 0xFF);
}