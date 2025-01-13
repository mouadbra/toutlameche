# Compilateur et options
CC = gcc
CFLAGS = -Wall -Wextra -g `pkg-config --cflags gtk+-3.0`
LDFLAGS = `pkg-config --libs gtk+-3.0`

# Cibles principales
OBJ = main.o saisie.o sauvegarde.o chargement.o sequence.o tab_dyn.o arbre.o hash_x86_64.o list_x86_64.o ihm.o
TARGET = prog

# Règle par défaut
all: $(TARGET)

# Règle pour créer l'exécutable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) -lm $(LDFLAGS)

# Règles pour les fichiers objets
main.o: main.c saisie.h sauvegarde.h chargement.h sequence.h tab_dyn.h arbre.h ihm.h hash.h
	$(CC) -DMODULE_ES -c $(CFLAGS) main.c

saisie.o: saisie.c saisie.h arbre.h sequence.h hash.h
	$(CC) -c $(CFLAGS) saisie.c

sauvegarde.o: sauvegarde.c sauvegarde.h arbre.h
	$(CC) -c $(CFLAGS) sauvegarde.c

chargement.o: chargement.c chargement.h arbre.h hash.h
	$(CC) -c $(CFLAGS) chargement.c

sequence.o: sequence.c sequence.h hash.h
	$(CC) -c $(CFLAGS) sequence.c

tab_dyn.o: tab_dyn.c tab_dyn.h
	$(CC) -c $(CFLAGS) tab_dyn.c

arbre.o: arbre.c arbre.h tab_dyn.h sequence.h
	$(CC) -c $(CFLAGS) arbre.c

ihm.o: ihm.c ihm.h
	$(CC) -c $(CFLAGS) ihm.c

# Nettoyage des fichiers objets et de l'exécutable
clean:
	rm -f $(OBJ) $(TARGET)
