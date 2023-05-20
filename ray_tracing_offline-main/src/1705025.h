#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<bits/stdc++.h>
#include<iostream>

#include <cstdlib>
#include<fstream>

#include <windows.h>
#include <GL/glut.h>

using namespace std;


#define pi (2*acos(0.0))

//set INF to maximum value of double
#define INF 1e18
#define epsilon 0.0000001
#define PI acos(-1.0)


//logfile
ofstream dbg;


//define Point struct and class


class Point
{

public:
    double x;
    double y;
    double z;

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

        dbg<<"printing point:\tx= "<<x<<", y= "<<y<<" , z= "<<z<<std::endl;
    }

    void normalize()
    {
        double length=sqrt(x*x+y*y+z*z);
        x=x/length;
        y=y/length;
        z=z/length;
    }

    double length()
    {
        return sqrt(x*x+y*y+z*z);
    }

    double getDistance(Point p)
    {
        return (sqrt((x-p.x)*(x-p.x)+(y-p.y)*(y-p.y)+(z-p.z)*(z-p.z)));
    }

    friend ostream& operator<<(ostream& os, const Point& p)
    {
        os<<"("<<p.x<<","<<p.y<<","<<p.z<<")";
        return os;
    }
    
    friend istream& operator>>(istream& is, Point& p)
    {
        is>>p.x>>p.y>>p.z;
        return is;
    }

    //for add
    Point operator+(Point p);
    //for minus
    Point operator-(Point p);
    //negative
    Point neg();
    //for dot product
    double dot(Point p);
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

Point Point::neg()
{
   return Point(-x,-y,-z);
}

Point Point::operator*(double scalar)
{
    Point temp;
    temp.set_x(x*scalar);
    temp.set_y(y*scalar);
    temp.set_z(z*scalar);

    return temp;
}

double Point::dot(Point p)
{
   return x*p.get_x()+y*p.get_y()+z*p.get_z();
}

Point Point::operator%(Point p)
{
    Point temp;
    temp.set_x(y*p.get_z() - z*p.get_y());
    temp.set_y(z*p.get_x() - x*p.get_z());
    temp.set_z(x*p.get_y() - y*p.get_x());

    return temp;
}

class Color
{
public:
    double r,g,b;

    Color()
    {
        r=0.0;
        g=0.0;
        b=0.0;
    }

    Color(double r, double g, double b)
    {
        this->r=r;
        this->g=g;
        this->b=b;
    }

    friend ostream& operator<<(ostream& out, Color c)
    {
        out<<"r= "<<c.r<<" g= "<<c.g<<" b= "<<c.b<<endl;
        return out;
    }

    friend istream& operator>>(istream& in, Color& c)
    {
        in>>c.r>>c.g>>c.b;
        return in;
    }

    Color operator+(Color c)
    {
        Color temp;
        temp.r=r+c.r;
        temp.g=g+c.g;
        temp.b=b+c.b;
        return temp;
    }

    Color operator-(Color c)
    {
        Color temp;
        temp.r=r-c.r;
        temp.g=g-c.g;
        temp.b=b-c.b;
        return temp;
    }

    Color operator*(double scalar)
    {
        Color temp;
        temp.r=r*scalar;
        temp.g=g*scalar;
        temp.b=b*scalar;
        return temp;
    }

    Color operator*(Color color)
    {
        Color temp;
        
        temp.r=r*color.r;
        temp.g=g*color.g;
        temp.b=b*color.b;

        return temp;
    }

    void clip()
    {
        if(r>1.0)
            r=1.0;
        if(g>1.0)
            g=1.0;
        if(b>1.0)
            b=1.0;
        if(r<0.0)
            r=0.0;
        if(g<0.0)
            g=0.0;
        if(b<0.0)
            b=0.0;
    }

    void print()
    {
        dbg<<"r= "<<r<<" g= "<<g<<" b= "<<b<<std::endl;
    }

};

class LightCoeff
{
public:
    double ambient, diffuse, specular, reflection_coefficient;

    LightCoeff()
    {
        ambient=0.0;
        diffuse=0.0;
        specular=0.0;
        reflection_coefficient=0.0;
    }

    LightCoeff(double ambient, double diffuse, double specular, double reflection_coefficient)
    {
        this->ambient=ambient;
        this->diffuse=diffuse;
        this->specular=specular;
        this->reflection_coefficient=reflection_coefficient;
    }

    friend ostream& operator<<(ostream& out, LightCoeff lc)
    {
        out<<"ambient= "<<lc.ambient<<" diffuse= "<<lc.diffuse<<" specular= "<<lc.specular<<" reflection_coefficient= "<<lc.reflection_coefficient<<endl;
        return out;
    }

    friend istream& operator>>(istream& in, LightCoeff& lc)
    {
        in>>lc.ambient>>lc.diffuse>>lc.specular>>lc.reflection_coefficient;
        return in;
    }


};

//Ray Class
class Ray{
public:
    Point start;
    Point direction;
    Ray(Point start, Point direction)
    {
        this->start = start;
        this->direction = direction;
    }
};

