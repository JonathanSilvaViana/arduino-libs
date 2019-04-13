////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_MICROCHIP_CHIPKIT_BASIC_SHIELD_DISPLAY_h
#define _MITOV_MICROCHIP_CHIPKIT_BASIC_SHIELD_DISPLAY_h

#include <SPI.h>
#include <Mitov_Display_SSD1306.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	class Microchip_ChipKITBasicShieldDisplay : public DisplaySSD1306<0x20, odtSSD1306>
	{
		typedef DisplaySSD1306<0x20, odtSSD1306> inherited;

	protected:
		static const uint8_t RESET_PIN = 10;
		static const uint8_t DATA_COMMAND_PIN = 39;

	protected:
		virtual void SendCommand( uint8_t ACommand ) override
		{
/*
			ChipSelectOutputPin.SendValue<bool>( true );
			DataCommandOutputPin.SendValue<bool>( false );
			ChipSelectOutputPin.SendValue<bool>( false );


			ChipSelectOutputPin.SendValue<bool>( true );
*/
			digitalWrite( DATA_COMMAND_PIN, LOW ); // Data Command
			SPI.transfer( ACommand );
		}

		virtual void SendBuffer() override
		{
/*
			ChipSelectOutputPin.SendValue<bool>( true );
			DataCommandOutputPin.SendValue<bool>( true );
			ChipSelectOutputPin.SendValue<bool>( false );

			for (uint16_t i = 0; i < ( inherited::FWidth * inherited::FHeight / 8 ); ++i )
				FSPI.transfer( inherited::buffer[ i ] );

			ChipSelectOutputPin.SendValue<bool>( true );
*/
			digitalWrite( DATA_COMMAND_PIN, HIGH ); // Data Command
			for (uint16_t i = 0; i < ( inherited::FWidth * inherited::FHeight / 8 ); ++i )
				SPI.transfer( inherited::buffer[ i ] );
		}

	protected:
		virtual void SystemInit() override
		{
			pinMode( 38, OUTPUT ); // VDD
			digitalWrite( 38, LOW );
			delay( 1 );

			pinMode( RESET_PIN, OUTPUT );
			pinMode( DATA_COMMAND_PIN, OUTPUT ); // Data Command
			digitalWrite( RESET_PIN, HIGH );
			// VDD (3.3V) goes high at start, lets just chill for a ms
			delay(1);
			// bring reset low
			digitalWrite( RESET_PIN, LOW );
			// wait 10ms
			delay(10);
			// bring out of reset
			digitalWrite( RESET_PIN, HIGH );

			inherited::SystemInit();
			pinMode( 40, OUTPUT ); // VBAT
			digitalWrite( 40, LOW );
			delay( 100 );
		}

	public:
		Microchip_ChipKITBasicShieldDisplay( const unsigned char * AFont ) :
			inherited( 128, 32, AFont )
		{
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
