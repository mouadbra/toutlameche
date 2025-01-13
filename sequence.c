#include "sequence.h"
#include <stdio.h>
#include <string.h>




static char *sequence[Lg_N_grammes+1];
static int pos; //la position ou inserer le nouveau mot(la place vide)
static int posIt;//position de l'iterateur pour qu'il PARCOURS la sequence



void sequence_initialize(struct strhash_table *ht) {
      char * word = strhash_wordAdd(ht, "");
      for (int i = 0; i < Lg_N_grammes + 1; i++) {
      sequence[i] = word;
      if (sequence[i] == NULL) {

            printf("Erreur d'ajout du mot vide à la table de hachage pour sequence[%d]\n", i);
                                }
                                                  }
    //init variables 
    pos = 0;
    posIt = 0; 
                                                   }



/*Dans un tableau circulaire, la position logique de départ est celle juste après le dernier mot inséré 
(pos + 1), car les nouveaux mots écrasent les anciens lorsque le tableau est plein. */
void sequence_itStart(void) {
             if (pos == Lg_N_grammes) {
             // la sequence est pleine(4 cases du tableau sont remplie par la sequence), on repart du début 
             posIt = 0;               }
             else {
             //posIt apres pos
              posIt = pos + 1;
                 }   
                                              
                             }


 //avancer l iterateur et lecture des mot a inserer
 //Récupère le mot à la position actuelle de l'itérateur, puis avance cette position de manière circulaire.

 const char *sequence_itNext(void) {
    
    const char *currentWord = sequence[posIt];
      if (!currentWord) {
        printf("Erreur : Mot nul récupéré dans la séquence.\n");
        return "";
      }
    // Incrementer l'itérateur avec circularité
    posIt = (posIt + 1) % (Lg_N_grammes + 1);
    return currentWord;
}






//si il y a de la place pour inserer dans la sequence
//utilisé dans une boucle qui incremente le posIt
      int sequence_itHasNext(void) {
      return posIt != pos;
                              }


void sequence_addWord(const char *word, struct strhash_table *ht) {
                      if (!word || strlen(word) == 0) {
        printf("Erreur : Tentative d'ajout d'un mot invalide dans la séquence.\n");
        return;
    }
                  printf("Ajout du mot à la séquence : %s\n", word);
                 char *new_word = strhash_wordAdd(ht, word);
                 //contenu du ptr
                  sequence[pos] = new_word;

                 
                                                                  }

//ecriture(entré): nv mot
                const char *sequence_nextWord(void) {
                          return sequence[pos];
                                                    }

//progress
     void sequence_progress(void) {
     pos = (pos + 1) % (Lg_N_grammes + 1);
                              }


void sequence_print(void) {
  //init iterateur
        sequence_itStart();
        while (sequence_itHasNext()) {
        const char *mot = sequence_itNext();
        printf("%s\n", mot);
                                     }
                           }




char * sequence_printInTab(void) {
    static char result[300];
    
    sequence_itStart();
    while (sequence_itHasNext()) {
        const char *mot = sequence_itNext();
        strcat(result, mot);
        strcat(result, "/");

    }

    return result; 
}

                        