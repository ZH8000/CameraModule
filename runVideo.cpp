#include "ErrorDetector.hpp"

#define MAX_WIDTH   320
#define MAX_HEIGHT  240
#define SAVE_IMAGE  1
#define VOTING_THRESHOLD 2.0
#include <sys/time.h>
#include <stdio.h>

char * getOutputFolder(char * cameraNo) {
    char * result = (char *) malloc(sizeof(char) * 200);
    char * dateStamp = (char *) malloc(sizeof(char) * 200);
    memset(result, 0, sizeof(char) * 200);
    memset(dateStamp, 0, sizeof(char) * 200);
    time_t timestamp;
    struct tm * tm;
    timestamp = time(NULL);
    tm = localtime(&timestamp);
    strftime(dateStamp, 200, "/%Y-%m-%d/%H/%M", tm);
    strcat(result, "output");
    strcat(result, cameraNo);
    strcat(result, dateStamp);
    free(dateStamp);
    return result;
}

char * getOutputRawFolder(char * cameraNo) {
    char * result = (char *) malloc(sizeof(char) * 200);
    char * dateStamp = (char *) malloc(sizeof(char) * 200);
    memset(result, 0, sizeof(char) * 200);
    memset(dateStamp, 0, sizeof(char) * 200);
    time_t timestamp;
    struct tm * tm;
    timestamp = time(NULL);
    tm = localtime(&timestamp);
    strftime(dateStamp, 200, "/%Y-%m-%d/%H/%M", tm);
    strcat(result, "outputRaw");
    strcat(result, cameraNo);
    strcat(result, dateStamp);
    free(dateStamp);
    return result;
}

char * getTimestamp() {
    char * dateStamp = (char *) malloc(sizeof(char) * 200);
    memset(dateStamp, 0, sizeof(char) * 200);
    time_t timestamp;
    struct tm * tm;
    timestamp = time(NULL);
    tm = localtime(&timestamp);
    strftime(dateStamp, 200, "%Y-%m-%d %H:%M:%S", tm);
    return dateStamp;
}


