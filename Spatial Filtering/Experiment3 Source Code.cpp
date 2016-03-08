#include <cv.h>
#include <highgui.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <iostream>

using namespace cv;
using namespace std;

// Directory of the lder containing image files
string directory;

//Name of the display window
string window_name = "Spatial Filtering";

//Global Variables
int image=0;                //current image
int filter_type = 0;		//current filter type
int nsize = 3;				//current Neighbourhood size
int max_image;
int const max_type = 7;     // Maximum number of filter 6 in our case
int const max_nsize = 15;   // Maximum neighbourhood size
string img_name[30];        // array containing name of .jpg images containing in the folder

//Defining TrackBar names
string trackbar_image = "Image:"; 
string trackbar_filter = "Filter: 0: Mean  1: Median \n 2: Gradient 3: Laplacian   4: Sobel Horizontal  5: Sobel Vertical \n 6: Sobel Diagnol 7: Sobel Combined\r\n";
string trackbar_nsize = "Neighborhood Size:";

// Display image in window 
//Concatenates orignal and output image into a single image to display into single window
void displayimages(Mat image1, Mat image2)
{
    Mat Image(image1.rows,image1.cols+ image2.cols+20,image1.type());
    Mat left(Image, cv::Rect(0, 0, image1.cols, image1.rows));
    image1.copyTo(left);
    Mat right(Image, cv::Rect(image1.cols+20, 0, image2.cols, image2.rows));
    image2.copyTo(right);
    imshow(window_name, Image);
}

//Filter Operators

//sort the window using insertion sort
//insertion sort is best for this sorting
void insertionSort(int window[])
{
    int temp, i , j;
    for(i = 0; i < 9; i++){
        temp = window[i];
        for(j = i-1; j >= 0 && temp < window[j]; j--){
            window[j+1] = window[j];
        }
        window[j+1] = temp;
    }
}

// Function to the median filter which return Mat object corresponding to the output image
Mat MedianFilter(Mat image, int window_size)
{
    Mat fimage;                        //Creating new image object      
    fimage = image.clone();

    int swindow = sqrt(window_size);
    //create a sliding window of window_size
    int window[window_size];          //Create array neighbourhood pixel values

    for(int y = 0; y < image.rows; y++)
        {
          for(int x = 0; x < image.cols; x++)
            fimage.at<uchar>(y,x) = 0.0;           // Initializing image pixels from 0
        }
 
        for(int y = 0; y < image.rows; y++)
          {
            for(int x = 0; x < image.cols; x++)
              {
                // Pick up window element
                for (int i = 0; i < swindow; i++)
                  {
                    for (int j = 0; j < swindow; j++)
                      // Handling boundary cases
                      if ((y - (swindow/2) + j) > 0 && (y - (swindow/2) + j) < image.rows && (x - (swindow/2) + i) > 0 && (x - (swindow/2) + i) < image.cols)
                        window[3*i+j] = image.at<uchar>(y - (swindow/2) + j, x - (swindow/2) + i);
                      else if ((y - (swindow/2) + j) < 0 && (x - (swindow/2) + i) > 0 && (x - (swindow/2) + i) < image.cols)
                        window[3*i+j] = image.at<uchar>(0, x - (swindow/2) + i);
                      else if ((y - (swindow/2) + j) > image.rows-1 && (x - (swindow/2) + i) > 0 && (x - (swindow/2) + i) < image.cols)
                        window[3*i+j] = image.at<uchar>(image.rows-1, x - (swindow/2) + i);
                      else if ((x - (swindow/2) + i) < 0 && (y - (swindow/2) + j) > 0 && (y - (swindow/2) + j) < image.rows)
                        window[3*i+j] = image.at<uchar>(y - (swindow/2) + j, 0);
                      else if ((x - (swindow/2) + i) > image.cols-1 && (y - (swindow/2) + j) > 0 && (y - (swindow/2) + j) < image.rows)
                        window[3*i+j] = image.at<uchar>(y - (swindow/2) + j, image.cols-1);
                      else if ((y - (swindow/2) + j) < 0 && (x - (swindow/2) + i) < 0)
                        window[3*i+j] = image.at<uchar>(0, 0);
                      else if ((y - (swindow/2) + j) < 0 && (x - (swindow/2) + i) > image.cols-1)
                        window[3*i+j] = image.at<uchar>(0, image.cols-1);
                      else if ((y - (swindow/2) + j) > image.rows-1 && (x - (swindow/2) + i) < 0)
                        window[3*i+j] = image.at<uchar>(image.rows-1, 0);
                      else if ((y - (swindow/2) + j) > image.rows-1 && (x - (swindow/2) + i) > image.cols-1)
                        window[3*i+j] = image.at<uchar>(image.rows-1, image.cols-1);
                  }
                
                // sort the window to find median
                insertionSort(window);
 
                // assign the median to centered element of the matrix
                fimage.at<uchar>(y,x) = window[4];
              }

          }
 
        // namedWindow("Median Filter Output");
        // imshow("Median Filter Output", fimage);
          return fimage;
}

