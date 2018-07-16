/**
* @file main.cpp
* @brief Main file of project
* \mainpage Description
* Project which visualizes joint tracking using ORBBEC Astra 3D camera with OpenGL
*/


#include "stdafx.h"

#include <GL\glew.h>
#include <GL\freeglut.h>

#include <thread>
#include <stdlib.h>
#include <iostream>

#include <astra\astra.hpp>

#include "CStreamListener.h"
#include "CAstraStream.h"

#define OFFSET 1.15

// Currently stream, sensor and reader variables are global because of OpenGL which
// doesn't function well when programming objective oriented

float camX = 1.5, camY = 0;

/// Class which implements astra::on_frame_ready, serves as interrupt when frame is recieved
AstraStream stream;

/// Sensor and reader are pretty self-explainatory I'd say.
/// They are used to enable particular stream, capture frame and stuff like that.
astra::StreamSet sensor;
astra::StreamReader reader;

/// GLUT window dimensions.
double w;
double h;

/// Control variable - user must choose '1' from menu to start the stream
int gameRunning = 0;

/// Draws line in OpenGL window
/// @param x1 first x coordinate
/// @param y1 first y coordinate
/// @param z1 first z coordinate - currently drawing is 2D so this is commented out
/// @param x2 second x coordinate
/// @param y2 second y coordinate
/// @param z2 second z coordinate - currently drawing is 2D so this is commented out
/// @returns nothing
void drawLine(float x1, float y1, /*float z1, */float x2, float y2/*, float z2*/);

/// OpenGL redraw function - draws joint points and lines between them
/// @returns nothing
void Draw();

/// OpenGL timer function - set to repaint canvas every x ms (in this case 20) if control variable is 1
/// @param iUnused unused
/// @returns nothing
void Timer(int iUnused);

/// Starts the camera, all the streams, adds frame listener
/// @returns nothing
void astraInit();

/// Initializes canvas (clears it and sets projection)
/// @return nothing
void Initialize();

/// Used for 3D view, currently not used
/// @param w OpenGL window width
/// @param h OpenGL window height
void resize(int w, int h);

/// OpenGL window hotkeys
/// @param key pressed key
/// @param int unused (?)
/// @param int unused (?)
/// @returns nothing
void keyboard(unsigned char key, int, int);

/* Initialize OpenGL Graphics */
void initGL() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
	glClearDepth(1.0f);                   // Set background depth to farthest
	glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
	glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
	glShadeModel(GL_SMOOTH);   // Enable smooth shading
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
	glMatrixMode(GL_MODELVIEW);     // To operate on model-view matrix

									// Render a color-cube consisting of 6 quads with different colors
	glLoadIdentity();                 // Reset the model-view matrix
	glTranslatef(camX, camY, -1500);  // Move right and into the screen

	glBegin(GL_QUADS);                // Begin drawing the color cube with 6 quads
									  // Top face (y = 1.0f)
									  // Define vertices in counter-clockwise (CCW) order with normal pointing out
	glColor3f(0.0f, 100.0f, 0.0f);     // Green
	glVertex3f(100.0f, 100.0f, -100.0f);
	glVertex3f(-100.0f, 100.0f, -100.0f);
	glVertex3f(-100.0f, 100.0f, 100.0f);
	glVertex3f(100.0f, 100.0f, 100.0f);

	// Bottom face (y = -1.0f)
	glColor3f(1.0f, 0.5f, 0.0f);     // Orange
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);

	// Front face  (z = 1.0f)
	glColor3f(1.0f, 0.0f, 0.0f);     // Red
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);

	// Back face (z = -1.0f)
	glColor3f(1.0f, 1.0f, 0.0f);     // Yellow
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);

	// Left face (x = -1.0f)
	glColor3f(0.0f, 0.0f, 1.0f);     // Blue
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);

	// Right face (x = 1.0f)
	glColor3f(1.0f, 0.0f, 1.0f);     // Magenta
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glEnd();  // End of drawing color-cube

			  // Render a pyramid consists of 4 triangles
	glLoadIdentity();                  // Reset the model-view matrix
	glTranslatef(-1.5f, 0.0f, -6.0f);  // Move left and into the screen

	glBegin(GL_TRIANGLES);           // Begin drawing the pyramid with 4 triangles
									 // Front
	glColor3f(1.0f, 0.0f, 0.0f);     // Red
	glVertex3f(0.0f, 1.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);     // Green
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glColor3f(0.0f, 0.0f, 1.0f);     // Blue
	glVertex3f(1.0f, -1.0f, 1.0f);

	// Right
	glColor3f(1.0f, 0.0f, 0.0f);     // Red
	glVertex3f(0.0f, 1.0f, 0.0f);
	glColor3f(0.0f, 0.0f, 1.0f);     // Blue
	glVertex3f(1.0f, -1.0f, 1.0f);
	glColor3f(0.0f, 1.0f, 0.0f);     // Green
	glVertex3f(1.0f, -1.0f, -1.0f);

	// Back
	glColor3f(1.0f, 0.0f, 0.0f);     // Red
	glVertex3f(0.0f, 1.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);     // Green
	glVertex3f(1.0f, -1.0f, -1.0f);
	glColor3f(0.0f, 0.0f, 1.0f);     // Blue
	glVertex3f(-1.0f, -1.0f, -1.0f);

	// Left
	glColor3f(1.0f, 0.0f, 0.0f);       // Red
	glVertex3f(0.0f, 1.0f, 0.0f);
	glColor3f(0.0f, 0.0f, 1.0f);       // Blue
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glColor3f(0.0f, 1.0f, 0.0f);       // Green
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glEnd();   // Done drawing the pyramid

	glutSwapBuffers();  // Swap the front and back frame buffers (double buffering)
}

