////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_COMPLEMENTARYFILTER_h
#define _MITOV_COMPLEMENTARYFILTER_h

#include <Mitov.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	class BasicComplementary : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::ValueSinkPin<float>			InputPin;
		OpenWire::ValueSimpleSinkPin<float>		RateInputPin;
		OpenWire::TypedStartSourcePin<float>	OutputPin;

	public:
		bool	Enabled : 1;

	protected:
		bool			FHasTime : 1;
		unsigned long	FLastTime = 0;

	protected:
		void DoReceive( void *_Data )
		{
			if( ! Enabled )
				OutputPin.SetValue( *(float *)_Data );

		}

	public:
		BasicComplementary() :
			FHasTime( false ),
			Enabled( true )
		{
			InputPin.SetCallback( MAKE_CALLBACK( BasicComplementary::DoReceive ) );
		}

	};
//---------------------------------------------------------------------------
	class ComplementaryAnalog : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::ValueSinkPin<float>			InputPin;
		OpenWire::ValueSimpleSinkPin<float>		RateInputPin;
		OpenWire::TypedStartSourcePin<float>	OutputPin;

	public:
		float	TimeConstant = 0.075f;
		bool	Enabled : 1;

	protected:
		bool			FHasTime : 1;
		unsigned long	FLastTime = 0;

	protected:
		void DoReceive( void *_Data )
		{
			if( ! Enabled )
				OutputPin.SetValue( *(float *)_Data );

		}

	protected:
		virtual void SystemLoopEnd() 
		{
//			inherited::SystemLoopEnd();

			unsigned long ATime = micros();
			if( ! FHasTime )			
			{
				FHasTime = true;
				FLastTime = ATime;
				OutputPin.SetValue( InputPin.Value );
				return;
			}

			if( ! Enabled )
				return;

			float dT = ( ATime - FLastTime ) / 1000000.0f;
			FLastTime = ATime;

			float a = TimeConstant / ( TimeConstant + dT );

			float AAngle = a * ( OutputPin.Value + RateInputPin.Value * dT) + (1-a) * InputPin.Value;

			OutputPin.SetValue( AAngle );
		}

	public:
		ComplementaryAnalog() :
			FHasTime( false ),
			Enabled( true )
		{
			InputPin.SetCallback( MAKE_CALLBACK( ComplementaryAnalog::DoReceive ) );
		}

	};
//---------------------------------------------------------------------------
	class ComplementaryAngle : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::ValueSinkPin<float>			InputPin;
		OpenWire::ValueSimpleSinkPin<float>		RateInputPin;
		OpenWire::TypedStartSourcePin<float>	OutputPin;

	public:
		float	TimeConstant = 0.075f;
		Mitov::TAngleUnits	Units : 2;
		bool	Enabled : 1;

	protected:
		bool			FHasTime : 1;
		unsigned long	FLastTime = 0;

	protected:
		void DoReceive( void *_Data )
		{
			if( ! Enabled )
				OutputPin.SetValue( *(float *)_Data );

		}

	protected:
		virtual void SystemLoopEnd() 
		{
//			inherited::SystemLoopEnd();

			unsigned long ATime = micros();
			if( ! FHasTime )			
			{
				FHasTime = true;
				FLastTime = ATime;
				OutputPin.SetValue( InputPin.Value );
				return;
			}

			if( ! Enabled )
				return;

			float dT = ( ATime - FLastTime ) / 1000000.0f;
			FLastTime = ATime;

			float a = TimeConstant / ( TimeConstant + dT );

			float AAngle = a * ( OutputPin.Value + RateInputPin.Value * dT ) + (1-a) * InputPin.Value;

			switch( Units )
			{
				case auDegree:
					AAngle = posmod( AAngle, 360.0f );
					break;

				case auRadians:
					AAngle = posmod( AAngle, 2 * PI );
					break;

				case auNormalized:
					AAngle = posmod( AAngle, 1.0f );
					break;

			}

			OutputPin.SetValue( AAngle );
		}

	public:
		ComplementaryAngle() :
			Units( auDegree ),
			FHasTime( false ),
			Enabled( true )
		{
			InputPin.SetCallback( MAKE_CALLBACK( ComplementaryAngle::DoReceive ) );
		}
	};
