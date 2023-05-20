#include<bits/stdc++.h>
#include<iostream>
#include<fstream>
#include<cmath>
#include<cstdio>
#include<cstdlib>
#include<ctime>

//header file for bitmap image
#include "bitmap_image.hpp"

using namespace std;

#define PI (2 * acos(0.0))
#define INF numeric_limits<double>::infinity()

#define PRECISION 7



//convert degree to radian
double degreeToRadian(double degree)
{
    return degree*(PI/180.0);
}

class Color{
public:
    int r,g,b;

    Color(){
        // randomly assign r,g,b values
        r = rand() % 256;
        g = rand() % 256;
        b = rand() % 256;
    }

    Color(int r, int g, int b){
        this->r = r;
        this->g = g;
        this->b = b;
    }

    void print()
    {
        cout << "(" << r << "," << g << "," << b << ")" << endl;
    }
};


//Point class
class Point
{
public:
    double x;
    double y;
    double z;
    double w;

    Point()
    {
       x=y=z=0;
    }

    Point(double x, double y, double z)
    {
        this->x=x;
        this->y=y;
        this->z=z;
        this->w=1.0;
    }

    Point(double x, double y, double z, double w)
    {
        this->x=x;
        this->y=y;
        this->z=z;
        this->w=w;
    }


    friend ifstream &operator>>(ifstream &input, Point &point) {
        input >> point.x >> point.y >> point.z;
        return input;
    }

    friend ofstream &operator<<(ofstream &output, Point &point) {
        output << point.x <<" "<< point.y<< " " << point.z;
        return output;
    }

    //print point
    void print()
    {
        cout<<x<<" "<<y<<" "<<z<<endl;
    }

    //overload + operator
    Point operator+(const Point &point) const
    {
        Point temp;
        temp.x=x+point.x;
        temp.y=y+point.y;
        temp.z=z+point.z;
        return temp;
    }

    //overload - operator
    Point operator-(const Point &point) const
    {
        Point temp;
        temp.x=x-point.x;
        temp.y=y-point.y;
        temp.z=z-point.z;
        return temp;
    }

    //overload * operator
    Point operator*(const double num) const
    {
        Point temp;
        temp.x=x*num;
        temp.y=y*num;
        temp.z=z*num;
        return temp;
    }

    //overload / operator
    Point operator/(const double num) const
    {
        Point temp;
        temp.x=x/num;
        temp.y=y/num;
        temp.z=z/num;
        return temp;
    }

    //overload dot operator
    double dot(Point &point)
    {
        return x*point.x+y*point.y+z*point.z;
    }

    //overload cross operator
    Point cross(Point &point)
    {
        Point temp;
        temp.x=y*point.z-z*point.y;
        temp.y=z*point.x-x*point.z;
        temp.z=x*point.y-y*point.x;
        return temp;
    }

    //normalize the point
    Point normalize()
    {
        double d=sqrt(x*x+y*y+z*z);
        Point temp;
        temp.x=x/d;
        temp.y=y/d;
        temp.z=z/d;
        return temp;
    }
};


class Triangle
{
public:
    Point points[3];
    Color color;

    Triangle()
    {
        points[0]=Point(0,0,0);
        points[1]=Point(0,0,0);
        points[2]=Point(0,0,0);
        color=Color();
    }

    Triangle(Point p1, Point p2, Point p3)
    {
        points[0]=p1;
        points[1]=p2;
        points[2]=p3;
        color=Color();
    }
};

//distance from point p to plane defined by p1,p2,p3
Point getZDepth(Point p, Point p1, Point p2, Point p3)
{
    Point t = p3-p1;
    Point normal=(p2-p1).cross(t);
    normal=normal.normalize();
    double d=normal.dot(p1);
    double z=normal.dot(p)-d;
    Point temp;
    temp.x=p.x;
    temp.y=p.y;
    temp.z=z;
    return temp;
}


 int getMinIntercept(Point line1, Point line2, Point line3, int row, double Top_y, double Left_x, double dx, double dy)
 {
    //m
 }

  int getMaxIntercept(Point line1, Point line2, Point line3, int row, double Top_y, double Left_x, double dx, double dy)
 {
    //m
 }