// Function to MEan Filter
Mat MeanFilter(Mat image, int window_size)
{
    Mat fimage;
    fimage = image.clone();

    // Taking square root of window size
    int swindow = sqrt(window_size);

    //create a sliding window of window_size
    int window[window_size];

    for(int y = 0; y < image.rows; y++)
        {
          for(int x = 0; x < image.cols; x++)
            fimage.at<uchar>(y,x) = 0.0;
        }
 
        for(int y = 0; y < image.rows; y++)
          {
            for(int x = 0; x < image.cols; x++)
              {
                // Pick up window element
                for (int i = 0; i < swindow; i++)
                  {
                    for (int j = 0; j < swindow; j++)
                      // Handling boundary cases
                      if ((y - (swindow/2) + j) > 0 && (y - (swindow/2) + j) < image.rows && (x - (swindow/2) + i) > 0 && (x - (swindow/2) + i) < image.cols)
                        window[3*i+j] = image.at<uchar>(y - (swindow/2) + j, x - (swindow/2) + i);
                      else if ((y - (swindow/2) + j) < 0 && (x - (swindow/2) + i) > 0 && (x - (swindow/2) + i) < image.cols)
                        window[3*i+j] = image.at<uchar>(0, x - (swindow/2) + i);
                      else if ((y - (swindow/2) + j) > image.rows-1 && (x - (swindow/2) + i) > 0 && (x - (swindow/2) + i) < image.cols)
                        window[3*i+j] = image.at<uchar>(image.rows-1, x - (swindow/2) + i);
                      else if ((x - (swindow/2) + i) < 0 && (y - (swindow/2) + j) > 0 && (y - (swindow/2) + j) < image.rows)
                        window[3*i+j] = image.at<uchar>(y - (swindow/2) + j, 0);
                      else if ((x - (swindow/2) + i) > image.cols-1 && (y - (swindow/2) + j) > 0 && (y - (swindow/2) + j) < image.rows)
                        window[3*i+j] = image.at<uchar>(y - (swindow/2) + j, image.cols-1);
                      else if ((y - (swindow/2) + j) < 0 && (x - (swindow/2) + i) < 0)
                        window[3*i+j] = image.at<uchar>(0, 0);
                      else if ((y - (swindow/2) + j) < 0 && (x - (swindow/2) + i) > image.cols-1)
                        window[3*i+j] = image.at<uchar>(0, image.cols-1);
                      else if ((y - (swindow/2) + j) > image.rows-1 && (x - (swindow/2) + i) < 0)
                        window[3*i+j] = image.at<uchar>(image.rows-1, 0);
                      else if ((y - (swindow/2) + j) > image.rows-1 && (x - (swindow/2) + i) > image.cols-1)
                        window[3*i+j] = image.at<uchar>(image.rows-1, image.cols-1);
                  }
                
                // Calculating the mean of window elements
                int sum = 0;
                for (int i = 0; i < window_size; i++)
                  sum = sum + window[i]; 
                float window_mean = sum/window_size;

                // assign the mean to centered element of the matrix
                fimage.at<uchar>(y,x) = cvRound(window_mean);
              }
          }

        // namedWindow("Mean Filter Output");
        // imshow("Mean Filter Output", fimage);
          return fimage;
}

