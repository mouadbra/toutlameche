#include "ihm.h"
#include <string.h>

static void setup_treeview(AppData *app_data);
static char** extract_last_words(GtkTextBuffer *buffer, int *word_count);

// Callback pour la fermeture de la fenêtre
static void on_window_destroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

// Callback pour la mise à jour des prédictions
static void on_text_changed(GtkTextBuffer *buffer, gpointer user_data) {
    AppData *app_data = (AppData *)user_data;
    
    if (!app_data->prediction_enabled) {
        return;
    }
    
    // Obtenir le dernier caractère saisi
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buffer, &end);
    if (!gtk_text_iter_backward_char(&end)) {
        return;  // Le buffer est vide
    }
    
    gunichar last_char = gtk_text_iter_get_char(&end);
    
    // Traiter uniquement lorsque la saisie est terminée par un espace ou une ponctuation
    if (g_unichar_isspace(last_char) || g_unichar_ispunct(last_char)) {
        g_print("\n=== Nouvelle modification du texte ===\n");
        
        int word_count;
        char **last_words = extract_last_words(buffer, &word_count);
        
        g_print("Mots extraits du buffer (%d mots):\n", word_count);
        for (int i = 0; i < word_count; i++) {
            g_print("  Mot %d: %s\n", i + 1, last_words[i]);
        }
        
        // Initialisation de la séquence si nécessaire
        sequence_initialize(app_data->ht);
        
        // Ajouter les mots à la séquence
        for (int i = 0; i < word_count; i++) {
            //g_print("Ajout du mot à la séquence : %s\n", last_words[i]);
            sequence_addWord(last_words[i], app_data->ht);
            sequence_progress();
        }
        
        // Afficher l'état de la séquence
        g_print("\nÉtat de la séquence après mise à jour:\n");
        sequence_print();
        
        // Nettoyer la liste des prédictions
        gtk_list_store_clear(app_data->liststore);
        
        // Vérifier si un N-gramme peut être formé
        if (word_count >= Lg_N_grammes) {
            // Recherche ou ajout du N-gramme dans l'arbre
            g_print("\nÉtat de l'arbre avant recherche du N-gramme:\n");
            arbre_display(app_data->racine, 0);
            
            Arbre *ngramme = arbre_searchOrAddNGram(app_data->racine, app_data->ht);
            
            // Ajouter explicitement le dernier mot dans le N-gramme
            const char *dernier_mot = last_words[word_count - 1];
            g_print("Ajout du mot '%s' dans le N-gramme.\n", dernier_mot);
            arbre_addWord(ngramme, dernier_mot, app_data->ht);
            
            // Rechercher la prédiction
            const char *prediction = arbre_findMostFrequentWord(ngramme);
            
            // Afficher la prédiction
            g_print("\nPrédiction trouvée : %s\n", prediction ? prediction : "aucune");
            
            if (prediction != NULL) {
                GtkTreeIter iter;
                gtk_list_store_append(app_data->liststore, &iter);
                gtk_list_store_set(app_data->liststore, &iter,
                                 0, prediction,
                                 -1);
            }
            
            // Afficher l'état de l'arbre après mise à jour
            g_print("\nÉtat de l'arbre après mise à jour:\n");
            arbre_display(app_data->racine, 0);
        } else {
            g_print("\nPas assez de mots pour former un N-gramme (%d/%d)\n", 
                    word_count, Lg_N_grammes);
        }
        
        // Libérer la mémoire allouée
        for (int i = 0; i < word_count; i++) {
            g_free(last_words[i]);
        }
        g_free(last_words);
        
        g_print("\n=== Fin du traitement ===\n\n");
    }
}
// Callback pour l'insertion d'une prédiction sélectionnée
static void on_prediction_selected(GtkTreeView *treeview, 
    GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data) {
    AppData *app_data = (AppData *)user_data;
    
    GtkTreeIter iter;
    GtkTreeModel *model = gtk_tree_view_get_model(treeview);
    if (gtk_tree_model_get_iter(model, &iter, path)) {
        char *word;
        gtk_tree_model_get(model, &iter, 0, &word, -1);
        
        app_data->prediction_enabled = FALSE;
        gtk_text_buffer_insert_at_cursor(app_data->buffer, word, -1);
        gtk_text_buffer_insert_at_cursor(app_data->buffer, " ", -1);
        app_data->prediction_enabled = TRUE;
        
        g_free(word);
    }
}

