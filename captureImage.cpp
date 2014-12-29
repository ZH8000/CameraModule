#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <gsl/gsl_fit.h>

#include "CalibratorWindow.hpp"

#include <string>
#include <sstream>
#include <iostream>

#define MAX_WIDTH   320
#define MAX_HEIGHT  240

using namespace cv;
using namespace std;

class FeatureProcessor {

public:
    FeatureProcessor(Mat * image, CalibratorWindow * calibrator);
    int getFeatureCount(int featureThreshold);
    void drawFeatureCount(int featureCount);
    vector<KeyPoint> getValidKeyPoints();
    LineFunction getFitLine(vector<KeyPoint> keyPoints);

private:

    CalibratorWindow * calibrator;
    Mat * image;
    Mat grayImage;

};

FeatureProcessor::FeatureProcessor(Mat * image, CalibratorWindow * calibrator) {
    this->image = image;
    this->calibrator = calibrator;
    cvtColor(*image, grayImage, CV_BGR2GRAY);
}

vector<KeyPoint> FeatureProcessor::getValidKeyPoints() {
    SurfFeatureDetector detector(calibrator->getFeatureMinHessian());
    vector<KeyPoint> keyPoints;
    vector<KeyPoint> keyPointsInBounds;

    detector.detect(*image, keyPoints);

    for(vector<KeyPoint>::iterator it = keyPoints.begin(); it != keyPoints.end(); it++) {
        if (calibrator->isInsideBoundary(it->pt) && it->size <= 30) {
            keyPointsInBounds.push_back(*it);
        }
    }

    return keyPointsInBounds;
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
    putText(*image, messageString, Point(5, 120), CV_FONT_HERSHEY_PLAIN, 1, CV_RGB(255, 0, 0));
}

LineFunction FeatureProcessor::getFitLine(vector<KeyPoint> keyPoints) {
    int validPointCount = keyPoints.size();

    double * xOfPoints = (double *) malloc(sizeof(double) * validPointCount);
    double * yOfPoints = (double *) malloc(sizeof(double) * validPointCount);
    double * weights = (double *) malloc(sizeof(double) * validPointCount);
    int counter = 0;

    for(vector<KeyPoint>::iterator point = keyPoints.begin(); point != keyPoints.end(); point++) {
        xOfPoints[counter] = point->pt.x;
        yOfPoints[counter] = point->pt.y;
        weights[counter] = 10.0 / point->size;
        counter++;
    }

    double c0, slope, cov00, cov01, cov11, sumsq;

    gsl_fit_wlinear (
        yOfPoints, 1, weights, 1, xOfPoints, 1, 
        validPointCount, &c0, &slope, &cov00, &cov01, &cov11, &sumsq
    );

    return LineFunction::fromInverseXY(slope, c0);
}

int main(int, char**)
{
    VideoCapture cap(1); // open the default camera

    if(!cap.isOpened())  // check if we succeeded
        return -1;

    namedWindow("Preview",1);
    CalibratorWindow calibrator("Calibrator", "Preview", MAX_WIDTH, MAX_HEIGHT);
    calibrator.showWindow();

    for(;;)
    {
        Mat frame;
        Mat resizedImage;
        Mat grayImage;
        Mat imageWithKeyPoints;

        cap >> frame;

        resize(frame, resizedImage, Size(MAX_WIDTH, MAX_HEIGHT));

        FeatureProcessor processor(&resizedImage, &calibrator);
        int featureCount = processor.getFeatureCount(500);
        processor.drawFeatureCount(featureCount);

        calibrator.drawBoundary(resizedImage);
        vector<KeyPoint> keyPoints = processor.getValidKeyPoints();
        drawKeypoints(resizedImage, keyPoints, imageWithKeyPoints, CV_RGB(255, 0, 0), DrawMatchesFlags::DEFAULT);
        LineFunction fitLine = processor.getFitLine(keyPoints);
        fitLine.drawSlopeInfo(imageWithKeyPoints);

        imshow("Preview", imageWithKeyPoints);
        waitKey(30);
    }

    return 0;
}
