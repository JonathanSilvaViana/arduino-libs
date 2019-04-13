////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_MICROCHIP_CHIPKIT_BASIC_SHIELD_h
#define _MITOV_MICROCHIP_CHIPKIT_BASIC_SHIELD_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
/*
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
*/
//---------------------------------------------------------------------------
	template<int C_B0, int C_B1, int C_B2, int C_B3> class Microchip_ChipKITBasicShieldButtons : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::TypedSourcePin<bool>	OutputPins[ 4 ];

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
	    	OutputPins[ 0 ].SetValue( digitalRead( C_B0 ) != LOW, true );
	    	OutputPins[ 1 ].SetValue( digitalRead( C_B1 ) != LOW, true );
	    	OutputPins[ 2 ].SetValue( digitalRead( C_B2 ) != LOW, true );
	    	OutputPins[ 3 ].SetValue( digitalRead( C_B3 ) != LOW, true );
//			inherited::SystemLoopBegin( currentMicros );
		}

		virtual void SystemStart() override
		{
	    	OutputPins[ 0 ].SetValue( digitalRead( C_B0 ) != LOW, false );
	    	OutputPins[ 1 ].SetValue( digitalRead( C_B1 ) != LOW, false );
	    	OutputPins[ 2 ].SetValue( digitalRead( C_B2 ) != LOW, false );
	    	OutputPins[ 3 ].SetValue( digitalRead( C_B3 ) != LOW, false );
//			inherited::SystemStart();
		}
	};
//---------------------------------------------------------------------------
	class Microchip_ChipKITBasicShieldLeds : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::IndexedSinkPin<uint8_t>	InputPins[ 8 ];

	protected:
		void DoReceive( int AIndex, void *_Data )
		{
			digitalWrite( AIndex, *(bool *)_Data );
		}

		virtual void SystemStart() override
		{
			for( int i = 26; i < 34; ++i )
			{
				pinMode( i, OUTPUT );
				digitalWrite( i, LOW );
			}
		}

	public:
		Microchip_ChipKITBasicShieldLeds()
		{
			for( int i = 0; i < 8; ++i )
				InputPins[ i ].SetCallback( ( 7 - i ) + 26, this, (OpenWire::TOnPinIndexedReceive)&Microchip_ChipKITBasicShieldLeds::DoReceive );

//			InputPins[ 0 ].SetCallback( this, (OpenWire::TOnPinReceive)&Microchip_ChipKITBasicShieldLeds::DoReceive1 );
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
