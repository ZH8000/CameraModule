#ifndef __CALIBRATOR_WINDOW_H__
#define __CALIBRATOR_WINDOW_H__

#include <opencv2/opencv.hpp>
#include "LineFunction.hpp"
#include <iostream>

using namespace std;
using namespace cv;

class CalibratorWindow {

public:
    CalibratorWindow(const string calibratorWindow, const string imageWindow, const int maxWidth, const int maxHeight, char * camearName);
    void drawBoundary(Mat & image);
    void showWindow();

    bool isInsideBoundary(Point point);
    bool isUnderTopLine(Point point);
    bool isAboveBottomLine(Point point);
    bool isAboveLeftLine(Point point);
    bool isUnderRightLine(Point point);

    inline LineFunction getTopLineFunction()    { return LineFunction(Point(0, topLineY1), Point(maxWidth, topLineY2)); }
    inline LineFunction getBottomLineFunction() { return LineFunction(Point(0, bottomLineY1), Point(maxWidth, bottomLineY2)); }
    inline LineFunction getLeftLineFunction()   { return LineFunction(Point(leftLineX1, 0), Point(leftLineX2, maxWidth)); }
    inline LineFunction getRightLineFunction()  { return LineFunction(Point(rightLineX1, 0), Point(rightLineX2, maxWidth)); }
    inline int getFeatureMinHessian() {return this->featureMinHessian;}
    inline int getMinKeyPointSize() { return this->keyPointSizeThreshold; }
    inline int getMaxAngleDiff() { return this->angleThreshold; }
    inline int getMinKeyPoint() { return this->keyPointCountThreshold; }

private:

    string calibratorWindow;
    string imageWindow;
    char * cameraName;
    int maxWidth;
    int maxHeight;

    int topLineY1;
    int topLineY2;
    int bottomLineY1;
    int bottomLineY2;
    int leftLineX1;
    int leftLineX2;
    int rightLineX1;
    int rightLineX2;
    int angleThreshold;
    int margin;
    int proturdingThreshold;
    int featureMinHessian;
    int keyPointSizeThreshold;
    int keyPointCountThreshold;

    void addTrackbarToWindow();
    static void onCalibratorChange(int value, void * userData);
    char * getConfigFilename();


};

#endif
