/*---------Including necessary Header Files------------------------------*/
#include <cv.h>
#include <highgui.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <cstdlib>
#include <cmath>
/*-------------------------------------------------------------*/

using namespace std;
using namespace cv;

//define macros 
#define pi 3.1415926
#define e 2.7182

string directory;

//input image stack
string img_pile[30];

//slider variables as global for access by all functions
int slider_val = 0;
int filter_val = 0;
int band = 0;


//declaring Mat variables for storing and displaying images 
Mat input,outputfft,inputFFT,output,filter_fft;

//global variables for access by different functions 
int x;
int y;
int a=1;
 
 //structure for complex number
struct cmp_num
{
      double re;
      double im;
};

 typedef struct cmp_num cmplx;  
 
 //global variables for creating complex addition, subtraction and multiplication
 cmplx *c_sum;
 cmplx *diff;
 cmplx *complex_mul;
 
 cmplx *s;//complex pointer for storing twiddle factor
 
 int *rev;//variable for bit reversal
 int bits=0;//variable for counting bits for bit reversal
 
 cmplx *first_stage,*out_stage;//global variable for 1D fft
 cmplx **rowfft,**colfft,**output_colfft;//global variable for 2D fft

  //function for add of complex number
cmplx *add(cmplx *a,cmplx *b)
{
	
	//adding real part
	c_sum->re=a->re+b->re;
    //adding img. part
	c_sum->im=a->im+b->im;
    //return result
	return(c_sum);
} 

//function for subtracting complex number
cmplx *subtraction(cmplx *a,cmplx *b)
{
	
	//subtracting real part 
	diff->re=(a->re)-(b->re);
	//subtracting img. part
    diff->im=(a->im)-(b->im);
	//return result
    return(diff);
}

//function for multipling complex number
cmplx *multiplication(cmplx *a,cmplx *b)
{
	
	complex_mul->re=(a->re)*(b->re)-(a->im)*(b->im);
    complex_mul->im=(a->re)*(b->im)+(a->im)*(b->re);
    return(complex_mul);
}   

//function for calculating e^(-i2pj/k)
cmplx *twid(int k,int j)
{
	s->re = cos((float)(2*pi*j)/k);
	s->im = -1*sin((float)(2*pi*j)/k);
	return(s);   
}  

//function for calculating e^(i2pj/k)
cmplx *twid_i(int k,int j)
{
	s->re = cos((float)(2*pi*j)/k);  
	s->im = sin((float)(2*pi*j)/k);  
	return(s);
}


/*------Function for bit reversal------*/
int bitrev(int t)
{
	bits =0;// variable for storing  max no of bits for every index
	int y=x;//storing the no of rows in m
	
	//loop for calculating no of bits
	while(y!=1)
	{
		y=y/2;
		bits=bits+1;
	}
	
	
	y=t;
	//loop for reversing the bits
	for(int i=0;i<bits;i++)
	{
		rev[i]=y%2;
		y=y/2;
	}
	int reverse=0;

	//loop for converting from binary to decimal
	for(int i=0;i<bits;i++)
	{
		reverse=reverse+rev[bits-i-1]*pow((double)2,i);
	}
	return(reverse); //returning the reverse output value
}

//function for calculating 1D FFT
cmplx *FFT1D(cmplx *input)
{
	double real,imag;
	first_stage=(cmplx*)malloc(x*sizeof(cmplx));//array for storing input values
	out_stage=(cmplx*)malloc(x*sizeof(cmplx));// array for storing computed values
	
	//storing bit reversed order in input array
	for(int i=0;i<x;i++)
	{
		first_stage[i].re=input[bitrev(i)].re;
		first_stage[i].im=input[bitrev(i)].im;
	}
	

	//loop for fft calculation
	for(int u=2;u<=x;u=u*2)//counting no of stages of fft e.g for 512 point fft stages is 9
	{
		//loop for adding and subtracting in the butterfly
		for(int i=0;i<x;i=i+u)
		{
			for(int j=0;j<u/2;j++)
			{
				//storing values in output array
				*(out_stage+i+j)=*add(first_stage+i+j,first_stage+i+j+u/2);
				*(out_stage+i+j+u/2)=*subtraction(first_stage+i+j,first_stage+i+j+u/2);
			}
		}
		if(u<x)
		{
			//loop for multiplication of the twiddle factor
			for(int i=0;i<x;i++)
			{
				if((i-u)%(2*u)==0)
				{
					for(int j=0;j<u;j++)
					{
						//storing values in output array after suitable twiddle factor multiplication
						*(out_stage+i+j) = *multiplication(twid(2*u,j),(out_stage+i+j));
					}
				}
			}
		}
		
		//replacing the output values in the input array for re computation
		for(int i=0;i<x;i++)
		{
			(first_stage+i)->re=(out_stage+i)->re;
			(first_stage+i)->im=(out_stage+i)->im;
		}
	}

	return(out_stage);//returning pointer to the 1D FFt values
}

