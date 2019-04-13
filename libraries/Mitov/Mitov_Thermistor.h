////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_THERMISTOR_h
#define _MITOV_THERMISTOR_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

// Uses http://www.thinksrs.com/downloads/programs/Therm%20Calc/NTCCalibrator/NTCcalculator.htm

namespace Mitov
{
//---------------------------------------------------------------------------
	class ThermistorSteinhartHart : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

	public:
		float	FixedResistor = 10000.0;
		float	A = 1.131836368e-3;
		float	B = 2.337064764e-4;
		float	C = 0.8916215763e-7;

		bool	Enabled : 1;
		bool	InFahrenheit : 1;
		bool	ThermistorToGround : 1;

	protected:
		void DoReceive( void *_Data )
		{
			if( ! Enabled )
			{
				OutputPin.Notify( _Data );
				return;
			}

			float AValue = constrain( *((float *)_Data), 0.0f, 1.0f );
			if( ThermistorToGround )
				AValue = 1.0 - AValue;

			if( AValue == 0 )
				AValue = 0.0001;

			double resistance = (( FixedResistor / AValue ) - FixedResistor );

			double log_r  = log( resistance );
			double log_r3 = log_r * log_r * log_r;

			float ASteinhartHart = 1.0 / ( A + B * log_r + C * log_r3 );
			ASteinhartHart -= 273.15;
			if( InFahrenheit )
				ASteinhartHart = ASteinhartHart * 9/5 + 32;

			OutputPin.Notify( &ASteinhartHart );

//			float AResult = ASteinhartHart;

//			return steinhartHart(resistance) - voltage * voltage / (K * _resistance);
		}

	public:
		ThermistorSteinhartHart() :
			Enabled( true ),
			InFahrenheit( false ),
			ThermistorToGround( false )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ThermistorSteinhartHart::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	class TArduinoThermistorPoint
	{
	public:
		float	Resistance;
		float	Temperature;

	};
//---------------------------------------------------------------------------
	class TArduinoThermistorPointsSettings
	{
	public:
		TArduinoThermistorPoint	Points[ 3 ];
		bool	InFahrenheit = false;

	};
//---------------------------------------------------------------------------
	class ThermistorSteinhartHart3Points : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

	public:
		float	FixedResistor = 10000.0;
		float	A;
		float	B;
		float	C;

		bool	Enabled : 1;
		bool	InFahrenheit : 1;
		bool	ThermistorToGround : 1;

		TArduinoThermistorPointsSettings	Table;

	public:
		void CalculateCoefficients()
		{
			float L1 = log( Table.Points[ 0 ].Resistance );
			float L2 = log( Table.Points[ 1 ].Resistance );
			float L3 = log( Table.Points[ 2 ].Resistance );

			float Y1 = 1 / GetKTemt( Table.Points[ 0 ].Temperature );
			float Y2 = 1 / GetKTemt( Table.Points[ 1 ].Temperature );
			float Y3 = 1 / GetKTemt( Table.Points[ 2 ].Temperature );

			float YY2 = ( Y2 - Y1 ) / ( L2 - L1 );
			float YY3 = ( Y3 - Y1 ) / ( L3 - L1 );
			C = (( YY3 - YY2 ) / ( L3 - L2 )) * pow( L1 + L2 + L3, -1 );
			B = YY2 - C * ( L1 * L1 + L1 * L2 + L2 * L2 );
			A = Y1 - ( B + L1 * L1 * C ) * L1;
		}

	protected:
		inline float GetKTemt( float AValue )
		{
			if( Table.InFahrenheit )
				AValue = ( AValue - 32 ) / ( 9/5 ) ;

			return AValue + 273.15;
		}

	protected:
		void DoReceive( void *_Data )
		{
			if( ! Enabled )
			{
				OutputPin.Notify( _Data );
				return;
			}

			float AValue = constrain( *((float *)_Data), 0.0f, 1.0f );
			if( ThermistorToGround )
				AValue = 1.0 - AValue;

			if( AValue == 0 )
				AValue = 0.0001;

			double resistance = (( FixedResistor / AValue ) - FixedResistor );

			double log_r  = log( resistance );
			double log_r3 = log_r * log_r * log_r;

			float ASteinhartHart = 1.0 / ( A + B * log_r + C * log_r3 );

			ASteinhartHart -= 273.15;
			if( InFahrenheit )
				ASteinhartHart = ASteinhartHart * 9/5 + 32;

			OutputPin.Notify( &ASteinhartHart );
		}

	protected:
		virtual void SystemStart() override
		{
//			inherited::SystemStart();
			CalculateCoefficients();
		}

	public:
		ThermistorSteinhartHart3Points() :
			Enabled( true ),
			InFahrenheit( false ),
			ThermistorToGround( false )
		{
			Table.Points[ 0 ].Resistance = 25395.0;
			Table.Points[ 0 ].Temperature = 5.0;

			Table.Points[ 1 ].Resistance = 10000.0;
			Table.Points[ 1 ].Temperature = 25.0;

			Table.Points[ 2 ].Resistance = 4366.0;
			Table.Points[ 2 ].Temperature = 45.0;

			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ThermistorSteinhartHart3Points::DoReceive );
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
