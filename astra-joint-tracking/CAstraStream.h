#pragma once

#include <astra\astra.hpp>

#include "CStreamListener.h"

#define LICENSE_KEY "<Insert license key>"

class AstraStream : public astra::FrameListener
{
public:
	bool							streamRunning;


	AstraStream();
	bool							init();
	void							terminate();
	virtual	void					on_frame_ready(astra::StreamReader& reader,
												   astra::Frame& frame) override;
	~AstraStream();

	astra::BodyList					getBodies();

private:
	astra::StreamSet				m_sensor;
	astra::StreamReader				m_reader;
	//StreamListener					m_listener;

protected:
	astra::BodyList					m_bodies;

	astra::DepthStream				configure_depth(astra::StreamReader& reader);
};
