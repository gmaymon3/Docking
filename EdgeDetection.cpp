#include "EdgeDetection.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>


EdgeDetection::EdgeDetection(double lowThreshold, double highThreshold)
{
    _lowThreshold = lowThreshold;
    _highThreshold = highThreshold;
}

void EdgeDetection::detectEdges(const cv::Mat& inputImage)
{
    // Check if input image is empty
    if (inputImage.empty())
    {
        std::cerr << "Error: The input image is empty!" << std::endl;
        return;
    }

    // Convert the input image to grayscale
    cv::Mat grayImage;
    cv::cvtColor(inputImage, grayImage, cv::COLOR_BGR2GRAY);

    // Apply Gaussian blur to reduce noise
    cv::Mat blurredImage;
    cv::GaussianBlur(grayImage, blurredImage, cv::Size(5, 5), 1.5);

    // Perform Canny edge detection
    cv::Canny(blurredImage, _edgeImage, _lowThreshold, _highThreshold);
}

cv::Mat EdgeDetection::getEdgeImage() const
{
    return _edgeImage;
}

void EdgeDetection::showImages() const
{
    if (!_edgeImage.empty())
    {
        cv::imshow("Edge Detection", _edgeImage);
    }
    else
    {
        std::cerr << "Error: No edge image to display!" << std::endl;
    }
}

// Method to detect circles in the edge-detected image
std::vector<cv::Vec3f> EdgeDetection::detectCircles(double dp, double minDist, double param1, double param2, int minRadius, int maxRadius)
{
    std::vector<cv::Vec3f> circles; // To store the detected circles

    if (_edgeImage.empty())
    {
        std::cerr << "Error: Edge image is empty!" << std::endl;
        return circles;
    }

    // Perform Hough Circle Transform on the edge-detected image
    cv::HoughCircles(_edgeImage, circles, cv::HOUGH_GRADIENT, dp, minDist, param1, param2, minRadius, maxRadius);

    return circles; // Return the list of detected circles
}


// Method to output circles in the edge-detected image
void EdgeDetection::outputCircles(double dp, double minDist, double param1, double param2, int minRadius, int maxRadius)
{
    std::vector<cv::Vec3f> circles; // To store the detected circles

    if (_edgeImage.empty())
    {
        std::cerr << "Error: No edge image to display!" << std::endl;
    }

    // Perform Hough Circle Transform on the edge-detected image
    cv::HoughCircles(_edgeImage, circles, cv::HOUGH_GRADIENT, dp, minDist, param1, param2, minRadius, maxRadius);
    // Create a display image to show detected edges and circles
    
    cv::Mat displayImage = _edgeImage.clone();
    cv::cvtColor(displayImage, displayImage, cv::COLOR_GRAY2BGR); // Convert to color to display circles
    for (const auto& circle : circles)
    {
        cv::Point center(cvRound(circle[0]), cvRound(circle[1]));
        int radius = cvRound(circle[2]);
        cv::circle(displayImage, center, radius, cv::Scalar(0, 255, 0), 2);
        cv::circle(displayImage, center, 2, cv::Scalar(0, 0, 255), 3);
        std::cout << "The circle has a radius of: " << radius << std::endl;
    }

    // Display the processed frame (optional)
    cv::imshow("Processed Pic", displayImage);
}

static std::vector<double> linspace(double start, double end, int num) {
    std::vector<double> result;

    if (num <= 0) {
        return result; // Return empty vector if num is not positive
    }
    if (num == 1) {
        result.push_back(start); // Return single value if num is 1
        return result;
    }

    double step = (end - start) / (num - 1);
    for (int i = 0; i < num; ++i) {
        result.push_back(start + i * step);
    }
    return result;
}

void EdgeDetection::processVideo(const std::string& videoPath, const std::string& outputPath)
{
    cv::VideoCapture cap(videoPath);
    if (!cap.isOpened())
    {
        std::cerr << "Error: Could not open video file!" << std::endl;
        return;
    }

    // Get the frame width and height from the input video
    int frame_width = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int frame_height = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    int fps = static_cast<int>(cap.get(cv::CAP_PROP_FPS));

    // Initialize VideoWriter to save the output video
    cv::VideoWriter videoWriter(outputPath, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, cv::Size(frame_width, frame_height));
    
    if (!videoWriter.isOpened())
    {
        std::cerr << "Error: Could not open video writer!" << std::endl;
        return;
    }

    // RPO Telemetry - This is the expected RPO distance between the 2 spacecraft. It can help determine which circle is the correct docking ring. 
    // This is assuming we get RPO (Rendevous/Docking Operation) data at the same rate as camera data. 
    int start_dist_meter = 50; 
    int end_dist_meter = 0; 
    int total_frames = 153; 
    auto distance_meters = linspace(start_dist_meter, end_dist_meter, total_frames);

    // Loop through each frame of the video
    cv::Mat frame;
    int frameCount = 0; 
    double max_diam_at_start = 75; 
    double max_diam_at_dock = 140;
    auto max_diam_curr = 75.0;
    auto min_diam_curr = 65.0;

    while (true)
    {
        cap >> frame;  // Capture frame-by-frame
        if (frame.empty()) break;  // End of video

        // Apply edge detection
        detectEdges(frame);

        // Convert spacecraft distance to expected ring size. 
        frameCount++;
        max_diam_curr = max_diam_at_start + (distance_meters[(total_frames-frameCount)] - end_dist_meter) * (max_diam_at_dock - max_diam_at_start) / (start_dist_meter - end_dist_meter);
        min_diam_curr = max_diam_curr - 15; 
        //std::cout << "The min diam is: " << min_diam_curr << ", The max diam is: " << max_diam_curr << std::endl;
        //int min_diam_curr_int = static_cast<int>(min_diam_curr);
        //int max_diam_curr_int = static_cast<int>(max_diam_curr);

        // Detect circles in the edge-detected frame
        std::vector<cv::Vec3f> circles = detectCircles(1.5, 20, 100, 60, static_cast<int>(min_diam_curr), static_cast<int>(max_diam_curr));

        // Create a display image to show detected edges and circles
        cv::Mat displayImage = frame.clone();
        for (const auto& circle : circles)
        {
            cv::Point center(cvRound(circle[0]), cvRound(circle[1]));
            int radius = cvRound(circle[2]);
            cv::circle(displayImage, center, radius, cv::Scalar(0, 255, 0), 2);
            cv::circle(displayImage, center, 2, cv::Scalar(0, 0, 255), 3);
            std::cout << "The circle has a radius of: " << radius << std::endl; 
        }

        // Display the processed frame (optional)
        cv::imshow("Processed Video", displayImage);

        // Write the processed frame to the output video
        videoWriter.write(displayImage);

        // Press 'q' to exit early
        if (cv::waitKey(10) == 'q') break;
    }
    //std::cout << "The total number of frames is: " << frameCount << std::endl;

    cap.release();
    videoWriter.release();
    cv::destroyAllWindows();
}