// Function to Laplacian Operator
Mat LaplacianFilter(Mat image, int window_size)
{
    Mat fimage;
    fimage = image.clone();

    int swindow = sqrt(window_size);

    //create a sliding window of window_size
    int window[window_size];

    for(int y = 0; y < image.rows; y++)
        {
          for(int x = 0; x < image.cols; x++)
            fimage.at<uchar>(y,x) = 0.0;
        }
 
        for(int y = 0; y < image.rows; y++)
          {
            for(int x = 0; x < image.cols; x++)
              {
                // Pick up window element
                for (int i = 0; i < swindow; i++)
                  {
                    for (int j = 0; j < swindow; j++)
                      // Handling boundary cases
                      if ((y - (swindow/2) + j) > 0 && (y - (swindow/2) + j) < image.rows && (x - (swindow/2) + i) > 0 && (x - (swindow/2) + i) < image.cols)
                        window[3*i+j] = image.at<uchar>(y - (swindow/2) + j, x - (swindow/2) + i);
                      else if ((y - (swindow/2) + j) < 0 && (x - (swindow/2) + i) > 0 && (x - (swindow/2) + i) < image.cols)
                        window[3*i+j] = image.at<uchar>(0, x - (swindow/2) + i);
                      else if ((y - (swindow/2) + j) > image.rows-1 && (x - (swindow/2) + i) > 0 && (x - (swindow/2) + i) < image.cols)
                        window[3*i+j] = image.at<uchar>(image.rows-1, x - (swindow/2) + i);
                      else if ((x - (swindow/2) + i) < 0 && (y - (swindow/2) + j) > 0 && (y - (swindow/2) + j) < image.rows)
                        window[3*i+j] = image.at<uchar>(y - (swindow/2) + j, 0);
                      else if ((x - (swindow/2) + i) > image.cols-1 && (y - (swindow/2) + j) > 0 && (y - (swindow/2) + j) < image.rows)
                        window[3*i+j] = image.at<uchar>(y - (swindow/2) + j, image.cols-1);
                      else if ((y - (swindow/2) + j) < 0 && (x - (swindow/2) + i) < 0)
                        window[3*i+j] = image.at<uchar>(0, 0);
                      else if ((y - (swindow/2) + j) < 0 && (x - (swindow/2) + i) > image.cols-1)
                        window[3*i+j] = image.at<uchar>(0, image.cols-1);
                      else if ((y - (swindow/2) + j) > image.rows-1 && (x - (swindow/2) + i) < 0)
                        window[3*i+j] = image.at<uchar>(image.rows-1, 0);
                      else if ((y - (swindow/2) + j) > image.rows-1 && (x - (swindow/2) + i) > image.cols-1)
                        window[3*i+j] = image.at<uchar>(image.rows-1, image.cols-1);
                  }
                
                // Calculating the sum of window elements
                int sum = 0;
                for (int i = 0; i < window_size; i++)
                  sum = sum + window[i]; 

                int window_sum = sum - window_size*window[window_size/2];

               //window_sum = (window_sum + 8*255)/16;  // Normalizing intensity values between 0-255 since this windows sum varies from -8*255 to 8*255

                window_sum = (window_sum > 255) ? 255 : window_sum;
                window_sum = (window_sum < 0) ? -window_sum : window_sum;
 
                // assign the median to centered element of the matrix
                fimage.at<uchar>(y,x) = window_sum;
              }
          }
 
        //namedWindow("Laplacian Filter Output");
        //imshow("Laplacian Filter Output", fimage);
        return fimage;
}

