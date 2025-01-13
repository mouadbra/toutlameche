#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "arbre.h"
#include "sequence.h"
#include "hash.h"


void saisirTexte(Arbre *racine, struct strhash_table *ht)
{
    char buffer[1024];
    char *mot;
    Arbre *ngramme;
    sequence_initialize(ht);
    printf("Entrez une phrase (ou appuyez sur TAB pour prédire) : \n");
     int i=0;
    while (1)
    {
        printf("> ");

        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        if (strcmp(buffer, "\t") == 0)
        {

            Arbre *ngramme = arbre_searchOrAddNGram(racine,ht);
            const char *prediction = arbre_findMostFrequentWord(ngramme);

            if (prediction != NULL)
            {
                printf("Prédiction : %s\n", prediction);
            }
            else
            {
                printf("Aucune prédiction disponible.\n");
            }
        }
        else if (strcmp(buffer, "FIN") == 0)
        {
            printf("Saisie termine.\n");
            break;
        }
        else
        {     


            mot = strtok(buffer, " ,.!?;");
            while (mot != NULL)
            {     
                 if(i>=Lg_N_grammes){
                    ngramme = arbre_searchOrAddNGram(racine,ht);
                    printf("ajoute de %s \n", mot);
                    arbre_addWord(ngramme,mot,ht);
                    arbre_display(racine, 0);
                }
                sequence_addWord(mot, ht);
                sequence_progress();
                i++;
                printf("sequence : \n");
                sequence_print();

                mot = strtok(NULL, " ,.!?;");
            }
        }
    }
}