//---------------------------------------------------------------------------
	class Complementary2 : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::ValueSinkPin<float>			InputPin;
		OpenWire::ValueSimpleSinkPin<float>		RateInputPin;
		OpenWire::TypedStartSourcePin<float>	OutputPin;

	public:
		float	Coefficient = 10.0f;
		bool	Enabled : 1;

	protected:
		bool			FHasTime : 1;
		unsigned long	FLastTime = 0;

	protected:
		void DoReceive( void *_Data )
		{
			if( ! Enabled )
				OutputPin.SetValue( *(float *)_Data );

		}

	protected:
		virtual void SystemLoopEnd() 
		{
//			inherited::SystemLoopEnd();

			unsigned long ATime = micros();
			if( ! FHasTime )			
			{
				FHasTime = true;
				FLastTime = ATime;
				OutputPin.SetValue( InputPin.Value );
				return;
			}

			if( ! Enabled )
				return;

			float dT = ( ATime - FLastTime ) / 1000000.0f;
			FLastTime = ATime;

			float x_angle2C = OutputPin.Value;
			float newAngle = InputPin.Value;

			float x1 = (newAngle -   x_angle2C) * Coefficient * Coefficient;
			float y1 = dT * x1 + y1;
			float x2 = y1 + (newAngle -   x_angle2C) * 2 * Coefficient + RateInputPin.Value;
			x_angle2C = dT * x2 + x_angle2C;

			OutputPin.SetValue( x_angle2C );
		}

	public:
		Complementary2() :
			FHasTime( false ),
			Enabled( true )
		{
			InputPin.SetCallback( MAKE_CALLBACK( Complementary2::DoReceive ) );
		}

	};
//---------------------------------------------------------------------------
	class ComplementaryAngle2 : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::ValueSinkPin<float>			InputPin;
		OpenWire::ValueSimpleSinkPin<float>		RateInputPin;
		OpenWire::TypedStartSourcePin<float>	OutputPin;

	public:
		float	Coefficient = 10.0f;
		Mitov::TAngleUnits	Units : 2;
		bool	Enabled : 1;

	protected:
		bool			FHasTime : 1;
		unsigned long	FLastTime = 0;

	protected:
		void DoReceive( void *_Data )
		{
			if( ! Enabled )
				OutputPin.SetValue( *(float *)_Data );

		}

	protected:
		virtual void SystemLoopEnd() 
		{
//			inherited::SystemLoopEnd();

			unsigned long ATime = micros();
			if( ! FHasTime )			
			{
				FHasTime = true;
				FLastTime = ATime;
				OutputPin.SetValue( InputPin.Value );
				return;
			}

			if( ! Enabled )
				return;

			float dT = ( ATime - FLastTime ) / 1000000.0f;
			FLastTime = ATime;

			float x_angle2C = OutputPin.Value;
			float newAngle = InputPin.Value;

			float x1 = (newAngle -   x_angle2C) * Coefficient * Coefficient;
			float y1 = dT * x1 + y1;
			float x2 = y1 + (newAngle -   x_angle2C) * 2 * Coefficient + RateInputPin.Value;
			x_angle2C = dT * x2 + x_angle2C;

			switch( Units )
			{
				case auDegree:
					x_angle2C = posmod( x_angle2C, 360.0f );
					break;

				case auRadians:
					x_angle2C = posmod( x_angle2C, 2 * PI );
					break;

				case auNormalized:
					x_angle2C = posmod( x_angle2C, 1.0f );
					break;

			}

			OutputPin.SetValue( x_angle2C );
		}

	public:
		ComplementaryAngle2() :
			Units( auDegree ),
			FHasTime( false ),
			Enabled( true )
		{
			InputPin.SetCallback( MAKE_CALLBACK( ComplementaryAngle2::DoReceive ) );
		}

	};
//---------------------------------------------------------------------------
}

#endif
