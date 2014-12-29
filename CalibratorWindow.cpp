#include "CalibratorWindow.hpp"



bool CalibratorWindow::isInsideBoundary(Point point) {
    return isAboveLeftLine(point) && isUnderRightLine(point) &&
           isUnderTopLine(point) && isUnderTopLine(point);
}

bool CalibratorWindow::isAboveLeftLine(Point point) {
    if (leftLineX1 == leftLineX2) {
        return point.x >= (leftLineX1 + margin);
    }
    else {
        LineFunction lineFunction = getLeftLineFunction();
        return point.x >= ((point.y - (lineFunction.c)) / lineFunction.ax) + margin;
    }
}

bool CalibratorWindow::isUnderRightLine(Point point) {
    if (rightLineX1 == rightLineX2) {
        return point.x <= (rightLineX1 - margin);
    }
    else {
        LineFunction lineFunction = getRightLineFunction();
        return point.x <= ((point.y - (lineFunction.c)) / lineFunction.ax) - margin;
    }
}

bool CalibratorWindow::isUnderTopLine(Point point) {
    
    if (topLineY1 == topLineY2) { 
        return point.y >= topLineY1 + margin; 
    }
    else {
        LineFunction lineFunction = getTopLineFunction();
        return point.y >= ((lineFunction.ax * point.x + lineFunction.c) + margin);
    }
}

bool CalibratorWindow::isAboveBottomLine(Point point) {
    
    if (topLineY1 == topLineY2) { 
        return point.y <= topLineY1 - margin; 
    }
    else {
        LineFunction lineFunction = getTopLineFunction();
        return point.y <= ((lineFunction.ax * point.x + lineFunction.c) - margin);
    }
}

void CalibratorWindow::onCalibratorChange(int value, void * userData) {

    CalibratorWindow * calibrator = (CalibratorWindow *) userData;

    /*
    cout << "================================================" << "\n";
    cout << "TopBottom: " << calibrator->getTopLineFunction() << "\t\t" << calibrator->getBottomLineFunction() << "\n";
    cout << "LeftRight: " << calibrator->getLeftLineFunction() << "\t\t" << calibrator->getRightLineFunction() << "\n";
    cout << "================================================" << "\n";
    */
}

CalibratorWindow::CalibratorWindow(const string calibratorWindow, const string imageWindow, const int maxWidth, const int maxHeight) {
    this->calibratorWindow = calibratorWindow;
    this->imageWindow = imageWindow;
    this->maxWidth = maxWidth;
    this->maxHeight = maxHeight;
}

void CalibratorWindow::showWindow() {
    cv::namedWindow(calibratorWindow);
    this->addTrackbarToWindow();
}

void CalibratorWindow::addTrackbarToWindow() {
    this->topLineY1 = 0;
    this->topLineY2 = 0;
    this->bottomLineY1 = maxHeight;
    this->bottomLineY2 = maxHeight;
    this->leftLineX1 = 0;
    this->leftLineX2 = 0;
    this->rightLineX1 = maxWidth;
    this->rightLineX2 = maxWidth;
    this->angleThreshold = 10;
    this->margin = 0;
    this->proturdingThreshold = 0;
    this->featureMinHessian = 800;
    this->keyPointSizeTreshold = 30;

    createTrackbar("TopLine Y1", calibratorWindow, &(this->topLineY1), maxHeight, &onCalibratorChange, this);
    createTrackbar("TopLine Y2", calibratorWindow, &(this->topLineY2), maxHeight, &onCalibratorChange, this);
    createTrackbar("BottomLine Y1", calibratorWindow, &(this->bottomLineY1), maxHeight, &onCalibratorChange, this);
    createTrackbar("BottomLine Y2", calibratorWindow, &(this->bottomLineY2), maxHeight, &onCalibratorChange, this);
    createTrackbar("LeftLine X1", calibratorWindow, &(this->leftLineX1), maxWidth, &onCalibratorChange, this);
    createTrackbar("LeftLine X2", calibratorWindow, &(this->leftLineX2), maxWidth, &onCalibratorChange, this);
    createTrackbar("RightLine X1", calibratorWindow, &(this->rightLineX1), maxWidth, &onCalibratorChange, this);
    createTrackbar("RightLine X2", calibratorWindow, &(this->rightLineX2), maxWidth, &onCalibratorChange, this);
    createTrackbar("Angle Threshold", calibratorWindow, &(this->angleThreshold), 90);
    createTrackbar("Inside Margin", calibratorWindow, &(this->margin), 100);
    createTrackbar("Proturding Threshold", calibratorWindow, &(this->proturdingThreshold), 100);
    createTrackbar("Feature Min Hessian", calibratorWindow, &(this->featureMinHessian), 2000);
    createTrackbar("Key Point Size Treshold", calibratorWindow, &(this->keyPointSizeTreshold), 200);

}

void CalibratorWindow::drawBoundary(Mat & image) {
    cv::line(image, Point(0, topLineY1), Point(maxWidth, topLineY2), Scalar(0, 0, 255), 1.5);
    cv::line(image, Point(0, bottomLineY1), Point(maxWidth, bottomLineY2), Scalar(0, 255, 255), 1.5);
    cv::line(image, Point(leftLineX1, 0), Point(leftLineX2, maxWidth), Scalar(255, 0, 255), 1.5);
    cv::line(image, Point(rightLineX1, 0), Point(rightLineX2, maxWidth), Scalar(255, 255, 0), 1.5);
}

