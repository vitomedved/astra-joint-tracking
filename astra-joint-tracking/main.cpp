
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

void drawLine(float x1, float y1, float x2, float y2)
{
	glBegin(GL_LINES);
		glVertex2f(x1, h - y1);
		glVertex2f(x2, h - y2);
	glEnd();
}

//Paints canvas (X-Y pairs of each joint point including lines between them)
void Draw()
{
	auto bodies = stream.getBodies();

	glClear(GL_COLOR_BUFFER_BIT); // clear display window

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPointSize(5.0f);

	for (auto body : bodies)
	{
		auto joints = body.joints();

		//Draw joint points
		for (int i = 0; i < joints.size() - 1; i++)
		{
			auto xi = joints[i].depth_position().x;
			auto yi = joints[i].depth_position().y;

			if (i == 0)
			{
				glPointSize(30.0f);
			}

			glBegin(GL_POINTS);
			glColor3f(1.0, 1.0, 0.0);
			glVertex2i(xi, h - yi);
			glEnd();

			if (i == 0)
			{
				glPointSize(5.0f);
			}
		}

		//Draw lines
		drawLine(
			joints[(int)astra::JointType::Head].depth_position().x,
			joints[(int)astra::JointType::Head].depth_position().y,
			joints[(int)astra::JointType::Neck].depth_position().x,
			joints[(int)astra::JointType::Neck].depth_position().y
		);

		drawLine(
			joints[(int)astra::JointType::Neck].depth_position().x,
			joints[(int)astra::JointType::Neck].depth_position().y,
			joints[(int)astra::JointType::ShoulderSpine].depth_position().x,
			joints[(int)astra::JointType::ShoulderSpine].depth_position().y
		);

		drawLine(
			joints[(int)astra::JointType::LeftShoulder].depth_position().x,
			joints[(int)astra::JointType::LeftShoulder].depth_position().y,
			joints[(int)astra::JointType::ShoulderSpine].depth_position().x,
			joints[(int)astra::JointType::ShoulderSpine].depth_position().y
		);

		drawLine(
			joints[(int)astra::JointType::RightShoulder].depth_position().x,
			joints[(int)astra::JointType::RightShoulder].depth_position().y,
			joints[(int)astra::JointType::ShoulderSpine].depth_position().x,
			joints[(int)astra::JointType::ShoulderSpine].depth_position().y
		);

		drawLine(
			joints[(int)astra::JointType::RightShoulder].depth_position().x,
			joints[(int)astra::JointType::RightShoulder].depth_position().y,
			joints[(int)astra::JointType::RightElbow].depth_position().x,
			joints[(int)astra::JointType::RightElbow].depth_position().y
		);

		drawLine(
			joints[(int)astra::JointType::LeftShoulder].depth_position().x,
			joints[(int)astra::JointType::LeftShoulder].depth_position().y,
			joints[(int)astra::JointType::LeftElbow].depth_position().x,
			joints[(int)astra::JointType::LeftElbow].depth_position().y
		);

		drawLine(
			joints[(int)astra::JointType::LeftWrist].depth_position().x,
			joints[(int)astra::JointType::LeftWrist].depth_position().y,
			joints[(int)astra::JointType::LeftElbow].depth_position().x,
			joints[(int)astra::JointType::LeftElbow].depth_position().y
		);

		drawLine(
			joints[(int)astra::JointType::RightWrist].depth_position().x,
			joints[(int)astra::JointType::RightWrist].depth_position().y,
			joints[(int)astra::JointType::RightElbow].depth_position().x,
			joints[(int)astra::JointType::RightElbow].depth_position().y
		);

		drawLine(
			joints[(int)astra::JointType::RightWrist].depth_position().x,
			joints[(int)astra::JointType::RightWrist].depth_position().y,
			joints[(int)astra::JointType::RightHand].depth_position().x,
			joints[(int)astra::JointType::RightHand].depth_position().y
		);

		drawLine(
			joints[(int)astra::JointType::LeftWrist].depth_position().x,
			joints[(int)astra::JointType::LeftWrist].depth_position().y,
			joints[(int)astra::JointType::LeftHand].depth_position().x,
			joints[(int)astra::JointType::LeftHand].depth_position().y
		);

		drawLine(
			joints[(int)astra::JointType::MidSpine].depth_position().x,
			joints[(int)astra::JointType::MidSpine].depth_position().y,
			joints[(int)astra::JointType::ShoulderSpine].depth_position().x,
			joints[(int)astra::JointType::ShoulderSpine].depth_position().y
		);

		drawLine(
			joints[(int)astra::JointType::MidSpine].depth_position().x,
			joints[(int)astra::JointType::MidSpine].depth_position().y,
			joints[(int)astra::JointType::BaseSpine].depth_position().x,
			joints[(int)astra::JointType::BaseSpine].depth_position().y
		);

		drawLine(
			joints[(int)astra::JointType::LeftHip].depth_position().x,
			joints[(int)astra::JointType::LeftHip].depth_position().y,
			joints[(int)astra::JointType::BaseSpine].depth_position().x,
			joints[(int)astra::JointType::BaseSpine].depth_position().y
		);

		drawLine(
			joints[(int)astra::JointType::RightHip].depth_position().x,
			joints[(int)astra::JointType::RightHip].depth_position().y,
			joints[(int)astra::JointType::BaseSpine].depth_position().x,
			joints[(int)astra::JointType::BaseSpine].depth_position().y
		);

		drawLine(
			joints[(int)astra::JointType::RightHip].depth_position().x,
			joints[(int)astra::JointType::RightHip].depth_position().y,
			joints[(int)astra::JointType::RightKnee].depth_position().x,
			joints[(int)astra::JointType::RightKnee].depth_position().y
		);

		drawLine(
			joints[(int)astra::JointType::LeftHip].depth_position().x,
			joints[(int)astra::JointType::LeftHip].depth_position().y,
			joints[(int)astra::JointType::LeftKnee].depth_position().x,
			joints[(int)astra::JointType::LeftKnee].depth_position().y
		);

		drawLine(
			joints[(int)astra::JointType::LeftFoot].depth_position().x,
			joints[(int)astra::JointType::LeftFoot].depth_position().y,
			joints[(int)astra::JointType::LeftKnee].depth_position().x,
			joints[(int)astra::JointType::LeftKnee].depth_position().y
		);

		drawLine(
			joints[(int)astra::JointType::RightFoot].depth_position().x,
			joints[(int)astra::JointType::RightFoot].depth_position().y,
			joints[(int)astra::JointType::RightKnee].depth_position().x,
			joints[(int)astra::JointType::RightKnee].depth_position().y
		);
	}

	///////////////////////HAND POINTS??
	//Currently commented out in on_frame_ready listener because it does not work as expected
	//This will draw nothing at this point

	auto handPoints = stream.getHandPoints();
	for (auto handPoint : handPoints)
	{

		auto x = handPoint.depthPosition.x;
		auto y = handPoint.depthPosition.y;

		glBegin(GL_POINTS);
		glVertex2i(x, h - y);
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
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(350, 10);
	glutCreateWindow("Skeleton Tracking - Orbbec Astra");
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

	auto handStream = reader.stream<astra::HandStream>();
	//handStream.start();

	reader.add_listener(stream);

	std::thread t1(setDrawing, &iArgc, cppArgv);

	while (stream.streamRunning)
	{
		astra_update();
	}

	astra::terminate();

	t1.join();
	astra::terminate();

	return 0;
}