// Configuration du TreeView
static void setup_treeview(AppData *app_data) {
    app_data->liststore = gtk_list_store_new(1, G_TYPE_STRING);
    gtk_tree_view_set_model(GTK_TREE_VIEW(app_data->treeview), 
        GTK_TREE_MODEL(app_data->liststore));
    
    GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(
        "Prédictions", gtk_cell_renderer_text_new(), "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(app_data->treeview), column);
}

static char** extract_last_words(GtkTextBuffer *buffer, int *word_count) {
    GtkTextIter start, end;
    char *text, **words;
    
   
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
    
   
    words = g_malloc(sizeof(char*) * Lg_N_grammes);
    *word_count = 0;
    
    
    char *text_copy = g_strdup(text);
    char *saveptr;  
    
    
    char *token = strtok_r(text_copy, " \t\n.,;:!?\"()[]", &saveptr);
    
    
    char *temp_words[100];  //100 mots
    int temp_count = 0;
    
    while (token != NULL && temp_count < 100) {
        temp_words[temp_count++] = g_strdup(token);
        token = strtok_r(NULL, " \t\n.,;:!?\"()[]", &saveptr);
    }
    
    
    int start_idx = (temp_count > Lg_N_grammes) ? (temp_count - Lg_N_grammes) : 0;
    *word_count = 0;
    
    for (int i = start_idx; i < temp_count; i++) {
        words[*word_count] = g_strdup(temp_words[i]);
        (*word_count)++;
    }
    
  
    for (int i = 0; i < temp_count; i++) {
        g_free(temp_words[i]);
    }
    g_free(text_copy);
    g_free(text);
    
    return words;
}

static void on_save_document_clicked(GtkMenuItem *menuitem, gpointer user_data) {
    AppData *app_data = (AppData *)user_data;
    
    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Sauvegarder le document",
        GTK_WINDOW(app_data->window),
        GTK_FILE_CHOOSER_ACTION_SAVE,
        "_Annuler", GTK_RESPONSE_CANCEL,
        "_Enregistrer", GTK_RESPONSE_ACCEPT,
        NULL
    );
    
   
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);
    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "document.txt");
    
   
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Fichiers texte");
    gtk_file_filter_add_pattern(filter, "*.txt");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
    
   
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        
        
        GtkTextIter start, end;
        gtk_text_buffer_get_bounds(app_data->buffer, &start, &end);
        char *text = gtk_text_buffer_get_text(app_data->buffer, &start, &end, FALSE);
        
      
        FILE *file = fopen(filename, "w");
        if (file != NULL) {
            fputs(text, file);
            fclose(file);
            
         
            GtkWidget *message = gtk_message_dialog_new(
                GTK_WINDOW(app_data->window),
                GTK_DIALOG_DESTROY_WITH_PARENT,
                GTK_MESSAGE_INFO,
                GTK_BUTTONS_OK,
                "Le document a été sauvegardé avec succès dans %s",
                filename
            );
            gtk_dialog_run(GTK_DIALOG(message));
            gtk_widget_destroy(message);
        } else {
          
            GtkWidget *error = gtk_message_dialog_new(
                GTK_WINDOW(app_data->window),
                GTK_DIALOG_DESTROY_WITH_PARENT,
                GTK_MESSAGE_ERROR,
                GTK_BUTTONS_OK,
                "Erreur lors de la sauvegarde du fichier %s",
                filename
            );
            gtk_dialog_run(GTK_DIALOG(error));
            gtk_widget_destroy(error);
        }
        
        g_free(text);
        g_free(filename);
    }
    
    gtk_widget_destroy(dialog);
}