//Light Sources Class
//Point Light Source Class

void LightDraw(Point p, Color c)
{
   //draw a 3d rectangle at the point p with color c
   glColor3d(c.r,c.g,c.b);
   glBegin(GL_QUADS);{
        glVertex3f(p.get_x() , p.get_y() + 10, p.get_z()+10);
        glVertex3f(p.get_x() , p.get_y() + 10, p.get_z()-10);
        glVertex3f(p.get_x() , p.get_y() - 10, p.get_z()-10);
        glVertex3f(p.get_x() , p.get_y() - 10, p.get_z()+10);
    }glEnd();
   
}

class PointLight
{

public:
    Point light_pos;
    Color color;

    PointLight()
    {
        light_pos = Point(0,0,0);
        color = Color(1,1,1);

    }

    PointLight(Point light_pos, Color color)
    {
        this->light_pos = light_pos;
        this->color = color;
    }

    void draw()
    {
        LightDraw(light_pos,color);
    }

};

class SpotLight
{

public:
    PointLight point_light;
    Point light_direction;
    double cutoff_angle;

    SpotLight(PointLight point_light, Point light_direction, double cutoff_angle)
    {
        this->point_light = point_light;
        this->light_direction = light_direction;
        this->cutoff_angle = cutoff_angle;
    }

    void draw()
    {
        point_light.draw();
        //also draw the direction of the light
        glBegin(GL_LINES);
        glVertex3d(point_light.light_pos.x,point_light.light_pos.y,point_light.light_pos.z);
        glVertex3d(point_light.light_pos.x+light_direction.x,point_light.light_pos.y+light_direction.y,point_light.light_pos.z+light_direction.z);
        glEnd();
    }
};


//Object class
class Object{
public:
    Point reference_point; // should have x, y, z (Here, point is Point3D from the specs)
    double height, width, length;

    Color color;
    LightCoeff coEfficients; // ambient, diffuse,
                            //specular, reflection coefficients
    int shine; // exponent term of specular 
    

    Object(){

    }
    virtual void draw()
    {

    }
    void setColor(Color color)
    {
        this->color = color;
    }

    void setShine(int shine)
    {
        this->shine = shine;
    }

    void setCoEfficients(LightCoeff coEfficients)
    {
        this->coEfficients = coEfficients;
    }

    virtual double intersect(Ray *r, Color &color, int level){
        return -1.0;
    }

    void addAmbientComponent(Color &color, Color intersectPoint_color)
    {
        color = intersectPoint_color*coEfficients.ambient;
        //color.clip();
    }

    void addDiffuseSpecularComponent(Color &color, Color intersectPoint_color,Point intersect_point, PointLight pointLight, Ray* castRay, Ray* ray , Point N, double angle)
    {
        //add diffuse component

        //compute lambert Value
        double lambertValue = N.dot(castRay->direction.neg());
        
        //R = 2*(L.N).N - L
        Point R = (castRay->direction) - N*((castRay->direction.dot(N))*(2.0));
        R.normalize();

        //R = R.neg();

        color = color + ((pointLight.color*cos(angle)) *intersectPoint_color)*(coEfficients.diffuse*max(lambertValue,0.0));

        //compute phongValue
        double phongValue = R.dot(ray->direction.neg());

        color = color + (pointLight.color*cos(angle)*intersectPoint_color)*(coEfficients.specular*max(pow(phongValue,shine),0.0));
    }

    void addRecursiveReflectionComponent(Color &color, Color reflected_color)
    {
        color = color + (reflected_color*coEfficients.reflection_coefficient);
    }

};





//extern variables from 1705025.cpp
Point pos;

vector<Object*> objects;
vector<PointLight> pointLights;
vector<SpotLight> spotLights;
int recursion_level;


//Shape classes
//all the Shape classes inherit from the Object class


//Sphere Shape class
class Sphere : public Object{

public:
   
    Sphere(Point center, double radius)
    {
        reference_point = center;
        length = radius;
    }
    void draw()
    {
       //draw a sphere with centre = reference_point and radius = length
       int slices = 24;
       int stacks = 24;
       //move the sphere to the reference_point
       glColor3f(color.r, color.g, color.b);
       glTranslatef(reference_point.get_x(), reference_point.get_y(), reference_point.get_z());
       drawSphere(length, slices, stacks, color);
    }


    void drawSphere(double radius,int slices,int stacks, Color color)
    {
        Point points[100][100];
        int i,j;
        double h,r;
        //generate points
        for(i=0;i<=stacks;i++)
        {
            h=radius*sin(((double)i/(double)stacks)*(pi/2));
            r=radius*cos(((double)i/(double)stacks)*(pi/2));
            for(j=0;j<=slices;j++)
            {
                points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
                points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
                points[i][j].z=h;
            }
        }
        //draw quads using generated points
        for(i=0;i<stacks;i++)
        {
            glColor3f(color.r, color.g, color.b);
            for(j=0;j<slices;j++)
            {
                glBegin(GL_QUADS);{
                    //upper hemisphere
                    glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
                    glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
                    glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
                    glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                    //lower hemisphere
                    glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
                    glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
                    glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
                    glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
                }glEnd();
            }
	    }
    }


