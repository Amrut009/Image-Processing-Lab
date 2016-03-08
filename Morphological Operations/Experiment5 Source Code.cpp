#include <cv.h>
#include <highgui.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

//Name of the display window
string window_name = "Morphological Operations";
//Storing location of input image
string image_loc;
//Global Variables for Trackbar
int function = 0;
int max_function = 3;			  //Total 4 funtion erode, dilate, open and close
string trackbar_function = "Functions: 0. Dilate 1. Erode \r\n 2. Open 3. Close";  //Trackbar Title

int structure = 0;
int max_structure = 4;
string trackbar_structure = "Structure:";

//Displaying two images in  a single window
void displayimages(Mat image1, Mat image2, string window_name)
{
    Mat Image(image1.rows,image1.cols+ image2.cols+20,image1.type());
    Mat left(Image, cv::Rect(0, 0, image1.cols, image1.rows));
    image1.copyTo(left);
    Mat right(Image, cv::Rect(image1.cols+20, 0, image2.cols, image2.rows));
    image2.copyTo(right);
    imshow(window_name, Image);
}

//Function for Dilation with center pixel as reference point

Mat bdilate(Mat input, Mat element)
{
	int origRow = element.rows/2;
	int origCol = element.cols/2;
	Mat output = input.clone();
	int nr = input.rows;
	int nc = input.cols;
	for(int i = 0; i<nr;i++)
	   for(int j = 0 ; j<nc;j++)
	   {
			if ((int)input.at<uchar>(i,j)) 
			{       
        			// if the foreground pixel is not zero, then fill in the pixel
       				// covered by the s.e.
        			for (int m=0; m<element.rows; m++)
         		 	     for (int n=0; n<element.cols; n++) 
				     {
            				if ((i-origRow+m) >= 0 && (j-origCol+n) >=0 && (i-origRow+m) < nr && (j-origCol+n) < nc)
					{
              					if (!(int)output.at<uchar>(i-origRow+m, j-origCol+n))
						{
							if((int)element.at<uchar>(m,n))
								output.at<uchar>(i-origRow+m, j-origCol+n) = 255;
							else
							 	output.at<uchar>(i-origRow+m, j-origCol+n) = 0;
						}
					}
				      }
          		}
	   }

 	return output;	
}
//Function for erosion with reference point center pixel
Mat berode(Mat input, Mat se)
{
  Mat temp;
  int nr, nc, nrse, ncse, origRow, origCol;
  int i, j, m, n;

  nrse = se.rows;
  ncse = se.cols;

  origRow = nrse/2;
  origCol = ncse/2;

  nr = input.rows;
  nc = input.cols;
 
  temp = input.clone();

  for (i=0; i<nr; i++)
  {
    for (j=0; j<nc; j++)
    {
      if ((int)input.at<uchar>(i,j)) // if the foreground pixel is 1
      {    
        for (m=0; m<nrse; m++)
        {
          for (n=0; n<ncse; n++)
          {
            if ((i-origRow+m) >= 0 && (j-origCol+n) >=0 &&
                (i-origRow+m) < nr && (j-origCol+n) < nc)
            {
                int temp_image = (int)input.at<uchar>(i-origRow+m,j-origCol+n);
                int temp_se = (int)se.at<uchar>(m,n);
                if (!temp_image && temp_se) temp.at<uchar>(i,j) = 0;
               
            }
          }
          }
      }
    }
  }
 return temp;
}
//Trackbar callback

void morpho(int,void*)
{
	Mat input_gray  = imread(image_loc, CV_LOAD_IMAGE_GRAYSCALE);
    	if(! input_gray.data )                              // Check for invalid input
    	{
         	cout <<  "Could not open or find the image" << std::endl ;
        	return ;
    	}
	Mat input = input_gray.clone();
	threshold(input_gray, input, 150, 255,THRESH_BINARY);
	
	Mat element;
	if(structure == 0)
	{
		element = getStructuringElement( MORPH_RECT, Size( 1, 2));
	}
	else if(structure == 1)
	{
		element =  getStructuringElement( MORPH_RECT, Size( 3, 3));
	}
	else if(structure == 2)
	{
		element = getStructuringElement( MORPH_CROSS, Size( 3, 3));
	}
	else if(structure == 3)
	{
		element = getStructuringElement( MORPH_RECT, Size( 9, 9));
	}
	else if(structure == 4)	
	{
		element = getStructuringElement( MORPH_RECT, Size( 15, 15));
	}
	
	//imshow("Structuring Element", element);
	Mat output;
	
	if(function ==0)
	{
		output = bdilate( input, element);
	}
	else if(function ==1)
	{
		output = berode( input, element );
	}
	else if(function ==2)
	{
		output = berode( input, element );
		output = bdilate(output, element);			
	}
	else if(function ==3)
	{
		output = bdilate( input, element );
		output = berode(output, element);
	}
	
	displayimages(input, output, window_name);
	
}

/**
 * @function main
 */
int main( int argc, char** argv )
{
  
  if(argc < 2)
  {
	cout<<"Usage ./a.out <image location>\n";
	return -1;
  }

  image_loc = argv[1];

  /// Create a window to display results
  namedWindow( window_name, CV_WINDOW_AUTOSIZE );

  /// Create Trackbar to choose type of Threshold
  createTrackbar( trackbar_function, window_name, &function, max_function, morpho );
  createTrackbar( trackbar_structure,window_name, &structure, max_structure, morpho );
  
  morpho(0,0);

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
