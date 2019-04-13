////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ADAFRUIT_TRINKET_M0_RGB_LEDS_h
#define _MITOV_ADAFRUIT_TRINKET_M0_RGB_LEDS_h

#include <Mitov.h>
#include <Adafruit_DotStar.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	class ArduinoAdafruitTrinketM0_RGBLedModule : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
#ifdef _MITOV_ADAFRUIT_TRINKET_M0_RGB_LED_CLOCK_PIN_
		OpenWire::ConnectSinkPin		ClockInputPin;
#endif

	public:
		OpenWire::SinkPin	ColorInputPin;

	public:
		TColor	InitialValue;
		float   Brightness = 1.0f;

	public:
		Adafruit_DotStar	FPixel;
		bool	FModified = true;

	public:
		void SetBrightness( float AValue )
		{
			if( Brightness == AValue )
				return;

			IntSetBrightness( AValue );
		}

	protected:
		void IntSetBrightness( float AValue )
		{
			Brightness = AValue;
			FPixel.setBrightness( AValue * 255 );
			FModified = true;
		}

	protected:
#ifdef _MITOV_ADAFRUIT_TRINKET_M0_RGB_LED_CLOCK_PIN_
		void DoClockReceive( void *_Data )
		{
			if( FModified )
			{
				FPixel.show();
				FModified = false;
			}
		}
#endif

		void DoReceive( void *_Data )
		{
			TColor &AColor = *(TColor *)_Data;
			if( AColor == InitialValue )
				return;

			InitialValue = AColor;
			FPixel.setPixelColor( 0, InitialValue.Red, InitialValue.Green, InitialValue.Blue );
			FModified = true;
		}

	protected:
		virtual void SystemInit() override
		{
//			FPixel.setPin( 17 );

			FPixel.begin();
//			FPixel.updateType(NEO_GRB + NEO_KHZ800);
//			FPixel.updateLength(10);
			IntSetBrightness( Brightness );
			FPixel.setPixelColor( 0, InitialValue.Red, InitialValue.Green, InitialValue.Blue );
			FPixel.show();

//			inherited::SystemInit();
		}

		virtual void SystemLoopEnd() override
		{
			if( FModified )
#ifdef _MITOV_ADAFRUIT_TRINKET_M0_RGB_LED_CLOCK_PIN_
				if( ! ClockInputPin.IsConnected())
#endif
				{
					FPixel.show();
					FModified = false;
				}

//			inherited::SystemLoopEnd();
		}

	public:
		ArduinoAdafruitTrinketM0_RGBLedModule() :
			FPixel( 1, 7, 8, DOTSTAR_BGR )
		{
#ifdef _MITOV_ADAFRUIT_TRINKET_M0_RGB_LED_CLOCK_PIN_
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ArduinoAdafruitTrinketM0_RGBLedModule::DoClockReceive );
#endif
			ColorInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ArduinoAdafruitTrinketM0_RGBLedModule::DoReceive );
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif // _MITOV_ADAFRUIT_TRINKET_M0_RGB_LEDS_h