//functionn for calculating 2D FFT
cmplx** FFT2D(string *img_pile, int slider_val)
{
	string image_loc = directory + img_pile[slider_val];
	Mat inp  = imread(image_loc, CV_LOAD_IMAGE_UNCHANGED);   // Read the file
	
	
	//declaring 2D arrays for storing fft values
	cmplx **input_image,**output_fft,**output_filter;

	input_image=(cmplx**)malloc(x*sizeof(cmplx*));
	for(int i=0;i<x;i++)
	{
		input_image[i]=(cmplx*)malloc(y*sizeof(cmplx));
	}
	
	//preprocessing of the image for shifting the magnitude spectrum i.e. multiply image pixels by (-1)^(x+y)
	for(int i=0;i<y;i++)
	{
		for(int j=0;j<x;j++)
		{
			if((i+j)%2==0)
			{
				input_image[i][j].re=( double)inp.at<unsigned char>(i,j);
			}
			else
			{
				input_image[i][j].re=-1*( double)inp.at<unsigned char>(i,j);
			}
			input_image[i][j].im=0;
		}
	}
	//creating matrix dynamically
	rowfft=(cmplx**)malloc(x*sizeof(cmplx*));
	colfft=(cmplx**)malloc(x*sizeof(cmplx*));
	output_colfft=(cmplx**)malloc(x*sizeof(cmplx*));
	for(int i=0;i<x;i++)
	{
		rowfft[i]=(cmplx*)malloc(y*sizeof(cmplx)); 
		colfft[i]=(cmplx*)malloc(x*sizeof(cmplx));
		output_colfft[i]=(cmplx*)malloc(y*sizeof(cmplx));
	}
	
	//calculating 1D fft of each row
	for(int i=0;i<x;i++)
	{
		rowfft[i]=FFT1D(input_image[i]);//storing the fft coefficients
	}

	//transpose the matrix for further fft impleentation
	for(int i=0;i<x;i++)
	{
		for(int j=0;j<y;j++)
		{
			colfft[i][j].re=rowfft[j][i].re;  
	        colfft[i][j].im=rowfft[j][i].im;
		}
	}

	//calculating 1D fft of each row of the transposed matrix
	for(int i=0;i<x;i++)
	{
		output_colfft[i]=FFT1D(colfft[i]);
	}


	free(rowfft);
	free(colfft);

	//return 2D FFT of input
	return(output_colfft);
}

cmplx *IFFT1D(cmplx *input)
{
	double real,imag;

	for(int i=0;i<x;i++)
	{
		first_stage[i].re=input[bitrev(i)].re;
		first_stage[i].im=input[bitrev(i)].im;
	}
//loop for Calculating 1D Inverse fast fourier transform 
  for(int u=2;u<=x;u=u*2)//check for the no of stages, smae ass fft
  {
	 //summation and subtraction loop//
	  for(int i=0;i<x;i=i+u)
	  {
		  for(int j=0;j<u/2;j++)
		  {
			  *(out_stage+i+j)=*add(first_stage+i+j,first_stage+i+j+u/2);
              *(out_stage+i+j+u/2)=*subtraction(first_stage+i+j,first_stage+i+j+u/2);
          }
	  }
	  if(u<x)
	  {
		  //twiddle factor multiplication loop
		  for(int i=0;i<x;i++)
		  {
			  if((i-u)%(2*u)==0)
			  {
				  for(int j=0;j<u;j++)
				  {
					  *(out_stage+i+j) = *multiplication(twid_i(2*u,j),(out_stage+i+j));
                  }
			  }
		  }
	  }
	  //putting the computed values back into the first_stage for recomputation
	  for(int i=0;i<x;i++)
	  {
		  (first_stage+i)->re=(out_stage+i)->re;
		  (first_stage+i)->im=(out_stage+i)->im;
	  }                                               
   }
	return(out_stage);//returning the ifft
}
//
cmplx** IFFT2D(cmplx **input_image){

	//2D arrays for storing the inverse values
	rowfft=(cmplx**)malloc(x*sizeof(cmplx*));
	colfft=(cmplx**)malloc(x*sizeof(cmplx*));
	output_colfft=(cmplx**)malloc(x*sizeof(cmplx*));
	for(int i=0;i<x;i++)
	{
		rowfft[i]=(cmplx*)malloc(y*sizeof(cmplx)); 
		colfft[i]=(cmplx*)malloc(x*sizeof(cmplx));
		output_colfft[i]=(cmplx*)malloc(y*sizeof(cmplx));
	}
	for(int i=0;i<x;i++)
	{
		rowfft[i]=FFT1D(input_image[i]);
	}  
	
	//transposing the matrix for recomputation of ifft
	for(int i=0;i<x;i++)
	{
		for(int j=0;j<y;j++)
		{
			colfft[i][j].re=rowfft[j][i].re; 
			colfft[i][j].im=rowfft[j][i].im;
		}
	}
	//final calculation of ifft and returning values
	for(int i=0;i<x;i++)
	{
		output_colfft[i]=FFT1D(colfft[i]);
	}

	return(output_colfft);
}

