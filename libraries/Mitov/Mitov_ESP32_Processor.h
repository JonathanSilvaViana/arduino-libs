////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ESP32_PROCESSOR_h
#define _MITOV_ESP32_PROCESSOR_h

#include <Mitov.h>
#include <Esp.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	class ESP32ProcessorReset : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	ClockInputPin;

	protected:
		void DoClockReceive( void * )
		{
			ESP.restart();
		}

	public:
		ESP32ProcessorReset()
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ESP32ProcessorReset::DoClockReceive );
		}

	};
//---------------------------------------------------------------------------
	class ESP32ProcessorDeepSleep : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	ClockInputPin;

	public:
		uint32_t	Period = 1000000;

	protected:
		void DoClockReceive( void * )
		{
			ESP.deepSleep( Period );
		}

	public:
		ESP32ProcessorDeepSleep()
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ESP32ProcessorDeepSleep::DoClockReceive );
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
