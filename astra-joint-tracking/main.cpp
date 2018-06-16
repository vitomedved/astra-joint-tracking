
#include "stdafx.h"

#include <astra\astra.hpp>
#include "CStreamListener.h"

int main()
{

	astra::initialize();

	const char* licenseString = "<INSERT LICENSE KEY HERE>";
	orbbec_body_tracking_set_license(licenseString);

	astra::StreamSet sensor;
	astra::StreamReader reader = sensor.create_reader();

	StreamListener listener;

	reader.stream<astra::DepthStream>().start();

	reader.stream<astra::BodyStream>().start();

	reader.add_listener(listener);

	while (true)
	{
		astra_update();
	}

	astra::terminate();

	return 0;
}