void reshape(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
											   // Compute aspect ratio of the new window
	if (height == 0) height = 1;                // To prevent divide by 0
	GLfloat aspect = (GLfloat)width / (GLfloat)height;

	// Set the viewport to cover the new window
	glViewport(0, 0, width, height);

	// Set the aspect ratio of the clipping volume to match the viewport
	glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
	glLoadIdentity();             // Reset
								  // Enable perspective projection with fovy, aspect, zNear and zFar
	gluPerspective(45.0f, aspect, 0.1f, 10000.0f);
}

/// Initializes GLUT window, display functions, and other variables needed for canvas (OpenGL) to work
/// @param iArgc first parameter of main function
/// @param cppArgv second parameter of main function
/// @returns nothing
void setDrawing(int *iArgc, char **cppArgv)
{
	glutInit(iArgc, cppArgv);            // Initialize GLUT
	glutInitDisplayMode(GLUT_DOUBLE); // Enable double buffered mode
	glutInitWindowSize(640, 480);   // Set the window's initial width & height
	glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
	glutCreateWindow("Skeleton Tracking - Orbbec Astra");          // Create window with the given title
	glutDisplayFunc(Draw);       // Register callback handler for window re-paint event
	glutReshapeFunc(reshape);       // Register callback handler for window re-size event
	initGL();                       // Our own OpenGL initialization
	glutKeyboardFunc(keyboard);
	Timer(0);
	glutMainLoop();
	//glutInit(iArgc, cppArgv);
	//glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	//glutInitWindowSize(640, 480);


	////glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	////glutInitWindowSize(640, 480);
	//glutInitWindowPosition(350, 10);
	//glutCreateWindow("Skeleton Tracking - Orbbec Astra");

	////glEnable(GL_DEPTH_TEST);
	////glEnable(GL_LIGHTING);
	////glEnable(GL_LIGHT0);
	////glEnable(GL_COLOR_MATERIAL);
	////glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	////glutReshapeFunc(resize);

	//Initialize();
	//glutDisplayFunc(Draw);

	//glutKeyboardFunc(keyboard);

	//Timer(0);
	//glutMainLoop();
}

/// Main function of the program
int main(int iArgc, char **cppArgv)
{
	int var = 0;
	while (1)
	{
		std::cout << "Press 1 to start the camera and game, 2 to leave\n";
		std::cin >> var;

		if (var == 1)
		{
			gameRunning = 1;

			// astraInit() will set gameRunning to 1 if successfully initialized
			// TODO: implement boolean instead of void
			astraInit();

			if (gameRunning)
			{
				// Start new thread for OpenGL drawing, in this thread update camera
				std::thread t1(setDrawing, &iArgc, cppArgv);

				// Currently stream is always running
				// TODO: Combine stream.streamRunning with gameRunning, useless to use both variables
				while (stream.streamRunning)
				{
					astra_update();
				}
				// After stream is not running, set control variable and properly terminate camera
				gameRunning = 0;

				astra::terminate();

				// Wait for thread to finish
				t1.join();
			}
			var = 0;
		}
		else if (var == 2)
		{
			break;
		}
	}
	return 0;
}

