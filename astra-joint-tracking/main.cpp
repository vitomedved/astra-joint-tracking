
#include "stdafx.h"

#include <GL\glew.h>
#include <GL\freeglut.h>

#include <thread>
#include <stdlib.h>
#include <iostream>

#include <astra\astra.hpp>

#include "CStreamListener.h"
#include "CAstraStream.h"


AstraStream stream;

//Used for GLUT window dimensions.
double w;
double h;

//Paints canvas (X-Y pairs of each joint point including lines between them)
void Draw()
{
	auto bodies = stream.getBodies();
	if (bodies.empty())
	{
		return;
	}

	glClear(GL_COLOR_BUFFER_BIT); // clear display window

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPointSize(5.0f);

	auto joints = bodies[0].joints();

	for (auto joint : joints)
	{
		auto x = joint.depth_position().x;
		auto y = joint.depth_position().y;

		glBegin(GL_POINTS);
			glColor3f(0.0, 0.0, 0.5);
			glVertex2i(w - x, h - y);
		glEnd();
	}

	glutSwapBuffers();
}

//Timer is set to repaint canvas every x ms (in this case 20)
void Timer(int iUnused)
{
	//TODO: check if is good
	glutPostRedisplay();
	glutTimerFunc(20, Timer, 0);
	//astra_update();
}

//Initializes canvas (clears it and sets projection)
void Initialize()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	w = glutGet(GLUT_WINDOW_WIDTH);
	h = glutGet(GLUT_WINDOW_HEIGHT);

	gluOrtho2D(0.0, w, 0.0, h);
}

//Initializes GLUT window, display functions, and other variavles needed for canvas (OpenGL) to work
void setDrawing(int *iArgc, char **cppArgv)
{
	glutInit(iArgc, cppArgv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(958, 718);
	glutInitWindowPosition(350, 10);
	glutCreateWindow("Skeleton Tracking");
	Initialize();
	glutDisplayFunc(Draw);
	Timer(0);
	glutMainLoop();
}

int main(int iArgc, char **cppArgv)
{

	astra::initialize();

	const char* licenseString = "<INSERT LICENSE KEY HERE>";
	orbbec_body_tracking_set_license(licenseString);

	astra::StreamSet sensor;
	astra::StreamReader reader = sensor.create_reader();

	//AstraStream listener;

	auto depthStream = reader.stream<astra::DepthStream>();
	depthStream.start();

	auto bodyStream = reader.stream<astra::BodyStream>();
	bodyStream.start();

	reader.add_listener(stream);

	std::thread t1(setDrawing, &iArgc, cppArgv);

	while (true)
	{
		astra_update();
	}

	astra::terminate();

	//AstraStream stream;

	//stream.init();

	//std::thread t1(setDrawing, &iArgc, cppArgv);
	//setDrawing(iArgc, cppArgv);

	while (stream.streamRunning)
	{
		astra_update();
	}

	t1.join();
	stream.terminate();

	return 0;
}
