#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "arbre.h"

// Fonction auxiliaire pour lire une ligne et extraire les informations
static void parseLine(char *line, char **word, int *occurrences, int *isLeaf, int *isEmpty) {
    *word = NULL;
    *occurrences = 0;
    *isLeaf = 0;
    *isEmpty = 0;
    
    // Supprime les espaces au début
    while (*line == ' ' || *line == '\t') line++;
    
    if (strstr(line, "#empty#") != NULL) {
        *isEmpty = 1;
        return;
    }
    
    if (strstr(line, "-->") != NULL) {
        *isLeaf = 1;
        sscanf(line, "[ p=%d --> %ms ]", occurrences, word);
    } else {
        sscanf(line, "[ %ms", word);
        if (*word) {
            // Enlever le dernier caractère si c'est un ']'
            size_t len = strlen(*word);
            if (len > 0 && (*word)[len-1] == ']') {
                (*word)[len-1] = '\0';
            }
        }
    }
}

// Fonction pour compter le niveau d'indentation
static int countIndentation(const char *line) {
    int count = 0;
    while (*line == '\t') {
        count++;
        line++;
    }
    return count;
}

Arbre *loadArbre(FILE *file, struct strhash_table *ht) {
    char *line = NULL;
    size_t len = 0;
    size_t read;
    
    // Lire la première ligne pour créer la racine
    read = getline(&line, &len, file);
    if (read == -1) {
        free(line);
        return NULL;
    }
    
    Arbre *root = arbre_create(ht);
    Arbre *current = root;
    int currentLevel = 0;
    
    // Pile pour garder trace des parents
    Arbre **stack = malloc(1000 * sizeof(Arbre *));
    int stackTop = 0;
    stack[stackTop++] = root;
    
    while ((read = getline(&line, &len, file)) != -1) {
        char *word;
        int occurrences, isLeaf, isEmpty;
        int level = countIndentation(line);
        
        if (strstr(line, "]") != NULL && strlen(line) <= level + 3) {
            // Ligne de fermeture
            if (stackTop > 0) stackTop--;
            continue;
        }
        
        parseLine(line, &word, &occurrences, &isLeaf, &isEmpty);
        
        if (isEmpty) continue;
        
        if (word) {
            // Ajuster le niveau de pile selon l'indentation
            while (stackTop > level) stackTop--;
            
            // Créer le nouveau nœud
            Arbre *newNode = arbre_node_create(word, ht);
            if (isLeaf) {
                newNode->occurrences = occurrences;
            }
            
            // L'ajouter comme enfant du nœud parent
            if (stackTop > 0) {
                tabdyn_add(stack[stackTop-1]->children, newNode);
            }
            
            // Empiler le nouveau nœud
            stack[stackTop++] = newNode;
            
            free(word);
        }
    }
    
    free(stack);
    free(line);
    return root;
}

Arbre* loadArbreFromFile(const char *filename, struct strhash_table *ht) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Erreur : Impossible d'ouvrir le fichier %s\n", filename);
        return NULL;
    }
    
    Arbre *arbre = loadArbre(file, ht);
    if (arbre) {
        printf("Arbre chargé avec succès depuis %s\n", filename);
    }
    
    fclose(file);
    return arbre;  // Retourne l'arbre chargé
}