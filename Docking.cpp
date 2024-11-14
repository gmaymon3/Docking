// Docking Solution : This file contains the 'main' function. Program execution begins and ends there.

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "EdgeDetection.h"

using namespace cv; 
using namespace std; 


int main()
{
	// Load Image
	string image_path = "Resources/intial_shot.jpg"; 
	cout << image_path << endl; 
	Mat img = imread(image_path, IMREAD_COLOR);
	string label = "Docking Ring";
	imshow(label, img); 

    // Use Edge Detection Object to apply Gaussian blur and perform edge detection
    EdgeDetection edgeDetector(100,200); 
    edgeDetector.detectEdges(img);

	// Detect circles
	std::vector<cv::Vec3f> circles = edgeDetector.detectCircles(1.5,20,100,60,60,75);

	// Video of capture 
	 // Load Video
    string video_path = "Resources/Snipped_Docking_mid.mp4";

	cout << video_path << endl;
	EdgeDetection edgeDetectorVideo(100, 200);
	edgeDetectorVideo.processVideo(video_path, "output_video_with_circles.avi");

	waitKey(0);
}