//Ideal LOW PASS Filter
cmplx** Ideal_lpf(cmplx **filter_input)
{
	//array for storing filter coefficients
	cmplx **filter_output;
	filter_output=(cmplx**)malloc(x*sizeof(cmplx*));
	for(int i=0;i<x;i++)
	{
		filter_output[i]=(cmplx*)malloc(y*sizeof(cmplx)); 
	}
   
	double d;

	//checking the filter criteria and hence assigning values to different pixels
	for(int i=0;i<x;i++)
	{
		for(int j=0;j<y;j++)
		{
			d=sqrt(double ((i-x/2)*(i-x/2)+(j-y/2)*(j-y/2)));//formula for calculating filter values
			if(d<=band)
			{
				//calculating the output spectrum
				filter_output[i][j].re=filter_input[i][j].re;
				filter_output[i][j].im=filter_input[i][j].im;
				filter_fft.at<unsigned char>(i,j)=255;//for displaying filter spectrum
			}
			else
			{
				//calculating the output spectrum
				filter_output[i][j].re=(double)0;
				filter_output[i][j].im=(double)0;
				filter_fft.at<unsigned char>(i,j)=0;//for displaying filter spectrum
			}
		}
	}
	return (filter_output);
}

//Ideal HIGH PASS Filter
cmplx** Ideal_hpf(cmplx **filter_input)
{
	//array for storing filter coefficients
	cmplx **filter_output;
	filter_output=(cmplx**)malloc(x*sizeof(cmplx*));
	for(int i=0;i<x;i++)
	{
		filter_output[i]=(cmplx*)malloc(y*sizeof(cmplx)); 
	}

	double d;
	for(int i=0;i<x;i++)
	{
		for(int j=0;j<y;j++)
		{
			d=sqrt(double ((i-x/2)*(i-x/2)+(j-y/2)*(j-y/2)));//checking for filter values
			if(d<band)
			{
				filter_output[i][j].re=(double)0;
				filter_output[i][j].im=(double)0;
				filter_fft.at<unsigned char>(i,j)=0;
			}
			else
			{
				filter_output[i][j].re=filter_input[i][j].re;
				filter_output[i][j].im=filter_input[i][j].im;
				filter_fft.at<unsigned char>(i,j)=255;
			}
		}
	}
	return (filter_output);
}

//Butterworth LOW PASS Filter
cmplx** But_lpf(cmplx **filter_input)
{
	cmplx **filter_output;
	filter_output=(cmplx**)malloc(x*sizeof(cmplx*));
	for(int i=0;i<x;i++)
	{
		filter_output[i]=(cmplx*)malloc(y*sizeof(cmplx)); 
	}
	
	double d;
	
	for(int i=0;i<x;i++)
	{
		for(int j=0;j<y;j++)
		{
			d=(double ((i-x/2)*(i-x/2)+(j-y/2)*(j-y/2)));//formula for calculating coefficient
			d=d/(band*band);
			d=1+pow((double) d,2);
			double x= 1.0/d;
			filter_fft.at<unsigned char>(i,j)=255*x;
			filter_output[i][j].re=filter_input[i][j].re/d;
			filter_output[i][j].im=filter_input[i][j].im/d;
		}
	}
	return (filter_output);
}

