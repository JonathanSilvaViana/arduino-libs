////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_KALMANFILTER_h
#define _MITOV_KALMANFILTER_h

#include <Mitov.h>
#include <Mitov_ComplementaryFilter.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	class KalmanAnalog : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::ValueSinkPin<float>			InputPin;
		OpenWire::ValueSimpleSinkPin<float>		RateInputPin;
		OpenWire::TypedStartSourcePin<float>	OutputPin;

	public:
		float	ValueProcessNoise = 0.01;
		float	RateProcessNoise = 0.0003;
		float	ValueObservedNoise = 0.01;
		bool	Enabled : 1;

	protected:
		bool			FHasTime : 1;
		unsigned long	FLastTime = 0;
		float x_bias = 0;
		float P_00 = 0, P_01 = 0, P_10 = 0, P_11 = 0;	

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

			float dt = ( ATime - FLastTime ) / 1000000.0f;
			FLastTime = ATime;

			float x_angle = OutputPin.Value;

			x_angle += dt * ( RateInputPin.Value - x_bias );

			P_00 +=  - dt * (P_10 + P_01) + ValueProcessNoise * dt;
			P_01 +=  - dt * P_11;
			P_10 +=  - dt * P_11;
			P_11 +=  + RateProcessNoise * dt;
    
			float y = InputPin.Value - x_angle;
			float S = P_00 + ValueObservedNoise;
			float K_0 = P_00 / S;
			float K_1 = P_10 / S;
    
			x_angle +=  K_0 * y;
			x_bias  +=  K_1 * y;
			P_00 -= K_0 * P_00;
			P_01 -= K_0 * P_01;
			P_10 -= K_1 * P_00;
			P_11 -= K_1 * P_01;

			OutputPin.SetValue( x_angle );
		}

	public:
		KalmanAnalog() :
			FHasTime( false ),
			Enabled( true )
		{
			InputPin.SetCallback( MAKE_CALLBACK( KalmanAnalog::DoReceive ) );
		}

	};
//---------------------------------------------------------------------------
	class KalmanAngle : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::ValueSinkPin<float>			InputPin;
		OpenWire::ValueSimpleSinkPin<float>		RateInputPin;
		OpenWire::TypedStartSourcePin<float>	OutputPin;

	public:
		float	ValueProcessNoise = 0.01;
		float	RateProcessNoise = 0.0003;
		float	ValueObservedNoise = 0.01;

		Mitov::TAngleUnits	Units : 2;
		bool	Enabled : 1;

	protected:
		bool			FHasTime : 1;
		unsigned long	FLastTime = 0;
		float x_bias = 0;
		float P_00 = 0, P_01 = 0, P_10 = 0, P_11 = 0;	


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

			float dt = ( ATime - FLastTime ) / 1000000.0f;
			FLastTime = ATime;

			float x_angle = OutputPin.Value;

			x_angle += dt * ( RateInputPin.Value - x_bias );

			P_00 +=  - dt * (P_10 + P_01) + ValueProcessNoise * dt;
			P_01 +=  - dt * P_11;
			P_10 +=  - dt * P_11;
			P_11 +=  + RateProcessNoise * dt;
    
			float y = InputPin.Value - x_angle;
			float S = P_00 + ValueObservedNoise;
			float K_0 = P_00 / S;
			float K_1 = P_10 / S;
    
			x_angle +=  K_0 * y;
			x_bias  +=  K_1 * y;
			P_00 -= K_0 * P_00;
			P_01 -= K_0 * P_01;
			P_10 -= K_1 * P_00;
			P_11 -= K_1 * P_01;

			switch( Units )
			{
				case auDegree:
					x_angle = posmod( x_angle, 360.0f );
					break;

				case auRadians:
					x_angle = posmod( x_angle, 2 * PI );
					break;

				case auNormalized:
					x_angle = posmod( x_angle, 1.0f );
					break;

			}

			OutputPin.SetValue( x_angle );
		}

	public:
		KalmanAngle() :
			Units( auDegree ),
			Enabled( true ),
			FHasTime( false )
		{
			InputPin.SetCallback( MAKE_CALLBACK( KalmanAngle::DoReceive ) );
		}

	};
//---------------------------------------------------------------------------
}

#endif
