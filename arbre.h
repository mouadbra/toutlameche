#ifndef ARBRE_H
#define ARBRE_H

#include "tab_dyn.h"
#include "sequence.h"
#include "hash.h"

typedef struct Arbre {
    char *word;
    int occurrences;
    TabDyn *children;
} Arbre;

Arbre *arbre_node_create(const char *word, struct strhash_table *ht);
Arbre *arbre_create(struct strhash_table *ht);
void arbre_destroy(Arbre *arbre);
Arbre *arbre_searchOrAddNGram(Arbre *root, struct strhash_table *ht);
void arbre_addWord(Arbre *node, const char *word, struct strhash_table *ht);
char *arbre_findMostFrequentWord(Arbre *node);
void arbre_display(Arbre *node, int level);



#endif // ARBRE_H
