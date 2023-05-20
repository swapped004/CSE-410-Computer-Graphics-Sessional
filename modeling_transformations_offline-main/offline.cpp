#include<bits/stdc++.h>
#include<iostream>
#include<fstream>
#include<cmath>
#include<cstdio>
#include<cstdlib>

using namespace std;

#define PI (2 * acos(0.0))

#define PRECISION 7

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


int main()
{
    cout<<"Hello There!"<<endl;

    ifstream in;
    in.open("scene.txt");

    ofstream stage1, stage2, stage3, stage4;

    stage1.open("stage1.txt");
    stage2.open("stage2.txt");
    stage3.open("stage3.txt");
    stage4.open("stage4.txt");

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
        }

    }




    return 0;
}