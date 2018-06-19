#pragma once

//#include <astra\astra.hpp>
#include "CAstraStream.h"

class StreamListener : public astra::FrameListener
{
public:
	virtual void on_frame_ready(astra::StreamReader& reader,
								astra::Frame& frame) override;

private:
	
};
