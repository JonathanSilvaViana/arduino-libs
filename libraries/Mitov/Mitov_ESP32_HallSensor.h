////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_BUTTON_h
#define _MITOV_BUTTON_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
#ifdef _MITOV_ESP32_HALL_SENSOR_CLOCK_PIN_
	class ESP32HallSensorModule : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	ClockInputPin;
		OpenWire::SourcePin	OutputPin;

	public:
		bool	Enabled = true;

	protected:
		void DoReceiveClock( void *_Data )
		{
			if( Enabled )
			{
				float AValue = hallRead();
				OutputPin.Notify( &AValue );
			}
		}

	public:
		ESP32HallSensorModule()
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ESP32HallSensorModule::DoReceiveClock );
		}
	};

#else // _MITOV_ESP32_HALL_SENSOR_CLOCK_PIN_
	class ESP32HallSensorModule : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::TypedSourcePin<float>	OutputPin;

	public:
		bool	Enabled = true;

	protected:
		void ReadSensor( bool AOnlyChange )
		{
			if( Enabled )
			{
				float AValue = hallRead();
				OutputPin.SetValue( AValue, AOnlyChange );
			}
		}

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			ReadSensor( true );
//			inherited::SystemLoopBegin( currentMicros );
		}

		virtual void SystemStart() override
		{
			ReadSensor( false );
//			inherited::SystemStart();
		}


	};

#endif // _MITOV_ESP32_HALL_SENSOR_CLOCK_PIN_
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
