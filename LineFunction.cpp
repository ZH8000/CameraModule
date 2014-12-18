#include "LineFunction.hpp"

LineFunction LineFunction::fromInverseXY(double slope, double c) {
    double y1 = slope * 0 + c;   // (0, y1)
    double y2 = slope * 100 + c; // (100, y2)

    return LineFunction(Point2d(y1, 0), Point2d(y2, 100));
}

LineFunction::LineFunction(double ax, double by, double c) {
    this->ax = ax;
    this->by = by;
    this->c = c;
}

LineFunction::LineFunction(Point2d point1, Point2d point2)
{ 
    double slope = (point2.y - point1.y) / (point2.x - point1.x);
    double c = -(slope * point1.x) + point1.y;
    this->ax = slope;
    this->by = -1;
    this->c = c;
}

double LineFunction::getAngleWith(LineFunction that) {

    if (!isinf(that.ax) && !isnan(that.ax)) {
        double a1 = (this->ax * that.ax) + (this->by * that.by);
        double distanceLine1 = (this->ax * this->ax) + (this->by * this->by);
        double distanceLine2 = (that.ax * that.ax) + (that.by * that.by);
        double a2 = a1 / sqrt(distanceLine1 * distanceLine2);
        double angleInRadin = acos(a2);
        return angleInRadin * (180 / M_PI);
    } else {
        return 90 - getAngleWith(LineFunction(Point2d(0, 0), Point2d(100, 0)));
    }

}

double LineFunction::getAngleDiffWith(LineFunction that) {
    double angle1 = this->getAngleWith(that);
    double angle2 = 180 - angle1;
    return abs(angle1 - angle2);
}

std::ostream & operator<<(std::ostream & stream, LineFunction const & obj) { 
    stream << "LineFunction(" << obj.ax << "X + " << obj.by << "Y + " << obj.c << " = 0)";
    return stream;
}