    double intersect(Ray *r, Color &color, int level)
    {
        //calculate tmin
        Point Ro = r->start;
        Point Rd = r->direction;
        //normalize the direction vector
        //Rd.normalize();

        //calculate the vector from the sphere's centre to the ray's origin
        Point V = Ro - reference_point;

        double a = Rd.dot(Rd);
        double b = 2*V.dot(Rd);
        double c = V.dot(V) - length*length;

        double discriminant = b*b - 4*a*c;
        double tmin;

        if(discriminant < 0)
        {
            tmin = INF;
        }
        else
        {
            double t1 = (-b - sqrt(discriminant))/(2*a);
            double t2 = (-b + sqrt(discriminant))/(2*a);
            if(t1 > 0.0)
            {
                tmin = t1;
            }
            else if(t2 > 0.0)
            {
                tmin = t2;
            }
            else
            {
                tmin = INF;
            }
        }


        if(level == 0)
            return tmin;

        //illumination with Phong model
        //I	= Iaka + Ip [kd max {(L.N),0} + ks max{(R.V)k ,0}]

        //calculate the intersection point from tmin
        Point intersect_point = Ro + Rd*tmin;


        //calculate the normal at the intersection point
        Point N = intersect_point - reference_point;
        //normalize the Normal vector
        N.normalize();

        //if camera is inside the sphere, reverse the normal vector
        if(pos.getDistance(reference_point) <= length)
        {
            N = N.neg();
        }	

        //calculate the ambient component of illumination
        //here intersection point color is the same as the whole surface color of sphere
        addAmbientComponent(color, this->color);


        //calculate the diffuse and specular components of illumination

        for(int i=0; i<pointLights.size(); i++)
        {
            Point cast_dir = intersect_point-pointLights[i].light_pos;
            cast_dir.normalize();
            Ray *castRay = new Ray(pointLights[i].light_pos, cast_dir);

            //check if intersection_point is in shadow
            double t_min = INF;

            for(int j=0; j<objects.size(); j++)
            {
               Color dummyColor;
               double t = objects[j]->intersect(castRay, dummyColor, 0);

               if(t < t_min && t > 0)
               {
                   t_min = t;
               }
            }


            Point shadowIntersection_point = castRay->start + castRay->direction*t_min;

            if(shadowIntersection_point.getDistance(castRay->start) < intersect_point.getDistance(castRay->start) - epsilon)
            {
                //object is in the shadow
                //no need to compute defuse and specular components
                continue;
            }

            else
            {
                //object is not in the shadow
                //calculate the diffuse and specular component of illumination
                addDiffuseSpecularComponent(color, this->color, intersect_point, pointLights[i], castRay, r, N, 0.0);
            }
        }


        for(int i=0; i< spotLights.size();i++)
        {
            PointLight pointLight = spotLights[i].point_light; 

            Point cast_dir = intersect_point-pointLight.light_pos;
            cast_dir.normalize();

            Ray *castRay = new Ray(pointLights[i].light_pos, cast_dir);

            //check if castRay exceeds cutoff angle
            double angle = acos(spotLights[i].light_direction.dot(castRay->direction)/(spotLights[i].light_direction.length()*castRay->direction.length()));
            
            if((angle*180.0/PI) > spotLights[i].cutoff_angle)
            {
                angle =PI/2;
                //intensity is 0 and no change in color
                continue;
            }
            //check if intersection_point is in shadow
            double t_min = INF;

            for(int j=0; j<objects.size(); j++)
            {
               Color dummyColor;
               double t = objects[j]->intersect(castRay, dummyColor, 0);

               if(t < t_min && t > 0)
               {
                   t_min = t;
               }
            }


            Point shadowIntersection_point = castRay->start + castRay->direction*t_min;

            if(shadowIntersection_point.getDistance(castRay->start) < intersect_point.getDistance(castRay->start) - epsilon)
            {
                //object is in the shadow
                //no need to compute defuse and specular components
                continue;
            }

            else
            {
                //object is not in the shadow
                //calculate the diffuse and specular component of illumination
                addDiffuseSpecularComponent(color, this->color, intersect_point, pointLight, castRay,r, N, angle);
            }

        }


        if(level >= recursion_level)
        {
            return tmin;
        }


        Point reflection_direction = Rd - N*(N.dot(Rd)*2.0);
        //normalize the reflection direction vector
        reflection_direction.normalize(); 

        //Reflected Ray
        //move the intersection point a little bit towards the reflection direction
        //to avoid self reflection
        Ray *reflectedRay = new Ray(intersect_point+reflection_direction, reflection_direction);

        //find the nearest intersecting object
        double t_min = INF;
        int index = -1;
        
        for(int i=0; i< objects.size() ; i++)
        {
            Color dummyColor;
            double t = objects[i]->intersect(reflectedRay, dummyColor, 0);

            if(t < t_min && t > 0)
            {
                t_min = t;
                index = i;
            }
        }

        //get the reflected color
        Color reflected_color;
        if(index != -1)
        {
            t_min = objects[index]->intersect(reflectedRay, reflected_color, level+1);
        }

        reflected_color.clip();
        color.clip();
                
        addRecursiveReflectionComponent(color, reflected_color);

        //make sure the values of r,g,b are between 0 and 1
        color.clip();

        return tmin;
    }
};

