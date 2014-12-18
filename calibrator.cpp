#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>

using namespace cv;

#define MAX_WIDTH       640
#define MAX_HEIGHT      480
#define CALIBRATOR_WINDOW      "Calibrator"
#define IMAGE_WINDOW           "Image"

class CalibratorWindow {

public:
    void drawBoundary(Mat & image);
    void showWindow();

private:
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

};

void CalibratorWindow::showWindow() {
    cv::namedWindow(CALIBRATOR_WINDOW);
    this->addTrackbarToWindow();
}

void CalibratorWindow::addTrackbarToWindow() {
    this->topLineY1 = 0;
    this->topLineY2 = 0;
    this->bottomLineY1 = MAX_HEIGHT;
    this->bottomLineY2 = MAX_HEIGHT;
    this->leftLineX1 = 0;
    this->leftLineX2 = 0;
    this->rightLineX1 = MAX_WIDTH;
    this->rightLineX2 = MAX_WIDTH;
    this->angleThreshold = 10;
    this->margin = 0;
    this->proturdingThreshold = 0;

    createTrackbar("TopLine Y1", CALIBRATOR_WINDOW, &(this->topLineY1), MAX_HEIGHT);
    createTrackbar("TopLine Y2", CALIBRATOR_WINDOW, &(this->topLineY2), MAX_HEIGHT);
    createTrackbar("BottomLine Y1", CALIBRATOR_WINDOW, &(this->bottomLineY1), MAX_HEIGHT);
    createTrackbar("BottomLine Y2", CALIBRATOR_WINDOW, &(this->bottomLineY2), MAX_HEIGHT);
    createTrackbar("LeftLine X1", CALIBRATOR_WINDOW, &(this->leftLineX1), MAX_WIDTH);
    createTrackbar("LeftLine X2", CALIBRATOR_WINDOW, &(this->leftLineX2), MAX_WIDTH);
    createTrackbar("RightLine X1", CALIBRATOR_WINDOW, &(this->rightLineX1), MAX_WIDTH);
    createTrackbar("RightLine X2", CALIBRATOR_WINDOW, &(this->rightLineX2), MAX_WIDTH);
    createTrackbar("Angle Threshold", CALIBRATOR_WINDOW, &(this->angleThreshold), 90);
    createTrackbar("Inside Margin", CALIBRATOR_WINDOW, &(this->margin), 100);
    createTrackbar("Proturding Threshold", CALIBRATOR_WINDOW, &(this->proturdingThreshold), 100);

}

void CalibratorWindow::drawBoundary(Mat & image) {
    cv::line(image, Point(0, topLineY1), Point(MAX_WIDTH, topLineY2), Scalar(0, 0, 255), 1.5);
    cv::line(image, Point(0, bottomLineY1), Point(MAX_WIDTH, bottomLineY2), Scalar(0, 255, 255), 1.5);
    cv::line(image, Point(leftLineX1, 0), Point(leftLineX2, MAX_WIDTH), Scalar(255, 0, 255), 1.5);
    cv::line(image, Point(rightLineX1, 0), Point(rightLineX2, MAX_WIDTH), Scalar(255, 255, 0), 1.5);
}

int main(int argc, char ** argv) {

    CalibratorWindow calibratorWindow;
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
