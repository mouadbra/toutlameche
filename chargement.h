#ifndef CHARGEMENT_H
#define CHARGEMENT_H

#include "arbre.h"
#include "hash.h"


Arbre *loadArbre(FILE *file, struct strhash_table *ht);
Arbre *loadArbreFromFile(const char *filename, struct strhash_table *ht);
#endif