//triangle Shape class
class Triangle : public Object{
public:
    Point p1, p2, p3;
    Triangle(Point p1, Point p2, Point p3)
    {
        reference_point = p1;
        length = p2.get_x() - p1.get_x();
        height = p2.get_y() - p1.get_y();
        width = p2.get_z() - p1.get_z();

        this->p1 = p1;
        this->p2 = p2;
        this->p3 = p3;
    }
    void draw()
    {
        glColor3f(color.r, color.g, color.b);
        glBegin(GL_TRIANGLES);{
            glVertex3f(p1.get_x(), p1.get_y(), p1.get_z());
            glVertex3f(p2.get_x(), p2.get_y(), p2.get_z());
            glVertex3f(p3.get_x(), p3.get_y(), p3.get_z());
        }glEnd();
    }

    double intersect(Ray *r, Color &color, int level)
    {
        Point Ro = r->start;
        Point Rd = r->direction;
        //normalize the direction vector
        //Rd.normalize();

        Point a = p1;
        Point b = p2;
        Point c = p3;

        double tmin;
       

        Point e1 = b - a;
        Point e2 = c - a;
        Point h = Rd%e2;
        double a_1 = e1.dot(h);



        if( a_1> -epsilon && a_1<epsilon )
        {
           tmin = INF;
        }

        else
        {
            double f = 1.0/a_1;
            Point s = Ro - a;
            double u = f*(s.dot(h));

            if( u<0.0 || u>1.0 )
            {
                tmin = INF;
            }

            else
            {
                Point q = s%e1;
                double v = f*(Rd.dot(q));
                if( v<0.0 || u+v>1.0 )
                {
                    tmin = INF;
                }

                else
                {
                    double t = f*(e2.dot(q));
                    if(t > epsilon)
                    {
                        tmin = t;
                    }
                    else
                    {
                        tmin = INF;
                    }

                }
            }
        }

        

        if(level == 0)
        {
            return tmin;
        }


        //illumination with phong model

        //get the intersection point
        Point intersect_point = Ro + Rd*tmin;

        //get the normal of the triangle
        Point N = (b - a)%(c - a);
        N.normalize();

        //get the normal in the direction of the viewer
        if(N.dot(Rd) > 0)
        {
            N = N.neg();
        }

        //intersect color is same everywhere on the triangle
        //add the ambient component
        addAmbientComponent(color, this->color);

        //calculate the diffuse and specular components of illumination

        for(int i=0; i<pointLights.size(); i++)
        {
            Point ray_dir = intersect_point - pointLights[i].light_pos;
            ray_dir.normalize();
            Ray *castRay = new Ray(pointLights[i].light_pos, ray_dir);

            //check if intersection_point is in shadow
            double t_min = INF;

            for(int j=0; j<objects.size(); j++)
            {
               Color dummyColor;
               double t = objects[j]->intersect(castRay, dummyColor, 0);

               if(t < t_min && t > 0)
               {
                   t_min = t;
               }
            }


            Point shadowIntersection_point = castRay->start + castRay->direction*t_min;

            if(shadowIntersection_point.getDistance(castRay->start) < intersect_point.getDistance(castRay->start) - epsilon)
            {
                //object is in the shadow
                //no need to compute defuse and specular components
                continue;
            }

            else
            {
                //object is not in the shadow
                //calculate the diffuse and specular component of illumination
                addDiffuseSpecularComponent(color, this->color, intersect_point, pointLights[i], castRay, r, N, 0.0);
            }
        }


        for(int i=0; i< spotLights.size();i++)
        {
            PointLight pointLight = spotLights[i].point_light; 
            Point ray_dir = intersect_point - pointLight.light_pos;
            ray_dir.normalize();
            Ray *castRay = new Ray(pointLight.light_pos, ray_dir);

            //check if castRay exceeds cutoff angle
            double angle = acos(spotLights[i].light_direction.dot(castRay->direction)/(spotLights[i].light_direction.length()*castRay->direction.length()));
            
            if((angle*180.0/PI) > spotLights[i].cutoff_angle)
            {
                //intensity is 0 and no change in color
                continue;
            }
            //check if intersection_point is in shadow
            double t_min = INF;

            for(int j=0; j<objects.size(); j++)
            {
               Color dummyColor;
               double t = objects[j]->intersect(castRay, dummyColor, 0);

               if(t < t_min && t > 0)
               {
                   t_min = t;
               }
            }


            Point shadowIntersection_point = castRay->start + castRay->direction*t_min;

            if(shadowIntersection_point.getDistance(castRay->start) < intersect_point.getDistance(castRay->start) - epsilon)
            {
                //object is in the shadow
                //no need to compute defuse and specular components
                continue;
            }

            else
            {
                //object is not in the shadow
                //calculate the diffuse and specular component of illumination
                //Color updated_color = color * cos(angle);
                addDiffuseSpecularComponent(color, this->color, intersect_point, pointLight, castRay, r,  N, angle);
            }

        }

        if(level >= recursion_level)
        {
            return tmin;
        }

        Point reflection_direction = Rd - N*(N.dot(Rd)*2.0);
        //normalize the reflection direction vector
        reflection_direction.normalize(); 

        //Reflected Ray
        //move the intersection point a little bit towards the reflection direction
        //to avoid self reflection
        Ray *reflectedRay = new Ray(intersect_point+reflection_direction, reflection_direction);

        //find the nearest intersecting object
        double t_min = INF;
        int index = -1;
        
        for(int i=0; i< objects.size() ; i++)
        {
            Color dummyColor;
            double t = objects[i]->intersect(reflectedRay, dummyColor, 0);

            if(t < t_min && t > 0)
            {
                t_min = t;
                index = i;
            }
        }

        //get the reflected color
        Color reflected_color;
        if(index != -1)
        {
            t_min = objects[index]->intersect(reflectedRay, reflected_color, level+1);
        }

        //make sure the values of r,g,b are between 0 and 1
        reflected_color.clip();
        color.clip();

        addRecursiveReflectionComponent(color, reflected_color);


        //make sure the values of r,g,b are between 0 and 1
        color.clip();

        return tmin;
    }
};

