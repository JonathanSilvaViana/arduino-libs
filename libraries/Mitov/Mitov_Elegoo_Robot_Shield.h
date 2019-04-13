////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ELEGOO_MOTOR_SHIELD_h
#define _MITOV_ELEGOO_MOTOR_SHIELD_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
#ifdef __MITOV_ELEGOO_MOTOR_SHIELD_V3__
	typedef	MotorShieldChannel<5,8,7, 6,9,11> ElegooRobotMotorShieldChannel;
	typedef	MotorShieldChannel_Slopped<5,8,7, 6,9,11> ElegooRobotMotorShieldChannel_Slopped;
#else
	#ifdef __MITOV_ELEGOO_MOTOR_SHIELD_V2_REMAPPED__
		typedef	MotorShieldChannel<5,7,11, 6,8,9> ElegooRobotMotorShieldChannel;
		typedef	MotorShieldChannel_Slopped<5,7,11, 6,8,9> ElegooRobotMotorShieldChannel_Slopped;
	#else // __MITOV_ELEGOO_MOTOR_SHIELD_V2_REMAPPED__
		typedef	MotorShieldChannel<5,7,6, 11,8,9> ElegooRobotMotorShieldChannel;
		typedef	MotorShieldChannel_Slopped<5,7,6, 11,8,9> ElegooRobotMotorShieldChannel_Slopped;
	#endif // __MITOV_ELEGOO_MOTOR_SHIELD_V2_REMAPPED__
