#include "tab_dyn.h"


TabDyn *tabdyn_create(size_t initial_size, int fact) {
    TabDyn *tab = (TabDyn *)malloc(sizeof(TabDyn));
    if (!tab) {
        return NULL; 
    }
    tab->size = 0;
    tab->sizeMax = initial_size;
    tab->facteur= fact;

    tab->tableau = (void **)malloc(initial_size * sizeof(void *));
    
    if (!tab->tableau) {
        free(tab);
        return NULL ;
    }


   
    
    return tab;
}


void tabdyn_destroy(TabDyn *tab) {
    if (tab) {
        free(tab->tableau); 
        free(tab);          
    }
}


void *tabdyn_get(TabDyn *tab, size_t position) {
    if (position >= tab->size) {
        printf("Position invalide\n");
        return NULL;
    }
    return tab->tableau[position];
}


void tabdyn_set(TabDyn *tab, size_t position, void *element) {
    // si la position dépasse sizeMax
    if (position >= tab->sizeMax) {
        size_t newSizeMax = tab->sizeMax;
        while (position >= newSizeMax) {
            newSizeMax *= tab->facteur;  
        }
        tabdyn_reallocate(tab, newSizeMax);

        
    }

    //si la position dépasse la taille significative (size)
    if (position >= tab->size) {
        //remplir les cases intermediaires avec NULL
       for (size_t i = tab->size; i <= position; i++) {
            tab->tableau[i] = NULL;
        } 
        
        tab->size = position + 1;
    }

    // ajoute ou remplace l'élément à la position donnée
    tab->tableau[position] = element;
}





void tabdyn_add(TabDyn *tab, void *element) {
    if (tab->size >= tab->sizeMax) {
        size_t oldSizeMax = tab->sizeMax;
        tabdyn_reallocate(tab, tab->sizeMax * tab->facteur);

        
        for (size_t i = oldSizeMax; i < tab->sizeMax; i++) {
            tab->tableau[i] = NULL;
        }
    }

    tab->tableau[tab->size] = element;
    tab->size++;
}


void tabdyn_insert(TabDyn *tab, size_t position, void *element) {
    
    if (position >= tab->sizeMax) {
        size_t new_sizeMax = tab->sizeMax;
        while (position >= new_sizeMax) {
            new_sizeMax *= tab->facteur;
        }
        tabdyn_reallocate(tab, new_sizeMax);
        
        


    }


    
    if (position > tab->size) {
        for (size_t i = tab->size; i < position; i++) {
            tab->tableau[i] = NULL; 
        }

        tab->size = position + 1;

    } else 
    
    {
        // insertion dans la zone significatif
        for (size_t i = tab->size; i > position; i--) {
            tab->tableau[i] = tab->tableau[i - 1];
        }

        tab->size++; 
    }

    


    tab->tableau[position] = element;
}






void tabdyn_reallocate(TabDyn *tab, size_t new_size) {
    tab->tableau = (void **)realloc(tab->tableau, new_size * sizeof(void *));
    if (!tab->tableau) {
        perror("Erreur de réallocation du tableau");
        exit(EXIT_FAILURE);
    }
    tab->sizeMax = new_size;
}


int tabdyn_find(TabDyn *tab, void *element, int (*compare)(void *, void *)) {
    for (size_t i = 0; i < tab->size; i++) {
          if (tab->tableau[i] != NULL && compare(tab->tableau[i], element) == 0) {
         return i;
                      }

    }
    return -1;
}



