#include <stdio.h>
#include <string.h>
#include "arbre.h"
#include "sauvegarde.h"




void saveArbre(Arbre *arbre, FILE *file, int level)
{
    if (!arbre)
        return;

    for (int i = 0; i < level; i++)
    {
        fprintf(file, "\t");
    }

    if (arbre->word && strlen(arbre->word) > 0 && arbre->children->size == 0)
    {
        fprintf(file, "[ p=%d --> %s ]\n", arbre->occurrences, arbre->word);
    }
    else if (strlen(arbre->word) == 0)
    {
        fprintf(file, "[ #empty# \n");
    }
    else
    {
        fprintf(file, " [ %s \n", arbre->word);
    }

    if (arbre->children)
    {
        for (size_t i = 0; i < arbre->children->size; i++)
        {
            saveArbre((Arbre *)arbre->children->tableau[i], file, level + 1);
            
            

        }
    }


    if (arbre->children->size > 0){

            fprintf(file, "\n");
            for (int i = level; i >0 ; i--)
            {
                fprintf(file, "\t");
            }
            fprintf(file, "] \n");}


}





void saveArbreFile(Arbre *racine, const char *filename)
{
    FILE *file = fopen(filename, "a");
    if (!file)
    {
        return;
    }
    saveArbre(racine, file, 0);
    fclose(file);
}
