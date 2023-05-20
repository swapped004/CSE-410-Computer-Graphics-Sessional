//necessary C++ stl headers
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<bits/stdc++.h>
#include<iostream>
#include <fstream>

//necessary header files for openGl
#include <windows.h>
#include <GL/glut.h>


//include all the classes from the header file
#include "1705025.h"
#include "bitmap_image.hpp"

bool debug = false;
//code segment
double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;

//perspective
double fov_y = 80.0;
double windowWidth = 500.0;
double windowHeight = 500.0;

//global variables 
//for camera
//camera position
double angle_step=1.0;
double pos_step=1.0;
extern Point pos;
//camera up vector
Point u;
//camera right vector
Point r;
//camera looking direction vector
Point l;

//logfile
extern ofstream dbg;



//variables for ray tracing

//object and light source vectors
extern vector<Object*> objects;
extern vector<PointLight> pointLights;
extern vector<SpotLight> spotLights;

//recursion levels for relflection
extern int recursion_level;
//pixels along both directions
int max_pixels;



//captured image number
int image_number=1;

//load Object, PointLight, SpotLight vectors from file
void loadData()
{
	std::cout<<"in load data"<<std::endl;
	//load objects from file
	ifstream in("C:\\Users\\Swapnil\\Desktop\\vscode_openGl\\opengl-cpp-template\\src\\scene.txt");
	
	in>>recursion_level>>max_pixels;

	//number of objects
	int n;
	in >> n;
	std::cout<<"Number of objects: "<<n<<std::endl;

	//input objects
	for(int i=0;i<n;i++)
	{
		Object *temp;

		string type;
		in >> type;

		if(type == "sphere")
		{
			//centre
			Point center;
			in >> center;
			//radius
			double radius;
			in >> radius;

			temp = new Sphere(center,radius);
			
		}

		else if(type == "triangle")
		{
			//3 vertices
			Point p1,p2,p3;
			in >> p1 >> p2 >> p3;

			temp = new Triangle(p1,p2,p3);
		}

		else if(type == "general")
		{
			//Quadratic equation coefficients
			QuadraticEquationCoefficients qe;
			in >> qe;
			//centre
			Point cube_reference_point;
			in >> cube_reference_point;

			//lenght, height, width
			double length,height,width;
			in >> length >> width >> height;

			temp = new General(qe,cube_reference_point,length,height,width);

		}

		else
		{
			std::cout<<"Invalid object type"<<std::endl;
			continue;
		}

		//set other object properties
		//Color
		Color color;
		in >> color;

		//Light Coefficients
		LightCoeff coeff;
		in >> coeff;

		//shininess
		int shininess;
		in >> shininess;

		temp->setColor(color);
		temp->setCoEfficients(coeff);
		temp->setShine(shininess);

		objects.push_back(temp);
	}

	//number of point lights
	in >> n;

	//input point lights
	for(int i=0;i<n;i++)
	{
		//position
		Point position;
		in >> position;

		//Color of light
		Color color;
		in >> color;

		PointLight temp(position,color);

		pointLights.push_back(temp);
	}

	//number of spot lights
	in >> n;

	//input spot lights
	for(int i=0;i<n;i++)
	{
		//position
		Point position;
		in >> position;

		//Color of light
		Color color;
		in >> color;

		PointLight temp(position,color);

		//direction
		Point direction;
		in >> direction;

		//cutoff angle
		double cutoff_angle;
		in >> cutoff_angle;

		SpotLight temp2(temp,direction,cutoff_angle);

		spotLights.push_back(temp2);
	}

	//input floor
	
	Object *floor = new Floor(1000,20);

	//set color
	Color color = Color(0,0,0);
	floor->setColor(color);

	//set light coefficients
	LightCoeff coeff = LightCoeff(0.3, 0.4, 0.2, 0.2);
	floor->setCoEfficients(coeff);

	//set shininess
	int shininess = 3;
	floor->setShine(shininess);

	objects.push_back(floor);

}

