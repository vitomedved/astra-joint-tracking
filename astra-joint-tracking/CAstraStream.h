/**
* @file CAstraStream.h
* @brief Currently only used as frame listener
*
*
*/


#pragma once

#include <astra\astra.hpp>

#include "CStreamListener.h"

#define LICENSE_KEY "<Insert license key>"

/// Class AstraStream which is used as FrameListener (for now)
// TODO: implement objected oriented design with objective OpenGL
class AstraStream : public astra::FrameListener
{
public:
	/// Control variable which determines whether stream is running
	bool										streamRunning;

	// Constructor
	//											AstraStream();
	// Initializes camera
	// @returns true if init is successfull
	// @returns false if init is not successfull
	//bool										init();
	// Properly terminates the camera
	// @returns nothing
	//void										terminate();
	/// FrameListener which acts as interrup when frame is recieved
	/// @returns nothing
	virtual	void								on_frame_ready(
														astra::StreamReader& reader,
														astra::Frame& frame) override;
	// Destructor
	//											~AstraStream();
	/// Getter for currently tracked bodies
	/// @returns currently tracked bodies or NULL
	astra::BodyList								getBodies();
	// Getter for currently tracked hands
	// @returns currently tracked hand points or NULL
	//astra::HandFrame::HandPointList				getHandPoints();

private:
	// Basic astra stream variable needed to start a stream
	//astra::StreamSet							m_sensor;
	// Basic astra stream variable which can get frames of started stream
	//astra::StreamReader							m_reader;
	// Stream listener which extends astra::FrameListener
	//StreamListener							m_listener;
	/// Currently tracked bodies
	astra::BodyList								m_bodies;
	// Currently tracked hand points
	//astra::HandFrame::HandPointList				m_handPoints;

protected:

	//astra::DepthStream						configure_depth(astra::StreamReader& reader);
};