static void on_load_document_clicked(GtkMenuItem *menuitem, gpointer user_data) {
    AppData *app_data = (AppData *)user_data;
    
    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Charger un document",
        GTK_WINDOW(app_data->window),
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Annuler", GTK_RESPONSE_CANCEL,
        "_Ouvrir", GTK_RESPONSE_ACCEPT,
        NULL
    );
    
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Fichiers texte");
    gtk_file_filter_add_pattern(filter, "*.txt");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
    
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        
        FILE *file = fopen(filename, "r");
        if (file != NULL) {
            fseek(file, 0, SEEK_END);
            long file_size = ftell(file);
            rewind(file);
            
            char *buffer = g_malloc(file_size + 1);
            if (buffer != NULL) {
                size_t read_size = fread(buffer, 1, file_size, file);
                buffer[read_size] = '\0';  
                
                 app_data->prediction_enabled = FALSE;
                
                 gtk_text_buffer_set_text(app_data->buffer, buffer, -1);
                
                 app_data->prediction_enabled = TRUE;
                
                 GtkWidget *message = gtk_message_dialog_new(
                    GTK_WINDOW(app_data->window),
                    GTK_DIALOG_DESTROY_WITH_PARENT,
                    GTK_MESSAGE_INFO,
                    GTK_BUTTONS_OK,
                    "Le document a été chargé avec succès depuis %s",
                    filename
                );
                gtk_dialog_run(GTK_DIALOG(message));
                gtk_widget_destroy(message);
                
                g_free(buffer);
            } else {
               
                GtkWidget *error = gtk_message_dialog_new(
                    GTK_WINDOW(app_data->window),
                    GTK_DIALOG_DESTROY_WITH_PARENT,
                    GTK_MESSAGE_ERROR,
                    GTK_BUTTONS_OK,
                    "Erreur d'allocation mémoire lors du chargement du fichier"
                );
                gtk_dialog_run(GTK_DIALOG(error));
                gtk_widget_destroy(error);
            }
            fclose(file);
        } else {
            
            GtkWidget *error = gtk_message_dialog_new(
                GTK_WINDOW(app_data->window),
                GTK_DIALOG_DESTROY_WITH_PARENT,
                GTK_MESSAGE_ERROR,
                GTK_BUTTONS_OK,
                "Erreur lors de l'ouverture du fichier %s",
                filename
            );
            gtk_dialog_run(GTK_DIALOG(error));
            gtk_widget_destroy(error);
        }
        g_free(filename);
    }
    
    gtk_widget_destroy(dialog);
}



static void on_save_database_clicked(GtkMenuItem *menuitem, gpointer user_data) {
    AppData *app_data = (AppData *)user_data;
    
   
    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Sauvegarder la base de données",
        GTK_WINDOW(app_data->window),
        GTK_FILE_CHOOSER_ACTION_SAVE,
        "_Annuler", GTK_RESPONSE_CANCEL,
        "_Enregistrer", GTK_RESPONSE_ACCEPT,
        NULL
    );
    
     
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);
    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "prediction_tree.txt");
    
    
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Fichiers base de données");
    gtk_file_filter_add_pattern(filter, "*.txt");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
    
    
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        
        
        if (app_data->racine != NULL) {
            saveArbreFile(app_data->racine, filename);
           
            FILE *test_file = fopen(filename, "r");
            if (test_file != NULL) {
                fclose(test_file);
                
                 
                GtkWidget *message = gtk_message_dialog_new(
                    GTK_WINDOW(app_data->window),
                    GTK_DIALOG_DESTROY_WITH_PARENT,
                    GTK_MESSAGE_INFO,
                    GTK_BUTTONS_OK,
                    "La base de données a été sauvegardée avec succès dans %s",
                    filename
                );
                gtk_dialog_run(GTK_DIALOG(message));
                gtk_widget_destroy(message);
            } else {
                
                GtkWidget *error = gtk_message_dialog_new(
                    GTK_WINDOW(app_data->window),
                    GTK_DIALOG_DESTROY_WITH_PARENT,
                    GTK_MESSAGE_ERROR,
                    GTK_BUTTONS_OK,
                    "Erreur lors de la sauvegarde de la base de données dans %s",
                    filename
                );
                gtk_dialog_run(GTK_DIALOG(error));
                gtk_widget_destroy(error);
            }
        } else {
             
            GtkWidget *error = gtk_message_dialog_new(
                GTK_WINDOW(app_data->window),
                GTK_DIALOG_DESTROY_WITH_PARENT,
                GTK_MESSAGE_ERROR,
                GTK_BUTTONS_OK,
                "Impossible de sauvegarder : l'arbre de prédiction est vide"
            );
            gtk_dialog_run(GTK_DIALOG(error));
            gtk_widget_destroy(error);
        }
        
        g_free(filename);
    }
    
    gtk_widget_destroy(dialog);
}