void drawLine(float x1, float y1, /*float z1, */float x2, float y2/*, float z2*/)
{

	/*x1 += 320.0;
	x2 += 320.0;
	y1 += 240.0;
	y2 += 240.0;*/

	glBegin(GL_LINES);
	glVertex2f(x1, h - y1/*, -z1*/);
	glVertex2f(x2, h - y2/*, -z2*/);
	glEnd();
}

void Draw()
{
	/*
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//brise ekran
	glMatrixMode(GL_MODELVIEW);
	light();
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 1500.0, //gdje je kamera (eye)
	0.0, 0.0, 1.0, //gdje gledamo (center)
	0.0f, 1.0f, 0.0f); //up-vektor (up)
	*/

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
	glMatrixMode(GL_MODELVIEW);     // To operate on model-view matrix

	glLoadIdentity();                 // Reset the model-view matrix

	auto bodies = stream.getBodies();

	auto nearestPoint = stream.m_nearestPoint;

	//glClear(GL_COLOR_BUFFER_BIT); // clear display window

	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();

	glPointSize(5.0f);

	/*glBegin(GL_POINTS);
		glColor3f(1.0, 1.0, 1.0);
		glVertex2i(nearestPoint->x, nearestPoint->y);
	glEnd();*/

	std::cout << stream.m_pointFrame.length() << std::endl;

	for (auto body : bodies)
	{
		auto joints = body.joints();

		auto mostLeftJoint = joints[0];
		auto mostRightJoint = joints[0];
		auto mostTopJoint = joints[0];
		auto mostBottomJoint = joints[0];
		auto mostFrontJoint = joints[0];
		auto mostBackJoint = joints[0];

		for (/*auto joint : joints*/int i = 0; i < joints.size() - 1; i++)
		{
			const astra::Vector3f* data = stream.m_pointFrame.data();

			if (joints[i].status() == astra::JointStatus::NotTracked)
			{
				continue;
			}

			if (joints[i].world_position().x < mostLeftJoint.world_position().x)
			{
				mostLeftJoint = joints[i];
			}
			if (joints[i].world_position().x > mostRightJoint.world_position().x)
			{
				mostRightJoint = joints[i];
			}
			if (joints[i].world_position().y > mostTopJoint.world_position().y)
			{
				mostTopJoint = joints[i];
			}
			if (joints[i].world_position().y < mostBottomJoint.world_position().y)
			{
				mostBottomJoint = joints[i];
			}
			if (joints[i].world_position().z > mostFrontJoint.world_position().z)
			{
				mostFrontJoint = joints[i];
			}
			if (joints[i].world_position().z < mostBackJoint.world_position().z)
			{
				mostBackJoint = joints[i];
			}

			auto xi = joints[i].world_position().x/100;
			auto yi = joints[i].world_position().y/100;
			auto zi = -joints[i].world_position().z/100;

			glBegin(GL_POINTS);
			glColor3f(1.0, 0.0, 0.0);
			glVertex3f(xi, yi, zi);
			glEnd();

		}
		glLineWidth(5.0);
		glBegin(GL_LINES);
		glColor3f(1.0, 1.0, 0.0);
		glVertex3f(mostLeftJoint.world_position().x, mostBottomJoint.world_position().y, mostBackJoint.world_position().z);
		/*glVertex3f(mostRightJoint.world_position().x, mostBottomJoint.world_position().y, mostBackJoint.world_position().z);
		glVertex3f(mostLeftJoint.world_position().x, mostBottomJoint.world_position().y, mostFrontJoint.world_position().z);
		glVertex3f(mostRightJoint.world_position().x, mostBottomJoint.world_position().y, mostFrontJoint.world_position().z);
		glVertex3f(mostLeftJoint.world_position().x, mostTopJoint.world_position().y, mostBackJoint.world_position().z);
		glVertex3f(mostRightJoint.world_position().x, mostTopJoint.world_position().y, mostBackJoint.world_position().z);*/
		glVertex3f(mostLeftJoint.world_position().x, mostTopJoint.world_position().y, mostFrontJoint.world_position().z);
		/*glVertex3f(mostRightJoint.world_position().x, mostTopJoint.world_position().y, mostFrontJoint.world_position().z);*/
		glEnd();

		// Drawing point between left elbow and left shoulder
		/*auto xiLeftElbow = joints[(int)astra::JointType::LeftElbow].world_position().x / 100;
		auto yiLeftElbow = joints[(int)astra::JointType::LeftElbow].world_position().y / 100;
		auto ziLeftElbow = -joints[(int)astra::JointType::LeftElbow].world_position().z / 100;

		auto xiLeftShoulder = joints[(int)astra::JointType::LeftShoulder].world_position().x / 100;
		auto yiLeftShoulder = joints[(int)astra::JointType::LeftShoulder].world_position().y / 100;
		auto ziLeftShoulder = -joints[(int)astra::JointType::LeftShoulder].world_position().z / 100;

		auto midHandX = (xiLeftElbow + xiLeftShoulder) / 2;
		auto midHandY = (yiLeftElbow + yiLeftShoulder) / 2;
		auto midHandZ = (ziLeftElbow + ziLeftShoulder) / 2;

		glBegin(GL_POINTS);
			glColor3f(1.0, 1.0, 0.0);
			glVertex3f(midHandX, midHandY, midHandZ);
		glEnd();*/

		auto xiLeftHand = joints[(int)astra::JointType::LeftHand].world_position().x / 100;
		auto yiLeftHand = joints[(int)astra::JointType::LeftHand].world_position().y / 100;
		auto ziLeftHand = -joints[(int)astra::JointType::LeftHand].world_position().z / 100;

		auto xiLeftWrist = joints[(int)astra::JointType::LeftWrist].world_position().x / 100;
		auto yiLeftWrist = joints[(int)astra::JointType::LeftWrist].world_position().y / 100;
		auto ziLeftWrist = -joints[(int)astra::JointType::LeftWrist].world_position().z / 100;


		const astra::Vector3f* data = stream.m_pointFrame.data();

		for (int i = 0; i < stream.m_pointFrame.length(); i+=2)
		{
		const astra::Vector3f* dataPoint = data + i;
		if(dataPoint->x == 0) continue;

		//std::cout << "LEFT: " << mostLeftJoint.world_position().x / 100 << std::endl
		//	<< "RIGHT: " << mostRightJoint.world_position().x / 100 / 100 << std::endl
		//	<< "TOP: " << mostTopJoint.world_position().y / 100 << std::endl
		//	<< "BOTTOM: " << mostBottomJoint.world_position().y / 100 << std::endl
		//	<< "BACK: " << mostBackJoint.world_position().z / 100 << std::endl
		//	<< "FRONT: " << mostFrontJoint.world_position().z / 100 << std::endl;

		
		//ako je datapoint->x manji od jointpoint->x + neki_radius onda je unutar trazenog prostora
		if ((dataPoint->x / 100 > mostLeftJoint.world_position().x / 100 - OFFSET)
			&& (dataPoint->x / 100 < mostRightJoint.world_position().x / 100 + OFFSET)
			&& (dataPoint->y / 100 < mostTopJoint.world_position().y / 100 + OFFSET)
			&& (dataPoint->y / 100 > mostBottomJoint.world_position().y / 100 - OFFSET)
			&& (dataPoint->z / 100 > mostBackJoint.world_position().z / 100 - OFFSET - 1)
			&& (dataPoint->z / 100 < mostFrontJoint.world_position().z / 100 + OFFSET))
		{
			//Then I am in range
			glPointSize(1.0);
			glBegin(GL_POINTS);
			glColor3f(0.0, 1.0, 1.0);
			glVertex3f(dataPoint->x / 100, dataPoint->y / 100, -dataPoint->z / 100);
			glEnd();
		}
		

		/*glBegin(GL_POINTS);
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(dataPoint->x/100, dataPoint->y/100, -dataPoint->z/100);
		glEnd();*/
		}

		////Draw joint points
		//for (int i = 0; i < joints.size() - 1; i++)
		//{
		//	if (joints[i].status() == astra::JointStatus::NotTracked)
		//	{
		//		continue;
		//	}

		//	//TODO: Getting the midpoint between 2 joints:
		//	// x = (x1+x2) / 2
		//	// y = (y1+y2) / 2
		//	// z = (z1+z2) / 2
		//	auto xi = joints[i].depth_position().x;
		//	auto yi = joints[i].depth_position().y;
		//	//auto zi = -joints[i].depth_position().z;

		//	if (i == 0)
		//	{
		//		glPointSize(30.0f);
		//	}

		//	glBegin(GL_POINTS);
		//	glColor3f(1.0, 0.0, 0.0);
		//	glVertex2i(xi, h - yi/*, zi*/);
		//	glEnd();

		//	if (i == 0)
		//	{
		//		glPointSize(5.0f);
		//	}
		//}


		//float midHandX = (joints[(int)astra::JointType::LeftElbow].depth_position().x + joints[(int)astra::JointType::LeftShoulder].depth_position().x) / 2;
		//float midHandY = (joints[(int)astra::JointType::LeftElbow].depth_position().y + joints[(int)astra::JointType::LeftShoulder].depth_position().y) / 2;

		//glColor3f(0.0, 1.0, 0.0);
		//glBegin(GL_POINTS);
		//	glVertex2i(midHandX, h - midHandY);
		//glEnd();

		//glColor3f(1.0, 1.0, 0.0);
		////Draw lines
		//drawLine(
		//	joints[(int)astra::JointType::Head].depth_position().x,
		//	joints[(int)astra::JointType::Head].depth_position().y,
		//	//joints[(int)astra::JointType::Head].depth_position().z,
		//	joints[(int)astra::JointType::Neck].depth_position().x,
		//	joints[(int)astra::JointType::Neck].depth_position().y
		//	//,joints[(int)astra::JointType::Neck].depth_position().z
		//);

		//drawLine(
		//	joints[(int)astra::JointType::Neck].depth_position().x,
		//	joints[(int)astra::JointType::Neck].depth_position().y,
		//	//joints[(int)astra::JointType::Neck].depth_position().z,
		//	joints[(int)astra::JointType::ShoulderSpine].depth_position().x,
		//	joints[(int)astra::JointType::ShoulderSpine].depth_position().y
		//	//,joints[(int)astra::JointType::ShoulderSpine].depth_position().z
		//);

		//drawLine(
		//	joints[(int)astra::JointType::LeftShoulder].depth_position().x,
		//	joints[(int)astra::JointType::LeftShoulder].depth_position().y,
		//	//joints[(int)astra::JointType::LeftShoulder].depth_position().z,
		//	joints[(int)astra::JointType::ShoulderSpine].depth_position().x,
		//	joints[(int)astra::JointType::ShoulderSpine].depth_position().y
		//	//,joints[(int)astra::JointType::ShoulderSpine].depth_position().z
		//);

		//drawLine(
		//	joints[(int)astra::JointType::RightShoulder].depth_position().x,
		//	joints[(int)astra::JointType::RightShoulder].depth_position().y,
		//	//joints[(int)astra::JointType::RightShoulder].depth_position().z,
		//	joints[(int)astra::JointType::ShoulderSpine].depth_position().x,
		//	joints[(int)astra::JointType::ShoulderSpine].depth_position().y
		//	//,joints[(int)astra::JointType::ShoulderSpine].depth_position().z
		//);

		//drawLine(
		//	joints[(int)astra::JointType::RightShoulder].depth_position().x,
		//	joints[(int)astra::JointType::RightShoulder].depth_position().y,
		//	//joints[(int)astra::JointType::RightShoulder].depth_position().z,
		//	joints[(int)astra::JointType::RightElbow].depth_position().x,
		//	joints[(int)astra::JointType::RightElbow].depth_position().y
		//	//,joints[(int)astra::JointType::RightElbow].depth_position().z
		//);

		//drawLine(
		//	joints[(int)astra::JointType::LeftShoulder].depth_position().x,
		//	joints[(int)astra::JointType::LeftShoulder].depth_position().y,
		//	//joints[(int)astra::JointType::LeftShoulder].depth_position().z,
		//	joints[(int)astra::JointType::LeftElbow].depth_position().x,
		//	joints[(int)astra::JointType::LeftElbow].depth_position().y
		//	//,joints[(int)astra::JointType::LeftElbow].depth_position().z
		//);

		//drawLine(
		//	joints[(int)astra::JointType::LeftWrist].depth_position().x,
		//	joints[(int)astra::JointType::LeftWrist].depth_position().y,
		//	//joints[(int)astra::JointType::LeftWrist].depth_position().z,
		//	joints[(int)astra::JointType::LeftElbow].depth_position().x,
		//	joints[(int)astra::JointType::LeftElbow].depth_position().y
		//	//,joints[(int)astra::JointType::LeftElbow].depth_position().z
		//);

		//drawLine(
		//	joints[(int)astra::JointType::RightWrist].depth_position().x,
		//	joints[(int)astra::JointType::RightWrist].depth_position().y,
		//	//joints[(int)astra::JointType::RightWrist].depth_position().z,
		//	joints[(int)astra::JointType::RightElbow].depth_position().x,
		//	joints[(int)astra::JointType::RightElbow].depth_position().y
		//	//,joints[(int)astra::JointType::RightElbow].depth_position().z
		//);

		//drawLine(
		//	joints[(int)astra::JointType::RightWrist].depth_position().x,
		//	joints[(int)astra::JointType::RightWrist].depth_position().y,
		//	//joints[(int)astra::JointType::RightWrist].depth_position().z,
		//	joints[(int)astra::JointType::RightHand].depth_position().x,
		//	joints[(int)astra::JointType::RightHand].depth_position().y
		//	//,joints[(int)astra::JointType::RightHand].depth_position().z
		//);

		//drawLine(
		//	joints[(int)astra::JointType::LeftWrist].depth_position().x,
		//	joints[(int)astra::JointType::LeftWrist].depth_position().y,
		//	//joints[(int)astra::JointType::LeftWrist].depth_position().z,
		//	joints[(int)astra::JointType::LeftHand].depth_position().x,
		//	joints[(int)astra::JointType::LeftHand].depth_position().y
		//	//,joints[(int)astra::JointType::LeftHand].depth_position().z
		//);

		//drawLine(
		//	joints[(int)astra::JointType::MidSpine].depth_position().x,
		//	joints[(int)astra::JointType::MidSpine].depth_position().y,
		//	//joints[(int)astra::JointType::MidSpine].depth_position().z,
		//	joints[(int)astra::JointType::ShoulderSpine].depth_position().x,
		//	joints[(int)astra::JointType::ShoulderSpine].depth_position().y
		//	//,joints[(int)astra::JointType::ShoulderSpine].depth_position().z
		//);

		//drawLine(
		//	joints[(int)astra::JointType::MidSpine].depth_position().x,
		//	joints[(int)astra::JointType::MidSpine].depth_position().y,
		//	//joints[(int)astra::JointType::MidSpine].depth_position().z,
		//	joints[(int)astra::JointType::BaseSpine].depth_position().x,
		//	joints[(int)astra::JointType::BaseSpine].depth_position().y
		//	//,joints[(int)astra::JointType::BaseSpine].depth_position().z
		//);

		//drawLine(
		//	joints[(int)astra::JointType::LeftHip].depth_position().x,
		//	joints[(int)astra::JointType::LeftHip].depth_position().y,
		//	//joints[(int)astra::JointType::LeftHip].depth_position().z,
		//	joints[(int)astra::JointType::BaseSpine].depth_position().x,
		//	joints[(int)astra::JointType::BaseSpine].depth_position().y
		//	//,joints[(int)astra::JointType::BaseSpine].depth_position().z
		//);

		//drawLine(
		//	joints[(int)astra::JointType::RightHip].depth_position().x,
		//	joints[(int)astra::JointType::RightHip].depth_position().y,
		//	//joints[(int)astra::JointType::RightHip].depth_position().z,
		//	joints[(int)astra::JointType::BaseSpine].depth_position().x,
		//	joints[(int)astra::JointType::BaseSpine].depth_position().y
		//	//,joints[(int)astra::JointType::BaseSpine].depth_position().z
		//);

		//drawLine(
		//	joints[(int)astra::JointType::RightHip].depth_position().x,
		//	joints[(int)astra::JointType::RightHip].depth_position().y,
		//	//joints[(int)astra::JointType::RightHip].depth_position().z,
		//	joints[(int)astra::JointType::RightKnee].depth_position().x,
		//	joints[(int)astra::JointType::RightKnee].depth_position().y
		//	//,joints[(int)astra::JointType::RightKnee].depth_position().z
		//);

		//drawLine(
		//	joints[(int)astra::JointType::LeftHip].depth_position().x,
		//	joints[(int)astra::JointType::LeftHip].depth_position().y,
		//	//joints[(int)astra::JointType::LeftHip].depth_position().z,
		//	joints[(int)astra::JointType::LeftKnee].depth_position().x,
		//	joints[(int)astra::JointType::LeftKnee].depth_position().y
		//	//,joints[(int)astra::JointType::LeftKnee].depth_position().z
		//);

		//drawLine(
		//	joints[(int)astra::JointType::LeftFoot].depth_position().x,
		//	joints[(int)astra::JointType::LeftFoot].depth_position().y,
		//	//joints[(int)astra::JointType::LeftFoot].depth_position().z,
		//	joints[(int)astra::JointType::LeftKnee].depth_position().x,
		//	joints[(int)astra::JointType::LeftKnee].depth_position().y
		//	//,joints[(int)astra::JointType::LeftKnee].depth_position().z
		//);

		//drawLine(
		//	joints[(int)astra::JointType::RightFoot].depth_position().x,
		//	joints[(int)astra::JointType::RightFoot].depth_position().y,
		//	//joints[(int)astra::JointType::RightFoot].depth_position().z,
		//	joints[(int)astra::JointType::RightKnee].depth_position().x,
		//	joints[(int)astra::JointType::RightKnee].depth_position().y
		//	//,joints[(int)astra::JointType::RightKnee].depth_position().z
		//);
	}

	///////////////////////HAND POINTS??
	//Currently commented out in on_frame_ready listener because it does not work as expected
	/*
	auto handPoints = stream.getHandPoints();
	for (auto handPoint : handPoints)
	{

	auto x = handPoint.depthPosition.x;
	auto y = handPoint.depthPosition.y;

	glBegin(GL_POINTS);
	glVertex2i(x, h - y);
	glEnd();
	}
	*/

	glutSwapBuffers();
}

