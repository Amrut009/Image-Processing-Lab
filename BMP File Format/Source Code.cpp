#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

#pragma pack(push,1)								// push current alignment to stack and set it to 1 byte boundary

typedef struct
{
	unsigned short signature;						// magic number used to identify the BMP file: 0x42 0x4D (Hex code points for B and M).
	unsigned int filesize;							// size of the BMP file in bytes 
	unsigned short reserved1, reserved2;		    // reserved.            
	unsigned int offset, infosize, width, height;   //offset,bytes for info header(40),width,height in pixels
	unsigned short planes, bitperpix;				// number of color planes(1),no. of bits per pixel  
	unsigned int compression;						// Type of compression
	unsigned int ImageSize;							// Image size in bytes (including paddding)   
	unsigned int xPPM;						        // Horizontal resolution in pixels per metre
	unsigned int yPPM;						        // Vertical resolution in pixels per metre
	unsigned int colors;							// Colors used in BMP
	unsigned int impcolors;					        // Important Colors used in BMP
}BITMAPHEADER;

typedef struct										// Struct representing B,G,R values of a single pixel
{
	unsigned char blue, green, red;
}RGB;

typedef struct							// Struct representing an image with header and address pointing to 2D array of struct RB
{
	BITMAPHEADER bmpheader;
	RGB ** pixeldata;

} Image;

#pragma pack(pop)								 // restore original alignment from stack


/* Function to Read BMP Image */
void ReadBmp(FILE *bmpInput,Image * orgnlImage)
{

	int i, sizeperpix, sizepad, row, col,j;
	unsigned char *padding;

	 // Read BMP header
	fread(&(orgnlImage->bmpheader), 1, sizeof(BITMAPHEADER), bmpInput);

	// To verify validity of BMP file
	if (orgnlImage->bmpheader.signature != 0x4D42)						   
	{
		cout<<"Not a BMP file"<<endl;
		fclose(bmpInput);
		exit(0);
	}

	sizeperpix = orgnlImage->bmpheader.bitperpix / 8;

	// Zero Padding end of each row
	sizepad = (4 - ((orgnlImage->bmpheader.width*sizeperpix) % 4)) % 4;		
	
	// Image Height
	row = orgnlImage->bmpheader.height;

	// Image Width
	col = orgnlImage->bmpheader.width;										

	fseek(bmpInput, orgnlImage->bmpheader.offset, SEEK_SET);				//set the file pointer to the beginning of pixel data
	orgnlImage->pixeldata = (RGB **)malloc(row*sizeof(RGB*));				

	for (i = 0; i<row; i++)
	{
		orgnlImage->pixeldata[i] = (RGB *)malloc(col*sizeof(RGB));  // Dynamic Memory Allocation
		for (j = 0; j < col; j++)
		{
			fread(&(orgnlImage->pixeldata[i][j]), sizeperpix, 1, bmpInput); // Pixel Data to 2D array			
		}

		if (sizepad != 0){
			padding = (unsigned char*)malloc(sizepad);
			fread(&padding, 1, sizepad, bmpInput);
			free(padding);
		}
	}

	fclose(bmpInput);
	cout << "Image reading done" <<endl;
	cout << "" <<endl;
}

/* Function to produce Image Information Summary */
void printInfo(Image *orgnlImage){

	cout << "Type: " << orgnlImage->bmpheader.signature << endl;
	cout << "size: " << orgnlImage->bmpheader.filesize << endl;
	cout << "offset: " << orgnlImage->bmpheader.offset << endl;
	cout << "Info header size: " << orgnlImage->bmpheader.infosize << endl;
	cout << "Bitmap width: " << orgnlImage->bmpheader.width << endl;
	cout << "Bimap height " << orgnlImage->bmpheader.height<< endl;
	cout << "Color planes: " << orgnlImage->bmpheader.planes << endl;
	cout << "BPP: " << orgnlImage->bmpheader.bitperpix << endl;
	cout << "Compression Method " << orgnlImage->bmpheader.compression << endl;
	cout << "Raw bmp data size: " << orgnlImage->bmpheader.ImageSize << endl;
	cout << "Horizontal Resolution: " << orgnlImage->bmpheader.xPPM << endl;
	cout << "Vertical Resolution: " << orgnlImage->bmpheader.yPPM << endl;
	cout << "No of Color: " << orgnlImage->bmpheader.colors << endl;
	cout << "No of imp colors: " << orgnlImage->bmpheader.impcolors << endl;
	cout << "" <<endl;
}

