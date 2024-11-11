// FacialRecognition.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "EdgeDetection.h"

using namespace cv; 
using namespace std; 


int main()
{
    cout << "Hello World!\n";
	// Load Image
	string image_path = "Resources/Capture_pic.png"; 
	cout << image_path << endl; 
	Mat img = imread(image_path, IMREAD_COLOR);
	string label = "Docking Ring";
	imshow(label, img); 

    // Use Edge Detection Object to apply Gaussian blur and perform edge detection
    EdgeDetection edgeDetector(100,200); 
    edgeDetector.detectEdges(img); 

	// Show image
    edgeDetector.showImages(); 
	waitKey(0);
}