//Function to combined gradient filter 
Mat GradientFilter(Mat image, int window_size)
{
    Mat fimage;
    fimage = image.clone();

    int swindow = sqrt(window_size);

    //create a sliding window of window_size
    int window[window_size];

    // Getting Sobel Operator of desired Dimension

    // int gsobel[3][3] = {{1,2,1},{2,4,2},{1,2,1}};

    int xsobel[3][3] = {{1,1,1},{0,0,0},{-1,-1,-1}};

    int ysobel[3][3] = {{1,0,-1},{1,0,-1},{1,0,-1}};

    for(int y = 0; y < image.rows; y++)
        {
          for(int x = 0; x < image.cols; x++)
            fimage.at<uchar>(y,x) = 0.0;
        }
 
        for(int y = 0; y < image.rows; y++)
          {
            for(int x = 0; x < image.cols; x++)
              {
                int gx = 0;
                int gy = 0;


                // Pick up window element
                for (int i = 0; i < swindow; i++)
                  {
                    for (int j = 0; j < swindow; j++)
                    {
                      // Handling boundary cases
                      if ((y - (swindow/2) + j) > 0 && (y - (swindow/2) + j) < image.rows && (x - (swindow/2) + i) > 0 && (x - (swindow/2) + i) < image.cols)
                        window[3*i+j] = image.at<uchar>(y - (swindow/2) + j, x - (swindow/2) + i);
                      else if ((y - (swindow/2) + j) < 0 && (x - (swindow/2) + i) > 0 && (x - (swindow/2) + i) < image.cols)
                        window[3*i+j] = image.at<uchar>(0, x - (swindow/2) + i);
                      else if ((y - (swindow/2) + j) > image.rows-1 && (x - (swindow/2) + i) > 0 && (x - (swindow/2) + i) < image.cols)
                        window[3*i+j] = image.at<uchar>(image.rows-1, x - (swindow/2) + i);
                      else if ((x - (swindow/2) + i) < 0 && (y - (swindow/2) + j) > 0 && (y - (swindow/2) + j) < image.rows)
                        window[3*i+j] = image.at<uchar>(y - (swindow/2) + j, 0);
                      else if ((x - (swindow/2) + i) > image.cols-1 && (y - (swindow/2) + j) > 0 && (y - (swindow/2) + j) < image.rows)
                        window[3*i+j] = image.at<uchar>(y - (swindow/2) + j, image.cols-1);
                      else if ((y - (swindow/2) + j) < 0 && (x - (swindow/2) + i) < 0)
                        window[3*i+j] = image.at<uchar>(0, 0);
                      else if ((y - (swindow/2) + j) < 0 && (x - (swindow/2) + i) > image.cols-1)
                        window[3*i+j] = image.at<uchar>(0, image.cols-1);
                      else if ((y - (swindow/2) + j) > image.rows-1 && (x - (swindow/2) + i) < 0)
                        window[3*i+j] = image.at<uchar>(image.rows-1, 0);
                      else if ((y - (swindow/2) + j) > image.rows-1 && (x - (swindow/2) + i) > image.cols-1)
                        window[3*i+j] = image.at<uchar>(image.rows-1, image.cols-1);

                      gx += window[3*i+j] * xsobel[i][j];
                      gy += window[3*i+j] * ysobel[i][j];
                    }
                  }

                int window_sobel = abs(gx) + abs(gy);

                window_sobel = (window_sobel > 255) ? 255 : window_sobel;
                window_sobel = (window_sobel < 0) ? 0 : window_sobel;

                // assign the obtained value to centered element of the matrix
                fimage.at<uchar>(y,x) = window_sobel;
              }

          }
 
        //namedWindow("Sobel Filter Output");
        //imshow("Sobel Filter Output", fimage);
        return fimage;
}

