#ifndef PPM_UTILS_H
#define PPM_UTILS_H

#define MAX_COLOR_VALUE 255  // le valeur max de couleurs

// Structure pour représenter un pixel
typedef struct {
    int r, g, b;
} Pixel;

// Structure pour représenter l'image PPM entière en mémoire
typedef struct {
  char version[3];
  int width;
  int height; 
  int max_color; // valeur max de couleurs 255
  Pixel* pixels; // Tableau dynamique de pixels 
}PPMImage;

// Signatures des fonctions utilitaires partagées :

// 1. Charge une image depuis un fichier en mémoire
PPMImage* load_ppm(const char *filename);

// 2. Sauvegarde une image depuis la mémoire dans un fichier
int save_ppm(const char *filename, const PPMImage *image);

// 3. Libère la mémoire allouée pour la structure de l'image
void free_ppm(PPMImage *image);

// 4. Crée une copie profonde d'une image (utile pour les fonctions créant de nouvelles images)
PPMImage* copy_ppm(const PPMImage *source);

#endif // PPM_UTILS_H
