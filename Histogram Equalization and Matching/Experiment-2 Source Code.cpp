#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

void histDisplay(int histogram[], const char* name)
{
    int hist[256];
    for(int i = 0; i < 256; i++)
    {
        hist[i]=histogram[i];
    }
    // draw the histograms
    int hist_w = 512; int hist_h = 400;
    int bin_w = cvRound((double) hist_w/256);
 
    Mat histImage(hist_h, hist_w, CV_8UC1, Scalar(255, 255, 255));
 
    // find the maximum intensity element from histogram
    int max = hist[0];
    for(int i = 1; i < 256; i++){
        if(max < hist[i]){
            max = hist[i];
        }
    }
 
    // normalize the histogram between 0 and histImage.rows
 
    for(int i = 0; i < 256; i++){
        hist[i] = ((double)hist[i]/max)*histImage.rows;
    }
 
 
    // draw the intensity line for histogram
    for(int i = 0; i < 256; i++)
    {
        line(histImage, Point(bin_w*(i), hist_h),
                              Point(bin_w*(i), hist_h - hist[i]),
             Scalar(0,0,0), 1, 8, 0);
    }
 
    // display histogram
    namedWindow(name, CV_WINDOW_AUTOSIZE);
    imshow(name, histImage);
}
void getDisplayhist(Mat img, const char* name)
{
	int histogram[256];
	for(int i = 0; i < 256; i++)
    	{
      	  histogram[i] = 0;
    	}
 
    	// calculate the no of pixels for each intensity values
   		for(int y = 0; y < img.rows; y++)
   		{
        	for(int x = 0; x < img.cols; x++)
        	{
         	   histogram[(int)img.at<uchar>(y,x)]++;         	
        	}
        }
        histDisplay(histogram, name);
}
void equalizeHistogram(Mat img)
{
	if(img.channels() == 1)
	{
		int histogram[256];
 		// initialize all intensity values to 0
    	for(int i = 0; i < 256; i++)
    	{
      	  histogram[i] = 0;
    	}
 
    	// calculate the no of pixels for each intensity values
   		for(int y = 0; y < img.rows; y++)
   		{
        	for(int x = 0; x < img.cols; x++)
        	{
         	   histogram[(int)img.at<uchar>(y,x)]++;         	
        	}
        }
        histDisplay(histogram, "Orignal Histogram");
        int cumhistogram[256];
        cumhistogram[0] = histogram[0];
 
   		for(int i = 1; i < 256; i++)
   		{
      		cumhistogram[i] = histogram[i] + cumhistogram[i-1];
  		}

  		// Caluculate the size of image
    	int size = img.rows * img.cols;
    	float alpha = 255.0/size;
    	// Scale the histogram
    	int shistogram[256];
    	for(int i = 0; i < 256; i++)
   		{
        	shistogram[i] = cvRound((double)cumhistogram[i] * alpha);

        	// cout<<histogram[i];
        	// cout<<" ";
        	// cout<<cumhistogram[i];
        	// cout<<" ";
        	// cout<<shistogram[i]<<endl;
    	}

    	//histDisplay(shistogram, "Equalized Histogram");
 
    	Mat new_image = img.clone();
 
	    for(int y = 0; y < img.rows; y++)
	    {
    	    for(int x = 0; x < img.cols; x++)
    	    {
        	    new_image.at<uchar>(y,x) = saturate_cast<uchar>(shistogram[img.at<uchar>(y,x)]);        	   
    	    }
	    }
	    char* equalized_window = "Equalized Image";
	    namedWindow(equalized_window, CV_WINDOW_AUTOSIZE);
		imshow(equalized_window, new_image);
		getDisplayhist(new_image, "Final Histogram");
	}
	else
	{
		int bhistogram[256];
		int ghistogram[256];
		int rhistogram[256];
 		// initialize all intensity values to 0
    	for(int i = 0; i < 256; i++)
    	{
      	  bhistogram[i] = 0;
      	  ghistogram[i] = 0;
      	  rhistogram[i] = 0;
    	}
 
    	// calculate the no of pixels for each intensity values
   		for(int y = 0; y < img.rows; y++)
   		{
        	for(int x = 0; x < img.cols; x++)
        	{
         	   bhistogram[(int)img.at<cv::Vec3b>(y,x)[0]]++;  
         	   ghistogram[(int)img.at<cv::Vec3b>(y,x)[1]]++;
         	   rhistogram[(int)img.at<cv::Vec3b>(y,x)[2]]++;       	
        	}
        }
        int bcumhistogram[256];
        int gcumhistogram[256];
        int rcumhistogram[256];

        bcumhistogram[0] = bhistogram[0];
        gcumhistogram[0] = ghistogram[0];
        rcumhistogram[0] = rhistogram[0];
 
   		for(int i = 1; i < 256; i++)
   		{
      		bcumhistogram[i] = bhistogram[i] + bcumhistogram[i-1];
      		gcumhistogram[i] = ghistogram[i] + gcumhistogram[i-1];
      		rcumhistogram[i] = rhistogram[i] + rcumhistogram[i-1];
  		}

  		// Caluculate the size of image
    	int size = img.rows * img.cols;
    	float alpha = 255.0/size;
    	// Scale the histogram
    	int bshistogram[256];
    	int gshistogram[256];
    	int rshistogram[256];
    	for(int i = 0; i < 256; i++)
   		{
        	bshistogram[i] = cvRound((double)bcumhistogram[i] * alpha);
        	gshistogram[i] = cvRound((double)gcumhistogram[i] * alpha);
        	rshistogram[i] = cvRound((double)rcumhistogram[i] * alpha);
    	}
 
    	Mat new_image = img.clone();
 
	    for(int y = 0; y < img.rows; y++)
	    {
    	    for(int x = 0; x < img.cols; x++)
    	    {
        	    new_image.at<cv::Vec3b>(y,x)[0] = saturate_cast<uchar>(bshistogram[img.at<cv::Vec3b>(y,x)[0]]);
        	    new_image.at<cv::Vec3b>(y,x)[1] = saturate_cast<uchar>(gshistogram[img.at<cv::Vec3b>(y,x)[1]]);   
        	    new_image.at<cv::Vec3b>(y,x)[2] = saturate_cast<uchar>(rshistogram[img.at<cv::Vec3b>(y,x)[2]]);           	   
    	    }
	    }
	    char* equalized_window = "Equalized Image";
	    namedWindow(equalized_window, CV_WINDOW_AUTOSIZE);
		imshow(equalized_window, new_image);

	}
}
void histogramMaching(Mat img, Mat mimg)
{
	if(img.channels() == 1)
	{
		int histogram[256];
		int mhistogram[256];
 		// initialize all intensity values to 0
    	for(int i = 0; i < 256; i++)
    	{
    		histogram[i] = 0;
      	  	mhistogram[i] = 0;
    	}

    	// calculate the no of pixels for each intensity values
   		for(int y = 0; y < img.rows; y++)
   		{
        	for(int x = 0; x < img.cols; x++)
        	{
        		histogram[(int)img.at<uchar>(y,x)]++; 

        	}
        }

        for(int y = 0; y < mimg.rows; y++)
   		{
        	for(int x = 0; x < mimg.cols; x++)
        	{ 
         	  	mhistogram[(int)mimg.at<uchar>(y,x)]++;         	
        	}
        }

    	int cumhistogram[256];
    	int mcumhistogram[256];
        cumhistogram[0] = histogram[0];
        mcumhistogram[0] = mhistogram[0];

        int size = img.rows * img.cols;
    	float alpha = 255.0/size;
    	int msize = mimg.rows * mimg.cols;
    	float malpha = 255.0/msize;

 
   		for(int i = 1; i < 256; i++)
   		{
      		cumhistogram[i] = histogram[i] + cumhistogram[i-1];
      		mcumhistogram[i] = mhistogram[i] + mcumhistogram[i-1];
  		}
  		for(int i = 0; i< 256;i++)
  		{
  			cumhistogram[i] = cvRound((double)cumhistogram[i] * alpha);
      		mcumhistogram[i] = cvRound((double)mcumhistogram[i] * malpha);
  		}
  		int mapping[256];
  		for(int i = 0; i < 256; i++)
    	{
    		mapping[i] =0;
    	}
  		for(int i =0;i<256;i++)
  		{
  			for( int j =0 ;j<256;j++)
  			{
  				if(mcumhistogram[j] < cumhistogram[i])
  				{
  					mapping[i] = j;
  				}
  			}
  			// cout<< histogram[i];
  			// cout<< " ";
  			// cout<< mhistogram[i];
  			// cout<<" ";
  			// cout<< cumhistogram[i];
  			// cout<< " ";
  			// cout<< mcumhistogram[i];
  			// cout<< " ";
  			// cout<<mapping[i]<<endl;
  		}

    	Mat new_image = img.clone();
 
	    for(int y = 0; y < img.rows; y++)
	    {
    	    for(int x = 0; x < img.cols; x++)
    	    {
        	    new_image.at<uchar>(y,x) = saturate_cast<uchar>(mapping[img.at<uchar>(y,x)]);        	   
    	    }
	    }
	    char* matched_window = "Matched Image";
	    namedWindow(matched_window, CV_WINDOW_AUTOSIZE);
		imshow(matched_window, new_image);
		getDisplayhist(img, "Orignal Histogram");
		getDisplayhist(mimg, "Matching Histogram");
		getDisplayhist(new_image, "Final Histogram");

	}
	else
	{
		int bhistogram[256];
		int ghistogram[256];
		int rhistogram[256];
		int bmhistogram[256];
		int gmhistogram[256];
		int rmhistogram[256];
 		// initialize all intensity values to 0
    	for(int i = 0; i < 256; i++)
    	{
    		bhistogram[i] = 0;
    		ghistogram[i] = 0;
    		rhistogram[i] = 0;
    		bmhistogram[i] = 0;
    		gmhistogram[i] = 0;
    		rmhistogram[i] = 0;
    	}

    	// calculate the no of pixels for each intensity values
   		for(int y = 0; y < img.rows; y++)
   		{
        	for(int x = 0; x < img.cols; x++)
        	{
        		bhistogram[(int)img.at<cv::Vec3b>(y,x)[0]]++; 
        		ghistogram[(int)img.at<cv::Vec3b>(y,x)[1]]++; 
        		rhistogram[(int)img.at<cv::Vec3b>(y,x)[2]]++; 

        	}
        }

        for(int y = 0; y < mimg.rows; y++)
   		{
        	for(int x = 0; x < mimg.cols; x++)
        	{ 
         	  	bmhistogram[(int)mimg.at<cv::Vec3b>(y,x)[0]]++; 
        		gmhistogram[(int)mimg.at<cv::Vec3b>(y,x)[1]]++; 
        		rmhistogram[(int)mimg.at<cv::Vec3b>(y,x)[2]]++;         	
        	}
        }

    	int bcumhistogram[256];
    	int gcumhistogram[256];
    	int rcumhistogram[256];
    	int bmcumhistogram[256];
    	int gmcumhistogram[256];
    	int rmcumhistogram[256];
        bcumhistogram[0] = bhistogram[0];
        gcumhistogram[0] = ghistogram[0];
        rcumhistogram[0] = rhistogram[0];
        bmcumhistogram[0] = bmhistogram[0];
        gmcumhistogram[0] = gmhistogram[0];
        rmcumhistogram[0] = rmhistogram[0];

        int size = img.rows * img.cols;
    	float alpha = 255.0/size;
    	int msize = mimg.rows * mimg.cols;
    	float malpha = 255.0/msize;

 
   		for(int i = 1; i < 256; i++)
   		{
      		bcumhistogram[i] = bhistogram[i] + bcumhistogram[i-1];
      		gcumhistogram[i] = ghistogram[i] + gcumhistogram[i-1];
      		rcumhistogram[i] = rhistogram[i] + rcumhistogram[i-1];
      		bmcumhistogram[i] = bmhistogram[i] + bmcumhistogram[i-1];
      		gmcumhistogram[i] = gmhistogram[i] + gmcumhistogram[i-1];
      		rmcumhistogram[i] = rmhistogram[i] + rmcumhistogram[i-1];
  		}
  		for(int i = 0; i< 256;i++)
  		{
  			bcumhistogram[i] = cvRound((double)bcumhistogram[i] * alpha);
  			gcumhistogram[i] = cvRound((double)gcumhistogram[i] * alpha);
  			rcumhistogram[i] = cvRound((double)rcumhistogram[i] * alpha);
      		bmcumhistogram[i] = cvRound((double)bmcumhistogram[i] * malpha);
      		gmcumhistogram[i] = cvRound((double)gmcumhistogram[i] * malpha);
      		rmcumhistogram[i] = cvRound((double)rmcumhistogram[i] * malpha);
  		}
  		int bmapping[256];
  		int gmapping[256];
  		int rmapping[256];
  		for(int i = 0; i < 256; i++)
    	{
    		bmapping[i] =0;
    		gmapping[i] =0;
    		rmapping[i] =0;
    	}
  		for(int i =0;i<256;i++)
  		{
  			for( int j =0 ;j<256;j++)
  			{
  				if(bmcumhistogram[j] < bcumhistogram[i])
  				{
  					bmapping[i] = j;
  				}
  				if(gmcumhistogram[j] < gcumhistogram[i])
  				{
  					gmapping[i] = j;
  				}
  				if(rmcumhistogram[j] < rcumhistogram[i])
  				{
  					rmapping[i] = j;
  				}
  			}

  		}

    	Mat new_image = img.clone();
 
	    for(int y = 0; y < img.rows; y++)
	    {
    	    for(int x = 0; x < img.cols; x++)
    	    {
        	    new_image.at<cv::Vec3b>(y,x)[0] = (bmapping[img.at<cv::Vec3b>(y,x)[0]]);    
        	    new_image.at<cv::Vec3b>(y,x)[1] = (gmapping[img.at<cv::Vec3b>(y,x)[1]]); 
        	    new_image.at<cv::Vec3b>(y,x)[2] = (rmapping[img.at<cv::Vec3b>(y,x)[2]]);     	   
    	    }
	    }
	    char* matched_window = "Matched Image";
	    namedWindow(matched_window, CV_WINDOW_AUTOSIZE);
		imshow(matched_window, new_image);
	}

}

