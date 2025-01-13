#ifndef SAUVEGARDE_H
#define SAUVEGARDE_H

#include <stdio.h>
#include "arbre.h"


void saveArbre(Arbre *arbre, FILE *file, int level);
void saveArbreFile(Arbre *racine, const char *nomFichier);


#endif // SAUVEGARDE_H