//Function of sobel Operator for all horizontal, vertical, diagnal and combined form. It return output image correspnding to the filter type selected
Mat SobelFilter(Mat image, int window_size)
{
    Mat fimagex,fimagey,fimaged,fimage;
    fimage = image.clone();
    fimagex = image.clone();
    fimagey = image.clone();
    fimaged = image.clone();

    int swindow = sqrt(window_size);

    //create a sliding window of window_size
    int window[window_size];

    // Getting Sobel Operator of desired Dimension

    // int xsobel[swindow][swindow];
    // int ysobel[swindow][swindow];

    int xsobel[3][3] = {{1,0,-1},
                        {2,0,-2},
                        {1,0,-1}};
    int ysobel[3][3] = {{1,2,1},
                        {0,0,0},
                        {-1,-2,-1}};

    int dsobel[3][3] = {{0,1,2},
                        {-1,0,1},
                        {-2,-1,0}};

    // int gsobel[3][3] = {{1,2,1},{2,4,2},{1,2,1}};

    /*if (swindow == 3)
    {
      int xsobel[3][3] = {{1,0,-1},
                          {2,0,-2},
                          {1,0,-1}};
      int ysobel[3][3] = {{1,2,1},
                          {0,0,0},
                          {-1,-2,-1}};
      
    }

    else if (swindow == 5)
    {
      int xsobel[5][5] = {{2,1,0,-1,-2},
                          {3,2,0,-2,-3},
                          {4,3,0,-3,-4},
                          {3,2,0,-2,-3},
                          {2,1,0,-1,-2}};
      int ysobel[5][5] = {{2,3,4,3,2},
                          {1,2,3,2,1},
                          {0,0,0,0,0},
                          {-1,-2,-3,-2,-1},
                          {-2,-3,-4,-3,-2}};
    }

    else if (swindow == 7)
    {
      int xsobel[7][7] = {{3,2,1,0,-1,-2,-3},
                          {4,3,2,0,-2,-3,-4},
                          {5,4,3,0,-3,-4,-5},
                          {6,5,4,0,-4,-5,-6},
                          {5,4,3,0,-3,-4,-5},
                          {4,3,2,0,-2,-3,-4},
                          {3,2,1,0,-1,-2,-3}};
      int ysobel[7][7] = {{3,4,5,6,5,4,3},
                          {2,3,4,5,4,3,2},
                          {1,2,3,4,3,2,1},
                          {0,0,0,0,0,0,0},
                          {-1,-2,-3,-4,-3,-2,-1},
                          {-2,-3,-4,-5,-4,-3,-2},
                          {-3,-4,-5,-6,-5,-4,-3}};
    }
    */
    
    
    /*
    if (swindow != 3)
    {
      int sobel[swindow][swindow];
      sobel = cv2.filter2D(gsobel,-1,bsobel);
      swindow = swindow - 2;
      while(swindow > 3)
      {
        sobel = cv2.filter2D(gsobel,-1,sobel,borderType = cv2.BORDER_CONSTANT);
        swindow = swindow - 2;
      }
    }
    */
  
    for(int y = 0; y < image.rows; y++)
        {
          for(int x = 0; x < image.cols; x++)
          {
            fimagex.at<uchar>(y,x) = 0.0;
            fimagey.at<uchar>(y,x) = 0.0;
            fimaged.at<uchar>(y,x) = 0.0;
            fimage.at<uchar>(y,x) = 0.0;
          }
        }
 
        for(int y = 0; y < image.rows; y++)
          {
            for(int x = 0; x < image.cols; x++)
              {
                int gx = 0;
                int gy = 0;
                int gd = 0;
                // Pick up window element
                for (int i = 0; i < swindow; i++)
                  {
                    for (int j = 0; j < swindow; j++)
                    {
                      // Handling boundary cases
                      if ((y - (swindow/2) + j) > 0 && (y - (swindow/2) + j) < image.rows && (x - (swindow/2) + i) > 0 && (x - (swindow/2) + i) < image.cols)
                        window[3*i+j] = image.at<uchar>(y - (swindow/2) + j, x - (swindow/2) + i);
                      else if ((y - (swindow/2) + j) < 0 && (x - (swindow/2) + i) > 0 && (x - (swindow/2) + i) < image.cols)
                        window[3*i+j] = image.at<uchar>(0, x - (swindow/2) + i);
                      else if ((y - (swindow/2) + j) > image.rows-1 && (x - (swindow/2) + i) > 0 && (x - (swindow/2) + i) < image.cols)
                        window[3*i+j] = image.at<uchar>(image.rows-1, x - (swindow/2) + i);
                      else if ((x - (swindow/2) + i) < 0 && (y - (swindow/2) + j) > 0 && (y - (swindow/2) + j) < image.rows)
                        window[3*i+j] = image.at<uchar>(y - (swindow/2) + j, 0);
                      else if ((x - (swindow/2) + i) > image.cols-1 && (y - (swindow/2) + j) > 0 && (y - (swindow/2) + j) < image.rows)
                        window[3*i+j] = image.at<uchar>(y - (swindow/2) + j, image.cols-1);
                      else if ((y - (swindow/2) + j) < 0 && (x - (swindow/2) + i) < 0)
                        window[3*i+j] = image.at<uchar>(0, 0);
                      else if ((y - (swindow/2) + j) < 0 && (x - (swindow/2) + i) > image.cols-1)
                        window[3*i+j] = image.at<uchar>(0, image.cols-1);
                      else if ((y - (swindow/2) + j) > image.rows-1 && (x - (swindow/2) + i) < 0)
                        window[3*i+j] = image.at<uchar>(image.rows-1, 0);
                      else if ((y - (swindow/2) + j) > image.rows-1 && (x - (swindow/2) + i) > image.cols-1)
                        window[3*i+j] = image.at<uchar>(image.rows-1, image.cols-1);

                      gx += window[3*i+j] * xsobel[i][j];
                      gy += window[3*i+j] * ysobel[i][j];
                      gd += window[3*i+j] * dsobel[i][j];
                    }
                  }

                int window_sobel = abs(gx) + abs(gy);
                int xwindow_sobel = abs(gx);
                int ywindow_sobel = abs(gy);
                int dwindow_sobel = abs(gd);

                window_sobel = (window_sobel > 255) ? 255 : window_sobel;
                window_sobel = (window_sobel < 0) ? 0 : window_sobel;

                xwindow_sobel = (xwindow_sobel > 255) ? 255 : xwindow_sobel;
                xwindow_sobel = (xwindow_sobel < 0) ? 0 : xwindow_sobel;

                ywindow_sobel = (ywindow_sobel > 255) ? 255 : ywindow_sobel;
                ywindow_sobel = (ywindow_sobel < 0) ? 0 : ywindow_sobel;

                dwindow_sobel = (dwindow_sobel > 255) ? 255 : dwindow_sobel;
                dwindow_sobel = (dwindow_sobel < 0) ? 0 : dwindow_sobel;

                // assign the obtained value to centered element of the matrix
                fimage.at<uchar>(y,x) = window_sobel;
                fimagex.at<uchar>(y,x) = xwindow_sobel;
                fimagey.at<uchar>(y,x) = ywindow_sobel;
                fimaged.at<uchar>(y,x) = dwindow_sobel;
              }

          }


        if(filter_type == 7)
        	return fimage;
        else if(filter_type == 4)
        	return fimagex;
        else if(filter_type == 5)
        	return fimagey;
        else if(filter_type == 6)
        	return fimaged;
        //namedWindow("Sobel Filter Output");
        //imshow("Sobel Filter Output", fimage);
 
        //namedWindow("Sobel Filter Output Horizontal");
        //imshow("Sobel Filter Output Horizontal", fimagex);

        //namedWindow("Sobel Filter Output Vertical");
        //imshow("Sobel Filter Output Vertical", fimagey);

        //namedWindow("Sobel Filter Output Diagonal");
        //imshow("Sobel Filter Output Diagonal", fimaged);
}