void Timer(int iUnused)
{
	if (gameRunning)
	{
		glutPostRedisplay();
		glutTimerFunc(20, Timer, 0);
	}
	else
	{
		glutLeaveMainLoop();
	}
}

void Initialize()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	w = glutGet(GLUT_WINDOW_WIDTH);
	h = glutGet(GLUT_WINDOW_HEIGHT);

	gluOrtho2D(0.0, w, 0.0, h);
}

void resize(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glViewport(0, 0, w, h);
	glLoadIdentity();
	gluPerspective(45, w / h, 1, 10000); // view angle u y, aspect, near, far

	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	//gluLookAt(0.0, 0.0, 5.0, //cam
	//	0.0, 0.0, 0.0,		//eye
	//	0.0, 1.0, 0.0);		//up
}

void keyboard(unsigned char key, int, int)
{
	if (key == 'r') {
		//reset();
	}
	else if (key == 'q') {
		exit(0);
	}
	else if (key == 'a')
	{
		camX--;
	}
	else if (key == 's')
	{
		camY--;
	}
	else if (key == 'd')
	{
		camX++;
	}
	else if (key == 'w')
	{
		camY++;
	}
}

void astraInit()
{
	//Status is always returning SUCCESS, don't know why
	astra_status_t status = astra::initialize();

	// Currently there is no license key so if we uncomment this line, astra won't work due to invalid license key
	//const char* licenseString = "<INSERT LICENSE KEY HERE>";
	//orbbec_body_tracking_set_license(licenseString);

	reader = sensor.create_reader();

	auto depthStream = reader.stream<astra::DepthStream>();
	depthStream.start();

	auto colorStream = reader.stream<astra::ColorStream>();
	colorStream.start();
	astra_usb_info_t info = colorStream.usb_info();
	std::cout << "pid: " << info.pid << ", vid: " << info.vid << std::endl;

	if (!reader.get_latest_frame())
	{
		std::cout << "Camera not initialized properly\n";

		gameRunning = 0;

		return;
	}

	auto bodyStream = reader.stream<astra::BodyStream>();
	bodyStream.start();
	//auto features = bodyStream.get_body_features(bodyStream.id);

	auto pointStream = reader.stream<astra::PointStream>();
	pointStream.start();

	//auto handStream = reader.stream<astra::HandStream>();
	//handStream.start();

	reader.add_listener(stream);
}
