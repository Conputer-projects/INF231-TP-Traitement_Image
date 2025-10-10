#ifndef PMM_UTILS_H
#define PMM_UTILS_H

#define MAX_COLOR_VAL 255

// Structure pour représenter un pixel
typedef struct {
    int r, g, b;
} Pixel;

// Structure pour représenter l'image PPM entière en mémoire
typedef struct {
    char version[3]; // e.g., "P3"
    int width;
    int height;
    int max_color; // Devrait être 255 pour ce projet
    Pixel *pixels; // Tableau dynamique de pixels (taille : width * height)
} PPMImage;

// Signatures des fonctions utilitaires partagées :

// 1. Charge une image depuis un fichier en mémoire
PPMImage* load_ppm(const char *filename);

// 2. Sauvegarde une image depuis la mémoire dans un fichier
int save_ppm(const char *filename, const PPMImage *image);

// 3. Libère la mémoire allouée pour la structure de l'image
void free_ppm(PPMImage *image);

// 4. Crée une copie profonde d'une image (utile pour les fonctions créant de nouvelles images)
PPMImage* copy_ppm(const PPMImage *source);

// --- Operations de manipulation d'images ---
//  Fonction assombrir / éclaircir
void foncerEclaircir(PPMImage *img, char dominant_color, int value);

PPMImage* op_grayscale(const PPMImage *original);

PPMImage* op_negative(const PPMImage *original);

PPMImage* op_crop(const PPMImage *original, int r1, int r2, int c1, int c2);

PPMImage* op_median_filter(const PPMImage *original);

#endif