//Quadratic Equation Coefficients Class
class QuadraticEquationCoefficients{
public:
    double A, B, C, D, E, F, G, H, I, J;

    QuadraticEquationCoefficients()
    {
        A = B = C = D = E = F = G = H = I = J = 0;
    }

    QuadraticEquationCoefficients(double A, double B, double C, double D, double E, double F, double G, double H, double I, double J)
    {
        this->A = A;
        this->B = B;
        this->C = C;
        this->D = D;
        this->E = E;
        this->F = F;
        this->G = G;
        this->H = H;
        this->I = I;
        this->J = J;
    }

    friend ofstream& operator<<(ofstream& out, const QuadraticEquationCoefficients& qe)
    {
        out << qe.A << "x^2 + " << qe.B << "y^2 +" << qe.C << "z^2 +"
        << qe.D << "xy +" << qe.E << "yz +" << qe.F << "zx +" 
        << qe.G << "x +" << qe.H << "y +" << qe.I << "z +" 
        << qe.J << " = 0";
        return out;
    }

    friend ifstream& operator>>(ifstream& in, QuadraticEquationCoefficients& qe)
    {
        in >> qe.A >> qe.B >> qe.C >> qe.D >> qe.E >> qe.F >> qe.G >> qe.H >> qe.I >> qe.J;
        return in;
    }

    //print 
    void print()
    {
        dbg << "A: " << A << " B: " << B << " C: " << C << " D: " << D << " E: " << E << " F: " << F << " G: " << G << " H: " << H << " I: " << I << " J: " << J << endl;
    }
};

//general quadratic surface Shape class
class General : public Object{
public:
    QuadraticEquationCoefficients qecoeff;

    General(QuadraticEquationCoefficients qecoeff, Point reference_point, double length, double height, double width)
    {
        this->qecoeff = qecoeff;
        this->reference_point = reference_point;
        this->length = length;
        this->height = height;
        this->width = width;
    }

    void draw()
    {
        //draw a quadratic surface with equation Ax^2 + By^2 + Cz^2 + Dxy + Eyz + Fzx + Gx + Hy + Iz + J = 0
        //dont have to draw this yeeeeeeeeeee!
    }

