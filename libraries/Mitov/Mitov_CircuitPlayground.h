////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_CIRCUIT_PLAYGROUND_h
#define _MITOV_CIRCUIT_PLAYGROUND_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	class ArduinoCircuitPlaygroundButtons : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::TypedSourcePin<bool>	OutputPins[ 2 ];

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
	    	OutputPins[ 0 ].SetValue( digitalRead( 4 ) != LOW, true );
	    	OutputPins[ 1 ].SetValue( digitalRead( 19 ) != LOW, true );
//			inherited::SystemLoopBegin( currentMicros );
		}

		virtual void SystemStart() override
		{
	    	OutputPins[ 0 ].SetValue( digitalRead( 4 ) != LOW, false );
	    	OutputPins[ 1 ].SetValue( digitalRead( 19 ) != LOW, false );
//			inherited::SystemStart();
		}
	};
//---------------------------------------------------------------------------
	class ArduinoCircuitPlaygroundBuzzer : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;

	protected:
		void DoReceive( void *_Data )
		{
			digitalWrite( 5, ( *(bool*)_Data ) ? HIGH : LOW );
		}

	public:
		ArduinoCircuitPlaygroundBuzzer()
		{
			pinMode( 5, OUTPUT );
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ArduinoCircuitPlaygroundBuzzer::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	class ArduinoCircuitPlaygroundThermometer : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	protected:
		static const int SERIESRESISTOR = 10000;

		// resistance at 25 degrees C
		static const int THERMISTORNOMINAL = 10000;
		// temp. for nominal resistance (almost always 25 C)
		static const int TEMPERATURENOMINAL = 25;

		// The beta coefficient of the thermistor (usually 3000-4000)
		static const int BCOEFFICIENT = 3380;
		// the value of the 'other' resistor

	public:
#if( defined( _MITOV_CIRCUIT_PLAYGROUND_THERMOMETER_CLOCK_PIN_ ))
		OpenWire::ConnectSinkPin	ClockInputPin;
#endif
		OpenWire::TypedStartSourcePin<float>	OutputPin;

	public:
		bool Enabled : 1;
		bool InFahrenheit : 1;

	public:
		void ReadData( bool ASendAll )
		{
			if( ! Enabled )
				 return;

			float reading = analogRead( A0 );

			//Serial.print("Thermistor reading: "); Serial.println(reading);

			// convert the value to resistance
			reading = ((1023.0 * float( SERIESRESISTOR )) / reading);
			reading -= SERIESRESISTOR;

			//Serial.print("Thermistor resistance: "); Serial.println(reading);

			float steinhart = reading / THERMISTORNOMINAL;     // (R/Ro)
			steinhart = log(steinhart);                  // ln(R/Ro)
			steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
			steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
			steinhart = 1.0 / steinhart;                 // Invert
			steinhart -= 273.15;                         // convert to C

			if( InFahrenheit )
				steinhart = steinhart * ( 9.0/5.0 ) + 32.0;

			OutputPin.SetValue( steinhart, ASendAll );
		}

	protected:
#if( defined( _MITOV_CIRCUIT_PLAYGROUND_THERMOMETER_CLOCK_PIN_ ))
		void DoClockReceive( void *_Data )
		{
			ReadData( true );
		}
#endif

	public:
		void SystemLoopBegin( unsigned long currentMicros )
		{
#if( defined( _MITOV_CIRCUIT_PLAYGROUND_THERMOMETER_CLOCK_PIN_ ))
			if( ! ClockInputPin.IsConnected() )
#endif
				ReadData( false );

//			inherited::SystemLoopBegin( currentMicros );
		}

		void SystemStart()
		{
			ReadData( true );

//			inherited::SystemStart();
		}

	public:
		ArduinoCircuitPlaygroundThermometer() :
			Enabled( true ),
			InFahrenheit( false )
		{
#if( defined( _MITOV_CIRCUIT_PLAYGROUND_THERMOMETER_CLOCK_PIN_ ))
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ArduinoCircuitPlaygroundThermometer::DoClockReceive );
#endif
		}

	};
