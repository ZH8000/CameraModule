#ifndef __ERROR_DETECTOR_H__
#define __ERROR_DETECTOR_H__

#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <gsl/gsl_fit.h>

#include "CalibratorWindow.hpp"

#include <string>
#include <sstream>
#include <iostream>


using namespace cv;
using namespace std;

class ErrorDetector {

public:
    ErrorDetector(Mat * image, CalibratorWindow * calibrator);
    void drawFeatureCount(int featureCount);
    vector<KeyPoint> getValidKeyPoints();
    LineFunction getFitLine(vector<KeyPoint> keyPoints);
    void drawAngleInfo(double angle);
    void drawSlopeInfo(LineFunction & fitLine);
    void drawBoundary();
    void drawKeyPoints();
    double detectOblique(LineFunction fitLine);
    bool isOblique(LineFunction fitLine, int keyPointCount, double & angle, double & angleDiff);
    void drawTitle(char * title);

private:

    CalibratorWindow * calibrator;
    Mat * image;
    Mat grayImage;

};

#endif
