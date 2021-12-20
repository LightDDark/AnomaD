/*
 * animaly_detection_util.cpp
 *
 * Author: 314915869 Or Avraham Yehezkel & 316049121 Maayan Shavit
 */

#include <math.h>
#include <assert.h>
#include "anomaly_detection_util.h"

// Declares assisting functions
float* copyPointer(const float* x, int size);
void powfArr(float* x, int size, int power);

/*
* Function avg reiceve a float array x
* and returns the average of the array
* by summin x values and dividing them by given size
*/
float avg(const float* x, int size){
    // delcare variable for summin x values
    float sum = 0;
    // sum all values
    for (int i = 0; i < size; i++) {
        sum += *(x + i);
    }
    // returns sum divided by array size
    return (sum / size);
}

/*
* functions returns variance of x values (inside given array x),
* suing the formula Var(X) = avg(x^2) - avg(x)^2
* note that capital letters denotes Array/Pointer and small letters denotes values
*/
float var(const float* x, int size){
    // copy x values to a new array (dynamic), in order to not affect x with further manipulations. 
    float* newX = copyPointer(x, size);
    // calculate average
    float u = avg(newX, size);
    // square all x values inside new array
    powfArr(newX, size, 2);
    // calculate according to formula
    float var = avg(newX, size) - pow(u, 2);
    // delete allocated memory
    if (nullptr != newX) {
        delete[] newX;
    }
    return var;
}

/*
* returns the covariance of X and Y,
* by using the formula cov(X,Y) = avg(x-avg(X)(y-avg(Y))
* note that capital letters denotes Array/Pointer and small letters denotes values.
*/
float cov(const float* x, const float* y, int size){
    // calculates average of each X and Y
    float avgX = avg(x, size);
    float avgY = avg(y, size);
    // create a new Array for the multipcation of differance.
    float covArr[size];
    for (int i = 0; i < size; i++) {
        // multiply the diff from avg in each array
        covArr[i] = (x[i] - avgX) * (y[i] - avgY);
    }
    // return the average of the results
    return avg(covArr, size);
}


/*
* returns the Pearson correlation coefficient of X and Y,
* according to the formula cov(X,Y)/(sqr(var(x))*sqr(var(y)))
*/
float pearson(const float* x, const float* y, int size){
    // calculate the square root of var for each array
    float sqrVarX = sqrt(var(x, size));
    float sqrVarY = sqrt(var(y, size));
    // calculate according to formula
    float pearson = cov(x, y, size) / (sqrVarX * sqrVarY);
    // returns pearson
    return pearson;
}

/*
* function copyPointer, copies the content of float pointer x,
* given it's size into a new allocated memory.
*/
float* copyPointer(const float* x, int size) {
    // declares pointer to float at the SIZE of size
    float* newX = new float[size];
    // checks the allocation was succesful
    assert(nullptr != newX);
    // loops on x and copy values the new pointer
    for (int i = 0; i < size; i++) {
        newX[i] = x[i];
    }
    // returns new pinter
    return newX;
}

/*
* function recieves an float array, size, and power to raise with,
* and performs power on every x value inside array.
*/
void powfArr(float* x, int size, int power) {
    // loops on the array and power each value
    for (int i = 0; i < size; i++) {
        x[i] = pow(x[i], power);
    }
}

/*
    this function claculates the linear regression
    for an array of points. It returns the line equation
    for the calculated regression.
*/
Line linear_reg(Point** points, int size) {
    Line reg;
    float xArr[size];
    float yArr[size];
    /*
        looping through the given array of
        points, extracting the X and Y values
        and inserting them into new, separated arrays
    */
    for (int i = 0; i < size; i++) {
        xArr[i] = points[i]->x;
        yArr[i] = points[i]->y;
    }
    // calculating the x-constant of the equation
    // deviding the covariance of the points by the variance of the X variables.
    float a = ((cov(xArr,yArr, size))/(var(xArr,size)));
    // calculating the average for X and Y
    float avrgX = avg(xArr, size);
    float avrgY = avg(yArr, size);
    // calculating the b constant 
    float b = avrgY - (avrgX * a);
    // asssigning the values to the line object
    reg.a = a;
    reg.b = b;
    return reg;
}

/*
    this function claculates the deviation point
    between a given point and the rest of the points
*/
float dev(Point p,Point** points, int size){
    Line lineReg = linear_reg(points,size);
    float deviationP = fabs (p.y - lineReg.f(p.x));
    return deviationP;
}

/*
    this function claculates the deviation point 
    between a given point and a given linear regression line.
*/
float dev(Point p,Line l){
    float deviationP = fabs (p.y - l.f(p.x));
    return deviationP;
}

