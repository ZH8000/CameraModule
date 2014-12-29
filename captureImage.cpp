#include "ErrorDetector.hpp"

#define MAX_WIDTH   320
#define MAX_HEIGHT  240


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

        cap >> frame;

        resize(frame, resizedImage, Size(MAX_WIDTH, MAX_HEIGHT));

        ErrorDetector processor(&resizedImage, &calibrator);
        vector<KeyPoint> keyPoints = processor.getValidKeyPoints();

        LineFunction fitLine = processor.getFitLine(keyPoints);
        double angle = NAN;
        double angleDiff = NAN;
        
        bool isOblique = processor.isOblique(fitLine, angle, angleDiff);

        if (isOblique && !isnan(angle)) {
            putText(resizedImage, "OBLIQUE!!", Point(50, 50), CV_FONT_HERSHEY_PLAIN, 1.5, CV_RGB(255, 255, 0));
        }

        processor.drawBoundary();
        processor.drawFeatureCount(keyPoints.size());
        processor.drawAngleInfo(angle);
        processor.drawSlopeInfo(fitLine);
        processor.drawKeyPoints();

        imshow("Preview", resizedImage);
        waitKey(30);
    }

    return 0;
}
