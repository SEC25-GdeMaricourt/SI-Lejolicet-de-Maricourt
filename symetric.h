#include <sam.h>

#ifndef _SYMETRIC_
#define _SYMETRIC_

void send_time(void);
void get_time(void);

void send_key_challenge(void);
void get_challenge(char *challenge);
void get_digest(char *text);

void send_plain_text(char *text);
void send_auth(char *auth);
void get_plain_text(char *text);
void get_auth(char *auth);

void send_cypher_text_challenge(void);
void get_cypher_text_challenge(char *challenge);
void send_cypher_text(char *text);
void get_cypher_text(char *text);




void compute_auth(char *plain_text,char *auth);
void compute_challenge(char *challenge,char *digest);
void compute_random_challenge(char *random);
void compute_derived_key(char *challenge, char *derived_key);

void initRTC(void);

void initTRNG(void);
uint8_t getRandomByte(void);


#endif