//Filtering function which handles the input from tracbar  which are image, filter_type, neighbourhood size and displays input and output image in a window
void filtering(int,void*)
{
	if(nsize%2 == 1 && nsize >1)
	{
		Mat outImage;
		string image_loc = directory + img_name[image];
		Mat image;
    	image = imread(image_loc, CV_LOAD_IMAGE_GRAYSCALE);   // Read the file

    	if(! image.data )                              // Check for invalid input
    	{
        	cout <<  "Could not open or find the image" << std::endl ;
        	return ;
    	}

    	int wsize = nsize*nsize;

    	//Operation on input image
    	if(filter_type == 0)
    		outImage = MeanFilter(image,wsize);
    	else if(filter_type == 1)
    		outImage = MedianFilter(image, wsize);
    	else if(filter_type == 2)
    		outImage = GradientFilter(image, wsize);
    	else if(filter_type == 3)
    		outImage = LaplacianFilter(image, wsize);
    	else if(filter_type == 4)
    		outImage = SobelFilter(image, wsize);
    	else if(filter_type == 5)
    		outImage = SobelFilter(image, wsize);
    	else if(filter_type == 6)
    		outImage = SobelFilter(image, wsize);
    	else if(filter_type == 7)
    		outImage = SobelFilter(image, wsize);

    	displayimages(image, outImage);
	
	}

}

/**
 * @function main
 */
int main( int argc, char** argv )
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
				img_name[max_image++] = s1;  //Stores  name of .jpg files in the array
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

  /// Create a window to display results
  namedWindow( window_name, CV_WINDOW_AUTOSIZE );

  /// Create Trackbar to choose type of Threshold
  createTrackbar( trackbar_image, window_name, &image, max_image-1, filtering );
  createTrackbar( trackbar_filter, window_name, &filter_type, max_type, filtering );
  createTrackbar( trackbar_nsize,window_name, &nsize, max_nsize, filtering );
  
  filtering(0,0);

  /// Wait until user finishes program
  while(true)
  {
    int c;
    c = waitKey( 20 );
    if( (char)c == 27 )
      { break; }
   }

}

