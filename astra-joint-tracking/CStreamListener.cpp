
#include "stdafx.h"

#include "CStreamListener.h"

void StreamListener::on_frame_ready(astra::StreamReader& reader,
									astra::Frame& frame)
{
	std::cout << "Frame ready!" << std::endl;
}
