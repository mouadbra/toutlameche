#ifndef TAB_DYN_H
#define TAB_DYN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Définition de la structure du tableau dynamique
typedef struct {
    void **tableau;  
    size_t size;     
    size_t sizeMax;
    int facteur ;

} TabDyn;

// Fonction pour créer un tableau dynamique
TabDyn *tabdyn_create(size_t initial_size, int fact);

// Fonction pour détruire le tableau dynamique
void tabdyn_destroy(TabDyn *tab);

// Fonction pour insérer un élément à la fin du tableau
void tabdyn_add(TabDyn *tab, void *element);

// Fonction pour insérer un élément à une position donnée
void tabdyn_insert(TabDyn *tab, size_t position, void *element);

// Fonction pour récupérer un élément à une position donnée
void *tabdyn_get(TabDyn *tab, size_t position);

// Fonction pour écrire un élément à une position donnée
void tabdyn_set(TabDyn *tab, size_t position, void *element);

// Fonction pour réallouer le tableau avec une nouvelle taille
void tabdyn_reallocate(TabDyn *tab, size_t new_size);

// Fonction de recherche d'un élément dans le tableau
int tabdyn_find(TabDyn *tab, void *element, int (*compare)(void *, void *));

#endif
