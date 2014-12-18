#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
#include "CalibratorWindow.hpp"

#define MAX_WIDTH       640
#define MAX_HEIGHT      480
#define CALIBRATOR_WINDOW      "Calibrator"
#define IMAGE_WINDOW           "Image"

using namespace cv;
using namespace std;

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
