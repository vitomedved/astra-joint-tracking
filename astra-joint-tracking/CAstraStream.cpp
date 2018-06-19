
#include "stdafx.h"

#include "CAstraStream.h"


AstraStream::AstraStream() :
	streamRunning(false)
{
	std::cout << "Use init method to start stream" << std::endl;
}

bool AstraStream::init()
{
	bool depthInit = false;
	bool bodyInit = false;

	astra::initialize();

	const char* licenseString = LICENSE_KEY;
	orbbec_body_tracking_set_license(licenseString);

	m_reader = m_sensor.create_reader();

	auto depthStream = configure_depth(m_reader);
	depthStream.start();

	if (depthStream.is_available())
	{
		depthInit = true;
	}

	auto bodyStream = m_reader.stream<astra::BodyStream>();
	bodyStream.start();

	if (bodyStream.is_available())
	{
		bodyInit = true;
	}

	//m_reader.add_listener(/*m_listener*/);

	streamRunning = true;

	return depthInit && bodyInit ? true : false;
}

astra::DepthStream AstraStream::configure_depth(astra::StreamReader& reader)
{
	auto depthStream = reader.stream<astra::DepthStream>();

	//We don't have to set the mode to start the stream, but if you want to here is how:
	astra::ImageStreamMode depthMode;

	depthMode.set_width(640);
	depthMode.set_height(480);
	depthMode.set_pixel_format(astra_pixel_formats::ASTRA_PIXEL_FORMAT_DEPTH_MM);
	depthMode.set_fps(30);

	depthStream.set_mode(depthMode);

	return depthStream;
}

void AstraStream::terminate()
{
	astra::terminate();
	streamRunning = false;
}

void AstraStream::on_frame_ready(astra::StreamReader& reader,
	astra::Frame& frame)
{
	//std::cout << "Frame ready!" << std::endl;

	auto bodyFrame = frame.get<astra::BodyFrame>();

	if (bodyFrame.is_valid())
	{
		//auto bodies = bodyFrame.bodies();
		m_bodies = bodyFrame.bodies();

		if (!m_bodies.empty())
		{
			std::cout << " ";
		}
		/*for (auto body : bodies)
		{
		auto joint1 = body.joints();
		auto joint = joint1[0];

		auto x = joint.depth_position();
		std::cout << x.x << std::endl;
		}*/
	}
}

AstraStream::~AstraStream()
{
	terminate();
}

astra::BodyList AstraStream::getBodies()
{
	return m_bodies;
}
