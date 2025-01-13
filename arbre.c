#include "arbre.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

Arbre *arbre_node_create(const char *word, struct strhash_table *ht) {
    Arbre *node = (Arbre *)malloc(sizeof(Arbre));
    if (!node) {
        perror("Erreur d'allocation mémoire pour un nœud");
        exit(EXIT_FAILURE);
    }

    if (word && ht) {
        if (strlen(word) == 0) {
            node->word = strhash_wordAdd(ht, "");
        } else {
            node->word = strhash_wordAdd(ht, word);
        }
    }

    node->occurrences = 0;
    node->children = tabdyn_create(2, 2);
    return node;
}

static void arbre_node_destroy(Arbre *node) {
    if (!node) return;

    for (size_t i = 0; i < node->children->size; ++i) {
        Arbre *child = tabdyn_get(node->children, i);
        arbre_node_destroy(child);
    }
    tabdyn_destroy(node->children);
    free(node);
}

Arbre *arbre_create(struct strhash_table *ht) {
    Arbre *root = arbre_node_create("", ht);
    if (!root->word) {
        printf("Erreur : impossible d'initialiser la racine avec un mot vide.\n");
        free(root);
        return NULL;
    }
    printf("Arbre créé avec une racine vide\n");
    return root;
}

void arbre_destroy(Arbre *arbre) {
    arbre_node_destroy(arbre);
}

Arbre *arbre_searchOrAddNGram(Arbre *root, struct strhash_table *ht) {
    Arbre *current = root;
    sequence_itStart();

    while (sequence_itHasNext()) {
        const char *word = sequence_itNext();
        printf("Mot récupéré de la séquence : '%s'\n", word);
        
        if (!word || strlen(word) == 0) {
            printf("Erreur : Mot invalide dans la séquence.\n");
            continue;
        }

        int found = 0;
        printf("Recherche ou ajout du mot : '%s'\n", word);

        for (size_t i = 0; i < current->children->size; ++i) {
            Arbre *child = tabdyn_get(current->children, i);
            if (child->word == strhash_wordAdd(ht, word)) {
                current = child;
                found = 1;
                break;
            }
        }

        if (!found) {
            Arbre *new_node = arbre_node_create(word, ht);
            tabdyn_add(current->children, new_node);
            current = new_node;
        }
    }
    return current;
}

/*void arbre_addWord(Arbre *node, const char *word, struct strhash_table *ht) {
    if (!node || !word) return;

    int found = 0;
    printf("Ajout du mot '%s' dans le nœud %p\n", word, (void *)node);

    for (size_t i = 0; i < node->children->size; ++i) {
        Arbre *child = tabdyn_get(node->children, i);
        if (strcmp(child->word, word) == 0) {
            child->occurrences++;
            found = 1;
            printf("Mot trouvé, occurrences de '%s' incrémentées à %d\n", word, child->occurrences);
            break;
        }
    }

    if (!found) {
        Arbre *new_node = arbre_node_create(word, ht);
        new_node->occurrences = 1;
        tabdyn_add(node->children, new_node);
    }
}*/
void arbre_addWord(Arbre *node, const char *word, struct strhash_table *ht) {
    if (!node || !word) return;

    int found = 0;
    printf("Ajout du mot '%s' dans le nœud %p\n", word, (void *)node);

    for (size_t i = 0; i < node->children->size; ++i) {
        Arbre *child = tabdyn_get(node->children, i);
        if (strcmp(child->word, word) == 0) {
            child->occurrences++;
            found = 1;
            printf("Mot trouvé, occurrences de '%s' incrémentées à %d\n", word, child->occurrences);
            break;
        }
    }

    if (!found) {
        Arbre *new_node = arbre_node_create(word, ht);
        //new_node->occurrences = 1;
        tabdyn_add(node->children, new_node);
    }
}

char *arbre_findMostFrequentWord(Arbre *node) {
    if (!node || node->children->size == 0) return NULL;

    Arbre *max_node = NULL;
    printf("Recherche du mot le plus fréquent dans le nœud %p\n", (void *)node);

    for (size_t i = 0; i < node->children->size; ++i) {
        Arbre *child = tabdyn_get(node->children, i);
        printf("Enfant %zu : '%s' avec %d occurrences\n", i, child->word, child->occurrences);

        if (!max_node || child->occurrences > max_node->occurrences) {
            max_node = child;
        }
    }

    if (max_node) {
        printf("Mot le plus fréquent trouvé : '%s' avec %d occurrences\n", max_node->word, max_node->occurrences);
        return max_node->word;
    }
    printf("Aucun mot trouvé dans les enfants du nœud\n");
    return NULL;
}
void arbre_display(Arbre *node, int level) {
    if (!node) return;

    
    for (int i = 0; i < level; i++) {
        printf("  ");
    }

    
    printf("'%s' (%d occurrences)\n", node->word, node->occurrences);

    
    for (size_t i = 0; i < node->children->size; i++) {
        Arbre *child = tabdyn_get(node->children, i);
        arbre_display(child, level + 1);
    }
}
