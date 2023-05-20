#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<bits/stdc++.h>
#include<iostream>

#include <windows.h>
#include <GL/glut.h>

struct point
{
	double x,y,z;
};

class Point
{
private:
    double x;
    double y;
    double z;
public:
     Point()
    {
        x=0.0;
        y=0.0;
        z=0.0;
    }
    Point(double x,double y, double z)
    {
        this->x=x;
        this->y=y;
        this->z=z;
    }

    void set_x(double x)
    {
        this->x=x;
    }

    void set_y(double y)
    {
        this->y=y;
    }

    void set_z(double y)
    {
        this->z=y;
    }

    double get_x()
    {
        return x;
    }

    double get_y()
    {
        return y;
    }

    double get_z()
    {
        return z;
    }

    void print()
    {

        std::cout<<"printing point:\tx= "<<x<<", y= "<<y<<" , z= "<<z<<std::endl;
    }

    //for add
    Point operator+(Point p);
    //for minus
    Point operator-(Point p);
    //for dot product
    //Point operator.(const Point p);
    //for cross product
    Point operator%(Point p);
    //scalar multiplication
    Point operator*(double scalar);
};

Point Point::operator+(Point p)
{
    Point temp;
    temp.set_x(x+p.get_x());
    temp.set_y(y+p.get_y());
    temp.set_z(z+p.get_z());

    return temp;
}

Point Point::operator-(Point p)
{
    Point temp;
    temp.set_x(x-p.get_x());
    temp.set_y(y-p.get_y());
    temp.set_z(z-p.get_z());

    return temp;
}

Point Point::operator*(double scalar)
{
    Point temp;
    temp.set_x(x*scalar);
    temp.set_y(y*scalar);
    temp.set_z(z*scalar);

    return temp;
}