#endif // __MITOV_ELEGOO_MOTOR_SHIELD_V3__
/*
	class ElegooRobotMotorShieldChannel : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;

	public:
		float	InitialValue = 0.5f;
		bool	Enabled : 1;

	protected:
		bool	FIsSecond : 1;

	public:
		void	SetEnabled( bool AValue ) 
		{
			if( Enabled == AValue )
				return;

			Enabled = AValue;
			UpdateOutputs();
		}

	protected:
		void DoReceive( void *_Data )
		{
			float ASpeed = constrain( *(float *)_Data, 0.0, 1.0 );
			if( InitialValue == ASpeed )
				return;

			InitialValue = ASpeed;
			UpdateOutputs();
		}

		void	UpdateOutputs()
		{
			bool AForward;
			bool AReverse;
			float AOutSpeed;
			if( Enabled && ( InitialValue != 0.5 ))
			{
				AOutSpeed = abs( InitialValue - 0.5 ) * 2;
				bool ADirection = InitialValue > 0.5;

				AReverse = ADirection;
				AForward = ! ADirection;
			}

			else
			{
				AOutSpeed = 0;
				AReverse = false;
				AForward = false;
			}

			if( FIsSecond )
			{
#ifdef __MITOV_ELEGOO_MOTOR_SHIELD_V3__
				analogWrite( 6, ( AOutSpeed * PWMRANGE ) + 0.5 );
				digitalWrite( 9, AForward );
				digitalWrite( 11, AReverse );
#else
				digitalWrite( 8, AForward );
				digitalWrite( 9, AReverse );
				analogWrite( 11, ( AOutSpeed * PWMRANGE ) + 0.5 );
#endif
//				analogWrite( 3, ( AOutSpeed * PWMRANGE ) + 0.5 );
			}
			else
			{
#ifdef __MITOV_ELEGOO_MOTOR_SHIELD_V3__
				digitalWrite( 8, AForward );
				digitalWrite( 7, AReverse );
#else
				digitalWrite( 7, AForward );
				digitalWrite( 6, AReverse );
#endif
				analogWrite( 5, ( AOutSpeed * PWMRANGE ) + 0.5 );
			}

		}

	protected:
		virtual void SystemStart()
		{
//			inherited::SystemStart();
			UpdateOutputs();
		}

	public:
		ElegooRobotMotorShieldChannel( bool AIsSecond ) :
			Enabled( true ),
			FIsSecond( AIsSecond )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ElegooRobotMotorShieldChannel::DoReceive );
			if( AIsSecond )
			{
#ifdef __MITOV_ELEGOO_MOTOR_SHIELD_V3__
				pinMode( 6, OUTPUT );
#else
				pinMode( 8, OUTPUT );
#endif
				pinMode( 9, OUTPUT );
				pinMode( 11, OUTPUT );
			}
			else
			{
#ifdef __MITOV_ELEGOO_MOTOR_SHIELD_V3__
				pinMode( 8, OUTPUT );
#else
				pinMode( 6, OUTPUT );
#endif
				pinMode( 5, OUTPUT );
				pinMode( 7, OUTPUT );
			}
		}
	};
//---------------------------------------------------------------------------
	class ElegooRobotMotorShieldChannel_Slopped : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;

	public:
		float	SlopeRun = 0.0f;
		float	SlopeStop = 0.0f;
		float	InitialValue = 0.5f;
		bool	Enabled : 1;

	protected:
		bool	FIsSecond : 1;
		float	FCurrentValue = 0.0f;
		unsigned long	FLastTime = 0;

	public:
		void	SetEnabled( bool AValue ) 
		{
			if( Enabled == AValue )
				return;

			Enabled = AValue;
			UpdateOutputs();
		}

	protected:
		void DoReceive( void *_Data )
		{
			float ASpeed = constrain( *(float *)_Data, 0.0, 1.0 );
			if( InitialValue == ASpeed )
				return;

			InitialValue = ASpeed;
			UpdateOutputs();
		}

		void	UpdateOutputs()
		{
			bool AForward;
			bool AReverse;
			float AOutSpeed;
			if( FCurrentValue != 0.5 )
			{
				AOutSpeed = abs( FCurrentValue - 0.5 ) * 2;
				bool ADirection = FCurrentValue > 0.5;

				AReverse = ADirection;
				AForward = ! ADirection;
			}

			else
			{
				AOutSpeed = 0;
				AReverse = false;
				AForward = false;
			}

			if( FIsSecond )
			{
#ifdef __MITOV_ELEGOO_MOTOR_SHIELD_V3__
				analogWrite( 6, ( AOutSpeed * PWMRANGE ) + 0.5 );
				digitalWrite( 9, AForward );
				digitalWrite( 11, AReverse );
#else
				digitalWrite( 8, AForward );
				digitalWrite( 9, AReverse );
				analogWrite( 11, ( AOutSpeed * PWMRANGE ) + 0.5 );
#endif
//				analogWrite( 3, ( AOutSpeed * PWMRANGE ) + 0.5 );
			}
			else
			{
#ifdef __MITOV_ELEGOO_MOTOR_SHIELD_V3__
				digitalWrite( 8, AForward );
				digitalWrite( 7, AReverse );
#else
				digitalWrite( 7, AForward );
				digitalWrite( 6, AReverse );
#endif
				analogWrite( 5, ( AOutSpeed * PWMRANGE ) + 0.5 );
			}

		}

	protected:
		virtual void SystemStart()
		{
//			inherited::SystemStart();
			FCurrentValue = InitialValue;

			UpdateOutputs();
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			float ATargetValue = (Enabled) ? InitialValue : 0.5;

			if( FCurrentValue != ATargetValue )
			{
				float Slope = SlopeRun;
				if( FCurrentValue > ATargetValue )
				{
					if( ATargetValue > 0.5 )
						Slope = SlopeStop;
				}

				else
				{
					if( ATargetValue < 0.5 )
						Slope = SlopeStop;
				}

				if( Slope == 0 )
					FCurrentValue = ATargetValue;

				else
				{
					float ARamp = abs( ( currentMicros - FLastTime ) * Slope / 1000000 );
					if( FCurrentValue < ATargetValue )
					{
						FCurrentValue += ARamp;
						if( FCurrentValue > ATargetValue )
							FCurrentValue = ATargetValue;

					}
					else
					{
						FCurrentValue -= ARamp;
						if( FCurrentValue < ATargetValue )
							FCurrentValue = ATargetValue;

					}
				}

				UpdateOutputs();
			}

			FLastTime = currentMicros;

//			inherited::SystemLoopBegin( currentMicros );
		}

	public:
		ElegooRobotMotorShieldChannel_Slopped( bool AIsSecond ) :
			Enabled( true ),
			FIsSecond( AIsSecond )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ElegooRobotMotorShieldChannel_Slopped::DoReceive );
			if( AIsSecond )
			{
#ifdef __MITOV_ELEGOO_MOTOR_SHIELD_V3__
				pinMode( 6, OUTPUT );
#else
				pinMode( 8, OUTPUT );
#endif
				pinMode( 9, OUTPUT );
				pinMode( 11, OUTPUT );
			}
			else
			{
#ifdef __MITOV_ELEGOO_MOTOR_SHIELD_V3__
				pinMode( 8, OUTPUT );
#else
				pinMode( 6, OUTPUT );
#endif
				pinMode( 5, OUTPUT );
				pinMode( 7, OUTPUT );
			}
		}
	};
*/
//---------------------------------------------------------------------------
	class ElegooRobotLineFollowingSensors
	{
	public:
		OpenWire::TypedSourcePin<bool>	OutputPins[ 3 ];

	protected:
		bool	FChangeOnly = false;

	public:
		void SystemLoopBegin( unsigned long currentMicros )
		{
			OutputPins[ 0 ].SetValue( digitalRead( 10 ) == HIGH , FChangeOnly );
			OutputPins[ 1 ].SetValue( digitalRead( 4 ) == HIGH , FChangeOnly );
			OutputPins[ 2 ].SetValue( digitalRead( 2 ) == HIGH , FChangeOnly );

			FChangeOnly = true;
//			inherited::SystemLoopBegin( currentMicros );
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
