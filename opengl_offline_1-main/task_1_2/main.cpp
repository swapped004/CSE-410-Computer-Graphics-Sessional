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

/*
Point Point::operator.(const Point p)
{
    Point temp;
    temp.set_x(x*p.get_x());
    temp.set_y(y*p.get_y());
    temp.set_z(z*p.get_z());

    return temp;
}*/

Point Point::operator%(Point p)
{
    Point temp;
    temp.set_x(y*p.get_z() - z*p.get_y());
    temp.set_y(z*p.get_x() - x*p.get_z());
    temp.set_z(x*p.get_y() - y*p.get_x());

    return temp;
}

//global variables
//for camera
//camera position
double angle_step=1.0;
double pos_step=1.0;
Point pos;
//camera up vector
Point u;
//camera right vector
Point r;
//camera looking direction vector
Point l;

//half hemisphere radius
double hemi_rad;
double offset;

//cylinder parameters
double cyl_height;
double cyl_radius;
double cyl_offset;

//square parameters
double sqr_side;
double sqr_offset;

void drawAxes()
{
	if(drawaxes==1)
	{
		glColor3f(1.0, 0, 0);
		glBegin(GL_LINES);{
			glVertex3f( 100,0,0);
			glVertex3f(-100,0,0);

			glColor3f(0,0, 1.0);

			glVertex3f(0,-100,0);
			glVertex3f(0, 100,0);
            glColor3f(0, 1.0, 0);

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
	glBegin(GL_QUADS);{
		glVertex3f( a/2.0, a/2.0,0);
		glVertex3f( a/2.0,-a/2.0,0);
		glVertex3f(-a/2.0,-a/2.0,0);
		glVertex3f(-a/2.0, a/2.0,0);
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


void drawSphere(double radius,int slices,int stacks, bool lower)
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
			points[i][j].x=r*cos(((double)j/(double)slices)*(pi/2));
			points[i][j].y=r*sin(((double)j/(double)slices)*(pi/2));
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
        //glColor3f(1,0,0);
		for(j=0;j<slices;j++)
		{
			glBegin(GL_QUADS);{
			    //upper hemisphere
			    if(!lower)
                {
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                }

                 //lower hemisphere
                else
                {
                    glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
                    glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
                    glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
                    glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);

                }

			}glEnd();
		}
	}
}

void drawCylinder(double radius, int segments, double height) {
    Point points[segments+1][2];

    for(int i=0; i<=segments; i++) {
        points[i][0] = Point(radius*cos(((double)i/(double)segments)*pi/2),  radius*sin(((double)i/(double)segments)*pi/2),0);
        points[i][1] = Point(radius*cos(((double)i/(double)segments)*pi/2), radius*sin(((double)i/(double)segments)*pi/2), height);
    }

    for(int i=0; i<segments; i++) {
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


void drawSS()
{
    glColor3f(1,0,0);
    drawSquare(20);

    glRotatef(angle,0,0,1);
    glTranslatef(110,0,0);
    glRotatef(2*angle,0,0,1);
    glColor3f(0,1,0);
    drawSquare(15);

    glPushMatrix();
    {
        glRotatef(angle,0,0,1);
        glTranslatef(60,0,0);
        glRotatef(2*angle,0,0,1);
        glColor3f(0,0,1);
        drawSquare(10);
    }
    glPopMatrix();

    glRotatef(3*angle,0,0,1);
    glTranslatef(40,0,0);
    glRotatef(4*angle,0,0,1);
    glColor3f(1,1,0);
    drawSquare(5);
}

void drawAllSpheres()
{
    glPushMatrix();
    {
        glColor3f(1,0,0);
        glTranslatef(offset,offset,offset);
        drawSphere(hemi_rad,24,24,false);
    }
    glPopMatrix();


    glPushMatrix();
    {
        glColor3f(1,0,0);
        glTranslatef(-offset,-offset,offset);
        glRotatef(180.0,0,0,1);
        drawSphere(hemi_rad,24,24,false);

    }
    glPopMatrix();

    glPushMatrix();
    {
        glColor3f(1,0,0);
        glTranslatef(offset,-offset,offset);
        glRotatef(-90.0,0,0,1);
        drawSphere(hemi_rad,24,24,false);

    }
    glPopMatrix();

     glPushMatrix();
    {
        glColor3f(1,0,0);
        glTranslatef(-offset,offset,offset);
        glRotatef(90.0,0,0,1);
        drawSphere(hemi_rad,24,24,false);

    }
    glPopMatrix();



    //lower 4 half hemispheres
    glPushMatrix();
    {
        glColor3f(1,0,0);
        glTranslatef(offset,offset,-offset);
        drawSphere(hemi_rad,24,24,true);
    }
    glPopMatrix();


    glPushMatrix();
    {
        glColor3f(1,0,0);
        glTranslatef(-offset,-offset,-offset);
        glRotatef(180.0,0,0,1);
        drawSphere(hemi_rad,24,24,true);

    }
    glPopMatrix();

    glPushMatrix();
    {
        glColor3f(1,0,0);
        glTranslatef(offset,-offset,-offset);
        glRotatef(-90.0,0,0,1);
        drawSphere(hemi_rad,24,24,true);

    }
    glPopMatrix();

     glPushMatrix();
    {
        glColor3f(1,0,0);
        glTranslatef(-offset,offset,-offset);
        glRotatef(90.0,0,0,1);
        drawSphere(hemi_rad,24,24,true);

    }
    glPopMatrix();

}

void drawAllCylinders()
{
    //vertical cylinders
    glPushMatrix();
    {
        glColor3f(0,1,0);
        glTranslatef(cyl_offset,cyl_offset,-cyl_offset);
        drawCylinder(hemi_rad,100,cyl_height);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glColor3f(0,1,0);
        glTranslatef(-cyl_offset,-cyl_offset,-cyl_offset);
        glRotatef(180.0,0,0,1);
        drawCylinder(hemi_rad,100,cyl_height);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glColor3f(0,1,0);
        glTranslatef(-cyl_offset,cyl_offset,-cyl_offset);
        glRotatef(90.0,0,0,1);
        drawCylinder(hemi_rad,100,cyl_height);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glColor3f(0,1,0);
        glTranslatef(cyl_offset,-cyl_offset,-cyl_offset);
        glRotatef(-90.0,0,0,1);
        drawCylinder(hemi_rad,100,cyl_height);
    }
    glPopMatrix();

    //horizontal upper 4 cylinders
    glPushMatrix();
    {
        glColor3f(0,1,0);

        glTranslatef(offset,0,offset);
        glRotatef(-45,0,offset,0);
        glRotatef(90,1,0,0);
        glRotatef(-45,0,0,1);
        glTranslatef(0,0,-offset);
        drawCylinder(hemi_rad,100,cyl_height);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glColor3f(0,1,0);

        glTranslatef(offset,0,-offset);
        glRotatef(45,0,offset,0);
        glRotatef(90,1,0,0);
        glRotatef(-45,0,0,1);
        glTranslatef(0,0,-offset);
        drawCylinder(hemi_rad,100,cyl_height);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glColor3f(0,1,0);

        glTranslatef(0,offset,offset);
        glRotatef(45,offset,0,0);
        glRotatef(90,0,1,0);
        glRotatef(45,0,0,1);
        glTranslatef(0,0,-offset);
        drawCylinder(hemi_rad,100,cyl_height);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glColor3f(0,1,0);

        glTranslatef(0,offset,-offset);
        glRotatef(-45,offset,0,0);
        glRotatef(90,0,1,0);
        glRotatef(45,0,0,1);
        glTranslatef(0,0,-offset);
        drawCylinder(hemi_rad,100,cyl_height);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glColor3f(0,1,0);

        glTranslatef(-offset,0,offset);
        glRotatef(135,0,offset,0);
        glRotatef(90,1,0,0);
        glRotatef(180,0,0,1);
        glRotatef(45,0,0,1);
        glTranslatef(0,0,-offset);
        drawCylinder(hemi_rad,100,cyl_height);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glColor3f(0,1,0);

        glTranslatef(-offset,0,-offset);
        glRotatef(45,0,offset,0);
        glRotatef(90,1,0,0);
        glRotatef(180,0,0,1);
        glRotatef(45,0,0,1);
        glTranslatef(0,0,-offset);
        drawCylinder(hemi_rad,100,cyl_height);
    }
    glPopMatrix();

     glPushMatrix();
    {
        glColor3f(0,1,0);

        glTranslatef(0,-offset,offset);
        glRotatef(135,offset,0,0);
        glRotatef(-90,0,1,0);
        glRotatef(45,0,0,1);
        glTranslatef(0,0,-offset);
        drawCylinder(hemi_rad,100,cyl_height);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glColor3f(0,1,0);

        glTranslatef(0,-offset,-offset);
        glRotatef(-135,offset,0,0);
        glRotatef(-90,0,1,0);
        glRotatef(45,0,0,1);
        glTranslatef(0,0,-offset);
        drawCylinder(hemi_rad,100,cyl_height);
    }
    glPopMatrix();




}

void drawAllSquares()
{
    glPushMatrix();
    {
        glColor3f(1,1,1);
        glTranslatef(0,offset+hemi_rad,0);
        glRotatef(90,1,0,0);
        drawSquare(sqr_side);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glColor3f(1,1,1);
        glTranslatef(0,-offset-hemi_rad,0);
        glRotatef(90,1,0,0);
        drawSquare(sqr_side);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glColor3f(1,1,1);
        glTranslatef(offset+hemi_rad,0,0);
        glRotatef(90,0,1,0);
        drawSquare(sqr_side);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glColor3f(1,1,1);
        glTranslatef(-offset-hemi_rad,0,0);
        glRotatef(90,0,1,0);
        drawSquare(sqr_side);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glColor3f(1,1,1);
        glTranslatef(0,0,offset+hemi_rad);
        drawSquare(sqr_side);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glColor3f(1,1,1);
        glTranslatef(0,0,-offset-hemi_rad);
        drawSquare(sqr_side);
    }
    glPopMatrix();

}

void drawAll()
{
    drawAllSquares();
    drawAllSpheres();
    //glClearColor(0,0,0,0);
    drawAllCylinders();


}





void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case '1':
		    //rotate left
            l=l*cos(angle_step*pi/180.0)+(u%l)*sin(angle_step*pi/180.0);
            std::cout<<"l:\n"<<std::endl;
            l.print();
            r=r*cos(angle_step*pi/180.0)+(u%r)*sin(angle_step*pi/180.0);
             std::cout<<"r:\n"<<std::endl;
			//drawgrid=1-drawgrid;
			break;
        case '2':
             //rotate right
            l=l*cos(-angle_step*pi/180.0)+(u%l)*sin(-angle_step*pi/180.0);
            std::cout<<"l:\n"<<std::endl;
            l.print();
            r=r*cos(-angle_step*pi/180.0)+(u%r)*sin(-angle_step*pi/180.0);
            std::cout<<"r:\n"<<std::endl;
            r.print();
            break;

        case '3':
             //rotate up
            l=l*cos(angle_step*pi/180.0)+(r%l)*sin(angle_step*pi/180.0);
            std::cout<<"l:\n"<<std::endl;
            l.print();
            u=u*cos(angle_step*pi/180.0)+(r%u)*sin(angle_step*pi/180.0);
            std::cout<<"u:\n"<<std::endl;
            u.print();
            break;
         case '4':
             //rotate down
            l=l*cos(-angle_step*pi/180.0)+(r%l)*sin(-angle_step*pi/180.0);
            std::cout<<"l:\n"<<std::endl;
            l.print();
            u=u*cos(-angle_step*pi/180.0)+(r%u)*sin(-angle_step*pi/180.0);
            std::cout<<"u:\n"<<std::endl;
            u.print();
            break;

        case '5':
             //tilt clockwise
            u=u*cos(angle_step*pi/180.0)+(l%u)*sin(angle_step*pi/180.0);
            std::cout<<"u:\n"<<std::endl;
            u.print();
            r=r*cos(angle_step*pi/180.0)+(l%r)*sin(angle_step*pi/180.0);
            std::cout<<"r:\n"<<std::endl;
            r.print();
            break;

        case '6':
             //tilt counter-clockwise
            u=u*cos(-angle_step*pi/180.0)+(l%u)*sin(-angle_step*pi/180.0);
            std::cout<<"u:\n"<<std::endl;
            u.print();
            r=r*cos(-angle_step*pi/180.0)+(l%r)*sin(-angle_step*pi/180.0);
            std::cout<<"r:\n"<<std::endl;
            r.print();
            break;


		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
		    pos=pos-(l*pos_step);
			cameraHeight -= 3.0;
			break;
		case GLUT_KEY_UP:		// up arrow key
		    pos=pos+(l*pos_step);
			cameraHeight += 3.0;
			break;

		case GLUT_KEY_RIGHT:
		    pos=pos+(r*pos_step);
			cameraAngle += 0.03;
			break;
		case GLUT_KEY_LEFT:
		    pos=pos-(r*pos_step);
			cameraAngle -= 0.03;
			break;

		case GLUT_KEY_PAGE_UP:
		    pos=pos+(u*pos_step);
			break;
		case GLUT_KEY_PAGE_DOWN:
		    pos=pos-(u*pos_step);
			break;

		case GLUT_KEY_INSERT:
		     //restore camera position to default
            //camera position
            pos= Point(100.0, 100.0, 0);
            //up vector
            u=Point(0, 0, 1.0);
            //looking direction vector
            l=Point(-1.0/sqrt(2.0), -1.0/sqrt(2.0), 0);
            //right vector
            r=Point(-1.0/sqrt(2.0), 1.0/sqrt(2.0), 0);
			break;

		case GLUT_KEY_HOME:


		    if(offset>0)
            {
                hemi_rad+=1.0;
                offset-=1.0;
                cyl_offset=offset;
                cyl_height-=2.0;
                sqr_side=cyl_height;
            }


		    break;


		case GLUT_KEY_END:
		    if(hemi_rad>0)
            {
                 hemi_rad-=1.0;
                offset+=1.0;
                cyl_offset=offset;
                cyl_height+=2.0;
                sqr_side=cyl_height;

            }
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
	//gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	//gluLookAt(0,0,200,	0,0,0,	0,1,0);

	gluLookAt(pos.get_x(), pos.get_y(), pos.get_z(),        pos.get_x()+l.get_x(),pos.get_y()+l.get_y(),pos.get_z()+l.get_z(),       u.get_x(),u.get_y(),u.get_z() );


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

	//drawCube(30);
	//drawSquare(45);

	//draw_cylinder(16,64,100,0,0);
	//drawCylinder(16,100,64);
	drawAll();




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
	drawaxes=1;
	cameraHeight=150.0;
	cameraAngle=1.0;
	angle_step=2.0;
	pos_step=2.0;
	angle=0;

	//
	hemi_rad=4;
	offset=32;
	cyl_height=2*offset;
	cyl_radius=hemi_rad;
	cyl_offset=offset;
	sqr_side=cyl_height;


		//camera initialization
	pos= Point(100.0, 100.0, 0);
	//up vector
	u=Point(0, 0, 1.0);
	//looking direction vector
    l=Point(-1.0/sqrt(2.0), -1.0/sqrt(2.0), 0);
    //right vector
	r=Point(-1.0/sqrt(2.0), 1.0/sqrt(2.0), 0);

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





	printf("pos_x: %f", pos.get_x());
	printf("pos_y: %f", pos.get_y());
	printf("pos_z: %f", pos.get_z());
	printf("l_x: %f", l.get_x());
	printf("l_y: %f", l.get_y());
	printf("l_z: %f", l.get_z());
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
