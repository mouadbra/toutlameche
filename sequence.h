#ifndef SEQUENCE_H
#define SEQUENCE_H

#include "hash.h"


enum {Lg_N_grammes = 3};




void sequence_initialize(struct strhash_table * ht);
void sequence_itStart(void);
const char * sequence_itNext(void);
int sequence_itHasNext(void);
void sequence_addWord(const char * wordi, struct strhash_table *ht);
const char * sequence_nextWord(void);
void sequence_progress(void);
void sequence_print(void);
char * sequence_printInTab(void);

#endif 