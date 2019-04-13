////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_IQUID_CRYSTAL_DISPLAY_I2C_h
#define _MITOV_IQUID_CRYSTAL_DISPLAY_I2C_h

#include <Mitov.h>
#include <Mitov_LiquidCrystalDisplay.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	template<uint8_t C_ADDRESS> class LiquidCrystalDisplayI2C : public LiquidCrystalDisplayBasic
	{
		typedef Mitov::LiquidCrystalDisplayBasic inherited;

	protected:
		TwoWire &FWire;

	public:
		virtual void Send( uint8_t value, bool ACommand, bool A4Bits ) override 
		{
			if( ! A4Bits )
				Write4bits( value >> 4, ACommand );

			Write4bits( value & 0xF, ACommand );
		}

		void Write4bits( uint8_t value, bool ACommand )
		{
			value <<= 4;
			const uint8_t AEnableMask = 0b100;
			if( ! ACommand )
				value |= 1; // Set RS

			WriteRegister( value | AEnableMask );   // En HIGH
			WriteRegister( value & ~AEnableMask );  // En LOW
		}

		void WriteRegister( uint8_t AValue )
		{
			if( BacklightPositive )
			{
				if( Backlight )
					AValue |= 0b1000;

			}

			else
			{
				if( ! Backlight )
					AValue |= 0b1000;

			}

			FWire.beginTransmission( C_ADDRESS );
			FWire.write( AValue );
//			Serial.println( AValue, HEX );
			FWire.endTransmission();
		}

	public:
		void UpdateBacklight()
		{
			WriteRegister( 0 );
/*
			if( Backlight )
				inherited::FLcd->setBacklight( 255 );

			else
				inherited::FLcd->setBacklight( 0 );
*/
		}

	public:
		virtual void SystemInit() override
		{
			WriteRegister( 0 );

			// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
			// according to datasheet, we need at least 40ms after power rises above 2.7V
			// before sending commands. Arduino can turn on way before 4.5V so we'll wait 
			// 50
			// ---------------------------------------------------------------------------
			delay( 100 ); // 100ms delay

//			Serial.println( "STEP1" );

			// 4 bit mode
			// this is according to the hitachi HD44780 datasheet
			// figure 24, pg 46
      
			// we start in 8bit mode, try to set 4 bit mode
			Send( 0x03, true, true );
//			delayMicroseconds(4500); // wait min 4.1ms
      
			// second try
			Send( 0x03, true, true );
//			delayMicroseconds(4500); // wait min 4.1ms
      
			// third go!
			Send( 0x03, true, true );
//			delayMicroseconds(150);
      
			// finally, set to 4-bit interface
			Send( 0x02, true, true ); 

			// finally, set # lines, font size, etc.
			uint8_t ADisplayFunction = 0;
			if( FHeight > 1 )
				ADisplayFunction |= LCD_2LINE;

			if( Hight10Pixels && ( FHeight == 1 ))
				ADisplayFunction |= LCD_5x10DOTS;

//			Serial.print( "ADisplayFunction: " );
//			Serial.println( ADisplayFunction, HEX );

			Command( LCD_FUNCTIONSET | ADisplayFunction );  
   
//			Serial.println( "UpdateDisplayControl()" );
			UpdateDisplayControl();
   
			// clear the LCD
//			Serial.println( "Clear()" );
			Clear();

//			Serial.println( "UpdateEntryModeSet()" );
			UpdateEntryModeSet();
   
			// Initialize to default text direction (for romance languages)
//			_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
			// set the entry mode
//			command(LCD_ENTRYMODESET | _displaymode);

			inherited::SystemInit();
//			UpdateBacklight();
		}

	public:
		LiquidCrystalDisplayI2C( uint8_t AWidth, uint8_t AHeight, TwoWire &AWire ) :
			inherited( AWidth, AHeight ),
			FWire( AWire )
		{
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
