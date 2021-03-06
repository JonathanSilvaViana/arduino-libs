////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_EVIVE_h
#define _MITOV_EVIVE_h

#include <Mitov.h>
#include <Mitov_Microchip_MCP4725.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	class ArduinoEVIVEPotentiometers : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::TypedSourcePin<float>	OutputPins[ 2 ];

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
	    	OutputPins[ 0 ].SetValue( analogRead( 9 ) / 1023.0, true );
	    	OutputPins[ 1 ].SetValue( analogRead( 10 ) / 1023.0, true );
//			inherited::SystemLoopBegin( currentMicros );
		}

		virtual void SystemStart() override
		{
	    	OutputPins[ 0 ].SetValue( analogRead( 9 ) / 1023.0, false );
	    	OutputPins[ 1 ].SetValue( analogRead( 10 ) / 1023.0, false );
//			inherited::SystemStart();
		}
	};
//---------------------------------------------------------------------------
	class ArduinoEVIVENavigation : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::TypedSourcePin<bool>	DirectionOutputPins[ 4 ];
		OpenWire::TypedSourcePin<bool>	PressOutputPin;
		OpenWire::TypedSourcePin<float>	AnalogOutputPin;

	protected:
		static const int NAVKEYMOVE_UP_LOW       = 190;
		static const int NAVKEYMOVE_UP_HIGH      = 230;
		static const int NAVKEYMOVE_RIGHT_LOW    = 385;
		static const int NAVKEYMOVE_RIGHT_HIGH   = 425;
		static const int NAVKEYMOVE_DOWN_LOW     = 585;
		static const int NAVKEYMOVE_DOWN_HIGH    = 625;
		static const int NAVKEYMOVE_LEFT_LOW     = 820;
		static const int NAVKEYMOVE_LEFT_HIGH    = 860;

	protected:
		void UpdatePins( bool AChangeOnly )
		{
			int AAnalogValue = analogRead( 11 );
	    	AnalogOutputPin.SetValue( AAnalogValue / 1023.0, AChangeOnly );
//	    	AnalogOutputPin.SetValue( AAnalogValue, AChangeOnly );
			PressOutputPin.SetValue( digitalRead( 19 ), AChangeOnly);

			bool AUp = (( AAnalogValue > NAVKEYMOVE_UP_LOW ) && ( AAnalogValue < NAVKEYMOVE_UP_HIGH ));
			DirectionOutputPins[ 0 ].SetValue( AUp, AChangeOnly );

			bool ADown = (( AAnalogValue > NAVKEYMOVE_DOWN_LOW ) && ( AAnalogValue < NAVKEYMOVE_DOWN_HIGH ));
			DirectionOutputPins[ 1 ].SetValue( ADown, AChangeOnly );

			bool ALeft = (( AAnalogValue > NAVKEYMOVE_LEFT_LOW ) && ( AAnalogValue < NAVKEYMOVE_LEFT_HIGH ));
			DirectionOutputPins[ 2 ].SetValue( ALeft, AChangeOnly );

			bool ARight = (( AAnalogValue > NAVKEYMOVE_RIGHT_LOW ) && ( AAnalogValue < NAVKEYMOVE_RIGHT_HIGH ));
			DirectionOutputPins[ 3 ].SetValue( ARight, AChangeOnly );
		}

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			UpdatePins( true );

//			inherited::SystemLoopBegin( currentMicros );
		}

		virtual void SystemStart() override
		{
			UpdatePins( false );

//			inherited::SystemStart();
		}
	};
//---------------------------------------------------------------------------
	class ArduinoEVIVEButtons : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::TypedSourcePin<bool>	OutputPins[ 2 ];

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
	    	OutputPins[ 0 ].SetValue( digitalRead( 38 ) != LOW, true );
	    	OutputPins[ 1 ].SetValue( digitalRead( 39 ) != LOW, true );
//			inherited::SystemLoopBegin( currentMicros );
		}

		virtual void SystemStart() override
		{
	    	OutputPins[ 0 ].SetValue( digitalRead( 38 ) != LOW, false );
	    	OutputPins[ 1 ].SetValue( digitalRead( 39 ) != LOW, false );
//			inherited::SystemStart();
		}
	};
//---------------------------------------------------------------------------
	class ArduinoEVIVESlideSwitch
	{
	public:
		OpenWire::TypedSourcePin<bool>	OutputPins[ 2 ];
		OpenWire::TypedSourcePin<uint32_t>	ValueOutputPin;

	public:
		void Process( int APin1, int APin2, bool AChangeOnly )
		{
	    	OutputPins[ 0 ].SetValue( digitalRead( APin1 ) != LOW, AChangeOnly );
	    	OutputPins[ 1 ].SetValue( digitalRead( APin2 ) != LOW, AChangeOnly );

			if( OutputPins[ 0 ].Value )
				ValueOutputPin.SetValue( 0, AChangeOnly );

			else if( OutputPins[ 1 ].Value )
				ValueOutputPin.SetValue( 2, AChangeOnly );

			else
				ValueOutputPin.SetValue( 1, AChangeOnly );

		}

	};
//---------------------------------------------------------------------------
	class ArduinoEVIVESlideSwitches : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		ArduinoEVIVESlideSwitch Switch1;
		ArduinoEVIVESlideSwitch Switch2;

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			Switch1.Process( 40, 41, true );
			Switch2.Process( 42, 43, true );
//	    	OutputPins[ 0 ].SetValue( digitalRead( 38 ) != LOW, true );
//	    	OutputPins[ 1 ].SetValue( digitalRead( 39 ) != LOW, true );
//			inherited::SystemLoopBegin( currentMicros );
		}

		virtual void SystemStart() override
		{
			Switch1.Process( 40, 41, false );
			Switch2.Process( 42, 43, false );
//	    	OutputPins[ 0 ].SetValue( digitalRead( 38 ) != LOW, false );
//	    	OutputPins[ 1 ].SetValue( digitalRead( 39 ) != LOW, false );
//			inherited::SystemStart();
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