int main()
{
    srand(time(NULL));
    //read from config file
    ifstream in;
    in.open("config.txt");

    int screen_width, screen_height;
    in>>screen_width>>screen_height;

    double x_left, x_right, y_bottom, y_top;

    in>>x_left;
    in>>y_bottom;

    x_right=-x_left;
    y_top=-y_bottom;

    double z_near, z_far;
    in>>z_near>>z_far;

    //open stage3.txt
    ifstream in2;
    in2.open("stage3.txt");

    //read number of triangles
    Triangle triangles[100];

    int cnt = 0;
    
    Point points[3];
    while(in2>>points[0]>>points[1]>>points[2])
    {
        triangles[cnt]=Triangle(points[0],points[1],points[2]);
        cnt++;
    }

    //close stage3.txt
    in2.close();

    //check triangles number
    cout<<"triangles number: "<<cnt<<endl;

    for(int i=0;i<cnt;i++)
    {
        cout<<"triangle "<<i<<endl;
        triangles[i].points[0].print();
        triangles[i].points[1].print();
        triangles[i].points[2].print();
        triangles[i].color.print();

        cout<<endl;
    }

    //initialize zbuffer and frame buffer
    double dx = (x_right-x_left)/screen_width;
    double dy = (y_top-y_bottom)/screen_height;
    double Top_y = y_top-dy/2.0;
    double Bottom_y = y_bottom+dy/2.0;
    double Left_x = x_left+dx/2.0;
    double Right_x = x_right-dx/2.0;

    double z_max = z_far-z_near;

    //dynamically allocate zbuffer and frame buffer
    double** z_buffer = new double*[screen_height];
 
    for (int i = 0; i < screen_height; i++) {
        z_buffer[i] = new double[screen_width];

        for (int j = 0; j < screen_width; j++) {
            z_buffer[i][j] = z_max;
        }
    }

    Color** frame_buffer = new Color*[screen_height];

    for (int i = 0; i < screen_height; i++) {
        frame_buffer[i] = new Color[screen_width];

        for (int j = 0; j < screen_width; j++) {
            frame_buffer[i][j] = Color(0,0,0);
        }
    }

    //create a bitmap object with screen_width and screen_height

    bitmap_image image(screen_width, screen_height);

    //set background to black
    for(int i=0;i<screen_width;i++)
    {
        for(int j=0;j<screen_height;j++)
        {
            image.set_pixel(i,j,0,0,0);
        }
    }

    //draw triangles

    for(int i=0;i<cnt;i++)
    {
        //get triangle
        Triangle triangle=triangles[i];
        //get color
        Color color=triangle.color;
        //get points
        Point p1=triangle.points[0];
        Point p2=triangle.points[1];
        Point p3=triangle.points[2];

        p1.print();
        p2.print();
        p3.print();
        color.print();

        //clipping
        double max_y=max(p1.y,max(p2.y,p3.y));
        double min_y=min(p1.y,min(p2.y,p3.y));

        //get mapping of top and bottom scanlines
        int top_scanline, bottom_scanline;

        if(max_y >= y_top)
        {
            top_scanline = 0;
        }

        else
        {
            top_scanline = round((Top_y-max_y)/dy);
        }

        if(min_y <= y_bottom)
        {
            bottom_scanline = screen_height-1;
        }

        else
        {
            bottom_scanline = screen_height - (1 + round((min_y-Bottom_y)/dy));
        }
        // int top_scanline = round((Top_y-max_y)/dy);
        // int bottom_scanline = floor((Top_y-min_y)/dy);
        

        cout<<"z_near: "<<z_near<<endl;
        cout<<"z_far: "<<z_far<<endl;
        cout<<"dx: "<<dx<<endl;
        cout<<"dy: "<<dy<<endl;
        cout<<"Top_y: "<<Top_y<<endl;
        cout<<"Left_x: "<<Left_x<<endl;
        cout<<"x_left: "<<x_left<<endl;
        cout<<"x_right: "<<x_right<<endl;
        cout<<"y_bottom: "<<y_bottom<<endl;
        cout<<"y_top: "<<y_top<<endl;
        cout<<"min_y: "<<min_y<<endl;
        cout<<"max_y: "<<max_y<<endl;
        cout<<"top_scanline: "<<top_scanline<<endl;
        cout<<"bottom_scanline: "<<bottom_scanline<<endl;


        for(int row=top_scanline;row<=bottom_scanline;row++)
        {

            //get intersection of row and triangle
            int left_intersecting_column;
            int right_intersecting_column;
            
            double ys = Top_y-row*dy;

            Point intersecting_points[3];
            intersecting_points[0] = Point(INF, ys, 0 , 1);
            intersecting_points[1] = Point(INF, ys, 1 , 2);
            intersecting_points[2] = Point(INF, ys, 2 , 0);


            for(int j=0;j<3;j++)
            {
                Point p1= triangle.points[(int) intersecting_points[j].z];
                Point p2= triangle.points[(int) intersecting_points[j].w];

                if(p1.y != p2.y)
                {
                    intersecting_points[j].x = p1.x + (ys - p1.y)*(p1.x - p2.x)/(p1.y - p2.y);
                }
            }

            for(int j=0; j<3; j++)
            {
                Point p1= triangle.points[(int) intersecting_points[j].z];
                Point p2= triangle.points[(int) intersecting_points[j].w];

                if(intersecting_points[j].x != INF)
                {
                    if(intersecting_points[j].x > max(p1.x, p2.x) || intersecting_points[j].x < min(p1.x, p2.x) || intersecting_points[j].y > max(p1.y, p2.y) || intersecting_points[j].y < min(p1.y, p2.y))
                    {
                        intersecting_points[j].x = INF;
                    }
                }
            }

            int max_index = -1, min_index = -1;

            double maxX, minX;

            for(int j=0;j<3;j++)
            {
                if(max_index == -1 && min_index == -1)
                {
                    if(intersecting_points[j].x != INF)
                    {
                        max_index = j;
                        min_index = j;
                        maxX = intersecting_points[j].x;
                        minX = intersecting_points[j].x;
                    }
                }
                else
                {
                    if(intersecting_points[j].x != INF)
                    {
                        if(intersecting_points[j].x > maxX)
                        {
                            max_index = j;
                            maxX = intersecting_points[j].x;
                        }
                        if(intersecting_points[j].x < minX)
                        {
                            min_index = j;
                            minX = intersecting_points[j].x;
                        }
                    }
                }
            }


            if(intersecting_points[min_index].x <= Left_x)
            {
                left_intersecting_column = 0;
            }

            else
            {
                left_intersecting_column = round((intersecting_points[min_index].x - Left_x)/dx);
            }

            if(intersecting_points[max_index].x >= Right_x)
            {
                right_intersecting_column = screen_width-1;
            }
            else
            {
                right_intersecting_column = screen_width - (1 + round((Right_x - intersecting_points[max_index].x)/dx));
            }

            Point p1= triangle.points[(int) intersecting_points[min_index].z];
            Point p2= triangle.points[(int) intersecting_points[min_index].w];


            double za = p1.z + (intersecting_points[min_index].y - p1.y)*(p2.z - p1.z)/(p2.y - p1.y);
            

            p1= triangle.points[(int) intersecting_points[max_index].z];
            p2= triangle.points[(int) intersecting_points[max_index].w];

            double zb = p1.z + (intersecting_points[max_index].y - p1.y)*(p2.z - p1.z)/(p2.y - p1.y);


            double zp;

            double const_term = dx*(zb - za)/(intersecting_points[max_index].x - intersecting_points[min_index].x);

            cout<<"left column: "<<left_intersecting_column<<endl;
            cout<<"right column: "<<right_intersecting_column<<endl;

            for(int col = left_intersecting_column; col <= right_intersecting_column; col++)
            {
                if(col == left_intersecting_column)
                {
                    zp = za + ((Left_x + left_intersecting_column*dx) - intersecting_points[min_index].x)*(zb-za)/(intersecting_points[max_index].x - intersecting_points[min_index].x);
                }

                else
                {
                    zp = zp + const_term;
                }


                if(zp > z_near && zp < z_buffer[row][col])
                {
                    z_buffer[row][col] = zp;
                    //image.set_pixel(col,row,color.r,color.g,color.b);
                    frame_buffer[row][col] = color;
                }
            }


            

            // for(int col=left_intersecting_column;col<=right_intersecting_column;col++)
            // {
            //     //get z value
            //     double z = z_buffer[row][col];
            //     //get x value
            //     double x = Left_x+col*dx;
            //     //get y value
            //     double y = Top_y-row*dy;
            //     //get point
            //     Point p = Point(x,y,z_near);
            //     //get point on triangle
            //     Point p_on_triangle = (getZDepth(p,p1,p2,p3));
            //     //get z value on triangle
            //     double z_on_triangle = -p_on_triangle.z;
            //     //get z value on triangle
            //     // cout<<"row: "<<row<<endl;
            //     // cout<<"col: "<<col<<endl;
            //     // cout<<"z_on_triangle: "<<z_on_triangle<<endl;
            //     if(z_on_triangle < z)
            //     {
            //         //set pixel
            //         image.set_pixel(col,row,color.r,color.g,color.b);
            //         //set z value
            //         z_buffer[row][col] = z_on_triangle;
            //     }
            // }
        }

        cout<<endl<<endl;

    }


    ofstream out("z_buffer.txt");

    for(int i=0;i<screen_height;i++)
    {
        for(int j=0;j<screen_width;j++)
        {
            if(z_buffer[i][j] != 2.0)
                out<<z_buffer[i][j]<<" ";

            image.set_pixel(j,i,frame_buffer[i][j].r,frame_buffer[i][j].g,frame_buffer[i][j].b);
        }
        out<<endl;
    }



    //save image to file
    image.save_image("out.bmp");

    //free memory
    for (int i = 0; i < screen_height; i++) {
        delete[] z_buffer[i];
        delete[] frame_buffer[i];
    }

    delete[] z_buffer;
    delete[] frame_buffer;



    
    return 0;
}