    double intersect(Ray *r, Color &color, int level)
    {
        //qecoeff.print();
        Point Ro = r->start;
        Point Rd = r->direction;
        //normalize the direction vector
        //Rd.normalize();
        //calculate the coefficients of the quadratic equation
        double A = qecoeff.A;
        double B = qecoeff.B;
        double C = qecoeff.C;
        double D = qecoeff.D;
        double E = qecoeff.E;
        double F = qecoeff.F;
        double G = qecoeff.G;
        double H = qecoeff.H;
        double I = qecoeff.I;
        double J = qecoeff.J;

        // Aqt2 + Bqt + Cq = 0 with

        // Aq = Axd2 + Byd2 + Czd2 + Dxdyd + Exdzd + Fydzd

        // Bq = 2*Axoxd + 2*Byoyd + 2*Czozd + D(xoyd + yoxd) + E(xozd + zoxd) + F(yozd + ydzo) + Gxd + Hyd + Izd

        // Cq = Axo2 + Byo2 + Czo2 + Dxoyo + Exozo + Fyozo + Gxo + Hyo + Izo + J


        double Aq = A*Rd.x*Rd.x + B*Rd.y*Rd.y + C*Rd.z*Rd.z + D*Rd.x*Rd.y + E*Rd.x*Rd.z + F*Rd.y*Rd.z ;
        double Bq = 2*A*Ro.x*Rd.x + 2*B*Ro.y*Rd.y + 2*C*Ro.z*Rd.z + D*(Ro.x*Rd.y + Ro.y*Rd.x) + E*(Ro.x*Rd.z + Ro.z*Rd.x) + F*(Ro.y*Rd.z + Ro.z*Rd.y) + G*Rd.x + H*Rd.y + I*Rd.z;
        double Cq = A*Ro.x*Ro.x + B*Ro.y*Ro.y + C*Ro.z*Ro.z + D*Ro.x*Ro.y + E*Ro.x*Ro.z + F*Ro.y*Ro.z + G*Ro.x + H*Ro.y + I*Ro.z + J;

        double tmin, t0, t1;
        if(Aq == 0)
        {
            if(Bq != 0)
                t0 = -Cq/Bq;
            else
                t0 = INF;

            t1 = INF;
        }

        else
        {
            double discriminant = Bq*Bq - 4*Aq*Cq;

            if(discriminant < 0)
            {
                t0 = INF;
                t1 = INF;
            }

            else
            {
                t0 = (-Bq - sqrt(discriminant))/(2*Aq);
                t1 = (-Bq + sqrt(discriminant))/(2*Aq);
            }
        }


        //clipping using t0 and t1 and the reference point, length, height, width

        if(t0 != INF && t0>0)
        {
            //get intersection point
            Point intersect_point = Ro + Rd*t0;

            if(length != 0 && (intersect_point.x < reference_point.x || intersect_point.x > reference_point.x + length))
            {
                t0 = INF;
            }
            
            if(width != 0 && (intersect_point.y < reference_point.y || intersect_point.y > reference_point.y + width))
            {
                t0 = INF;
            }

            if(height != 0 && (intersect_point.z < reference_point.z || intersect_point.z > reference_point.z + height))
            {
                t0 = INF;
            }
        }

        else
        {
            t0 = INF;
        }

        if(t1 != INF && t1>0)
        {
            //get intersection point
            Point intersect_point = Ro + Rd*t1;

            if(length != 0 && (intersect_point.x < reference_point.x || intersect_point.x > reference_point.x + length))
            {
                t1 = INF;
            }
            
            if(width != 0 && (intersect_point.y < reference_point.y || intersect_point.y > reference_point.y + width))
            {
                t1 = INF;
            }

            if(height != 0 && (intersect_point.z < reference_point.z || intersect_point.z > reference_point.z + height))
            {
                t1 = INF;
            }
        }

        else
        {
            t1 = INF;
        }

        // tmin = (t0 < t1)? t0 : t1;

        if(t0 > 0 && t0 != INF)
        {
            if(t1 > 0 && t1 != INF)
            {
                tmin = (t0 < t1)? t0 : t1;
            }

            else
                tmin = t0;
        }

        else if(t1 > 0 && t1 != INF)
        {
            tmin = t1;
        }
    
        else
        {
            tmin = INF;
        }


        
        if(level == 0)
        {
            return tmin;
        }

        //illumination with phong model

        //get intersect_point
        Point intersect_point = Ro + Rd*tmin;

        //get Normal vector
        double delF_x, delF_y, delF_z;

        delF_x = 2*A*intersect_point.x + D*intersect_point.y + E*intersect_point.z + G;
        delF_y = 2*B*intersect_point.y + D*intersect_point.x + F*intersect_point.z + H;
        delF_z = 2*C*intersect_point.z + E*intersect_point.x + F*intersect_point.y + I;

        Point N(delF_x, delF_y, delF_z);
        N.normalize();
        //get the normal in direction of viewer

        if(N.dot(Rd) > 0)
        {
            N = N.neg();
        }

        //calculate the ambient component of illumination
        //here intersection point color is the same as the whole surface color of sphere
        addAmbientComponent(color, this->color);

        //calculate the diffuse and specular components of illumination

        for(int i=0; i<pointLights.size(); i++)
        {
            Point ray_dir = intersect_point - pointLights[i].light_pos;
            ray_dir.normalize();
            Ray *castRay = new Ray(pointLights[i].light_pos, ray_dir);

            //check if intersection_point is in shadow
            double t_min = INF;

            for(int j=0; j<objects.size(); j++)
            {
               Color dummyColor;
               double t = objects[j]->intersect(castRay, dummyColor, 0);

               if(t < t_min && t > 0)
               {
                   t_min = t;
               }
            }


            Point shadowIntersection_point = castRay->start + castRay->direction*t_min;

            if(shadowIntersection_point.getDistance(castRay->start) < intersect_point.getDistance(castRay->start) - epsilon)
            {
                //object is in the shadow
                //no need to compute defuse and specular components
                continue;
            }

            else
            {
                //object is not in the shadow
                //calculate the diffuse and specular component of illumination
                addDiffuseSpecularComponent(color, this->color, intersect_point, pointLights[i], castRay, r, N, 0.0);
            }
        }


        for(int i=0; i< spotLights.size();i++)
        {
            PointLight pointLight = spotLights[i].point_light; 
            Point ray_dir = intersect_point - pointLight.light_pos;
            ray_dir.normalize();
            Ray *castRay = new Ray(pointLight.light_pos, ray_dir);

            //check if castRay exceeds cutoff angle
            double angle = acos(spotLights[i].light_direction.dot(castRay->direction)/(spotLights[i].light_direction.length()*castRay->direction.length()));
            
            if((angle*180.0/PI) > spotLights[i].cutoff_angle)
            {
                //intensity is 0 and no change in color
                continue;
            }
            //check if intersection_point is in shadow
            double t_min = INF;

            for(int j=0; j<objects.size(); j++)
            {
               Color dummyColor;
               double t = objects[j]->intersect(castRay, dummyColor, 0);

               if(t < t_min && t > 0)
               {
                   t_min = t;
               }
            }


            Point shadowIntersection_point = castRay->start + castRay->direction*t_min;

            if(shadowIntersection_point.getDistance(castRay->start) < intersect_point.getDistance(castRay->start) - epsilon)
            {
                //object is in the shadow
                //no need to compute defuse and specular components
                continue;
            }

            else
            {
                //object is not in the shadow
                //calculate the diffuse and specular component of illumination
                //Color updated_color = color * cos(angle);
                addDiffuseSpecularComponent(color, this->color, intersect_point, pointLight, castRay, r, N, angle);
            }

        }

        if(level >= recursion_level)
        {
            return tmin;
        }

        Point reflection_direction = Rd - N*(N.dot(Rd)*2.0);
        //normalize the reflection direction vector
        reflection_direction.normalize(); 

        //Reflected Ray
        //move the intersection point a little bit towards the reflection direction
        //to avoid self reflection
        Ray *reflectedRay = new Ray(intersect_point+reflection_direction, reflection_direction);

        //find the nearest intersecting object
        double t_min = INF;
        int index = -1;
        
        for(int i=0; i< objects.size() ; i++)
        {
            Color dummyColor;
            double t = objects[i]->intersect(reflectedRay, dummyColor, 0);

            if(t < t_min && t > 0)
            {
                t_min = t;
                index = i;
            }
        }

        //get the reflected color
        Color reflected_color;
        if(index != -1)
        {
            t_min = objects[index]->intersect(reflectedRay, reflected_color, level+1);
        }

        //make sure the values of r,g,b are between 0 and 1
        reflected_color.clip();
        color.clip();

        addRecursiveReflectionComponent(color, reflected_color);

        //make sure the values of r,g,b are between 0 and 1
        color.clip();


        return tmin;

    }
};





