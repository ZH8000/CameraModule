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
    void drawFeatureCount(int featureCount);
    vector<KeyPoint> getValidKeyPoints();
    LineFunction getFitLine(vector<KeyPoint> keyPoints);
    void drawAngleInfo(double angle);
    void drawSlopeInfo(LineFunction & fitLine);
    void drawBoundary();
    void drawKeyPoints();

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

    detector.detect(grayImage, keyPoints);

    for(vector<KeyPoint>::iterator it = keyPoints.begin(); it != keyPoints.end(); it++) {
        if (calibrator->isUnderTopLine(it->pt) &&
            calibrator->isAboveLeftLine(it->pt) && 
            calibrator->isUnderRightLine(it->pt) &&
            it->size <= calibrator->getMinKeyPointSize()) {
            keyPointsInBounds.push_back(*it);
        }
    }

    return keyPointsInBounds;
}

void FeatureProcessor::drawFeatureCount(int featureCount) {

    std::stringstream message;
    message << "f:" << featureCount;
    string messageString = message.str();
    putText(*image, messageString, Point(5, 100), CV_FONT_HERSHEY_PLAIN, 1, CV_RGB(255, 0, 0));
}

void FeatureProcessor::drawSlopeInfo(LineFunction & fitLine) {
    int startPointX = 80;
    int startPointY = 120;
    int endPointX = 200;
    int endPointY = (int) (startPointY + (fitLine.inverseXYSlope * (endPointX - startPointX)));

    if (!isnan(fitLine.inverseXYSlope)) {
      cv::line(
          *image,
          cvPoint(startPointY, startPointX),
          cvPoint(endPointY, endPointX),
          CV_RGB(255,255,0), 2
      );
    }

    std::stringstream message;
    message << "slope:" << fitLine.inverseXYSlope;
    string messageString = message.str();
    
    putText(*image, messageString, Point(5, 115), CV_FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 255));
}

void FeatureProcessor::drawAngleInfo(double angle) {
    std::stringstream message;
    message << "angle:" << angle;
    string messageString = message.str();
    putText(*image, messageString, Point(5, 130), CV_FONT_HERSHEY_PLAIN, 1, CV_RGB(255, 0, 0));
}

void FeatureProcessor::drawBoundary() {
    calibrator->drawBoundary(*image);
}

void FeatureProcessor::drawKeyPoints() {
    SurfFeatureDetector detector(calibrator->getFeatureMinHessian());
    vector<KeyPoint> keyPoints;

    detector.detect(grayImage, keyPoints);

    for(vector<KeyPoint>::iterator it = keyPoints.begin(); it != keyPoints.end(); it++) {
        int radius = cvRound(it->size*1.2/9.*2);
        Point center = it->pt;

        if (it->size <= calibrator->getMinKeyPointSize()) {
            if (calibrator->isAboveLeftLine(it->pt) && calibrator->isUnderRightLine(it->pt)) {
                if (calibrator->isUnderTopLine(it->pt)) {
                    circle(*image, center, radius, CV_RGB(255, 0, 0));
                } else {
                    circle(*image, center, radius, CV_RGB(0, 255, 0));
                }
            } else {
                circle(*image, center, radius, CV_RGB(0, 0, 255));
            }
        } else {
            //circle(image, center, radius, CV_RGB(255, 0, 255));
        }
    }
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
        vector<KeyPoint> keyPoints = processor.getValidKeyPoints();

        LineFunction fitLine = processor.getFitLine(keyPoints);

        if (fitLine.isValidFitLine()) {
            LineFunction bottomLine = calibrator.getBottomLineFunction();
            double angle = bottomLine.getAngleWith(fitLine);
            double angleDiff = bottomLine.getAngleDiffWith(fitLine);
            processor.drawAngleInfo(angle);
        }

        processor.drawBoundary();
        processor.drawFeatureCount(keyPoints.size());
        processor.drawSlopeInfo(fitLine);
        processor.drawKeyPoints();

        imshow("Preview", resizedImage);
        waitKey(30);
    }

    return 0;
}
