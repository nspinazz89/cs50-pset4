/**
 * recover.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Recovers JPEGs from a forensic image.
 */
 #include <cs50.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <stdint.h>
 #include <stdbool.h>
 
 //type def uint8 and set block size to 512
 typedef uint8_t BYTE;
 
 const int blockSize = 512; 
 
 //prototypes
 bool isStartOfNew(BYTE buffer[]);
 
int main(void)
{
    //open the file
    FILE* infile = fopen("card.raw", "r");
    if (infile == NULL)
    {
        printf("Could not open the file.\n");
        return 1;
    }
    
    int jpgCount = 0; //counter for number of files
    BYTE buffer[blockSize]; //set buffer size
    
    char name[8]; //for the JPG file names
    FILE* outfile = NULL;
    
    //read until the end of the file
    while (fread(&buffer, 1, blockSize, infile))
    {
        if (isStartOfNew(buffer))//check to see if its the start of of a new jpg
        {
            if (outfile != NULL) //check to see whether anything has been written to the outfile
            {
                //if outfile is open close current one, and create a new one
                fclose(outfile); 
                sprintf(name, "%03d.jpg", jpgCount);
                outfile = fopen(name, "w");
                jpgCount++;
            }
            else
            {
                //else create a new one
                sprintf(name, "%03d.jpg", jpgCount);
                outfile = fopen(name, "w");
                jpgCount++;
            }
        }
        if (outfile != NULL)//if there is an open outfile write this block to it. 
        {
            fwrite(&buffer, 1, blockSize, outfile);
        }
    }
    //close everything and return 0
    fclose(outfile); 
    fclose(infile);
    return 0;
}

bool isStartOfNew(BYTE buffer[])
{
    //first 3 bytes of 
    BYTE firstThree[] = {0xff, 0xd8, 0xff};
    //last bite possibilities
    BYTE lastByte[] = {0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef};
    
    for (int i = 0; i < 3; i++)
    {
        //check to make sure first three match first 3 of buffer
        if (buffer[i] != firstThree[i])
        {
            return false;
        }
    }
    //check to make sure that the last byte is a viable option 
    for (int i = 0; i < 16; i++)
    {
        if (buffer[3] == lastByte[i])
        {
            return true;
        }
    }
    return false;
}