//Floor Class
class Floor: public Object{
public:
    double floorWidth;
    double tileWidth;
    Floor(double floorWidth, double tileWidth)
    {
        this->floorWidth = floorWidth;
        this->tileWidth = tileWidth;
        reference_point = Point(-floorWidth/2.0, -floorWidth/2.0, 0);
        length = tileWidth;
    }

    void draw()
    {
        //draw the floor with tiles of alternating colors
        glColor3f(color.r, color.g, color.b);

        for(int i = 0; i < floorWidth/tileWidth; i++)
        {
            for(int j = 0; j < floorWidth/tileWidth; j++)
            {
                if(i%2 == 0)
                {
                    if(j%2 == 0)
                    {
                        glColor3f(color.r, color.g, color.b);
                    }
                    else
                    {
                        glColor3f(1-color.r, 1-color.g, 1-color.b);
                    }
                }
                else
                {
                    if(j%2 == 0)
                    {
                        glColor3f(1-color.r, 1-color.g, 1-color.b);
                    }
                    else
                    {
                        glColor3f(color.r, color.g, color.b);
                    }
                }
                glBegin(GL_QUADS);{
                    glVertex3f(reference_point.get_x() + i*tileWidth, reference_point.get_y() + j*tileWidth, 0);
                    glVertex3f(reference_point.get_x() + i*tileWidth, reference_point.get_y() + j*tileWidth + tileWidth, 0);
                    glVertex3f(reference_point.get_x() + i*tileWidth + tileWidth, reference_point.get_y() + j*tileWidth + tileWidth, 0);
                    glVertex3f(reference_point.get_x() + i*tileWidth + tileWidth, reference_point.get_y() + j*tileWidth, 0);
                }glEnd();
            }
        }

    }

