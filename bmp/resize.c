#include <cs50.h>
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    //ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./resize factor infile outfile\n");
        return 1;
    }
    
    //store command line arguments
    int resizeFactor = atoi(argv[1]);
    if (resizeFactor <= 0 || resizeFactor > 100)
    {
        printf("The resize factor should be a positive int no greater than 100.\n");
        return 5;
    }
    
    
    char* infile = argv[2];
    char* outfile = argv[3];
    
    //open the input file
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }
    
    //open the output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }
    
    //read in the infile's file header
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);
    
    //read in the infile's info header
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);
    
    //ensure infile is proper file type
    if(bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
         fclose(outptr);
         fclose(inptr);
         fprintf(stderr, "Unsupported file format.\n");
         return 4;
    }
    
    //save original dimensions as ints and then alter bi's new dimensions 
    //before writing to outfile
    int originalHeight = bi.biHeight;
    int originalWidth = bi.biWidth;
    
    //update height and width
    bi.biHeight *= resizeFactor;
    bi.biWidth *= resizeFactor;
    
    //padding for original image and ending image
    int originalPadding = (4 - (originalWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int endPadding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    //calculate the new sizes taking into account padding and possible negative height
    bi.biSizeImage = (bi.biWidth * sizeof(RGBTRIPLE) + endPadding) * abs(bi.biHeight);
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) ;
    
    //write new headers
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);
    
    //int to store file position indicator 
    //int position;
    
    //iterate over pixels
    for (int i = 0, absOriginalHeight = abs(originalHeight); i < absOriginalHeight; i++) //iterate over the scanline
    {
        //position = ftell(inptr); //get the current position
        for (int j = 0; j < resizeFactor; j++) //vertical stretch
        {
            for (int k = 0; k < originalWidth; k++) //iterate over pixels in the scanline
            {
                RGBTRIPLE triple; //temporary storage
                
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr); //read the pixel
                
                for (int l = 0; l < resizeFactor; l++) //write the pixel k number of times to stretch horizontally
                {
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                }
            }
            fseek(inptr, originalPadding, SEEK_CUR);//skip reading the original padding
            
            for (int m = 0; m < endPadding; m++)//write the new padding if any
            {
                fputc(0x00, outptr); 
            }
            fseek(inptr, -(sizeof(RGBTRIPLE) * originalWidth + originalPadding), SEEK_CUR);
        }
        fseek(inptr, (sizeof(RGBTRIPLE) * originalWidth + originalPadding), SEEK_CUR);
    }
    fclose(inptr);
    fclose(outptr);
    return 0;
}