/**  @function main */
int main(int argc, char** argv)
{
	/// Load image
	string fname;
	cout << "Please input filename" << endl;
	cin >> fname;

	string ftype;
	cout<< "Please input file type G for Grayscale C for Coloured"<<endl; 
	cin >> ftype;
	Mat img;
	if(ftype == "G")
		img = imread(fname,CV_LOAD_IMAGE_GRAYSCALE); //open and read the image
	else
		img = imread(fname,CV_LOAD_IMAGE_COLOR); 

	if (img.empty())
	{
		cout << "Image cannot be loaded..!!" << endl;
		return -1;
	}
	/// Display image
	char* source_window = "Source image";
	namedWindow(source_window, CV_WINDOW_AUTOSIZE);
	imshow(source_window, img);

	string otype;
	cout<< "Please input operation type E for equalization M for Matching"<<endl; 
	cin >> otype;
	if(otype == "E")
		equalizeHistogram(img);
	else if(otype == "M")
	{
		string mfname;
		cout << "Please input maching filename" << endl;
		cin >> mfname;
		Mat mimg;
		if(ftype == "G")
			mimg = imread(mfname,CV_LOAD_IMAGE_GRAYSCALE); //open and read the image
		else
			mimg = imread(mfname,CV_LOAD_IMAGE_COLOR); 

		if (mimg.empty())
		{
			cout << "Image cannot be loaded..!!" << endl;
			return -1;
		}
		histogramMaching(img,mimg);
	}
	waitKey(0);
	return 0;
}