//---------------------------------------------------------------------------
	class ArduinoCircuitPlaygroundLightSensor : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
#if( defined( _MITOV_CIRCUIT_PLAYGROUND_LIGHT_CLOCK_PIN_ ))
		OpenWire::ConnectSinkPin	ClockInputPin;
#endif
		OpenWire::TypedStartSourcePin<float>	OutputPin;

	public:
		bool Enabled : 1;
		bool InLux : 1;

	public:
		void ReadData( bool ASendAll )
		{
			if( ! Enabled )
				 return;

			float AValue = analogRead( A5 ) / 1023.0;
			if( InLux )
				AValue *= 1500.0;

			OutputPin.SetValue( AValue, ASendAll );
		}

	protected:
#if( defined( _MITOV_CIRCUIT_PLAYGROUND_LIGHT_CLOCK_PIN_ ))
		void DoClockReceive( void *_Data )
		{
			ReadData( true );
		}
#endif

	public:
		void SystemLoopBegin( unsigned long currentMicros )
		{
#if( defined( _MITOV_CIRCUIT_PLAYGROUND_LIGHT_CLOCK_PIN_ ))
			if( ! ClockInputPin.IsConnected() )
#endif
				ReadData( false );

//			inherited::SystemLoopBegin( currentMicros );
		}

		void SystemStart()
		{
			ReadData( true );

//			inherited::SystemStart();
		}

	public:
		ArduinoCircuitPlaygroundLightSensor() :
			Enabled( true ),
			InLux( false )
		{
#if( defined( _MITOV_CIRCUIT_PLAYGROUND_LIGHT_CLOCK_PIN_ ))
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ArduinoCircuitPlaygroundLightSensor::DoClockReceive );
#endif
		}

	};
//---------------------------------------------------------------------------
	class ArduinoCircuitPlaygroundMicrophone : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
#if( defined( _MITOV_CIRCUIT_PLAYGROUND_MICROPHONE_CLOCK_PIN_ ))
		OpenWire::ConnectSinkPin	ClockInputPin;
#endif
		OpenWire::SourcePin	OutputPin;

	public:
		bool Enabled = true;

	public:
		void ReadData()
		{
			if( ! Enabled )
				 return;

			float AValue = analogRead( A4 ) / 1023.0;
			OutputPin.Notify( &AValue );
		}

	protected:
#if( defined( _MITOV_CIRCUIT_PLAYGROUND_MICROPHONE_CLOCK_PIN_ ))
		void DoClockReceive( void *_Data )
		{
			ReadData();
		}
#endif

	public:
		void SystemLoopBegin( unsigned long currentMicros )
		{
#if( defined( _MITOV_CIRCUIT_PLAYGROUND_MICROPHONE_CLOCK_PIN_ ))
			if( ! ClockInputPin.IsConnected() )
#endif
				ReadData();

//			inherited::SystemLoopBegin( currentMicros );
		}

		void SystemStart()
		{
			ReadData();

//			inherited::SystemStart();
		}

	public:
		ArduinoCircuitPlaygroundMicrophone()
		{
#if( defined( _MITOV_CIRCUIT_PLAYGROUND_MICROPHONE_CLOCK_PIN_ ))
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ArduinoCircuitPlaygroundMicrophone::DoClockReceive );
#endif
		}

	};
//---------------------------------------------------------------------------
	class ArduinoCircuitPlaygroundSlideSwitch : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::TypedSourcePin<bool>	OutputPin;

	public:
		void SystemLoopBegin( unsigned long currentMicros )
		{
	    	OutputPin.SetValue( digitalRead( 21 ) != LOW, true );
//			inherited::SystemLoopBegin( currentMicros );
		}

		void SystemStart()
		{
	    	OutputPin.SetValue( digitalRead( 21 ) != LOW, false );
//			inherited::SystemStart();
		}
	};

//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif // _MITOV_CIRCUIT_PLAYGROUND_h
