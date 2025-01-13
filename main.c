#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sequence.h"
#include "tab_dyn.h"
#include "arbre.h"
#include "saisie.h"
#include "sauvegarde.h"
#include "chargement.h"
#include <assert.h>


#define MODULE_TABLEAU 0
#define MODULE_SEQUENCE 0
#define MODULE_ARBRE 0
#define MODULE_ES 1



//int argc, char *argv[]
int main(int argc, char *argv[]) {
#if MODULE_TABLEAU
    printf("Tests du module tableau dynamique\n");
    TabDyn *tab = tabdyn_create(5, 2);

    int elem1 = 10, elem2 = 20, elem3 = 30;
    tabdyn_add(tab, &elem1);
    tabdyn_add(tab, &elem2);
    tabdyn_add(tab, &elem3);

    tabdyn_display(tab);

    tabdyn_destroy(tab);

#elif MODULE_SEQUENCE
struct strhash_table *ht = strhash_create(100); 
    

    
    sequence_initialize(ht);
    
    printf("Test de sequence_addWord :\n");
    sequence_addWord("mot1", ht);
    sequence_progress();
    sequence_addWord("mot2", ht);
    sequence_progress();
    sequence_addWord("mot3", ht);
    sequence_progress();
    sequence_addWord("mot4", ht);
    sequence_progress();
    sequence_addWord("mot5", ht);
    sequence_progress();
    sequence_addWord("mot6", ht);arbre_create
    sequence_progress();
    sequence_addWord("mot7", ht);
    sequence_progress();
    
    


    char * mots ;
    mots = sequence_printInTab() ;
    printf("%s \n",mots); 

    if (strcmp(mots,"mot5/mot6/mot7/")==0) {
               printf("test reussi \n") ;

              } else printf("test pas passé \n") ;
    



    printf("--------------------------------------\n");
    printf("--------------------------------------\n");








    printf("test de sequence_itStar et de l'itérateur...\n");
        sequence_itStart();
        sequence_print();

    printf("Test de `sequence_nextWord`...\n");
    const char *nextWord = sequence_nextWord();
    printf("Prochain mot: %s\n", nextWord);
    



   
    strhash_free(ht);
    

#elif MODULE_ARBRE
 
 
printf("Début des tests unitaires pour le module arbre\n");
struct strhash_table *ht = strhash_create(128);
Arbre *arbre = arbre_create(ht);

printf("\nTest 1 : Ajout du N-gramme 'je suis content'\n");
sequence_initialize(ht);
sequence_addWord("je", ht);
sequence_progress();
sequence_addWord("suis", ht);
sequence_progress();
sequence_addWord("content", ht);
sequence_progress();
Arbre *node1 = arbre_searchOrAddNGram(arbre, ht);

printf("\n--- Arbre après ajout du N-gramme 'je suis content' ---\n");
arbre_display(arbre, 0);

printf("\nTest 2 : Ajout des mots suivants pour 'je suis content'\n");
arbre_addWord(node1, "heureux", ht);
arbre_addWord(node1, "triste", ht);
arbre_addWord(node1, "heureux", ht);

printf("\n--- Arbre après ajout des prédictions pour 'je suis content' ---\n");
arbre_display(arbre, 0);

printf("\nRecherche du mot le plus fréquent pour 'je suis content'\n");
char *most_frequent1 = arbre_findMostFrequentWord(node1);
printf("Mot le plus fréquent : %s (Attendu : heureux)\n", most_frequent1);

printf("\nTest 4 : Ajout du mot 'triste' plusieurs fois\n");
arbre_addWord(node1, "triste", ht);
arbre_addWord(node1, "triste", ht);

printf("\n--- Arbre après ajout répété de 'triste' ---\n");
arbre_display(arbre, 0);

printf("\nRecherche du mot le plus fréquent pour 'je suis content'\n");
char *most_frequent2 = arbre_findMostFrequentWord(node1);
printf("Mot le plus fréquent : %s (Attendu : triste)\n", most_frequent2);

printf("\nTest 5 : Ajout du N-gramme 'je mange du'\n");
sequence_addWord("je", ht);
sequence_progress();
sequence_addWord("mange", ht);
sequence_progress();
sequence_addWord("du", ht);
sequence_progress();

Arbre *node2 = arbre_searchOrAddNGram(arbre, ht);

printf("\n--- Arbre après ajout du N-gramme 'je mange du' ---\n");
arbre_display(arbre, 0);

arbre_addWord(node2, "pain", ht);
arbre_addWord(node2, "fromage", ht);
arbre_addWord(node2, "pain", ht);

printf("\n--- Arbre après ajout des prédictions pour 'je mange du' ---\n");
arbre_display(arbre, 0);

printf("\nRecherche du mot le plus fréquent pour 'je mange du'\n");
char *most_frequent3 = arbre_findMostFrequentWord(node2);
printf("Mot le plus fréquent : %s (Attendu : pain)\n", most_frequent3);

arbre_destroy(arbre);
strhash_free(ht);
printf("\nTests terminés avec succès.\n");


#elif MODULE_ES

// Premier test : création et sauvegarde
struct strhash_table *ht = strhash_create(128);
Arbre *arbre = arbre_create(ht);
saisirTexte(arbre, ht);
printf("Sauvegarde de l'arbre dans un fichier...\n");
saveArbreFile(arbre, "arbre_sauv.txt");
printf("Arbre sauvegardé dans 'arbre_sauv.txt'.\n");
arbre_destroy(arbre);
strhash_free(ht);

// Deuxième test : chargement et utilisation
printf("\nChargement de l'arbre depuis le fichier...\n");
struct strhash_table *ht2 = strhash_create(128);  // Nouvelle table de hachage
Arbre *arbre_charge = loadArbreFromFile("arbre_sauv.txt", ht2);

if (arbre_charge) {
    printf("Contenu de l'arbre chargé :\n");
    arbre_display(arbre_charge, 0);
    printf("\nTest des prédictions avec l'arbre chargé :\n");
    saisirTexte(arbre_charge, ht2);
    arbre_destroy(arbre_charge);
}
strhash_free(ht2);

 
    #endif
    return 0;
}