void saveImage(char * cameraNo, Mat & frame, Mat & result, FILE * logFile, bool isRealOblique) {
    if (SAVE_IMAGE) {
        struct timeval tv;
        gettimeofday(&tv,NULL);

        char * outputFolder = getOutputFolder(cameraNo);
        char * outputRawFolder = getOutputRawFolder(cameraNo);
        char * filename = (char *) malloc(sizeof(char) * 500);
        char * rawFilename = (char *) malloc(sizeof(char) * 500);

        char * mkdirOutputFolder = (char *) malloc(sizeof(char) * 100);
        char * mkdirOutputRawFolder = (char *) malloc(sizeof(char) * 100);

        memset(mkdirOutputFolder, 0, sizeof(char) * 100);
        memset(mkdirOutputRawFolder, 0, sizeof(char) * 100);

        strcat(mkdirOutputFolder, "mkdir -p ");
        strcat(mkdirOutputFolder, outputFolder);
        strcat(mkdirOutputRawFolder, "mkdir -p ");
        strcat(mkdirOutputRawFolder, outputRawFolder);

        system(mkdirOutputFolder);
        system(mkdirOutputRawFolder);
        
        unsigned long long start_utime;

        start_utime = tv.tv_sec * 1000000 + tv.tv_usec;
        sprintf(filename, "%s/%16llu.png", outputFolder, start_utime);
        sprintf(rawFilename, "%s/%16llu.png", outputRawFolder, start_utime);

        Mat resizedFrame;
        resize(frame, resizedFrame, Size(MAX_WIDTH, MAX_HEIGHT));
        imwrite(filename, result);
        imwrite(rawFilename, resizedFrame);

        if (isRealOblique) {
          char * timestamp = getTimestamp();
          fprintf(logFile, "%s|%s\n", timestamp, filename);
          fflush(logFile);
          free(timestamp);
        }

        free(outputFolder);
        free(outputRawFolder);
        free(filename);
        free(rawFilename);
        free(mkdirOutputFolder);
        free(mkdirOutputRawFolder);
    }
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        cout << "Please use " << argv[0] << " [cameraNo] to run this program" << endl;
        return -1;
    }

    char * cameraNo = argv[1];

    VideoCapture cap(atoi(cameraNo)); // open the default camera

    if(!cap.isOpened())  {
        cerr << "Cannot open camera for device " << argv[1] << endl;
        return -1;
    }
 

    namedWindow("Preview",1);
    CalibratorWindow calibrator("Calibrator", "Preview", MAX_WIDTH, MAX_HEIGHT, cameraNo);
    calibrator.showWindow();
    
    int count = 0;
    bool isKeyFrame = false;
    int totalFrame = 0;
    int obliqueCount = 0;
    char * logFilename = (char *) malloc(sizeof(char) * 100);
    memset(logFilename, 0, sizeof(char) * 100);
    sprintf(logFilename, "log-%s.txt", cameraNo);
    FILE * logFile = fopen(logFilename, "a");

    for(;;)
    {
        bool isRealOblique = false;
        Mat frame;
        Mat resizedImage;

        cap >> frame;

        resize(frame, resizedImage, Size(MAX_WIDTH, MAX_HEIGHT));

        ErrorDetector processor(&resizedImage, &calibrator);
        vector<KeyPoint> keyPoints = processor.getValidKeyPoints();

        if (keyPoints.size() >= calibrator.getMinKeyPoint()) {
            isKeyFrame = true;
            LineFunction fitLine = processor.getFitLine(keyPoints);
            double angle = NAN;
            double angleDiff = NAN;
            
            bool isOblique = processor.isOblique(fitLine, keyPoints.size(), angle, angleDiff);

            char * isKeyFrame = (char *) malloc(sizeof(char) * 100);
            sprintf(isKeyFrame, "isKeyFrame:%d", totalFrame);
            putText(resizedImage, isKeyFrame, Point(10, 230), CV_FONT_HERSHEY_PLAIN, 1, CV_RGB(255, 0, 0), 1);
 
            if (isOblique && !isnan(angle)) {
                obliqueCount++;
                putText(resizedImage, "OBLIQUE", Point(10, 30), CV_FONT_HERSHEY_PLAIN, 1.5, CV_RGB(255, 0, 0), 2);
            }

            processor.drawAngleInfo(angle);
            processor.drawSlopeInfo(fitLine);
            processor.drawKeyPoints();
            totalFrame++;
        } else {

            if (isKeyFrame) {
                int threshold = (int) (totalFrame / VOTING_THRESHOLD);

                char * leaveFrame = (char *) malloc(sizeof(char) * 100);
                sprintf(leaveFrame, "result: %d/%d", obliqueCount, totalFrame);
                putText(resizedImage, leaveFrame, Point(10, 210), CV_FONT_HERSHEY_PLAIN, 1, CV_RGB(255, 0, 0), 1);
                sprintf(leaveFrame, "threshold: %d", threshold);
                putText(resizedImage, leaveFrame, Point(10, 230), CV_FONT_HERSHEY_PLAIN, 1, CV_RGB(255, 0, 0), 1);

                if (obliqueCount >= threshold) {
                    //! Send STOP singal!!!
                    putText(resizedImage, "REAL OBLIQUE!!", Point(10, 30), CV_FONT_HERSHEY_PLAIN, 1.5, CV_RGB(255, 0, 0), 2);
                    isRealOblique = true;
                }

                totalFrame = 0;
                isKeyFrame = false;
                obliqueCount = 0;
            }
        }

        processor.drawTitle(cameraNo);
        processor.drawBoundary();
        processor.drawFeatureCount(keyPoints.size());
        saveImage(cameraNo, frame, resizedImage, logFile, isRealOblique);

        imshow("Preview", resizedImage);
        waitKey(30);
        count++;
    }
    fclose(logFile);

    return 0;
}
