#include "minCircle.h"
#include <assert.h>
#include <math.h>

// returns distance between two points
float pointDistance(Point& p1, Point& p2) {
    // calculates and return distance
    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

// checks if points is within or on boundary of circle
bool isWithin(Circle c, Point& p) {
    // is distance is <= radius then it's within/on boundary
    return pointDistance(c.center, p) <= c.radius;
}


// creates a circle from three points
Circle createCircle(Point& p1, Point& p2, Point& p3) {
    // calculates remainders from p1
    double p2XRem = p2.x - p1.x;
    double p2YRem = p2.y - p1.y;
    double p3XRem = p3.x - p1.x;
    double p3YRem = p3.y - p1.y;
    // calculates squares
    double rem2Srq = pow(p2XRem, 2) +  pow(p2YRem, 2);
    double rem3Srq = pow(p3XRem, 2) +  pow(p3YRem, 2);
    double remSqr = p2XRem * p3YRem - p2YRem * p3YRem;
    // claculates center
    float centerX = p1.x + (p2YRem * rem2Srq - p3YRem * rem3Srq) / (2 * remSqr); 
    float centerY = p1.y + (p3XRem * rem3Srq - p2XRem * rem2Srq) / (2 * remSqr); 
    Point center(centerX, centerY);
    // returns Circle with radius of distance from p1 to center
    return Circle(center, pointDistance(center, p1));
} 

// creates a circle from two points
Circle createCircle(Point& p1, Point& p2) {
    // center will be half way between p1 and p2
    float xCenter = (p1.x + p2.x) / 2;
    float yCenter = (p1.y + p2.y) / 2;
    Point center(xCenter, yCenter);
    // radius will be half the distance between p1 and p2 (both on the boundary)
    return Circle(center, pointDistance(p1, p2));
}

// in case of having size<=3 points, we calculate the circle directly
Circle trivialMinCircle(vector<Point*>& boundaryPoints) {
    size_t size = boundaryPoints.size();
    // make sure there's at most 3 points
    assert(size <= 3);

    // calculate circle according to size
    switch (size) {
    case 0:
        return Circle(Point(0, 0), 0);
    case 1:
        return Circle(*boundaryPoints[0], 0);
    case 2:
        return createCircle(*boundaryPoints[0], *boundaryPoints[1]);
    case 3: 
        return createCircle(*boundaryPoints[0], *boundaryPoints[1],  *boundaryPoints[2]);
    }
}

/*
* calculate minimum circle recurcively using welzl algorithm,
* size is the number of points in points that have yet to be calculated,
* and boundary points represens the points that are sitting on
* the boundary of the circle.
*/
Circle welzlMiniCircle(Point** points, vector<Point*>& boundaryPoints, size_t size) {
    // Base - if no points left or we reached 3 points on boundary, we can calculate trivially
    if (size == 0 || boundaryPoints.size() == 3) {
        return trivialMinCircle(boundaryPoints);
    }

    // picks index randomly
    int index = rand() % size;
    Point* randPoint = points[index];

    // puts the selected point at the end of array to make it easier than removing it
    swap(points[index], points[size - 1]);

    // calculate the smallest circle without randPoint
    Circle smallCircle = welzlMiniCircle(points, boundaryPoints, size - 1);
    // if randPoint is within circle then we're done
    if (isWithin(smallCircle, *randPoint)) {
        return smallCircle;
    }
    // else push randPoint to be in boundary
    boundaryPoints.push_back(randPoint);

    // calculate again with randPoint being in boundary, and return
    return welzlMiniCircle(points, boundaryPoints, size - 1);
}

// Finds the smallest circle that encapsulates all points
Circle findMinCircle(Point** points,size_t size) {
     vector<Point*> boundaryPoints;
     return welzlMiniCircle(points, boundaryPoints, size);
}