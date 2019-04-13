////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_PULSE_METER_h
#define _MITOV_PULSE_METER_h

#include <Mitov.h>

namespace Mitov
{
	class PulseMeter : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled : 1;

	protected:
		bool			FOldValue : 1;
		unsigned long	FStartTime = 0;

	protected:
		void DoReceive( void *_Data )
		{
            if( ! Enabled )
				return;

			bool AValue = *(bool *)_Data;
//			Serial.println( AValue );
			if( FOldValue == AValue )
				return;

			unsigned long ANow = micros();
			FOldValue = AValue;
			if( AValue )
			{
				FStartTime = ANow;
				return;
			}

			uint32_t APeriod = ANow - FStartTime;

			OutputPin.Notify( &APeriod );
		}

	protected:
		virtual void SystemStart() override
		{
//			inherited::SystemStart();
			OutputPin.SendValue<uint32_t>( 0 );
		}

	public:
		PulseMeter() :
			Enabled( true ),
			FOldValue( false )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&PulseMeter::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	class PulseMeter_Interrupt : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

    public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled : 1;

	protected:
		bool			FOldValue : 1;
		bool			FChanged : 1;
		unsigned long	FStartTime = 0;
		uint32_t FPeriod	= 0;

	public:
		void InterruptHandler( bool AValue )
		{
			if( FOldValue == AValue )
				return;

			unsigned long ANow = micros();
			FOldValue = AValue;
			if( AValue )
			{
				FStartTime = ANow;
				return;
			}

			FPeriod = ANow - FStartTime;
			FChanged = true;
//			Serial.println( "Test" );

		}

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
//			inherited::SystemLoopBegin( currentMicros );

			if( ! Enabled )
				return;

			if( ! FChanged )
				return;

			uint32_t APeriod = FPeriod;
			FChanged = false;
			OutputPin.Notify( &APeriod );
		}

	public:
		PulseMeter_Interrupt() :
			Enabled( true ),
			FOldValue( false ),
			FChanged( false )
		{
		}
	};
}

#endif
