#include "CalibratorWindow.hpp"
#include <iostream>
#include <fstream>
#include <string>

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

char * CalibratorWindow::getConfigFilename() {
    const char * cameraName = this->cameraName;
    char * filename = (char *) malloc(sizeof(char) * (strlen("config.txt-") + strlen(cameraName) + 1)); 
    strcpy(filename, "config-");
    strcat(filename, cameraName);
    strcat(filename, ".txt");
    return filename;
}


void CalibratorWindow::onCalibratorChange(int value, void * userData) {

    CalibratorWindow * calibrator = (CalibratorWindow *) userData;
    char * filename = calibrator->getConfigFilename();

    ofstream configFile(filename, ios::out|ios::trunc);
    configFile << calibrator->topLineY1 << endl;
    configFile << calibrator->topLineY2 << endl;
    configFile << calibrator->bottomLineY1 << endl;
    configFile << calibrator->bottomLineY2 << endl;
    configFile << calibrator->leftLineX1 << endl;
    configFile << calibrator->leftLineX2 << endl;
    configFile << calibrator->rightLineX1 << endl;
    configFile << calibrator->rightLineX2 << endl;
    configFile << calibrator->angleThreshold << endl;
    configFile << calibrator->margin << endl;
    configFile << calibrator->proturdingThreshold << endl;
    configFile << calibrator->featureMinHessian << endl;
    configFile << calibrator->keyPointSizeThreshold << endl;
    configFile << calibrator->keyPointCountThreshold << endl;

    configFile.close();
    free(filename);
}

CalibratorWindow::CalibratorWindow(const string calibratorWindow, const string imageWindow, const int maxWidth, const int maxHeight, const char * cameraName) {
    this->calibratorWindow = calibratorWindow;
    this->imageWindow = imageWindow;
    this->maxWidth = maxWidth;
    this->maxHeight = maxHeight;
    this->cameraName = cameraName;
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
    this->keyPointSizeThreshold = 30;

    char * filename = this->getConfigFilename();
    ifstream configFile(filename);
    string line;

    if (configFile.is_open()) {
        getline(configFile, line);
        this->topLineY1 = atoi(line.c_str());

        getline(configFile, line);
        this->topLineY2 = atoi(line.c_str());

        getline(configFile, line);
        this->bottomLineY1 = atoi(line.c_str());

        getline(configFile, line);
        this->bottomLineY2 = atoi(line.c_str());

        getline(configFile, line);
        this->leftLineX1 = atoi(line.c_str());

        getline(configFile, line);
        this->leftLineX2 = atoi(line.c_str());

        getline(configFile, line);
        this->rightLineX1 = atoi(line.c_str());

        getline(configFile, line);
        this->rightLineX2 = atoi(line.c_str());

        getline(configFile, line);
        this->angleThreshold = atoi(line.c_str());

        getline(configFile, line);
        this->margin = atoi(line.c_str());

        getline(configFile, line);
        this->proturdingThreshold = atoi(line.c_str());

        getline(configFile, line);
        this->featureMinHessian = atoi(line.c_str());

        getline(configFile, line);
        this->keyPointSizeThreshold = atoi(line.c_str());

        getline(configFile, line);
        this->keyPointCountThreshold = atoi(line.c_str());

        configFile.close();
    }

    free(filename);

    createTrackbar("TopLine Y1", calibratorWindow, &(this->topLineY1), maxHeight, &onCalibratorChange, this);
    createTrackbar("TopLine Y2", calibratorWindow, &(this->topLineY2), maxHeight, &onCalibratorChange, this);
    createTrackbar("BottomLine Y1", calibratorWindow, &(this->bottomLineY1), maxHeight, &onCalibratorChange, this);
    createTrackbar("BottomLine Y2", calibratorWindow, &(this->bottomLineY2), maxHeight, &onCalibratorChange, this);
    createTrackbar("LeftLine X1", calibratorWindow, &(this->leftLineX1), maxWidth, &onCalibratorChange, this);
    createTrackbar("LeftLine X2", calibratorWindow, &(this->leftLineX2), maxWidth, &onCalibratorChange, this);
    createTrackbar("RightLine X1", calibratorWindow, &(this->rightLineX1), maxWidth, &onCalibratorChange, this);
    createTrackbar("RightLine X2", calibratorWindow, &(this->rightLineX2), maxWidth, &onCalibratorChange, this);
    createTrackbar("Angle Threshold", calibratorWindow, &(this->angleThreshold), 90, &onCalibratorChange, this);
    createTrackbar("Inside Margin", calibratorWindow, &(this->margin), 100, &onCalibratorChange, this);
    createTrackbar("Proturding Threshold", calibratorWindow, &(this->proturdingThreshold), 100, &onCalibratorChange, this);
    createTrackbar("Feature Min Hessian", calibratorWindow, &(this->featureMinHessian), 2000, &onCalibratorChange, this);
    createTrackbar("Key Point Size Threshold", calibratorWindow, &(this->keyPointSizeThreshold), 200, &onCalibratorChange, this);
    createTrackbar("Key Point Count Threshold", calibratorWindow, &(this->keyPointCountThreshold), 200, &onCalibratorChange, this);

}

void CalibratorWindow::drawBoundary(Mat & image) {
    cv::line(image, Point(0, topLineY1), Point(maxWidth, topLineY2), Scalar(0, 0, 255), 1.5);
    cv::line(image, Point(0, bottomLineY1), Point(maxWidth, bottomLineY2), Scalar(0, 255, 255), 1.5);
    cv::line(image, Point(leftLineX1, 0), Point(leftLineX2, maxWidth), Scalar(255, 0, 255), 1.5);
    cv::line(image, Point(rightLineX1, 0), Point(rightLineX2, maxWidth), Scalar(255, 255, 0), 1.5);
}