//Butterworth HIGH PASS Filter
cmplx** But_hpf(cmplx **filter_input)
{
	cmplx **filter_output;
	filter_output=(cmplx**)malloc(x*sizeof(cmplx*));
	for(int i=0;i<x;i++)
	{
		filter_output[i]=(cmplx*)malloc(y*sizeof(cmplx)); 
	}
	double d;

	for(int i=0;i<x;i++)
	{
		for(int j=0;j<y;j++)
		{
			d=(double ((i-x/2)*(i-x/2)+(j-y/2)*(j-y/2)));
			d=(band*band)/d;
			d=1+pow( (double)d,2);
			double x= 1.0/d;
			filter_fft.at<unsigned char>(i,j)=255*x;
			filter_output[i][j].re=filter_input[i][j].re/d;
			filter_output[i][j].im=filter_input[i][j].im/d;
		}
	}
	return (filter_output);
}

//Gaussian Low Pass Filter
cmplx** gaus_lpf(cmplx **filter_input)
{
	cmplx **filter_output;
	filter_output=(cmplx**)malloc(x*sizeof(cmplx*));
	for(int i=0;i<x;i++)
	{
		filter_output[i]=(cmplx*)malloc(y*sizeof(cmplx)); 
	}
	double d;
	for(int i=0;i<x;i++)
	{
		for(int j=0;j<y;j++)
		{
			d=(double ((i-x/2)*(i-x/2)+(j-y/2)*(j-y/2)))/2;
			d=-d/(band*band);
			d=pow(e,d );
			filter_fft.at<unsigned char>(i,j)=255*d;
			filter_output[i][j].re=filter_input[i][j].re*d;
			filter_output[i][j].im=filter_input[i][j].im*d;
		}
	}
	return (filter_output);
}

//Gaussian High Pass Filter
cmplx** gaus_hpf(cmplx **filter_input)
{
	
	cmplx **filter_output;
	filter_output=(cmplx**)malloc(x*sizeof(cmplx*));
	for(int i=0;i<x;i++)
	{
		filter_output[i]=(cmplx*)malloc(y*sizeof(cmplx)); 
	}
	double d;
	for(int i=0;i<x;i++)
	{
		for(int j=0;j<y;j++)
	{
		d=(double ((i-x/2)*(i-x/2)+(j-y/2)*(j-y/2)))/2;
		d=-d/(band*band);
		d=pow(e,d);
		d=1.0-d;
		filter_fft.at<unsigned char>(i,j)=255*d;
		filter_output[i][j].re=filter_input[i][j].re*d;
		filter_output[i][j].im=filter_input[i][j].im*d;
	}
}
return (filter_output);
}

