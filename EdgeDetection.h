#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>


class EdgeDetection
{
    public:
        // Constructor: Initializes the thresholds for edge detection
        EdgeDetection(double lowThreshold = 100.0, double highThreshold = 200.0);

        // Method to perform edge detection on an image
        void detectEdges(const cv::Mat& inputImage);

        // Method to get the edge-detected image
        cv::Mat getEdgeImage() const;

        // Method to show the images
        void showImages() const;

        // New method to detect circles
        std::vector<cv::Vec3f> detectCircles(double dp = 1.5, double minDist = 20,
            double param1 = 100, double param2 = 30,
            int minRadius = 0, int maxRadius = 0);

        void outputCircles(double dp, double minDist, double param1, double param2, int minRadius, int maxRadius);

        // New method to process a video file or stream and apply edge & circle detection
        void processVideo(const std::string& videoPath, const std::string& outputPath);

    private:
        double _lowThreshold;     // Lower threshold for Canny edge detection
        double _highThreshold;    // Upper threshold for Canny edge detection
        cv::Mat _edgeImage;       // Resulting image with edges
 };

