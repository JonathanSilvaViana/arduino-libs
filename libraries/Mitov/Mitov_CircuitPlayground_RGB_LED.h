////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_CIRCUIT_PLAYGROUND_RGB_LEDS_h
#define _MITOV_CIRCUIT_PLAYGROUND_RGB_LEDS_h

#include <Mitov.h>
#include <MitovEmbedded_Adafruit_NeoPixel/MitovEmbedded_Adafruit_NeoPixel.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	class ArduinoCircuitPlaygroundRGBLedsModule : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
#ifdef _MITOV_CIRCUIT_PLAYGROUND_RGB_LEDS_CLOCK_PIN_
		OpenWire::ConnectSinkPin		ClockInputPin;
#endif

	public:
		float   Brightness = 1.0f;

	public:
		MitovEmbedded_Adafruit_NeoPixel	FPixel;
		bool	FModified = true;

	public:
		void SetBrightness( float AValue )
		{
			if( Brightness == AValue )
				return;

			IntSetBrightness( AValue );
		}

	public:
		void SetPixelColor( int AIndex, TColor AColor )
		{
//			Serial.println( AColor.ToString() );
			FPixel.setPixelColor( AIndex, AColor.Red, AColor.Green, AColor.Blue );
//			FPixel.setPixelColor( AIndex, 255, 0, 0 );
			FModified = true;
		}

	protected:
		void IntSetBrightness( float AValue )
		{
			Brightness = AValue;
			FPixel.setBrightness( AValue * 255 );
			FModified = true;
		}

	protected:
#ifdef _MITOV_CIRCUIT_PLAYGROUND_RGB_LEDS_CLOCK_PIN_
		void DoClockReceive( void *_Data )
		{
			if( FModified )
			{
				FPixel.show();
				FModified = false;
			}
		}
#endif
	public:
		void SystemInit()
		{
			FPixel.setPin( 17 );

			FPixel.begin();
			FPixel.updateType(NEO_GRB + NEO_KHZ800);
			FPixel.updateLength(10);
			IntSetBrightness( Brightness );
			FPixel.show();

//			inherited::SystemInit();
		}

		void SystemLoopEnd()
		{
			if( FModified )
#ifdef _MITOV_CIRCUIT_PLAYGROUND_RGB_LEDS_CLOCK_PIN_
				if( ! ClockInputPin.IsConnected())
#endif
				{
					FPixel.show();
					FModified = false;
				}

//			inherited::SystemLoopEnd();
		}

	public:
		ArduinoCircuitPlaygroundRGBLedsModule()
//			FPixel( NEO_RGB + NEO_KHZ800 )
		{
#ifdef _MITOV_CIRCUIT_PLAYGROUND_RGB_LEDS_CLOCK_PIN_
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ArduinoCircuitPlaygroundRGBLedsModule::DoClockReceive );
#endif
		}
	};
//---------------------------------------------------------------------------
	class ArduinoCircuitPlaygroundRGBLed : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	ColorInputPin;

	public:
		TColor	InitialValue;

	protected:
		ArduinoCircuitPlaygroundRGBLedsModule &FOwner;
		uint8_t	FIndex;

	protected:
		void DoReceive( void *_Data )
		{
			TColor &AColor = *(TColor *)_Data;
			if( AColor == InitialValue )
				return;

			InitialValue = AColor;
			FOwner.SetPixelColor( FIndex, InitialValue );

//			digitalWrite( 5, ( *(bool*)_Data ) ? HIGH : LOW );
		}

		virtual void SystemStart() override
		{
			FOwner.SetPixelColor( FIndex, InitialValue );
			
//			inherited::SystemStart();
		}

	public:
		ArduinoCircuitPlaygroundRGBLed( ArduinoCircuitPlaygroundRGBLedsModule &AOwner, uint8_t AIndex ) :
			FOwner( AOwner ),
			FIndex( AIndex )
		{
//			pinMode( 5, OUTPUT );
			ColorInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ArduinoCircuitPlaygroundRGBLed::DoReceive );
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif // _MITOV_CIRCUIT_PLAYGROUND_RGB_LEDS_h