void displayimages(Mat image1, Mat image2, string window_name)
{
    Mat Image(image1.rows,image1.cols+ image2.cols+20,image1.type());
    Mat left(Image, cv::Rect(0, 0, image1.cols, image1.rows));
    image1.copyTo(left);
    Mat right(Image, cv::Rect(image1.cols+20, 0, image2.cols, image2.rows));
    image2.copyTo(right);
    imshow(window_name, Image);
}

 void filtering(int,void*)
{
		string image_loc = directory + img_pile[slider_val];
		Mat inp  = imread(image_loc, CV_LOAD_IMAGE_UNCHANGED);   // Read the file
	
	 
	 //creating windows for displaying different images using clone() function
	 inputFFT=inp.clone();
	 outputfft=inp.clone();
	 output=inp.clone();
	 filter_fft=inp.clone();

		x=inp.cols;//rows of the image
		y=inp.rows;//columns of the image

	cmplx **output_fft,**output_ifft,**output_filter;//arrays for storing the fft and ifft coefficients

	
	output_fft=FFT2D(img_pile, slider_val);	

	for(int i=0;i<x;i++)
	{
		for(int j=0;j<y;j++)
		{
			double real=(output_fft[i][j].re)*(output_fft[i][j].re) ;
			double imag=(output_fft[i][j].im)*(output_fft[i][j].im);
			double temp= sqrt(( double)real+(double)imag);
			inputFFT.at<unsigned char>(i,j)=10*log(temp+1);
		}
	}
	;
	
	
	
	/*------choosing the filter action------*/
	if (filter_val==0)
	{
		output_filter= Ideal_lpf(output_fft);
		}
	if (filter_val==1)
	{
		output_filter=Ideal_hpf(output_fft); 
		}
	if (filter_val==2)
	{	output_filter= But_lpf(output_fft);
		}
	if (filter_val==3)	
	{	output_filter=But_hpf(output_fft);
		}
	if (filter_val==4)
	{	output_filter=gaus_lpf(output_fft);
		}
	if (filter_val==5)
	{	output_filter=gaus_hpf(output_fft);
		}
	
	//displaying fft of output image
	for(int i=0;i<x;i++)
	{
		for(int j=0;j<y;j++)
		{
			double real=(output_filter[i][j].re)*(output_filter[i][j].re);
			double imag=(output_filter[i][j].im)*(output_filter[i][j].im);
			double temp= sqrt(( double)real+(double)imag);
			outputfft.at<uchar>(i,j)=10*log(temp+1);
		}
	}
	
	
	output_ifft=IFFT2D( output_filter);
	//postprocessing for displaying the image
	for(int i=0;i<y;i++)
	{
		for(int j=0;j<x;j++)
		{
			if((i+j)%2==0)
			{
				output_ifft[i][j].re=output_ifft[i][j].re;
				output_ifft[i][j].im=output_ifft[i][j].im;
			}
			else
			{
				output_ifft[i][j].re=-1*output_ifft[i][j].re;
				output_ifft[i][j].im=-1*output_ifft[i][j].im;
			} 
	        
		}
	}

	//displaying output image
	for(int i=0;i<x;i++)
	{
		for(int j=0;j<y;j++)
		{
			double real=(output_ifft[i][j].re)*(output_ifft[i][j].re) ;
			double imag=(output_ifft[i][j].im)*(output_ifft[i][j].im);
			double temp= sqrt(( double)real+imag);
			output.at<unsigned char>(x-1-i,x-1-j)=temp/(x*y);
		}
	}
	displayimages(inp, output, "Frequency Filtering");
	displayimages(inputFFT, outputfft, "FFT");
	imshow("Filter Mask",filter_fft);

	
	free(output_fft);
	free(output_ifft);
	free(first_stage);
	free(out_stage);
	
	cvWaitKey (1);
}


int main ( int argc, char** argv )
{
	directory = argv[1];
	cout<<"Image files of .jpg format present in the folder are:"<<endl;

  /// Load images of .jpg types from the input folder
	DIR *dir;
	struct dirent *ent;
	int max_image = 0;
	if ((dir = opendir (directory.c_str())) != NULL)
	{
	/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) 
		{
			string s1 = ent->d_name;
			if(s1.find(".jpg") != std::string::npos)
			{
				cout<<max_image; cout<<". ";
				img_pile[max_image++] = s1;  //Stores  name of .jpg files in the array
    			printf ("%s\n", s1.c_str());
			}	
    	}
  		closedir (dir);
	}
	else 
	{
  	/* could not open directory */
  		perror ("");
  		return EXIT_FAILURE;
  	}

	
	 //declaring arrays of globally declared pointers
	 c_sum=(cmplx *)malloc(sizeof(cmplx));
	 diff=(cmplx *)malloc(sizeof(cmplx));
	 complex_mul=(cmplx *)malloc(sizeof(cmplx));
	 s=(cmplx *)malloc(sizeof(cmplx));
	 rev=(int *)malloc(bits*sizeof(int));
		 
	//-----Create window----------//
	 namedWindow("Frequency Filtering", WINDOW_AUTOSIZE);
	 moveWindow("Frequency Filtering", 0, 0);
	
	 
	 //Create trackbar to image selection
	 createTrackbar("Image:", "Frequency Filtering", &slider_val, max_image-1,filtering);
	string trackbar_filter = "Filter: 0: Ideal_LPF  1: Ideal_HPF \n 2: Gaussian_LPF 3: Gaussian_HPF \n  4: Butterworth_LPF  5: Butterworth_HPF \r\n";
	 //Create trackbar to filter selection
	 createTrackbar(trackbar_filter, "Frequency Filtering", &filter_val, 5, filtering);
	 //creat trackbar for creating cut off 
	 createTrackbar("Cut-off Frequency", "Frequency Filtering", &band, 255,filtering);
	 //creat trackbar for order 
	 //createTrackbar("order.", "Frequency Filtering", &order, 4);
  	 filtering(0,0);

  /// Wait until user finishes program
  while(true)
  {
    int c;
    c = waitKey( 20 );
    if( (char)c == 27 )
      { break; }
   }
	
return 0;
}
