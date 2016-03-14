#include "stdio.h"

#include "iostream"

#include "stdlib.h"


#define Size 256
using namespace std;


int main(int argc, char* argv[])

{

	// get file name from argument
	
	if(argc != 4) {
		printf("[fatal] need to provide file name in argument!\n");
		exit(1);
	}

	// file pointer

	FILE *file;

	// image data array

	unsigned char Imagedata[Size][Size];



	// read image "lena.raw" into image data matrix

	if (!(file=fopen(argv[1],"rb")))

	{

		cout<<"Cannot open file!"<<endl;

		exit(1);

	}

	fread(Imagedata, sizeof(unsigned char), Size*Size, file);

	fclose(file);



	// do some image processing task...

	// task 1: horizontally fipping

	unsigned char h_flip[Size][Size];

	for(int row = 0; row < Size; row++) {
		for(int col = 0; col < Size; col++) {
			h_flip[row][col] = Imagedata[row][Size - col - 1];
		}
	}

	// task 2: vertically flipping

	unsigned char v_flip[Size][Size];

	for(int row = 0; row < Size; row++) {
		for(int col = 0; col < Size; col++) {
			v_flip[row][col] = Imagedata[Size - row - 1][col];
		}
	}

	// write the resulted image

	if (!(file=fopen(argv[2],"wb")))

	{

		cout<< "Cannot open file for writing at: h_flip.raw" <<endl;

		exit(1);

	}

	fwrite(h_flip, sizeof(unsigned char), Size*Size, file);

	fclose(file);

	if (!(file=fopen(argv[3],"wb")))

	{

		cout<< "Cannot open file for writing at: v_flip.raw" <<endl;

		exit(1);

	}

	fwrite(v_flip, sizeof(unsigned char), Size*Size, file);

	fclose(file);



	return 0;
}









	