//capture the image and save in bitmap file
void capture()
{
	std::cout<<"Capture is called"<<std::endl;

	//create a bitmap image
	bitmap_image image(max_pixels,max_pixels);

	//set the background color
	image.set_all_channels(0,0,0);

	//ray tracing algorithm
	double planeDistance = windowHeight/(2.0* tan(fov_y/2.0*PI/180.0));
	Point topleft = pos + (l*planeDistance) - r*(windowWidth/2.0) + u*(windowHeight/2.0);

	double du = windowWidth/max_pixels;
	double dv = windowHeight/max_pixels;

	//Choose middle of the grid cell
	topleft = topleft + r*(0.5*du) - u*(0.5*dv); 

	for(int i=0; i < max_pixels; i++)
	{
		for(int j=0;j < max_pixels;j++)
		{
			//calculate current pixel position
			Point curPixel = topleft + r*(i*du) - u*(j*dv);

			Point direction = curPixel - pos;
			direction.normalize();

			//cast Ray from camera to current pixel
			Ray *ray = new Ray(pos,direction);

			int nearest = -1;
			double tMin;
			tMin = INF;
	
			//for each object in the scene
			for(int k=0;k<objects.size();k++)
			{
				Color dummyColor;
				//check if the ray intersects with the object
				double t = objects[k]->intersect(ray,dummyColor,0);

				if(t < tMin && t > 0)
				{
					tMin = t;
					nearest = k;
				}
			}

			//get the color
			Color color;
			if(nearest != -1)
			{
				objects[nearest]->intersect(ray,color,1);
			}

			color.clip();

			//set the color of the pixel
			image.set_pixel(i,j,round(color.r*255.0),round(color.g*255.0),round(color.b*255.0));

			//print pixel and pixel color
			//dbg << "pixel color: " << i << " " << j << std::endl;
			//color.print();
		}
	}

	//save image
	string image_path = "C:\\Users\\Swapnil\\Desktop\\vscode_openGl\\opengl-cpp-template\\src\\Output1_"+std::to_string(image_number)+".bmp";
	image.save_image(image_path);

	std::cout<<"bmp captured"<<std::endl;
}

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




