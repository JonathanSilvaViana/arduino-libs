////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_L9110S_DUAL_MOTOR_h
#define _MITOV_L9110S_DUAL_MOTOR_h

#include <Mitov.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	class L9110SDualMotorDriverChannel : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	DirectionOutputPin;
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

				if( ADirection )
					AOutSpeed = 1 - AOutSpeed;

				SpeedOutputPin.Notify( &AOutSpeed );
				DirectionOutputPin.Notify( &ADirection );
			}

			else
			{
				SpeedOutputPin.SendValue( 0.5 );
				DirectionOutputPin.SendValue( false );
			}
		}

	protected:
		virtual void SystemStart() override
		{
//			inherited::SystemStart();
			UpdateOutputs();
		}

	public:
		L9110SDualMotorDriverChannel()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&L9110SDualMotorDriverChannel::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	class L9110SDualMotorDriverChannel_Slopped : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	DirectionOutputPin;
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

				if( ADirection )
					AOutSpeed = 1 - AOutSpeed;

				SpeedOutputPin.Notify( &AOutSpeed );
				DirectionOutputPin.Notify( &ADirection );
			}

			else
			{
				SpeedOutputPin.SendValue( 0.5 );
				DirectionOutputPin.SendValue( false );
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
		L9110SDualMotorDriverChannel_Slopped()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&L9110SDualMotorDriverChannel_Slopped::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	class L298NDualMotorDriverPWMChannel : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	ForwardOutputPin;
		OpenWire::SourcePin	ReverseOutputPin;

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
				if( InitialValue > 0.5 )
				{
					ReverseOutputPin.SendValue<float>( 0 );
					ForwardOutputPin.Notify( &AOutSpeed );
				}
				else
				{
					ForwardOutputPin.SendValue<float>( 0 );
					ReverseOutputPin.Notify( &AOutSpeed );
				}
			}

			else
			{
				ForwardOutputPin.SendValue<float>( 0 );
				ReverseOutputPin.SendValue<float>( 0 );
			}
		}

	protected:
		virtual void SystemStart() override
		{
//			inherited::SystemStart();
			UpdateOutputs();
		}

	public:
		L298NDualMotorDriverPWMChannel()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&L298NDualMotorDriverPWMChannel::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	class L298NDualMotorDriverPWMChannel_Slopped : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	ForwardOutputPin;
		OpenWire::SourcePin	ReverseOutputPin;

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
				if( FCurrentValue > 0.5 )
				{
					ReverseOutputPin.SendValue<float>( 0 );
					ForwardOutputPin.Notify( &AOutSpeed );
				}
				else
				{
					ForwardOutputPin.SendValue<float>( 0 );
					ReverseOutputPin.Notify( &AOutSpeed );
				}
			}

			else
			{
				ForwardOutputPin.SendValue<float>( 0 );
				ReverseOutputPin.SendValue<float>( 0 );
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
		L298NDualMotorDriverPWMChannel_Slopped()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&L298NDualMotorDriverPWMChannel_Slopped::DoReceive );
		}
	};
//---------------------------------------------------------------------------
}

#endif
