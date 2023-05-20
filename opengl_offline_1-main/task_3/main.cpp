#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<bits/stdc++.h>
#include<iostream>

#include <windows.h>
#include <GL/glut.h>

#define pi (2*acos(0.0))

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;

//operation mode
int mode=-1;




struct point
{
	double x,y,z;
};

//Point class for making calculations easier
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

    double length()
    {
        return sqrt(x*x+y*y+z*z);
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
    double operator^(Point p);
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


double Point::operator^(Point p)
{

    std::cout<<"dot product:"<<x*p.get_x()+y*p.get_y()+z*p.get_z()<<std::endl;
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

//wheel variables
double wheel_radius;
double wheel_height;
Point vertical_axis;
Point rotate_axis;
Point horizontal_axis;
double wheel_x;
double wheel_y;
Point wheel_pos;
double wheel_angle;
double wheel_vert_angle;
double angle_step;


void drawAxes()
{
	if(drawaxes==1)
	{
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);{
			glVertex3f( 100,0,0);
			glVertex3f(-100,0,0);

			glVertex3f(0,-100,0);
			glVertex3f(0, 100,0);

			glVertex3f(0,0, 100);
			glVertex3f(0,0,-100);
		}glEnd();
	}
}


void drawGrid()
{
	int i;
	if(drawgrid==1)
	{
		glColor3f(0.6, 0.6, 0.6);	//grey
		glBegin(GL_LINES);{
			for(i=-8;i<=8;i++){

				if(i==0)
					continue;	//SKIP the MAIN axes

				//lines parallel to Y-axis
				glVertex3f(i*10, -90, 0);
				glVertex3f(i*10,  90, 0);

				//lines parallel to X-axis
				glVertex3f(-90, i*10, 0);
				glVertex3f( 90, i*10, 0);
			}
		}glEnd();
	}
}

void drawSquare(double a)
{
    //glColor3f(1.0,0.0,0.0);
	glBegin(GL_QUADS);{
		glVertex3f( wheel_height/2, a,0);
		glVertex3f( wheel_height/2,-a,0);
		glVertex3f(-wheel_height/2,-a,0);
		glVertex3f(-wheel_height/2, a,0);
	}glEnd();
}


void drawCircle(double radius,int segments)
{
    int i;
    struct point points[100];
    glColor3f(0.7,0.7,0.7);
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw segments using generated points
    for(i=0;i<segments;i++)
    {
        glBegin(GL_LINES);
        {
			glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}

void drawCone(double radius,double height,int segments)
{
    int i;
    double shade;
    struct point points[100];
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw triangles using generated points
    for(i=0;i<segments;i++)
    {
        //create shading effect
        if(i<segments/2)shade=2*(double)i/(double)segments;
        else shade=2*(1.0-(double)i/(double)segments);
        glColor3f(shade,shade,shade);

        glBegin(GL_TRIANGLES);
        {
            glVertex3f(0,0,height);
			glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}


void drawSphere(double radius,int slices,int stacks)
{
	struct point points[100][100];
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
        glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
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

void drawCylinder(double radius, int segments, double height) {
    Point points[segments+1][2];

    for(int i=0; i<=segments; i++) {
        points[i][0] = Point(radius*cos(((double)i/(double)segments)*2*pi),  radius*sin(((double)i/(double)segments)*2*pi),0);
        points[i][1] = Point(radius*cos(((double)i/(double)segments)*2*pi), radius*sin(((double)i/(double)segments)*2*pi), height);
    }

    double r=0.1;
    for(int i=0; i<segments; i++) {
        /* ensuring alternating colors in two adjacent segments */
        if(i<segments/2)
        {
            r+=0.01;
        }

        else
            r-=0.01;

        glColor3f(r,r,r);

        glBegin(GL_QUADS);
        {
			glVertex3f(points[i][0].get_x(), points[i][0].get_y(), points[i][0].get_z());
			glVertex3f(points[i][1].get_x(), points[i][1].get_y(), points[i][1].get_z());
			glVertex3f(points[i+1][1].get_x(), points[i+1][1].get_y(), points[i+1][1].get_z());
			glVertex3f(points[i+1][0].get_x(), points[i+1][0].get_y(), points[i+1][0].get_z());
        }
        glEnd();
    }
}


void drawWheel()
{
    glPushMatrix();
    {
        //glColor3f(0.1,0.2,0.4);

        glTranslatef(wheel_x,wheel_y,wheel_radius);

        glRotatef(wheel_angle,horizontal_axis.get_x(),horizontal_axis.get_y(), horizontal_axis.get_z());
        glRotatef(wheel_vert_angle,vertical_axis.get_x(),vertical_axis.get_y(), vertical_axis.get_z());


        glRotatef(90.0,1,0,0);
        glTranslatef(0,0,-wheel_height/2);
        drawCylinder(wheel_radius,100,wheel_height);

    }
    glPopMatrix();


    glPushMatrix();
    {
        glColor3f(0.6,0.6,0.6);

        glTranslatef(wheel_x,wheel_y,wheel_radius);

        glRotatef(wheel_angle,horizontal_axis.get_x(),horizontal_axis.get_y(), horizontal_axis.get_z());
        glRotatef(wheel_vert_angle,vertical_axis.get_x(),vertical_axis.get_y(), vertical_axis.get_z());


        glRotatef(45.0,0,1,0);
        glRotatef(90,0,0,1);
        glRotatef(90,1,0,0);
        drawSquare(wheel_radius);

    }
    glPopMatrix();

    glPushMatrix();
    {
        glColor3f(0.6,0.6,0.6);

        glTranslatef(wheel_x,wheel_y,wheel_radius);

        glRotatef(wheel_angle,horizontal_axis.get_x(),horizontal_axis.get_y(), horizontal_axis.get_z());
        glRotatef(wheel_vert_angle,vertical_axis.get_x(),vertical_axis.get_y(), vertical_axis.get_z());

        glRotatef(135.0,0,1,0);
        glRotatef(90,0,0,1);
        glRotatef(90,1,0,0);
        drawSquare(wheel_radius);

    }
    glPopMatrix();



}

void keyboardListener(unsigned char key, int x,int y){
    double dist;
    Point x_axis=Point(1,0,0);
    Point y_axis=Point(0,1,0);
    double dot_product, x_angle, y_angle, x_component, y_component;

	switch(key){


		case '1':
			drawgrid=1-drawgrid;
			break;

        case 's':
            wheel_angle+=angle_step;

            std::cout<<"wheel angle"<<wheel_angle<<std::endl;

            //horz_pairs[current_index].second=wheel_angle;

            dist=(angle_step/360.0)*2.0*pi*wheel_radius;


            dot_product=rotate_axis^x_axis;

            x_angle=acos(dot_product/(x_axis.length()*rotate_axis.length()));

            dot_product=rotate_axis^y_axis;
            y_angle=acos(dot_product/(y_axis.length()*rotate_axis.length()));

            std::cout<<"x_angle: "<<x_angle<<", y_angle:"<<y_angle<<std::endl;

            x_component=dist*cos(x_angle);
            y_component=dist*cos(y_angle);

            std::cout<<"x_component: "<<x_component<<", y_component:"<<y_component<<std::endl;

            wheel_x+=x_component;
            wheel_y+=y_component;

            //vertical_axis=Point(wheel_x,wheel_y,1);
            //horizontal_axis.set_x(horizontal_axis.get_x()+x_component);
            //horizontal_axis.set_y(horizontal_axis.get_y()+y_component);
            //vertical_axis.set_x(vertical_axis.get_x()+x_component);
            //vertical_axis.set_y(vertical_axis.get_y()+y_component);

            std::cout<<"vert axis:"<<std::endl;
            vertical_axis.print();
            std::cout<<"hor axis:"<<std::endl;
            horizontal_axis.print();
            std::cout<<"rot axis:"<<std::endl;
            rotate_axis.print();


            break;

        case 'w':
            wheel_angle-=angle_step;
            std::cout<<"wheel angle"<<wheel_angle<<std::endl;


            //horz_pairs[current_index].second=wheel_angle;
            dist=(angle_step/360.0)*2.0*pi*wheel_radius;


            dot_product=rotate_axis^x_axis;
            std::cout<<"rotate axis length:"<<rotate_axis.length()<<"x axis length:"<<x_axis.length()<<std::endl;
            x_angle=acos(dot_product/(x_axis.length()*rotate_axis.length()));

            dot_product=rotate_axis^y_axis;
            y_angle=acos(dot_product/(y_axis.length()*rotate_axis.length()));

            std::cout<<"x_angle: "<<x_angle<<", y_angle:"<<y_angle<<std::endl;

            x_component=dist*cos(x_angle);
            y_component=dist*cos(y_angle);
            std::cout<<"x_component: "<<x_component<<", y_component:"<<y_component<<std::endl;

            wheel_x-=x_component;
            wheel_y-=y_component;

            //vertical_axis=Point(wheel_x,wheel_y,1);
            //horizontal_axis.set_x(horizontal_axis.get_x()-x_component);
           // horizontal_axis.set_y(horizontal_axis.get_y()-y_component);
            //vertical_axis.set_x(vertical_axis.get_x()-x_component);
            //vertical_axis.set_y(vertical_axis.get_y()-y_component);

            std::cout<<"vert axis:"<<std::endl;
            vertical_axis.print();
            std::cout<<"hor axis:"<<std::endl;
            horizontal_axis.print();
            std::cout<<"rot axis:"<<std::endl;
            rotate_axis.print();

            break;

        case 'a':
            wheel_vert_angle+=angle_step;

            //change axises
            rotate_axis=rotate_axis*cos(angle_step*pi/180.0)+(vertical_axis%rotate_axis)*sin(angle_step*pi/180.0);
            horizontal_axis=horizontal_axis*cos(angle_step*pi/180.0)+(vertical_axis%horizontal_axis)*sin(angle_step*pi/180.0);

             std::cout<<"vert axis:"<<std::endl;
            vertical_axis.print();
            std::cout<<"hor axis:"<<std::endl;
            horizontal_axis.print();
            std::cout<<"rot axis:"<<std::endl;
            rotate_axis.print();


            break;

        case 'd':
            wheel_vert_angle-=angle_step;

            std::cout<<"wheel vert angle"<<wheel_vert_angle<<std::endl;

            //change axises
            rotate_axis=rotate_axis*cos(-angle_step*pi/180.0)+(vertical_axis%rotate_axis)*sin(-angle_step*pi/180.0);
            horizontal_axis=horizontal_axis*cos(-angle_step*pi/180.0)+(vertical_axis%horizontal_axis)*sin(-angle_step*pi/180.0);

             std::cout<<"vert axis:"<<std::endl;
            vertical_axis.print();
            std::cout<<"hor axis:"<<std::endl;
            horizontal_axis.print();
            std::cout<<"rot axis:"<<std::endl;
            rotate_axis.print();

            break;

		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			cameraHeight -= 3.0;
			break;
		case GLUT_KEY_UP:		// up arrow key
			cameraHeight += 3.0;
			break;

		case GLUT_KEY_RIGHT:
			cameraAngle += 0.03;
			break;
		case GLUT_KEY_LEFT:
			cameraAngle -= 0.03;
			break;

		case GLUT_KEY_PAGE_UP:
			break;
		case GLUT_KEY_PAGE_DOWN:
			break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
			break;
		case GLUT_KEY_END:
			break;

		default:
			break;
	}
}


void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
				drawaxes=1-drawaxes;
			}
			break;

		case GLUT_RIGHT_BUTTON:
			//........
			break;

		case GLUT_MIDDLE_BUTTON:
			//........
			break;

		default:
			break;
	}
}



void display(){

	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/********************
	/ set-up camera here
	********************/
	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	glLoadIdentity();

	//now give three info
	//1. where is the camera (viewer)?
	//2. where is the camera looking?
	//3. Which direction is the camera's UP direction?

	//gluLookAt(100,100,100,	0,0,0,	0,0,1);
	gluLookAt(100*cos(cameraAngle), 50*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	//gluLookAt(0,0,200,	0,0,0,	0,1,0);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	drawAxes();
	drawGrid();

    //glColor3f(1,0,0);
    //drawSquare(10);

    //drawSS();

    //drawCircle(30,24);

    //drawCone(20,50,24);

	//drawSphere(30,24,20);

	drawWheel();




	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){
	angle+=0.05;
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid=1;
	drawaxes=0;
	cameraHeight=150.0;
	cameraAngle=1.0;
	angle=0;

	//wheel initial parameters
	wheel_radius=16;
	wheel_height=8;
	wheel_x=0;
	wheel_y=0;
	wheel_angle=0;
	wheel_vert_angle=0;
	wheel_pos=Point(0,0,0);
	vertical_axis=Point(0,0,1);
	rotate_axis=Point(1,0,0);
	horizontal_axis=Point(0,1,0);
	angle_step=2.0;




	//clear the screen
	glClearColor(0,0,0,0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(80,	1,	1,	1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("My OpenGL Program");

	init();

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	glutMainLoop();		//The main loop of OpenGL

	return 0;
}
