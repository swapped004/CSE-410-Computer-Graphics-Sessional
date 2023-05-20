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

#define PRECISION 7
#define PI (2 * acos(0.0))
#define INF numeric_limits<double>::infinity()

//Point class
class Point
{
public:
    double x;
    double y;
    double z;

    Point()
    {
       x=y=z=0;
    }

    Point(double x, double y, double z)
    {
        this->x=x;
        this->y=y;
        this->z=z;
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
    Point operator+(Point &point)
    {
        Point temp;
        temp.x=x+point.x;
        temp.y=y+point.y;
        temp.z=z+point.z;
        return temp;
    }

    //overload - operator
    Point operator-(Point &point)
    {
        Point temp;
        temp.x=x-point.x;
        temp.y=y-point.y;
        temp.z=z-point.z;
        return temp;
    }

    //overload * operator
    Point operator*(double num)
    {
        Point temp;
        temp.x=x*num;
        temp.y=y*num;
        temp.z=z*num;
        return temp;
    }

    //overload / operator
    Point operator/(double num)
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

double degreeToRadian(double degree);
Point R(Point x, Point a, double angle);


// class Matrix
class Matrix
{

public:
   vector<vector<double>> matrix;

    //init matrix with zeroMatrix

    void init_matrix(int row, int col)
    {
       matrix.resize(row,vector<double>(col,0));
    }

    Matrix()
    {
        init_matrix(4,4);
    }

    Matrix(int row, int col)
    {
        init_matrix(row,col);
    }

    Matrix(vector<vector<double>> matrix)
    {
        this->matrix=matrix;
    }

    //overload + operator
    Matrix operator+(Matrix &matrix)
    {
        int left_row=this->matrix.size();
        int left_col=this->matrix[0].size();
        int right_row = matrix.matrix.size();
        int right_col = matrix.matrix[0].size();

        if(left_row!=right_row || left_col!=right_col)
        {
            cout<<"Error: Matrix size not match!"<<endl;
            return *this;
        }
        else
        {
            Matrix temp(left_row,left_col);
            for(int i=0;i<left_row;i++)
            {
                for(int j=0;j<left_col;j++)
                {
                    temp.matrix[i][j]=this->matrix[i][j]+matrix.matrix[i][j];
                }
            }
            return temp;
        }
        
    }

    //overload - operator
    Matrix operator-(Matrix &matrix)
    {
        int left_row=this->matrix.size();
        int left_col=this->matrix[0].size();
        int right_row = matrix.matrix.size();
        int right_col = matrix.matrix[0].size();

        if(left_row!=right_row || left_col!=right_col)
        {
            cout<<"Error: Matrix size not match!"<<endl;
            return *this;
        }
        else
        {
            Matrix temp(left_row,left_col);
            for(int i=0;i<left_row;i++)
            {
                for(int j=0;j<left_col;j++)
                {
                    temp.matrix[i][j]=this->matrix[i][j]-matrix.matrix[i][j];
                }
            }
            return temp;
        }
        
    }

    //overload * operator
    Matrix operator*(Matrix &matrix)
    {
        int left_row=this->matrix.size();
        int left_col=this->matrix[0].size();
        int right_row = matrix.matrix.size();
        int right_col = matrix.matrix[0].size();

        if(left_col!=right_row)
        {
            cout<<"Error: Matrix size not match!"<<endl;
            return *this;
        }
        else
        {
            Matrix temp(left_row,right_col);
            for(int i=0;i<left_row;i++)
            {
                for(int j=0;j<right_col;j++)
                {
                    for(int k=0;k<left_col;k++)
                    {
                        temp.matrix[i][j]+=this->matrix[i][k]*matrix.matrix[k][j];
                    }
                }
            }
            return temp;
        }
    }

    Matrix getZeroMatrix(int row, int col)
    {
        return Matrix(row,col);
    }

    Matrix getIdentityMatrix(int row, int col)
    {
        Matrix temp(row,col);
        for(int i=0;i<row;i++)
        {
            for(int j=0;j<col;j++)
            {
                if(i==j)
                {
                    temp.matrix[i][j]=1;
                }
            }
        }
        return temp;
    }

    Matrix getTranslationMatrix(Point &point)
    {
        Matrix temp=getIdentityMatrix(4,4);
        temp.matrix[0][3]=point.x;
        temp.matrix[1][3]=point.y;
        temp.matrix[2][3]=point.z;
        return temp;
    }

    Matrix getScalingMatrix(Point &point)
    {
        Matrix temp=getIdentityMatrix(4,4);
        temp.matrix[0][0]=point.x;
        temp.matrix[1][1]=point.y;
        temp.matrix[2][2]=point.z;
        return temp;
    }

    Matrix getRotationMatrix(Point axis, double angle)
    {
        Point a=axis.normalize();

        Point i(1,0,0);
        Point j(0,1,0);
        Point k(0,0,1);

        //pass them to rodrigues function

        Point c1 = R(i,a,angle);
        Point c2 = R(j,a,angle);
        Point c3 = R(k,a,angle);

        //init matrix
        Matrix temp(4,4);

        //fill the matrix
        temp.matrix[0][0]=c1.x;
        temp.matrix[0][1]=c2.x;
        temp.matrix[0][2]=c3.x;
        temp.matrix[1][0]=c1.y;
        temp.matrix[1][1]=c2.y;
        temp.matrix[1][2]=c3.y;
        temp.matrix[2][0]=c1.z;
        temp.matrix[2][1]=c2.z;
        temp.matrix[2][2]=c3.z;
        temp.matrix[3][3]=1;

        return temp;
    }

    Matrix getViewTranslationMatrix(Point &eye)
    {
        Matrix temp=getIdentityMatrix(4,4);
        temp.matrix[0][3]=-eye.x;
        temp.matrix[1][3]=-eye.y;
        temp.matrix[2][3]=-eye.z;
        return temp;
    }

    Matrix getViewRotationMatrix(Point &r, Point &u, Point &l)
    {
        Matrix temp=getIdentityMatrix(4,4);
        temp.matrix[0][0]=r.x;
        temp.matrix[0][1]=r.y;
        temp.matrix[0][2]=r.z;
        temp.matrix[1][0]=u.x;
        temp.matrix[1][1]=u.y;
        temp.matrix[1][2]=u.z;
        temp.matrix[2][0]=-l.x;
        temp.matrix[2][1]=-l.y;
        temp.matrix[2][2]=-l.z;
        return temp;
    }

    Matrix getProjectionMatrix(double fovY, double aspectRatio, double near, double far)
    {
        double fovX = fovY * aspectRatio;
        double t = near*tan(degreeToRadian(fovX/2));
        double r = near*tan(degreeToRadian(fovY/2));

        Matrix temp=getZeroMatrix(4,4);
        temp.matrix[0][0]=near/r;
        temp.matrix[1][1]=near/t;
        temp.matrix[2][2]=-(far+near)/(far-near);
        temp.matrix[2][3]=-2*far*near/(far-near);
        temp.matrix[3][2]=-1;
        return temp;
        
    }

    Matrix setColumnMatrix(Point &point)
    {
        Matrix temp(4,1);
        temp.matrix[0][0]=point.x;
        temp.matrix[1][0]=point.y;
        temp.matrix[2][0]=point.z;
        temp.matrix[3][0]=1;
        return temp;
    }


    double getW()
    {
        return matrix[matrix.size()-1][matrix[0].size()-1];
    }
};

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





//global matrix
Matrix global;

//convert degree to radian
double degreeToRadian(double degree)
{
    return degree*(PI/180.0);
}

//rodrigues formula
Point R(Point x, Point a, double angle)
{
    double cos_angle=cos(degreeToRadian(angle));
    double sin_angle=sin(degreeToRadian(angle));

    Point p1=x*cos_angle;
    Point p2=a*(a.dot(x))*(1-cos_angle);
    Point p3=a.cross(x)*sin_angle;

    Point p = p1 + p2 + p3;

    return p;

}

Point Transform(Matrix &trans_matrix, Point point) {

    
    Matrix col_mat = global.setColumnMatrix(point); 
    Matrix temp=trans_matrix*col_mat;
    Point p(temp.matrix[0][0],temp.matrix[1][0],temp.matrix[2][0]);
    return p/temp.getW();

}


int main(int argc, char *argv[])
{
    srand(time(NULL));

    ifstream in;

    cout<<"no of arguments: "<<argc<<endl;

    if(argc > 1)
        in.open(argv[1]);
    else
        in.open("scene.txt");

    ofstream stage1, stage2, stage3;

    stage1.open("stage1.txt");
    stage2.open("stage2.txt");
    stage3.open("stage3.txt");

    cout<< setprecision(PRECISION) << fixed;
    stage1 << setprecision(PRECISION) << fixed;
	stage2 << setprecision(PRECISION) << fixed;
	stage3 << setprecision(PRECISION) << fixed;

    //read the file

    Point eye,lookat,up;
    in >> eye >> lookat >> up;

    Point l=(lookat-eye).normalize();
    Point r = (l.cross(up)).normalize();
    Point u = r.cross(l);

    //create the view matrix
    Matrix view_rot = global.getViewRotationMatrix(r,u,l);
    Matrix view_trans = global.getViewTranslationMatrix(eye);

    Matrix viewTransformMatrix = view_rot*view_trans;

    // //print l,r,u
    // l.print();
    // r.print();
    // u.print();

    //input 4th line
    double fovY, aspectRatio, near, far;
    in>> fovY >> aspectRatio >> near >> far;

    // cout<< fovY << " " << aspectRatio << " "<< near << " "<< far <<endl;

    //create the projection matrix
    Matrix projectionMatrix = global.getProjectionMatrix(fovY,aspectRatio,near,far);


    //stack of matrices
    stack<Matrix> S;

    //stack of stacks
    stack<stack<Matrix>> superS;

    S.push(global.getIdentityMatrix(4,4));

    //read the file
    string line;

    while(true)
    {
        in >> line;

        if(line == "triangle")
        {
            Point points[3];
            in >> points[0] >> points[1] >> points[2];

            //transform the points
            for(int i=0;i<3;i++)
            {
                 //stage 1: model transform
                Point model_transform = Transform(S.top(),points[i]);
                stage1 << model_transform <<endl;

                //stage 2: view transform
                Point view_transform = Transform(viewTransformMatrix,model_transform);
                stage2 << view_transform <<endl;	

                //stage 3: projection transform
                Point projection_transform = Transform(projectionMatrix,view_transform);
                stage3 << projection_transform <<endl;
            }

            stage1 << endl;
            stage2 << endl;
            stage3 << endl;
        }

        else if(line == "translate")
        {
            double tx, ty, tz;
            in >> tx >> ty >> tz;

            Point trans_point(tx,ty,tz);

            Matrix translate_matrix = global.getTranslationMatrix(trans_point);
            S.push(S.top()*translate_matrix);
        }

        else if(line == "scale")
        {
            double sx, sy, sz;
            in >> sx >> sy >> sz;

            Point scale_point(sx,sy,sz);

            Matrix scale_matrix = global.getScalingMatrix(scale_point);

            S.push(S.top()*scale_matrix);
        }

        else if(line == "rotate")
        {
            double angle, ax, ay, az;
            in >> angle >> ax >> ay >> az;

            Point axis_point(ax,ay,az);

            Matrix rotate_matrix = global.getRotationMatrix(axis_point,angle);

            S.push(S.top()*rotate_matrix);
        }

        else if( line == "push")
        {
            superS.push(S);
        }

        else if( line == "pop")
        {
            S = superS.top();
            superS.pop();
        }

        else if(line == "end")
        {
            break;
        }

        else
        {
            cout<<"Invalid Command"<<endl;
            break;
        }

    }





    //read from config file
    ifstream in1;
    if(argc > 2)
        in1.open(argv[2]);
    else
        in1.open("config.txt");

    int screen_width, screen_height;
    in1>>screen_width>>screen_height;

    double x_left, x_right, y_bottom, y_top;

    in1>>x_left;
    in1>>y_bottom;

    x_right=-x_left;
    y_top=-y_bottom;

    double z_near, z_far;
    in1>>z_near>>z_far;

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

    //close stage3.txt and config.txt
    in1.close();
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
            intersecting_points[0] = Point(INF, ys, 0 );
            intersecting_points[1] = Point(INF, ys, 1 );
            intersecting_points[2] = Point(INF, ys, 2 );


            for(int j=0;j<3;j++)
            {
                Point p1= triangle.points[(int) intersecting_points[j].z];
                Point p2 = triangle.points[(int) (intersecting_points[j].z+1)%3];

                if(p1.y != p2.y)
                {
                    intersecting_points[j].x = p1.x + (ys - p1.y)*(p1.x - p2.x)/(p1.y - p2.y);
                }
            }

            for(int j=0; j<3; j++)
            {
                Point p1= triangle.points[(int) intersecting_points[j].z];
                Point p2 = triangle.points[(int) (intersecting_points[j].z+1)%3];

                if(intersecting_points[j].x != INF)
                {
                    if(intersecting_points[j].x > max(p1.x, p2.x) || intersecting_points[j].x < min(p1.x, p2.x) || intersecting_points[j].y > max(p1.y, p2.y) || intersecting_points[j].y < min(p1.y, p2.y))
                    {
                        intersecting_points[j].x = INF;
                    }
                }
            }

            int max_index = -1, min_index = -1;

            double max_x, min_x;

            for(int j=0;j<3;j++)
            {
                if(max_index == -1 && min_index == -1)
                {
                    if(intersecting_points[j].x != INF)
                    {
                        max_index = j;
                        min_index = j;
                        max_x = intersecting_points[j].x;
                        min_x = intersecting_points[j].x;
                    }
                }
                else
                {
                    if(intersecting_points[j].x != INF)
                    {
                        if(intersecting_points[j].x > max_x)
                        {
                            max_index = j;
                            max_x = intersecting_points[j].x;
                        }
                        if(intersecting_points[j].x < min_x)
                        {
                            min_index = j;
                            min_x = intersecting_points[j].x;
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
            Point p2 = triangle.points[(int) (intersecting_points[min_index].z+1)%3];


            double za = p1.z + (intersecting_points[min_index].y - p1.y)*(p2.z - p1.z)/(p2.y - p1.y);
            

            p1= triangle.points[(int) intersecting_points[max_index].z];
            p2 = triangle.points[(int) (intersecting_points[max_index].z+1)%3];

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
        }

        cout<<endl<<endl;

    }


    ofstream out("z_buffer.txt");

    for(int i=0;i<screen_height;i++)
    {
        for(int j=0;j<screen_width;j++)
        {
            if(z_buffer[i][j] != 2.0)
                out<<z_buffer[i][j]<<"\t";

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