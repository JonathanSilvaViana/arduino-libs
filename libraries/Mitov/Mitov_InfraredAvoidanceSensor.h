////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_INFRARED_AVOIDANCE_SENSOR_h
#define _MITOV_INFRARED_AVOIDANCE_SENSOR_h

#include <Mitov.h>

namespace Mitov
{
	class InfraredAvoidanceSensor : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;
		OpenWire::SinkPin	EnableInputPin;
		OpenWire::SourcePin	EnableOutputPin;

	public:
		unsigned long	EnableDelay = 500;
        bool Enabled : 1;

	protected:
		bool            FDisabled : 1;
		bool            FCurrentValue : 1;
		unsigned long	FStartTime = 0;

	protected:
		void DoReceive( void *_Data )
		{
			FCurrentValue = *(bool *)_Data;
			if( ! FDisabled )
				OutputPin.SendValue( FCurrentValue );
		}

		void DoEnableReceive( void *_Data )
		{
			bool AValue = *(bool *)_Data;
			if( AValue == Enabled )
				return;

			Enabled = AValue;
			TryStartTimer();
		}


		void TryStartTimer()
		{
			if( Enabled )
			{
				FDisabled = true;
				FStartTime = millis();
			}

			EnableOutputPin.Notify( &Enabled );
		}

	protected:
		virtual void SystemInit()
		{
//			inherited::SystemInit();
			TryStartTimer();
		}

		virtual void SystemLoopBegin( unsigned long currentMicros )
		{
			if( FDisabled )
			{
				unsigned long	currentMilis = millis();
				if( currentMilis - FStartTime >= EnableDelay )
				{
					FDisabled = false;
					OutputPin.SendValue( FCurrentValue );
				}
			}

//			inherited::SystemLoopBegin( currentMicros );
		}

	public:
		InfraredAvoidanceSensor() :
			Enabled( true ),
			FCurrentValue( false ),
			FDisabled( false )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&InfraredAvoidanceSensor::DoReceive );
			EnableInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&InfraredAvoidanceSensor::DoEnableReceive );
		}

	};
}

#endif
