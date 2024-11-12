#include "EdgeDetection.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

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

    //// Draw the detected circles on a copy of the edge-detected image
    //cv::Mat displayImage = _edgeImage.clone();
    //cv::cvtColor(displayImage, displayImage, cv::COLOR_GRAY2BGR); // Convert to color to display circles

    //for (const auto& circle : circles)
    //{
    //    cv::Point center(cvRound(circle[0]), cvRound(circle[1]));
    //    int radius = cvRound(circle[2]);
    //    std::cout << "The radius of this circle is: " << radius << std::endl; 
    //    cv::circle(displayImage, center, radius, cv::Scalar(0, 255, 0), 2);  // Draw the circle
    //    cv::circle(displayImage, center, 2, cv::Scalar(0, 0, 255), 3);      // Draw the center
    //}

    // Show the image with circles
    //cv::imshow("Detected Circles", displayImage);

    return circles; // Return the list of detected circles
}


void EdgeDetection::processVideo(const std::string& videoPath, const std::string& outputPath)
{
    cv::VideoCapture cap(videoPath);
    if (!cap.isOpened())
    {
        std::cerr << "Error: Could not open video file!" << std::endl;
        return;
    }

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

    cv::Mat frame;
    while (true)
    {
        cap >> frame;  // Capture frame-by-frame
        if (frame.empty()) break;  // End of video

        // Apply edge detection
        detectEdges(frame);

        // Detect circles in the edge-detected frame
        std::vector<cv::Vec3f> circles = detectCircles(1.5, 20, 100, 60, 30, 80);

        // Create a display image to show detected edges and circles
        cv::Mat displayImage = frame.clone();
        for (const auto& circle : circles)
        {
            cv::Point center(cvRound(circle[0]), cvRound(circle[1]));
            int radius = cvRound(circle[2]);
            cv::circle(displayImage, center, radius, cv::Scalar(0, 255, 0), 2);
            cv::circle(displayImage, center, 2, cv::Scalar(0, 0, 255), 3);
        }

        // Display the processed frame (optional)
        cv::imshow("Processed Video", displayImage);

        // Write the processed frame to the output video
        videoWriter.write(displayImage);

        // Press 'q' to exit early
        if (cv::waitKey(10) == 'q') break;
    }

    cap.release();
    videoWriter.release();
    cv::destroyAllWindows();
}
