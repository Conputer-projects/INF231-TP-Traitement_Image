#include "ppm_utils.h"
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<ctype.h>

// Helper function to skip comments in the PPM file
static void skip_comments(FILE *fp) {
    int ch;
    // Loop as long as we find whitespace or comments
    while ((ch = fgetc(fp)) != EOF && isspace(ch)) {
        // This loop body is intentionally empty, it just consumes whitespace.
    }

    // If we found a comment character '#', read and discard the rest of the line.
    if (ch == '#') {
        char buffer[1024];
        fgets(buffer, sizeof(buffer), fp);
        // After skipping a comment line, we might have more whitespace or comments,
        // so we call the function again.
        skip_comments(fp);
    } else {
        // If the character was not whitespace or '#', put it back in the stream
        // so the next fscanf can read it.
        ungetc(ch, fp);
    }
}

static int clamp(int value){
	if (value < 0) return 0;
    if (value > 255) return 255;
	return value;
}
static int compare_uchars(const void *a, const void *b){
	return (*(unsigned char*)a - *(unsigned char*)b);
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
	fscanf(file, "%2s", image->version );
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
	if(image->max_color != 255){
		fprintf(stderr, "ERROR: Unsupported max color value %d\n", image->max_color);
		free(image);
		fclose(file);
		return NULL;
	}
	
	// Allouer mémoire pour les pixels
	int total_pixels = image->width * image->height;
	image->pixels = (Pixel*)malloc(total_pixels * sizeof(Pixel));
	if(!image->pixels ){
		fprintf(stderr,"ERROR: Failed to allocate memory for pixels\n");
		free(image);
		fclose(file);
		return NULL;
	}
	//Lire les données des pixels
	for(int i=0; i < total_pixels ; i++){
		int r, g, b;
		if(fscanf(file, "%d %d %d", &r, &g, &b) != 3){
			fprintf(stderr,"ERROR: Failed to read pixels at index %d. File may be currupted", i);
				free(image->pixels);
				free(image);
				return NULL;
		}
		image->pixels[i].r = r;
		image->pixels[i].g = g;
		image->pixels[i].b = b;
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
	for(int i=0; i < image->width * image->height ; i++){
		fprintf(file, "%d %d %d \t", image->pixels[i].r , image->pixels[i].g, image->pixels[i].b);
		if ((i + 1) % image->width == 0) {
			fprintf(file, "\n");
		}
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
	strcpy(copy->version , source->version);
	copy->width = source->width;
	copy->height = source->height;
	copy->max_color = source->max_color;
	
	//allouer l'espace pour pixels
	int total_pixels = copy->width * copy->height;
	copy->pixels = (Pixel*)malloc(total_pixels * sizeof(Pixel)); 
	if(!copy->pixels){
		free(copy);
		return NULL; 	
	}
	memcpy(copy->pixels, source->pixels, total_pixels*sizeof(Pixel));

	return copy; //// Renvoie la copie nouvellement créée
}

void foncerEclaircir(PPMImage *img, char dominant_color, int value){
	for (int i = 0; i < img->width * img->height; i++) {
		Pixel *p = &img->pixels[i];
		int is_dominant = 0;
		switch (toupper(dominant_color)) {
			case 'R': if (p->r >= p->g && p->r >= p->b) is_dominant = 1; break;
			case 'G': if (p->g >= p->r && p->g >= p->b) is_dominant = 1; break;
			case 'B': if (p->b >= p->r && p->b >= p->g) is_dominant = 1; break;
		}
		if (is_dominant) {
			p->r = clamp(p->r + value);
			p->g = clamp(p->g + value);
			p->b = clamp(p->b + value);
		}
	}
}

PPMImage* op_grayscale(const PPMImage *original){
	PPMImage* gray_img = copy_ppm(original);
	if(!gray_img) return NULL;
	for(int i = 0; i < gray_img->width*gray_img->height; i++){
		Pixel* p = &gray_img->pixels[i];
		unsigned char gray_val = (p->r + p->g + p->b)/3;
		p->r = p->b = p->g = gray_val;
	}
	return gray_img;
}

PPMImage* op_negative(const PPMImage *original){
	if(!original){
		fprintf(stderr, "ERROR: Source image is empty!\n");
		return NULL;
	}
	//creer copy de l'image originelle
	PPMImage* neg_img = copy_ppm(original);
	if(!neg_img){
		fprintf(stderr, "ERROR: Could not allocate memory for negative image!\n");
		return NULL;
	}
	for(int i = 0; i < neg_img->width * neg_img->height; i++){
		neg_img->pixels[i].r = original->max_color - original->pixels[i].r;
        neg_img->pixels[i].g = original->max_color - original->pixels[i].g;
        neg_img->pixels[i].b = original->max_color - original->pixels[i].b;
	}
	return neg_img;
}

PPMImage* op_crop(const PPMImage *original, int r1, int r2, int c1, int c2){
	if (r1 < 1 || r2 > original->height || c1 < 1 || c2 > original->width || r1 >= r2 || c1 >= c2) {
		return NULL;
	}
	PPMImage* cropped_img = (PPMImage*)malloc(sizeof(PPMImage));
	if(!cropped_img)
		return NULL;
	strcpy(cropped_img->version, original->version);
	cropped_img->width = c2-c1;
	cropped_img->height = r2-r1;
	cropped_img->max_color = original->max_color;

	int total_pixels = cropped_img->width * cropped_img->height;
	//alloue l'espace pour les cropped pixels
	cropped_img->pixels = (Pixel*)malloc(total_pixels*sizeof(Pixel));
	if(!cropped_img->pixels) return NULL; //verifie si allouer avec success


	for (int i = 0; i < cropped_img->height; i++) {
		for (int j = 0; j < cropped_img->width; j++) {
			int original_idx = ((r1 - 1) + i) * original->width + ((c1 - 1) + j);
			int cropped_idx = i * cropped_img->width + j;
			cropped_img->pixels[cropped_idx] = original->pixels[original_idx];
		}
	}
	return cropped_img;
}

PPMImage* op_median_filter(const PPMImage *original){
	PPMImage *filtered_img = copy_ppm(original);
     for (int y = 1; y < original->height - 1; y++) {
         for (int x = 1; x < original->width - 1; x++) {
             unsigned char r_neighbors[9], g_neighbors[9], b_neighbors[9];
             int k = 0;
             for (int ny = -1; ny <= 1; ny++) {
                 for (int nx = -1; nx <= 1; nx++) {
                     int n_idx = (y + ny) * original->width + (x + nx);
                     r_neighbors[k] = original->pixels[n_idx].r;
                     g_neighbors[k] = original->pixels[n_idx].g;
                     b_neighbors[k] = original->pixels[n_idx].b;
                     k++;
                 }
             }
             qsort(r_neighbors, 9, sizeof(unsigned char), compare_uchars);
             qsort(g_neighbors, 9, sizeof(unsigned char), compare_uchars);
             qsort(b_neighbors, 9, sizeof(unsigned char), compare_uchars);
             int current_idx = y * original->width + x;
             filtered_img->pixels[current_idx].r = r_neighbors[4];
             filtered_img->pixels[current_idx].g = g_neighbors[4];
             filtered_img->pixels[current_idx].b = b_neighbors[4];
         }
     }
     return filtered_img;
}
