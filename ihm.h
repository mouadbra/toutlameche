#ifndef IHM_H
#define IHM_H

#include <gtk/gtk.h>
#include "arbre.h"
#include "sequence.h"
#include "sauvegarde.h"
#include "chargement.h"
#include "hash.h"

// Structure principale pour les données de l'application
typedef struct {
    GtkWidget *window;
    GtkWidget *textview;
    GtkTextBuffer *buffer;
    GtkWidget *treeview;
    GtkListStore *liststore;
    Arbre *racine;
    struct strhash_table *ht;
    gboolean prediction_enabled;
} AppData;

// Fonctions publiques
void ihm_init(int *argc, char ***argv);
void ihm_run(Arbre *racine, struct strhash_table *ht);
void ihm_cleanup(AppData *app_data);

#endif // IHM_H