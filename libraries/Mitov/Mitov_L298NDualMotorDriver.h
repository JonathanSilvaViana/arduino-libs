////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_L298N_DUAL_MOTOR_h
#define _MITOV_L298N_DUAL_MOTOR_h

#include <Mitov.h>

namespace Mitov
{
	class L298NDualMotorDriverChannel : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	ForwardOutputPin;
		OpenWire::SourcePin	ReverseOutputPin;
		OpenWire::SourcePin	SpeedOutputPin;

	public:
		bool	Enabled = true;
		float	InitialValue = 0.5f;

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
			if( Enabled )
			{
				float AOutSpeed = abs( InitialValue - 0.5 ) * 2;
				bool ADirection = InitialValue > 0.5;

				ReverseOutputPin.Notify( &ADirection );
				ADirection = !ADirection;
				ForwardOutputPin.Notify( &ADirection );

				SpeedOutputPin.Notify( &AOutSpeed );
			}

			else
			{
				SpeedOutputPin.SendValue( 0.5 );
				ForwardOutputPin.SendValue( false );
				ReverseOutputPin.SendValue( false );
			}
		}

	protected:
		virtual void SystemStart()
		{
//			inherited::SystemStart();
			UpdateOutputs();
		}

	public:
		L298NDualMotorDriverChannel()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&L298NDualMotorDriverChannel::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	class L298NDualMotorDriverChannel_Slopped : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	ForwardOutputPin;
		OpenWire::SourcePin	ReverseOutputPin;
		OpenWire::SourcePin	SpeedOutputPin;

	public:
		float	SlopeRun = 0.0f;
		float	SlopeStop = 0.0f;
		float	InitialValue = 0.5f;
		bool	Enabled = true;

	protected:
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
			if( FCurrentValue != 0.5 )
			{
				float AOutSpeed = abs( FCurrentValue - 0.5 ) * 2;
				bool ADirection = FCurrentValue > 0.5;

				ReverseOutputPin.Notify( &ADirection );
				ADirection = !ADirection;
				ForwardOutputPin.Notify( &ADirection );

				SpeedOutputPin.Notify( &AOutSpeed );
			}

			else
			{
				SpeedOutputPin.SendValue( 0.5 );
				ForwardOutputPin.SendValue( false );
				ReverseOutputPin.SendValue( false );
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
		L298NDualMotorDriverChannel_Slopped()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&L298NDualMotorDriverChannel_Slopped::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	template<int PIN_ENABLE_1, int PIN_FORWARD_1, int PIN_REVERSE_1, int PIN_ENABLE_2, int PIN_FORWARD_2, int PIN_REVERSE_2> class MotorShieldChannel : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

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
				digitalWrite( PIN_FORWARD_2, AForward );
				digitalWrite( PIN_REVERSE_2, AReverse );
				analogWrite( PIN_ENABLE_2, ( AOutSpeed * PWMRANGE ) + 0.5 );
//				analogWrite( 3, ( AOutSpeed * PWMRANGE ) + 0.5 );
			}
			else
			{
				digitalWrite( PIN_FORWARD_1, AForward );
				digitalWrite( PIN_REVERSE_1, AReverse );
				analogWrite( PIN_ENABLE_1, ( AOutSpeed * PWMRANGE ) + 0.5 );
			}

		}

	public:
		void SystemStart()
		{
			UpdateOutputs();
		}

	public:
		MotorShieldChannel( bool AIsSecond ) :
			Enabled( true ),
			FIsSecond( AIsSecond )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&MotorShieldChannel::DoReceive );
			if( AIsSecond )
			{
				pinMode( PIN_ENABLE_2, OUTPUT );
				pinMode( PIN_FORWARD_2, OUTPUT );
				pinMode( PIN_REVERSE_2, OUTPUT );
			}
			else
			{
				pinMode( PIN_ENABLE_1, OUTPUT );
				pinMode( PIN_FORWARD_1, OUTPUT );
				pinMode( PIN_REVERSE_1, OUTPUT );
			}
		}
	};
//---------------------------------------------------------------------------
	template<int PIN_ENABLE_1, int PIN_FORWARD_1, int PIN_REVERSE_1, int PIN_ENABLE_2, int PIN_FORWARD_2, int PIN_REVERSE_2> class MotorShieldChannel_Slopped : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

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
				digitalWrite( PIN_FORWARD_2, AForward );
				digitalWrite( PIN_REVERSE_2, AReverse );
				analogWrite( PIN_ENABLE_2, ( AOutSpeed * PWMRANGE ) + 0.5 );
			}
			else
			{
				digitalWrite( PIN_FORWARD_1, AForward );
				digitalWrite( PIN_REVERSE_1, AReverse );
				analogWrite( PIN_ENABLE_1, ( AOutSpeed * PWMRANGE ) + 0.5 );
			}

		}

	public:
		void SystemStart()
		{
//			inherited::SystemStart();
			FCurrentValue = InitialValue;

			UpdateOutputs();
		}

		void SystemLoopBegin( unsigned long currentMicros )
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
		MotorShieldChannel_Slopped( bool AIsSecond ) :
			Enabled( true ),
			FIsSecond( AIsSecond )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&MotorShieldChannel_Slopped::DoReceive );
			if( AIsSecond )
			{
				pinMode( PIN_FORWARD_2, OUTPUT );
				pinMode( PIN_REVERSE_2, OUTPUT );
				pinMode( PIN_ENABLE_2, OUTPUT );
			}
			else
			{
				pinMode( PIN_FORWARD_1, OUTPUT );
				pinMode( PIN_REVERSE_1, OUTPUT );
				pinMode( PIN_ENABLE_1, OUTPUT );
			}
		}
	};
//---------------------------------------------------------------------------
}

#endif
