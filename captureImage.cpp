#include <opencv2/opencv.hpp>
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/features2d.hpp"

#include <string>
#include <sstream>
#include <iostream>

using namespace cv;
using namespace std;

class FeatureProcessor {

public:
    FeatureProcessor(Mat * image);
    int getFeatureCount(int featureThreshold);
    void drawFeatureCount(int featureCount);

private:

    Mat * image;
    Mat grayImage;

};

FeatureProcessor::FeatureProcessor(Mat * image) {
    this->image = image;
    cvtColor(*image, grayImage, CV_BGR2GRAY);
}

int FeatureProcessor::getFeatureCount(int featureThreshold) {
    SurfFeatureDetector detector(featureThreshold);
    vector<KeyPoint> keyPoints;
    detector.detect(*image, keyPoints);
    return keyPoints.size();
}

void FeatureProcessor::drawFeatureCount(int featureCount) {

    std::stringstream message;
    message << "f:" << featureCount;
    string messageString = message.str();
    putText(*image, messageString, Point(0, 100), FONT_HERSHEY_DUPLEX, 0.5, Scalar(0, 0, 255));
}


int main(int, char**)
{
    VideoCapture cap(0); // open the default camera

    if(!cap.isOpened())  // check if we succeeded
        return -1;

    namedWindow("Preview",1);

    for(;;)
    {
        Mat frame;
        Mat resizedImage;
        Mat grayImage;
        cap >> frame;

        resize(frame, resizedImage, Size(320, 240));

        FeatureProcessor processor(&resizedImage);
        int featureCount = processor.getFeatureCount(500);
        processor.drawFeatureCount(featureCount);

        imshow("Preview", resizedImage);

        if(waitKey(30) >= 0) break;
    }

    return 0;
}
