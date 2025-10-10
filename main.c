
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include"ppm_utils.h"

void print_header() {
    printf("====================================================\n");
    printf("   PPM Image Viewer/Editor\n");
    printf("====================================================\n");
    printf("Enter 'help' for a list of commands or 'quit' to exit.\n\n");
}

void print_help() {
    printf("\nAvailable Commands:\n");
    printf("  size <input_file.ppm>\n");
    printf("  gris <input_file.ppm> <output_file.ppm>\n");
    printf("  neg <input_file.ppm> <output_file.ppm>\n");
    printf("  dom <R|G|B> <value> <input_file.ppm> <output_file.ppm>\n");
    printf("  cut <input.ppm> <r1> <r2> <c1> <c2> <output.ppm>\n");
    printf("  fil <input_file.ppm> <output_file.ppm>\n");
    printf("  help\n");
    printf("  quit\n\n");
}

// Main function to parse command line arguments
int main() {
    char line[1024];
    char command[32];
 
    print_header();
    while(1){
        printf("ppmviewer> ");
        if (!fgets(line, sizeof(line), stdin)) break;
        line[strcspn(line, "\n")] = 0;

        char item_scanned = sscanf(line, "%s",command);
        if(item_scanned <= 0) continue;

        if(strcmp(command, "quit") == 0){
            break;
        }else if(strcmp(command, "help") == 0){
            print_help();
        }
        else if(strcmp(command, "size") == 0){
            char infile[255];
            if(sscanf(line,"%*s %255s", infile) == 1){
                PPMImage* img = load_ppm(infile);
                if(img){
                    printf("%d x %d\n", img->width, img->height);
                    free(img);
                }else printf("File not found or invalide!\n");
            }else printf("Usage: size <input_file.ppm>\n");
        }
        else if(strcmp(command, "gris") == 0){
            char infile[255], outfile[255];
            if(sscanf(line, "%*s %255s %255s",infile, outfile) == 2){
                PPMImage* img = load_ppm(infile);
                if(img){
                    PPMImage* gray_img = op_grayscale(img);
                    if(save_ppm(outfile, gray_img))
                        printf("Operation effectuee, %s Creer", outfile);
                    free_ppm(img);
                    free_ppm(gray_img);
                }else printf("ERROR: File %s not found!",infile);
                
            }else printf("Usage: gris <input_file.ppm> <output_file.ppm>\n");
        }
        else if(strcmp(command, "neg") == 0){
            char infile[255], outfile[255];
            if(sscanf(line, "%*s %255s %255s", infile, outfile) == 2){
                PPMImage* img = load_ppm(infile);
                if(img){
                    PPMImage* neg_img = op_negative(img);
                    if(save_ppm(outfile, neg_img)) printf("Operation effectuee, %s creer", outfile);
                    free_ppm(img);
                    free_ppm(neg_img);
                }else printf("ERROR: File %s not found", infile);
            } else printf("Usage:neg <input_file.ppm> <output_file.ppm>\n");  
        } 
        else if(strcmp(command, "dom") == 0){
            char color_str[2]; 
            int val;
            char in[255], out[255];
            if(sscanf(line,"%*s %1s %d %255s %255s", color_str, &val, in,out) == 4){
                char color = toupper(color_str[0]);
                if(color != 'R' && color !='G' && color != 'B'){
                    printf("Invalid Color. Enter 'R', 'G' or 'B'\n");
                    continue;
                }
                PPMImage* img = load_ppm(in);
                if(img){
                    foncerEclaircir(img, color, val);
                    if(save_ppm(out, img))
                        printf("Operation Effectuee, %s creer", out);
                }else 
                    printf("ERROR: File not found or invalid");
            } else 
                printf("Usage: dom <R|G|B> <value> <input_file.ppm> <output_file.ppm>\n");
        } 
        else if(strcmp(command, "fil") == 0){
            char infile[255], outfile[255];
            if(sscanf(line, "%*s %255s %255s", infile, outfile) == 2){
                PPMImage* img = load_ppm(infile);
                if(img){
                    PPMImage* fil_img = op_median_filter(img);
                    if(save_ppm(outfile, fil_img)) printf("Operation Effectuee, %s creer", outfile);
                    free_ppm(img);
                    free_ppm(fil_img);
                }else printf("ERROR: File not found or invalid");
            }printf("Usage: fil <input_file.ppm> <output_file.ppm>\n");
        }else{
            printf("Unknown command %s: Type 'help' for commands\n", command);
        }
    }
   printf("\nExiting program...");
   return 0;
}