static void on_load_database_clicked(GtkMenuItem *menuitem, gpointer user_data) {
    AppData *app_data = (AppData *)user_data;
    
    
    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Charger une base de données",
        GTK_WINDOW(app_data->window),
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Annuler", GTK_RESPONSE_CANCEL,
        "_Ouvrir", GTK_RESPONSE_ACCEPT,
        NULL
    );
    
     
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Fichiers base de données");
    gtk_file_filter_add_pattern(filter, "*.txt");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
    
     
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        
         
        gboolean should_load = TRUE;
        if (app_data->racine != NULL && app_data->racine->children->size > 0) {
            GtkWidget *confirm = gtk_message_dialog_new(
                GTK_WINDOW(app_data->window),
                GTK_DIALOG_DESTROY_WITH_PARENT,
                GTK_MESSAGE_QUESTION,
                GTK_BUTTONS_YES_NO,
                "Le chargement d'une nouvelle base de données remplacera les données existantes. Voulez-vous continuer ?"
            );
            should_load = (gtk_dialog_run(GTK_DIALOG(confirm)) == GTK_RESPONSE_YES);
            gtk_widget_destroy(confirm);
        }
        
        if (should_load) {
           
            app_data->prediction_enabled = FALSE;
            
            
            Arbre *new_tree = loadArbreFromFile(filename, app_data->ht);
            
            if (new_tree != NULL) {
                
                if (app_data->racine != NULL) {
                    arbre_destroy(app_data->racine);
                }
                
               
                app_data->racine = new_tree;
                
                
                GtkWidget *message = gtk_message_dialog_new(
                    GTK_WINDOW(app_data->window),
                    GTK_DIALOG_DESTROY_WITH_PARENT,
                    GTK_MESSAGE_INFO,
                    GTK_BUTTONS_OK,
                    "La base de données a été chargée avec succès depuis %s",
                    filename
                );
                gtk_dialog_run(GTK_DIALOG(message));
                gtk_widget_destroy(message);
                
                
                gtk_list_store_clear(app_data->liststore);
            } else {
              
                GtkWidget *error = gtk_message_dialog_new(
                    GTK_WINDOW(app_data->window),
                    GTK_DIALOG_DESTROY_WITH_PARENT,
                    GTK_MESSAGE_ERROR,
                    GTK_BUTTONS_OK,
                    "Erreur lors du chargement de la base de données depuis %s",
                    filename
                );
                gtk_dialog_run(GTK_DIALOG(error));
                gtk_widget_destroy(error);
            }
            
            app_data->prediction_enabled = TRUE;
        }
        
        g_free(filename);
    }
    
    gtk_widget_destroy(dialog);
}



static void on_quit_clicked(GtkMenuItem *menuitem, gpointer user_data) {
    AppData *app_data = (AppData *)user_data;
    
     
    GtkWidget *dialog = gtk_message_dialog_new(
        GTK_WINDOW(app_data->window),
        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_QUESTION,
        GTK_BUTTONS_YES_NO,
        "Voulez-vous enregistrer vos modifications avant de quitter ?"
    );
    
    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    
     if (response == GTK_RESPONSE_YES) {
         GtkWidget *save_dialog = gtk_message_dialog_new(
            GTK_WINDOW(app_data->window),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_QUESTION,
            GTK_BUTTONS_YES_NO,
            "Voulez-vous enregistrer le document ?"
        );
        
        gint save_doc_response = gtk_dialog_run(GTK_DIALOG(save_dialog));
        
        if (save_doc_response == GTK_RESPONSE_YES) {
             
            on_save_document_clicked(menuitem, user_data);
        }
        
        gtk_widget_destroy(save_dialog);
        
        
        GtkWidget *save_db_dialog = gtk_message_dialog_new(
            GTK_WINDOW(app_data->window),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_QUESTION,
            GTK_BUTTONS_YES_NO,
            "Voulez-vous sauvegarder la base de données ?"
        );
        
        gint save_db_response = gtk_dialog_run(GTK_DIALOG(save_db_dialog));
        
        if (save_db_response == GTK_RESPONSE_YES) {
          on_save_database_clicked(menuitem, user_data);
        }
        
        gtk_widget_destroy(save_db_dialog);
    }

    gtk_widget_destroy(dialog);
    
   
    gtk_main_quit();
}