void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case '1':
		    //rotate left
            l=l*cos(angle_step*pi/180.0)+(u%l)*sin(angle_step*pi/180.0);
            //std::cout<<"l:\n"<<std::endl;
            //l.print();
            r=r*cos(angle_step*pi/180.0)+(u%r)*sin(angle_step*pi/180.0);
            //std::cout<<"r:\n"<<std::endl;
			//drawgrid=1-drawgrid;
			break;
        case '2':
            //rotate right
            l=l*cos(-angle_step*pi/180.0)+(u%l)*sin(-angle_step*pi/180.0);
            //std::cout<<"l:\n"<<std::endl;
            //l.print();
            r=r*cos(-angle_step*pi/180.0)+(u%r)*sin(-angle_step*pi/180.0);
            //std::cout<<"r:\n"<<std::endl;
            //r.print();
            break;

        case '3':
            //rotate up
            l=l*cos(angle_step*pi/180.0)+(r%l)*sin(angle_step*pi/180.0);
            //std::cout<<"l:\n"<<std::endl;
            //l.print();
            u=u*cos(angle_step*pi/180.0)+(r%u)*sin(angle_step*pi/180.0);
            //std::cout<<"u:\n"<<std::endl;
            //u.print();
            break;
         case '4':
            //rotate down
            l=l*cos(-angle_step*pi/180.0)+(r%l)*sin(-angle_step*pi/180.0);
            //std::cout<<"l:\n"<<std::endl;
            //l.print();
            u=u*cos(-angle_step*pi/180.0)+(r%u)*sin(-angle_step*pi/180.0);
            //std::cout<<"u:\n"<<std::endl;
            //u.print();
            break;

        case '5':
            //tilt clockwise
            u=u*cos(angle_step*pi/180.0)+(l%u)*sin(angle_step*pi/180.0);
            std::cout<<"u:\n"<<std::endl;
            //u.print();
            r=r*cos(angle_step*pi/180.0)+(l%r)*sin(angle_step*pi/180.0);
            std::cout<<"r:\n"<<std::endl;
            //r.print();
            break;

        case '6':
            //tilt counter-clockwise
            u=u*cos(-angle_step*pi/180.0)+(l%u)*sin(-angle_step*pi/180.0);
            std::cout<<"u:\n"<<std::endl;
            //u.print();
            r=r*cos(-angle_step*pi/180.0)+(l%r)*sin(-angle_step*pi/180.0);
            std::cout<<"r:\n"<<std::endl;
            //r.print();
            break;

        case '7':
            //look up-left

            //look up
            l=l*cos(angle_step*pi/180.0)+(r%l)*sin(angle_step*pi/180.0);
            std::cout<<"l:\n"<<std::endl;
            //l.print();
            u=u*cos(angle_step*pi/180.0)+(r%u)*sin(angle_step*pi/180.0);
            std::cout<<"u:\n"<<std::endl;
            //u.print();


            //rotate left
            l=l*cos(angle_step*pi/180.0)+(u%l)*sin(angle_step*pi/180.0);
            std::cout<<"l:\n"<<std::endl;
            //l.print();
            r=r*cos(angle_step*pi/180.0)+(u%r)*sin(angle_step*pi/180.0);
             std::cout<<"r:\n"<<std::endl;
			//drawgrid=1-drawgrid;
			break;


        case '8':
            //look up-right

            //look up
            l=l*cos(angle_step*pi/180.0)+(r%l)*sin(angle_step*pi/180.0);
            std::cout<<"l:\n"<<std::endl;
            //l.print();
            u=u*cos(angle_step*pi/180.0)+(r%u)*sin(angle_step*pi/180.0);
            std::cout<<"u:\n"<<std::endl;
            //u.print();

            //rotate right
            l=l*cos(-angle_step*pi/180.0)+(u%l)*sin(-angle_step*pi/180.0);
            std::cout<<"l:\n"<<std::endl;
            //l.print();
            r=r*cos(-angle_step*pi/180.0)+(u%r)*sin(-angle_step*pi/180.0);
            std::cout<<"r:\n"<<std::endl;
            //r.print();

            break;

        case '9':
            //look down-left

            //look down
            l=l*cos(-angle_step*pi/180.0)+(r%l)*sin(-angle_step*pi/180.0);
            std::cout<<"l:\n"<<std::endl;
            //l.print();
            u=u*cos(-angle_step*pi/180.0)+(r%u)*sin(-angle_step*pi/180.0);
            std::cout<<"u:\n"<<std::endl;
            //u.print();

            //rotate left
            l=l*cos(angle_step*pi/180.0)+(u%l)*sin(angle_step*pi/180.0);
            std::cout<<"l:\n"<<std::endl;
            //l.print();
            r=r*cos(angle_step*pi/180.0)+(u%r)*sin(angle_step*pi/180.0);
            std::cout<<"r:\n"<<std::endl;
            //r.print();

            break;

        case '0':
            //look down-right

            //look down
             l=l*cos(-angle_step*pi/180.0)+(r%l)*sin(-angle_step*pi/180.0);
            std::cout<<"l:\n"<<std::endl;
            //l.print();
            u=u*cos(-angle_step*pi/180.0)+(r%u)*sin(-angle_step*pi/180.0);
            std::cout<<"u:\n"<<std::endl;
            //u.print();

            //rotate right
            l=l*cos(-angle_step*pi/180.0)+(u%l)*sin(-angle_step*pi/180.0);
            std::cout<<"l:\n"<<std::endl;
            //l.print();
            r=r*cos(-angle_step*pi/180.0)+(u%r)*sin(-angle_step*pi/180.0);
            std::cout<<"r:\n"<<std::endl;
            //r.print();

            break;

		case 'o':
			//capture the image and save in bitmap file
			capture();
			image_number++;

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

	//make a sphere object
	// Sphere s(Point(109,0,0), 40);
	// s.setColor(Color(1.0, 0 , 0));
	// s.draw();



	//sample floor
	// Object *floor = new Floor(1000,20);

	// //set color
	// Color color = Color(0,0,0);
	// floor->setColor(color);

	// //set light coefficients
	// LightCoeff coeff = LightCoeff(0.5,0.5,0.5,0.5);
	// floor->setCoEfficients(coeff);

	// //set shininess
	// int shininess = 7;
	// floor->setShine(shininess);

	// floor->draw();

	//draw objects
	for(int i=0;i<objects.size();i++){
		glPushMatrix();
		{
			objects[i]->draw();
		}
		glPopMatrix();
	}

	//draw point lights
	for(int i=0;i<pointLights.size();i++)
	{
		glPushMatrix();
		{
			pointLights[i].draw();
		}
		glPopMatrix();
	}

	//draw directional spotlights
	for(int i=0;i<spotLights.size();i++)
	{
		glPushMatrix();
		{
			spotLights[i].draw();
		}
		glPopMatrix();
	}



	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){
	angle+=0.05;
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//grid and axes booleans
	drawgrid=0;
	drawaxes=1;
	

	//camera initialization
	pos= Point(100.0, 100.0, 0);
	//up vector
	u=Point(0, 0, 1.0);
	//looking direction vector
    l=Point(-1.0/sqrt(2.0), -1.0/sqrt(2.0), 0);
    //right vector
	r=Point(-1.0/sqrt(2.0), 1.0/sqrt(2.0), 0);

    //steps to move or rotate camera
    angle_step=2.0;
	pos_step=2.0;
	angle=0;

    //not needed as of now
    cameraHeight=150.0;
	cameraAngle=1.0;

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
	fov_y = 80.0;
	gluPerspective(fov_y,	1,	1,	1000.0);
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

	//log file
	dbg.open("log.txt", std::ios::out);
}

void free_memory()
{
	std::cout << "free memory function called"<<std::endl;
	//free memory
	//delete vectors of objects and lights
	for(int i=0;i<objects.size();i++){
		delete objects[i];
	}
	objects.clear();

	
	pointLights.clear();
}

int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("RTX - Ray Tracer");

	init();
	

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	loadData();

	//On exit, free memory
	atexit(free_memory);

	glutMainLoop();		//The main loop of OpenGL

	return 0;
}
