
#include "stdafx.h"

#include <GL\glew.h>
#include <GL\freeglut.h>

#include <thread>
#include <stdlib.h>
#include <iostream>

#include <astra\astra.hpp>

#include "CStreamListener.h"
#include "CAstraStream.h"

// Currently stream, sensor and reader variables are global because of OpenGL which
// doesn't function well when programming objective oriented

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

/// Initializes GLUT window, display functions, and other variables needed for canvas (OpenGL) to work
/// @param iArgc first parameter of main function
/// @param cppArgv second parameter of main function
/// @returns nothing
void setDrawing(int *iArgc, char **cppArgv)
{
	glutInit(iArgc, cppArgv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(640, 480);


	//glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	//glutInitWindowSize(640, 480);
	glutInitWindowPosition(350, 10);
	glutCreateWindow("Skeleton Tracking - Orbbec Astra");

	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	//glEnable(GL_COLOR_MATERIAL);
	//glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	//glutReshapeFunc(resize);

	Initialize();
	glutDisplayFunc(Draw);

	glutKeyboardFunc(keyboard);

	Timer(0);
	glutMainLoop();
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
	auto bodies = stream.getBodies();

	glClear(GL_COLOR_BUFFER_BIT); // clear display window

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPointSize(5.0f);

	/*glBegin(GL_QUADS);
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(-1.5, -5.5, 100.3);
	glVertex3f(10.5, -5.5, 100.3);
	glVertex3f(10.5, -5.5, -1.3);
	glVertex3f(-1.5, -5.5, -1.3);
	glEnd();*/


	for (auto body : bodies)
	{
		auto joints = body.joints();

		//Draw joint points
		for (int i = 0; i < joints.size() - 1; i++)
		{
			auto xi = joints[i].depth_position().x;
			auto yi = joints[i].depth_position().y;
			//auto zi = -joints[i].depth_position().z;

			if (i == 0)
			{
				glPointSize(30.0f);
			}

			glBegin(GL_POINTS);
			glColor3f(1.0, 0.0, 0.0);
			glVertex2i(xi, h - yi/*, zi*/);
			glEnd();

			if (i == 0)
			{
				glPointSize(5.0f);
			}
		}
		glColor3f(1.0, 1.0, 0.0);
		//Draw lines
		drawLine(
			joints[(int)astra::JointType::Head].depth_position().x,
			joints[(int)astra::JointType::Head].depth_position().y,
			//joints[(int)astra::JointType::Head].depth_position().z,
			joints[(int)astra::JointType::Neck].depth_position().x,
			joints[(int)astra::JointType::Neck].depth_position().y
			//,joints[(int)astra::JointType::Neck].depth_position().z
		);

		drawLine(
			joints[(int)astra::JointType::Neck].depth_position().x,
			joints[(int)astra::JointType::Neck].depth_position().y,
			//joints[(int)astra::JointType::Neck].depth_position().z,
			joints[(int)astra::JointType::ShoulderSpine].depth_position().x,
			joints[(int)astra::JointType::ShoulderSpine].depth_position().y
			//,joints[(int)astra::JointType::ShoulderSpine].depth_position().z
		);

		drawLine(
			joints[(int)astra::JointType::LeftShoulder].depth_position().x,
			joints[(int)astra::JointType::LeftShoulder].depth_position().y,
			//joints[(int)astra::JointType::LeftShoulder].depth_position().z,
			joints[(int)astra::JointType::ShoulderSpine].depth_position().x,
			joints[(int)astra::JointType::ShoulderSpine].depth_position().y
			//,joints[(int)astra::JointType::ShoulderSpine].depth_position().z
		);

		drawLine(
			joints[(int)astra::JointType::RightShoulder].depth_position().x,
			joints[(int)astra::JointType::RightShoulder].depth_position().y,
			//joints[(int)astra::JointType::RightShoulder].depth_position().z,
			joints[(int)astra::JointType::ShoulderSpine].depth_position().x,
			joints[(int)astra::JointType::ShoulderSpine].depth_position().y
			//,joints[(int)astra::JointType::ShoulderSpine].depth_position().z
		);

		drawLine(
			joints[(int)astra::JointType::RightShoulder].depth_position().x,
			joints[(int)astra::JointType::RightShoulder].depth_position().y,
			//joints[(int)astra::JointType::RightShoulder].depth_position().z,
			joints[(int)astra::JointType::RightElbow].depth_position().x,
			joints[(int)astra::JointType::RightElbow].depth_position().y
			//,joints[(int)astra::JointType::RightElbow].depth_position().z
		);

		drawLine(
			joints[(int)astra::JointType::LeftShoulder].depth_position().x,
			joints[(int)astra::JointType::LeftShoulder].depth_position().y,
			//joints[(int)astra::JointType::LeftShoulder].depth_position().z,
			joints[(int)astra::JointType::LeftElbow].depth_position().x,
			joints[(int)astra::JointType::LeftElbow].depth_position().y
			//,joints[(int)astra::JointType::LeftElbow].depth_position().z
		);

		drawLine(
			joints[(int)astra::JointType::LeftWrist].depth_position().x,
			joints[(int)astra::JointType::LeftWrist].depth_position().y,
			//joints[(int)astra::JointType::LeftWrist].depth_position().z,
			joints[(int)astra::JointType::LeftElbow].depth_position().x,
			joints[(int)astra::JointType::LeftElbow].depth_position().y
			//,joints[(int)astra::JointType::LeftElbow].depth_position().z
		);

		drawLine(
			joints[(int)astra::JointType::RightWrist].depth_position().x,
			joints[(int)astra::JointType::RightWrist].depth_position().y,
			//joints[(int)astra::JointType::RightWrist].depth_position().z,
			joints[(int)astra::JointType::RightElbow].depth_position().x,
			joints[(int)astra::JointType::RightElbow].depth_position().y
			//,joints[(int)astra::JointType::RightElbow].depth_position().z
		);

		drawLine(
			joints[(int)astra::JointType::RightWrist].depth_position().x,
			joints[(int)astra::JointType::RightWrist].depth_position().y,
			//joints[(int)astra::JointType::RightWrist].depth_position().z,
			joints[(int)astra::JointType::RightHand].depth_position().x,
			joints[(int)astra::JointType::RightHand].depth_position().y
			//,joints[(int)astra::JointType::RightHand].depth_position().z
		);

		drawLine(
			joints[(int)astra::JointType::LeftWrist].depth_position().x,
			joints[(int)astra::JointType::LeftWrist].depth_position().y,
			//joints[(int)astra::JointType::LeftWrist].depth_position().z,
			joints[(int)astra::JointType::LeftHand].depth_position().x,
			joints[(int)astra::JointType::LeftHand].depth_position().y
			//,joints[(int)astra::JointType::LeftHand].depth_position().z
		);

		drawLine(
			joints[(int)astra::JointType::MidSpine].depth_position().x,
			joints[(int)astra::JointType::MidSpine].depth_position().y,
			//joints[(int)astra::JointType::MidSpine].depth_position().z,
			joints[(int)astra::JointType::ShoulderSpine].depth_position().x,
			joints[(int)astra::JointType::ShoulderSpine].depth_position().y
			//,joints[(int)astra::JointType::ShoulderSpine].depth_position().z
		);

		drawLine(
			joints[(int)astra::JointType::MidSpine].depth_position().x,
			joints[(int)astra::JointType::MidSpine].depth_position().y,
			//joints[(int)astra::JointType::MidSpine].depth_position().z,
			joints[(int)astra::JointType::BaseSpine].depth_position().x,
			joints[(int)astra::JointType::BaseSpine].depth_position().y
			//,joints[(int)astra::JointType::BaseSpine].depth_position().z
		);

		drawLine(
			joints[(int)astra::JointType::LeftHip].depth_position().x,
			joints[(int)astra::JointType::LeftHip].depth_position().y,
			//joints[(int)astra::JointType::LeftHip].depth_position().z,
			joints[(int)astra::JointType::BaseSpine].depth_position().x,
			joints[(int)astra::JointType::BaseSpine].depth_position().y
			//,joints[(int)astra::JointType::BaseSpine].depth_position().z
		);

		drawLine(
			joints[(int)astra::JointType::RightHip].depth_position().x,
			joints[(int)astra::JointType::RightHip].depth_position().y,
			//joints[(int)astra::JointType::RightHip].depth_position().z,
			joints[(int)astra::JointType::BaseSpine].depth_position().x,
			joints[(int)astra::JointType::BaseSpine].depth_position().y
			//,joints[(int)astra::JointType::BaseSpine].depth_position().z
		);

		drawLine(
			joints[(int)astra::JointType::RightHip].depth_position().x,
			joints[(int)astra::JointType::RightHip].depth_position().y,
			//joints[(int)astra::JointType::RightHip].depth_position().z,
			joints[(int)astra::JointType::RightKnee].depth_position().x,
			joints[(int)astra::JointType::RightKnee].depth_position().y
			//,joints[(int)astra::JointType::RightKnee].depth_position().z
		);

		drawLine(
			joints[(int)astra::JointType::LeftHip].depth_position().x,
			joints[(int)astra::JointType::LeftHip].depth_position().y,
			//joints[(int)astra::JointType::LeftHip].depth_position().z,
			joints[(int)astra::JointType::LeftKnee].depth_position().x,
			joints[(int)astra::JointType::LeftKnee].depth_position().y
			//,joints[(int)astra::JointType::LeftKnee].depth_position().z
		);

		drawLine(
			joints[(int)astra::JointType::LeftFoot].depth_position().x,
			joints[(int)astra::JointType::LeftFoot].depth_position().y,
			//joints[(int)astra::JointType::LeftFoot].depth_position().z,
			joints[(int)astra::JointType::LeftKnee].depth_position().x,
			joints[(int)astra::JointType::LeftKnee].depth_position().y
			//,joints[(int)astra::JointType::LeftKnee].depth_position().z
		);

		drawLine(
			joints[(int)astra::JointType::RightFoot].depth_position().x,
			joints[(int)astra::JointType::RightFoot].depth_position().y,
			//joints[(int)astra::JointType::RightFoot].depth_position().z,
			joints[(int)astra::JointType::RightKnee].depth_position().x,
			joints[(int)astra::JointType::RightKnee].depth_position().y
			//,joints[(int)astra::JointType::RightKnee].depth_position().z
		);
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
}

void keyboard(unsigned char key, int, int)
{
	if (key == 'r') {
		//reset();
	}
	else if (key == 'q') {
		exit(0);
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

	if (!reader.get_latest_frame())
	{
		std::cout << "Camera not initialized properly\n";

		gameRunning = 0;

		return;
	}

	auto bodyStream = reader.stream<astra::BodyStream>();
	bodyStream.start();

	//auto handStream = reader.stream<astra::HandStream>();
	//handStream.start();

	reader.add_listener(stream);
}
