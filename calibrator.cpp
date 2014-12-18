#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
#include "LineFunction.hpp"
#include <iostream>

using namespace cv;
using namespace std;

#define MAX_WIDTH       640
#define MAX_HEIGHT      480
#define CALIBRATOR_WINDOW      "Calibrator"
#define IMAGE_WINDOW           "Image"

class CalibratorWindow {

public:
    CalibratorWindow(const string calibratorWindow, const string imageWindow, const int maxWidth, const int maxHeight);
    void drawBoundary(Mat & image);
    void showWindow();
    inline LineFunction getTopLineFunction()    { return LineFunction(Point(0, topLineY1), Point(maxWidth, topLineY2)); }
    inline LineFunction getBottomLineFunction() { return LineFunction(Point(0, bottomLineY1), Point(maxWidth, bottomLineY2)); }
    inline LineFunction getLeftLineFunction()   { return LineFunction(Point(leftLineX1, 0), Point(leftLineX2, maxWidth)); }
    inline LineFunction getRightLineFunction()  { return LineFunction(Point(rightLineX1, 0), Point(rightLineX2, maxWidth)); }

private:

    string calibratorWindow;
    string imageWindow;
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

    void addTrackbarToWindow();
    static void onCalibratorChange(int value, void * userData);

};

void CalibratorWindow::onCalibratorChange(int value, void * userData) {

    CalibratorWindow * calibrator = (CalibratorWindow *) userData;

    cout << "================================================" << "\n";
    cout << "TopBottom: " << calibrator->getTopLineFunction() << "\t\t" << calibrator->getBottomLineFunction() << "\n";
    cout << "LeftRight: " << calibrator->getLeftLineFunction() << "\t\t" << calibrator->getRightLineFunction() << "\n";
    cout << "================================================" << "\n";
}

CalibratorWindow::CalibratorWindow(const string calibratorWindow, const string imageWindow, const int maxWidth, const int maxHeight) {
    this->calibratorWindow = calibratorWindow;
    this->imageWindow = imageWindow;
    this->maxWidth = maxWidth;
    this->maxHeight = maxHeight;
}

void CalibratorWindow::showWindow() {
    cv::namedWindow(CALIBRATOR_WINDOW);
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

}

void CalibratorWindow::drawBoundary(Mat & image) {
    cv::line(image, Point(0, topLineY1), Point(maxWidth, topLineY2), Scalar(0, 0, 255), 1.5);
    cv::line(image, Point(0, bottomLineY1), Point(maxWidth, bottomLineY2), Scalar(0, 255, 255), 1.5);
    cv::line(image, Point(leftLineX1, 0), Point(leftLineX2, maxWidth), Scalar(255, 0, 255), 1.5);
    cv::line(image, Point(rightLineX1, 0), Point(rightLineX2, maxWidth), Scalar(255, 255, 0), 1.5);
}

int main(int argc, char ** argv) {

    CalibratorWindow calibratorWindow(CALIBRATOR_WINDOW, IMAGE_WINDOW, MAX_WIDTH, MAX_HEIGHT);
    Mat image = imread("/mnt/WinD/Dropbox/Photos/Win8Login.jpg");
    calibratorWindow.showWindow();
    cv::namedWindow(IMAGE_WINDOW);

    while (true) {
      Mat resized;
      cv::resize(image, resized, Size(MAX_WIDTH, MAX_HEIGHT));
      calibratorWindow.drawBoundary(resized);
      cv::imshow(IMAGE_WINDOW, resized);
      cv::waitKey(100);
    }
}