    double intersect(Ray *r, Color &color, int level)
    {
        Point Ro = r->start;
        Point Rd = r->direction;
        
        //normalize the direction vector
        Rd.normalize();

        //H(P) = N.P + d , here d = -0

        //calculate the intersection point with the plane
        Point N = Point(0,0,1);

        //Right side of the floor
        if(N.dot(Rd) > 0)
        {
            N = N.neg();
        }

        double tmin = INF;

        //H(P) = N.P + d , here d = 0
        //if ray perpendiculur to the plane
        if(N.dot(Rd) == 0)
        {
            tmin = INF;
        }
        else
        {
            double t = -(N.dot(Ro))/(N.dot(Rd));

            if(t > 0)
            {
                //check if the intersection point is within the floor
                Point intersect_point = Ro + Rd*t;

                if(intersect_point.x < (-floorWidth/2.0) || intersect_point.x > (floorWidth/2.0) || intersect_point.y < (-floorWidth/2.0) || intersect_point.y > (floorWidth/2.0))
                {
                    tmin = INF;
                }
                else
                {
                    tmin = t;
                }
            }

            else
            {
                tmin = INF;
            }
        }

        if(level == 0)
        {
            return tmin;
        }

        //illumination with phong model
        Point intersect_point = Ro + Rd*tmin;

        Point ref_pos = intersect_point - reference_point;
        Color alternate_color = Color(1-(this->color.r), 1-(this->color.g), 1-(this->color.b));

        int temp = floor(ref_pos.x/tileWidth) + floor(ref_pos.y/tileWidth);

        Color intersect_point_color;
        if(temp%2 == 0)
        {
            intersect_point_color = this->color;
        }
        else
        {
            intersect_point_color = alternate_color;
        }

        //add Ambient light component
        addAmbientComponent(color, intersect_point_color);


        //add Diffuse and specular light component
        //calculate the diffuse and specular components of illumination

        for(int i=0; i<pointLights.size(); i++)
        {
            Point ray_dir = intersect_point - pointLights[i].light_pos;
            ray_dir.normalize();
            Ray *castRay = new Ray(pointLights[i].light_pos, ray_dir);

            //check if intersection_point is in shadow
            double t_min = INF;

            for(int j=0; j<objects.size(); j++)
            {
               Color dummyColor;
               double t = objects[j]->intersect(castRay, dummyColor, 0);

               if(t < t_min && t > 0)
               {
                   t_min = t;
               }
            }


            Point shadowIntersection_point = castRay->start + castRay->direction*t_min;

            if(shadowIntersection_point.getDistance(castRay->start) < intersect_point.getDistance(castRay->start) - epsilon)
            {
                //object is in the shadow
                //no need to compute defuse and specular components
                continue;
            }

            else
            {
                //object is not in the shadow
                //calculate the diffuse and specular component of illumination
                addDiffuseSpecularComponent(color, intersect_point_color, intersect_point, pointLights[i], castRay, r,  N, 0.0);
            }
        }

        for(int i=0; i< spotLights.size();i++)
        {
            PointLight pointLight = spotLights[i].point_light; 

            Point ray_dir = intersect_point - pointLight.light_pos;
            ray_dir.normalize();
            
            Ray *castRay = new Ray(pointLight.light_pos, ray_dir);

            //check if castRay exceeds cutoff angle
            double angle = acos(spotLights[i].light_direction.dot(castRay->direction)/(spotLights[i].light_direction.length()*castRay->direction.length()));
            //angle in degrees
            // angle = angle*180.0/PI;
            
            if((angle*180.0/PI) > spotLights[i].cutoff_angle)
            {
                //intensity is 0 and no change in color
                continue;
            }
            //check if intersection_point is in shadow
            double t_min = INF;

            for(int j=0; j<objects.size(); j++)
            {
               Color dummyColor;
               double t = objects[j]->intersect(castRay, dummyColor, 0);

               if(t < t_min && t > 0)
               {
                   t_min = t;
               }
            }


            Point shadowIntersection_point = castRay->start + castRay->direction*t_min;

            if(shadowIntersection_point.getDistance(castRay->start) < intersect_point.getDistance(castRay->start) - epsilon)
            {
                //object is in the shadow
                //no need to compute defuse and specular components
                continue;
            }

            else
            {
                //object is not in the shadow
                //calculate the diffuse and specular component of illumination
                //Color updated_color = color * cos(angle);
                addDiffuseSpecularComponent(color, intersect_point_color, intersect_point, pointLight, castRay, r,  N, angle);
            }

        }

        if(level >= recursion_level)
        {
            return tmin;
        }

        

        Point reflection_direction = Rd - N*(N.dot(Rd)*2.0);
        //normalize the reflection direction vector
        reflection_direction.normalize(); 

        //Reflected Ray
        //move the intersection point a little bit towards the reflection direction
        //to avoid self reflection
        Ray *reflectedRay = new Ray(intersect_point+reflection_direction, reflection_direction);

        //find the nearest intersecting object
        double t_min = INF;
        int index = -1;
        
        for(int i=0; i< objects.size() ; i++)
        {
            Color dummyColor;
            double t = objects[i]->intersect(reflectedRay, dummyColor, 0);

            if(t < t_min && t > 0)
            {
                t_min = t;
                index = i;
            }
        }

        //get the reflected color
        Color reflected_color;
        if(index != -1)
        {
            t_min = objects[index]->intersect(reflectedRay, reflected_color, level+1);
        }

        reflected_color.clip();
        color.clip();


        addRecursiveReflectionComponent(color, reflected_color);

        //make sure the values of r,g,b are between 0 and 1
        color.clip();


        return tmin;

    }

};