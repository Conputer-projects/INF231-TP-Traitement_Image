#include "ppm_utils.h"
#include<stdlib.h>
#include<string.h>
#include<stdio.h>

// Helper function to skip comments in the PPM file
void skip_comments(FILE *file) {
    char line[256]; // Buffer to hold each line
    while (fgets(line, sizeof(line), file)) {
        if (line[0] != '#') {
            // If the line is not a comment, seek back the position for the next read
            fseek(file, -strlen(line), SEEK_CUR);
            break;
        }
    }
}

PPMImage* load_ppm(const char *filename){
	FILE* file = fopen(filename, "r");
	if(!file){
		fprintf(stderr, "ERROR: Could not open file %s!\n", filename);
		return NULL;
	}
	
	PPMImage* image = (PPMImage*)malloc(sizeof(PPMImage));
	if(!image){
		fprintf(stderr, "ERROR: Failed to allocate memory for image\n");
		fclose(file);
		return NULL;
	}
	
	//lire les informations d'en-tête
	fscanf(file, "%s", image->version );
	if(strcmp(image->version , "P3") != 0){
		fprintf(stderr, "ERROR: Unsupported PPM format %s! \n", image->version );
		free(image);
		fclose(file);
		return NULL;
	}
	
	// Ignorer les commentaires avant de lire la largeur, la hauteur et la valeur de couleur maximale
	skip_comments(file);
	fscanf(file, "%d %d", &image->width , &image->height);
	skip_comments(file);
	fscanf(file, "%d", &image->max_color );
	if(image->max_color != MAX_COLOR_VAL){
		fprintf(stderr, "ERROR: Unsupported max color value %d\n", image->max_color);
		free(image);
		fclose(file);
		return NULL;
	}
	
	// Allouer mémoire pour les pixels
	image->pixels = (Pixel*)malloc(image->width * image->height * sizeof(Pixel));
	if(!image->pixels ){
		fprintf(stderr,"ERROR: Failed to allocate memory for pixels\n");
		free(image);
		fclose(file);
		return NULL;
	}
	//Lire les données des pixels
	int i;
	for(i=0; i < image->width * image->height ; i++){
		skip_comments(file);
		fscanf(file, "%d %d %d ",&image->pixels[i].r, &image->pixels[i].g, &image->pixels[i].b );
	}
	
	fclose(file);
	return image;
}

int save_ppm(const char *filename, const PPMImage *image){
	if(!filename || !image)
		return -1; // Renvoie une erreur si le nom de fichier ou l'image est NULL
	
	FILE* file = fopen(filename, "w");
	if(!file){
		fprintf(stderr, "ERROR: could not open file %s for writing\n", filename);
		return -1; // Renvoie une erreur si le nom de fichier ou l'image est NULL
	}
	
	// Ecrire l'en-tête PPM
	fprintf(file, "%s\n", image->version);
	fprintf(file, "%d %d \n", image->width, image->height);
	fprintf(file, "%d\n",image->max_color );
	
	// Ecrire les donnees de pixels
	int i;
	for(i=0; i < image->width * image->height ; i++){
		fprintf(file, "%d %d %d", image->pixels[i].r , image->pixels[i].g, image->pixels[i].b);
	}
	
	fclose(file);
	return 0;
}

void free_ppm(PPMImage *image){
	if (image) {
        // Free the pixel data
        free(image->pixels);
        // Free the PPMImage structure itself
        free(image);
    }
}

PPMImage* copy_ppm(const PPMImage *source){
	if(!source){
		return NULL; //Renvoie NULL si la source est NULL
	}
	PPMImage* copy = (PPMImage*)malloc(sizeof(PPMImage));
	if(!copy){
		return NULL;
	}
	
	// Copiez les informations d'en-tête
	strncpy(copy->version , source->version , 3);
	copy->width = source->width;
	copy->height = source->height;
	copy->max_color = source->max_color;
	
	//allouer l'espace pour pixels
	int i;
	copy->pixels = (Pixel*)malloc(copy->width * copy->height * sizeof(Pixel)); 
	for(i=0; i < copy->width * copy->height ; i++){
		copy->pixels[i] = source->pixels[i];
	}
	
	return copy; //// Renvoie la copie nouvellement créée
}