/* Flip Gray Scale Function */
void ConvertFlipGrayscale(RGB ** pixelarray, int row, int col, unsigned char*** gray, unsigned char*** flip)
{
	int i, j;
	*gray = (unsigned char**)malloc(sizeof(unsigned char*)* row);
	for (i = 0; i < row; i++)
	{
		(*gray)[i] = (unsigned char*)malloc(sizeof(unsigned char)* col);
	}
	for (i = 0; i < row; i++)
	{
		for (j = 0; j < col; j++)
		{
			(*gray)[i][j] = unsigned char((0.11*float(pixelarray[i][j].blue) + 0.59*float(pixelarray[i][j].green) + 0.3*float(pixelarray[i][j].red)));
		}
	}
	*flip = (unsigned char**)malloc(sizeof(unsigned char*)* col);
	for (i = 0; i < col; i++)
	{
		(*flip)[i] = (unsigned char*)malloc(sizeof(unsigned char)* row);
	}
	for (i = 0; i < col; i++)
	{
		for (j = 0; j < row; j++)
		{
			
			(*flip)[i][j] = (*gray)[row-1-j][col-1-i];  // Formula used for transposing
			
		}
	}

}
/****************************************************************************************/

/* Write BMP Image */
void WriteBmp(FILE *bmpOut, BITMAPHEADER header, unsigned char** pixarray)
{
	int i, j, sizepad;
	BITMAPHEADER head;

	// Padding zeros at the end
	unsigned char *padding;
	sizepad = (4 - (header.width % 4)) % 4;

	// Set original header & make necessary changes
	head = header;														
	head.bitperpix = 8;													
	head.ImageSize = header.height*(header.width + (sizepad));			
	head.colors = 256;													// Color Pallete size 256
	head.offset = header.offset + 1024;									// Color Table into account
	head.filesize = head.ImageSize + head.offset;

	// Write Modified Header to the file
	fwrite(&head, sizeof(unsigned char), 54, bmpOut);					

	// Define color table (RGBA32 format)
	unsigned char colorTable[1024];
	for (i = 0; i < 1024; i++)
	{
		colorTable[i] = unsigned char(i/4) ;							
		if (i % 4 == 3)
			colorTable[i] = 0;

	}

	// Write the color table to the file
	fwrite(colorTable, sizeof(unsigned char), 1024, bmpOut);

	// Write pixel data to the file
	for (i = 0; i<header.height; i++)
	{
		for (j = 0; j<header.width; j++)
		{
			fwrite(&pixarray[i][j], sizeof(unsigned char), 1, bmpOut);
		}
		//padding required only when number of columns is not a multiple of 4
		if (sizepad != 0)
			{
				padding = (unsigned char *)calloc(sizepad, sizeof(unsigned char));
				fwrite(padding, 1, sizepad, bmpOut);
			}
			}
	fclose(bmpOut);
	cout << "Image writing done" <<endl;
	cout << "" <<endl;
}

int main()
{
	FILE *bmpip, *bmpop;
	int temp;
	char ip[50];
	Image orgnlImage;
	unsigned char** grayscale;
	unsigned char** flippedgray;

	string fname;
	string fname_write;
	string fname_fwrite;

	cout << "Please input filename" << endl;
	cin >> fname;
	fname = "TestImages/" + fname + ".bmp";
	bmpip = fopen(fname.c_str(), "rb"); // open the input BMP color image and store the file address in fptr
	if(bmpip == NULL) cout<<"404: Not found"<<endl;

	// Read BMP - Function Call
	ReadBmp(bmpip, &orgnlImage);

	printInfo(&orgnlImage);
	
	cout << "Please output filename for Grayscale" << endl;
	cin >> fname_write;
	fname_write = "TestImages/" + fname_write + ".bmp";
    bmpop = fopen(fname_write.c_str(), "wb");
	if(bmpop == NULL) cout<<"404: Not found"<<endl;

	// Converts RGB pixel data into Grayscale
	ConvertFlipGrayscale(orgnlImage.pixeldata, orgnlImage.bmpheader.height, orgnlImage.bmpheader.width, &grayscale, &flippedgray);
	
	// Write BMP Image - Function Call
	WriteBmp(bmpop, orgnlImage.bmpheader,grayscale);
	fclose(bmpop);

	// [Flipping] - Interchanging height with width 
	temp = orgnlImage.bmpheader.height;
	orgnlImage.bmpheader.height = orgnlImage.bmpheader.width;	
	orgnlImage.bmpheader.width = temp;

	cout << "Please output filename for Flipped Grayscale" << endl;
	cin >> fname_fwrite;
	fname_fwrite = "TestImages/" + fname_fwrite + ".bmp";
    bmpop = fopen(fname_fwrite.c_str(), "wb");
	if(bmpop == NULL) cout<<"404: Not found"<<endl;

	// Write BMP Image - Function Call
	WriteBmp(bmpop, orgnlImage.bmpheader, flippedgray);
	fclose(bmpop);
	_getch();
}
/****************************************************************************************/