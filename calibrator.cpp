#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>

using namespace cv;

#define MAX_WIDTH       640
#define MAX_HEIGHT      480
#define CALIBRATOR_WINDOW      "Calibrator"
#define IMAGE_WINDOW           "Image"

class CalibratorBar {

public:
    void addTrackbarToWindow();
    void drawBoundary(Mat & image);

private:
    int topLineY1;
    int topLineY2;
    int bottomLineY1;
    int bottomLineY2;
    int leftLineX1;
    int leftLineX2;
    int rightLineX1;
    int rightLineX2;
};

void CalibratorBar::addTrackbarToWindow() {
    topLineY1 = 0;
    topLineY2 = 0;
    bottomLineY1 = MAX_HEIGHT;
    bottomLineY2 = MAX_HEIGHT;
    leftLineX1 = 0;
    leftLineX2 = 0;
    rightLineX1 = MAX_WIDTH;
    rightLineX2 = MAX_WIDTH;

    createTrackbar("TopLine Y1", CALIBRATOR_WINDOW, &(this->topLineY1), MAX_HEIGHT);
    createTrackbar("TopLine Y2", CALIBRATOR_WINDOW, &(this->topLineY2), MAX_HEIGHT);
    createTrackbar("BottomLine Y1", CALIBRATOR_WINDOW, &(this->bottomLineY1), MAX_HEIGHT);
    createTrackbar("BottomLine Y2", CALIBRATOR_WINDOW, &(this->bottomLineY2), MAX_HEIGHT);
    createTrackbar("LeftLine X1", CALIBRATOR_WINDOW, &(this->leftLineX1), MAX_WIDTH);
    createTrackbar("LeftLine X2", CALIBRATOR_WINDOW, &(this->leftLineX2), MAX_WIDTH);
    createTrackbar("RightLine X1", CALIBRATOR_WINDOW, &(this->rightLineX1), MAX_WIDTH);
    createTrackbar("RightLine X2", CALIBRATOR_WINDOW, &(this->rightLineX2), MAX_WIDTH);
}

void CalibratorBar::drawBoundary(Mat & image) {
    cv::line(image, Point(0, topLineY1), Point(MAX_WIDTH, topLineY2), Scalar(0, 0, 255), 1.5);
    cv::line(image, Point(0, bottomLineY1), Point(MAX_WIDTH, bottomLineY2), Scalar(0, 255, 255), 1.5);
    cv::line(image, Point(leftLineX1, 0), Point(leftLineX2, MAX_WIDTH), Scalar(255, 0, 255), 1.5);
    cv::line(image, Point(rightLineX1, 0), Point(rightLineX2, MAX_WIDTH), Scalar(255, 255, 0), 1.5);
}

int main(int argc, char ** argv) {
    

    CalibratorBar calibratorBar;
    Mat image = imread("/mnt/WinD/Dropbox/Photos/Win8Login.jpg");
    cv::namedWindow(IMAGE_WINDOW);
    cv::namedWindow(CALIBRATOR_WINDOW);
    calibratorBar.addTrackbarToWindow();

    while (true) {
      Mat resized;
      cv::resize(image, resized, Size(MAX_WIDTH, MAX_HEIGHT));
      calibratorBar.drawBoundary(resized);
      cv::imshow(IMAGE_WINDOW, resized);
      cv::waitKey(100);
    }
}