static void on_about_clicked(GtkMenuItem *menuitem, gpointer user_data) {
    AppData *app_data = (AppData *)user_data;
 
    GtkWidget *about_dialog = gtk_about_dialog_new();

   
    
    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about_dialog), "1.0");
    gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about_dialog), "© 2025 Par Mouad BRAHMI");
    gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(about_dialog), "Cette application permet de prédire des mots en fonction du texte entré.");
    gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(about_dialog), (const gchar *[]){"BRAHMI", NULL}); 

     gtk_dialog_run(GTK_DIALOG(about_dialog));
    
     gtk_widget_destroy(about_dialog);
}



// Initialisation de GTK
void ihm_init(int *argc, char ***argv) {
    gtk_init(argc, argv);
}

// Fonction principale de l'IHM
void ihm_run(Arbre *racine, struct strhash_table *ht) {
    GtkBuilder *builder;
    AppData *app_data;
    
    // Allocation et initialisation de la structure AppData
    app_data = g_slice_new(AppData);
    app_data->racine = racine;
    app_data->ht = ht;
    app_data->prediction_enabled = TRUE;
    
    // Chargement de l'interface Glade
    builder = gtk_builder_new();
    if (gtk_builder_add_from_file(builder, "interface.glade", NULL) == 0) {
        g_print("Erreur lors du chargement de l'interface\n");
        g_slice_free(AppData, app_data);
        return;
    }
    
    // Récupération des widgets
    app_data->window = GTK_WIDGET(gtk_builder_get_object(builder, "mainWindow"));
    app_data->textview = GTK_WIDGET(gtk_builder_get_object(builder, "textview"));
    app_data->treeview = GTK_WIDGET(gtk_builder_get_object(builder, "treeview"));
    app_data->buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(app_data->textview));
    
    // Configuration du TreeView
    setup_treeview(app_data);
    
    // Connexion des signaux
    g_signal_connect(app_data->window, "destroy", 
        G_CALLBACK(on_window_destroy), NULL);
    g_signal_connect(app_data->buffer, "changed", 
        G_CALLBACK(on_text_changed), app_data);
    g_signal_connect(app_data->treeview, "row-activated", 
        G_CALLBACK(on_prediction_selected), app_data);

        g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "menuItemSaveDoc")), 
    "activate", G_CALLBACK(on_save_document_clicked), app_data);

    g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "menuItemLoadDoc")), 
    "activate", G_CALLBACK(on_load_document_clicked), app_data);

    g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "menuItemSaveDB")), 
    "activate", G_CALLBACK(on_save_database_clicked), app_data);


   g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "menuItemLoadDB")), 
    "activate", G_CALLBACK(on_load_database_clicked), app_data);

    g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "menuItemQuit")),
                 "activate", G_CALLBACK(on_quit_clicked), app_data);


    g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "menuItemAbout")),
                 "activate", G_CALLBACK(on_about_clicked), app_data);


      
    
    g_object_unref(builder);
    
    // Affichage de la fenêtre
    gtk_widget_show_all(app_data->window);
    
    // Démarrage de la boucle principale
    gtk_main();
    
    // Nettoyage
    ihm_cleanup(app_data);
}

// Nettoyage des ressources
void ihm_cleanup(AppData *app_data) {
    if (app_data) {
        if (app_data->liststore)
            g_object_unref(app_data->liststore);
        g_slice_free(AppData, app_data);
    }
}

















