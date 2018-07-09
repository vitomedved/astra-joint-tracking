
#include "stdafx.h"

#include "CAstraStream.h"


AstraStream::AstraStream() :
	streamRunning(true)
{
	//TODO
}

//bool AstraStream::init()
//{
//	bool depthInit = false;
//	bool bodyInit = false;
//
//	astra::initialize();
//
//	const char* licenseString = LICENSE_KEY;
//	orbbec_body_tracking_set_license(licenseString);
//
//	m_reader = m_sensor.create_reader();
//
//	auto depthStream = configure_depth(m_reader);
//	depthStream.start();
//
//	if (depthStream.is_available())
//	{
//		depthInit = true;
//	}
//
//	auto bodyStream = m_reader.stream<astra::BodyStream>();
//	bodyStream.start();
//
//	if (bodyStream.is_available())
//	{
//		bodyInit = true;
//	}
//
//	//m_reader.add_listener(/*m_listener*/);
//
//	streamRunning = true;
//
//	return depthInit && bodyInit ? true : false;
//}

//astra::DepthStream AstraStream::configure_depth(astra::StreamReader& reader)
//{
//	auto depthStream = reader.stream<astra::DepthStream>();
//
//	//We don't have to set the mode to start the stream, but if you want to here is how:
//	astra::ImageStreamMode depthMode;
//
//	depthMode.set_width(640);
//	depthMode.set_height(480);
//	depthMode.set_pixel_format(astra_pixel_formats::ASTRA_PIXEL_FORMAT_DEPTH_MM);
//	depthMode.set_fps(30);
//
//	depthStream.set_mode(depthMode);
//
//	return depthStream;
//}

//void AstraStream::terminate()
//{
//	astra::terminate();
//	streamRunning = false;
//}

void AstraStream::on_frame_ready(astra::StreamReader& reader,
	astra::Frame& frame)
{

	auto bodyFrame = frame.get<astra::BodyFrame>();

	if (bodyFrame.is_valid())
	{
		m_bodies = bodyFrame.bodies();
	}

	/*auto handFrame = frame.get<astra::HandFrame>();
	if (handFrame.is_valid())
	{
		m_handPoints = handFrame.handpoints();
	}*/
}

AstraStream::~AstraStream()
{
	//terminate();
}

astra::BodyList AstraStream::getBodies()
{
	return m_bodies;
}

/*astra::HandFrame::HandPointList AstraStream::getHandPoints()
{
	return m_handPoints;
}*/
