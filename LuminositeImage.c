#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int r, g, b;
} Pixel;

typedef struct {
    int largeur, hauteur;
    int max_val;
    Pixel **pixels;
} Image;


// Lecture d’une image PPM de format texte P3
Image* lirePPM(const char *nomFichier) {
    FILE *f = fopen(nomFichier, "r");
    if (!f) {
        printf("Erreur : impossible d’ouvrir %s\n", nomFichier);
        return NULL;
    }

    char type[3];
    fscanf(f, "%s", type);
    if (type[0] != 'P' || type[1] != '3') {
        printf("Erreur : format non supporté (seul le format P3 est accepté)\n");
        fclose(f);
        return NULL;
    }

    Image *img = (Image*) malloc(sizeof(Image));
    fscanf(f, "%d %d", &img->largeur, &img->hauteur);
    fscanf(f, "%d", &img->max_val);

    // Allocation mémoire
    img->pixels = (Pixel**) malloc(img->hauteur * sizeof(Pixel*));
    for (int i = 0; i < img->hauteur; i++)
        img->pixels[i] = (Pixel*) malloc(img->largeur * sizeof(Pixel));

    // Lecture des pixels
    for (int i = 0; i < img->hauteur; i++) {
        for (int j = 0; j < img->largeur; j++) {
            fscanf(f, "%d %d %d",
                &img->pixels[i][j].r,
                &img->pixels[i][j].g,
                &img->pixels[i][j].b);
        }
    }

    fclose(f);
    return img;
}


// Écriture d’une image P3

void ecrirePPM(const char *nomFichier, Image *img) {
    FILE *f = fopen(nomFichier, "w");
    if (!f) {
        printf("Erreur : impossible d’écrire dans %s\n", nomFichier);
        return;
    }

    fprintf(f, "P3\n%d %d\n%d\n", img->largeur, img->hauteur, img->max_val);

    for (int i = 0; i < img->hauteur; i++) {
        for (int j = 0; j < img->largeur; j++) {
            Pixel p = img->pixels[i][j];
            fprintf(f, "%d %d %d  ", p.r, p.g, p.b);
        }
        fprintf(f, "\n");
    }

    fclose(f);
}
// Libération de mémoire
void libererImage(Image *img) {
    for (int i = 0; i < img->hauteur; i++)
        free(img->pixels[i]);
    free(img->pixels);
    free(img);
}

//  Fonction assombrir / éclaircir
void foncerEclaircir(Image *img, char couleur, int val) {
    for (int i = 0; i < img->hauteur; i++) {
        for (int j = 0; j < img->largeur; j++) {
            Pixel *p = &img->pixels[i][j];
            int max = p->r;
            char dom = 'R';
            if (p->g > max) { max = p->g; dom = 'G'; }
            if (p->b > max) { max = p->b; dom = 'B'; }

            if (dom == couleur) {
                p->r += val;
                p->g += val;
                p->b += val;

                // Clamp [0,255]
                if (p->r > 255) p->r = 255;
                if (p->g > 255) p->g = 255;
                if (p->b > 255) p->b = 255;
                if (p->r < 0) p->r = 0;
                if (p->g < 0) p->g = 0;
                if (p->b < 0) p->b = 0;
            }
        }
    }
}

// Programme principal
int main() {
    char fichierEntree[100], fichierSortie[100];
    char couleur;
    int valeur;

    printf(" Assombrir / Éclaircir les pixels \n");
    printf("Nom du fichier d’entrée (.ppm) : ");
    scanf("%s", fichierEntree);
    printf("Nom du fichier de sortie (.ppm) : ");
    scanf("%s", fichierSortie);
    printf("Couleur dominante à modifier (R/G/B) : ");
    scanf(" %c", &couleur);
    printf("Valeur (positive = éclaircir, négative = assombrir) : ");
    scanf("%d", &valeur);

    Image *img = lirePPM(fichierEntree);
    if (!img) return 1;

    foncerEclaircir(img, couleur, valeur);
    ecrirePPM(fichierSortie, img);
    libererImage(img);

    printf("Image traitée avec succès : %s\n", fichierSortie);
    return 0;
}
