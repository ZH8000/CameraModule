#include "ErrorDetector.hpp"

ErrorDetector::ErrorDetector(Mat * image, CalibratorWindow * calibrator) {
    this->image = image;
    this->calibrator = calibrator;
    cvtColor(*image, grayImage, CV_BGR2GRAY);
}

void ErrorDetector::drawTitle(char * title) {
    std::stringstream message;
    message << "Camera: " << title;
    string messageString = message.str();
    putText(*image, messageString, Point(10, 20), CV_FONT_HERSHEY_PLAIN, 1, CV_RGB(255, 0, 0));

}

vector<KeyPoint> ErrorDetector::getValidKeyPoints() {
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

void ErrorDetector::drawFeatureCount(int featureCount) {

    std::stringstream message;
    message << "f:" << featureCount;
    string messageString = message.str();
    putText(*image, messageString, Point(5, 100), CV_FONT_HERSHEY_PLAIN, 1, CV_RGB(255, 0, 0));
}

void ErrorDetector::drawSlopeInfo(LineFunction & fitLine) {
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

void ErrorDetector::drawAngleInfo(double angle) {
    std::stringstream message;
    message << "angle:" << angle;
    string messageString = message.str();
    putText(*image, messageString, Point(5, 130), CV_FONT_HERSHEY_PLAIN, 1, CV_RGB(255, 0, 0));
}

void ErrorDetector::drawBoundary() {
    calibrator->drawBoundary(*image);
}

void ErrorDetector::drawKeyPoints() {
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

LineFunction ErrorDetector::getFitLine(vector<KeyPoint> keyPoints) {
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

bool ErrorDetector::isOblique(LineFunction fitLine, int keyPointCount, double & angle, double & angleDiff) {

    if (fitLine.isValidFitLine() && keyPointCount >= calibrator->getMinKeyPoint()) {
        LineFunction bottomLine = calibrator->getBottomLineFunction();
        angle = bottomLine.getAngleWith(fitLine);
        angleDiff = bottomLine.getAngleDiffWith(fitLine);
        return angleDiff >= calibrator->getMaxAngleDiff();
    } else {
        return false;
    }
}

