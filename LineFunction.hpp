#ifndef __LINE_FUNCTION_H__
#define __LINE_FUNCTION_H__

#include <cv.h>
#include <math.h>

using namespace cv;

class LineFunction {
public:
    LineFunction(double ax, double by, double c);
    LineFunction(Point2d point1, Point2d point2);

    static LineFunction fromInverseXY(double slope, double c);
    double getAngleWith(LineFunction that);
    double getAngleDiffWith(LineFunction that);

    double ax;
    double by;
    double c;

    friend std::ostream & operator<<(std::ostream & stream, LineFunction const & obj);

};

#endif
