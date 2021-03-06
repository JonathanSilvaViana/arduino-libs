////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_PID_CONTROLLER_h
#define _MITOV_PID_CONTROLLER_h

#include <Mitov.h>

namespace Mitov
{
	class PIDController : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin			InputPin;
		OpenWire::SourcePin			OutputPin;
		OpenWire::ConnectSinkPin	ClockInputPin;
		OpenWire::SinkPin			ManualControlInputPin;

    public:
        bool	Enabled = true;
		float	ProportionalGain = 0.1f;
		float	IntegralGain = 0.1f;
		float	DerivativeGain = 0.1f;

		float	SetPoint = 0.0f;

		float	InitialValue = 0.0f;

	protected:
		unsigned long	FLastTime;
		float	FOutput = 0.0f;
		float	FInput;
		float	FLastInput;
		float	FITerm;

	public:
		void SetEnabled( bool AValue )
		{
            if( Enabled == AValue )
                return;

            Enabled = AValue;
			if( Enabled )
				Initialize();

		}

	protected:
		void DoReceive( void *_Data )
		{
			FInput = *(float *)_Data;
		}

		void DoManualControlReceive( void *_Data )
		{
			if( Enabled )
				return;

			FOutput = *(float *)_Data;
		}

		void DoClockReceive( void *_Data )
		{
			OutputPin.Notify( &FOutput );
		}

	protected:
		void Initialize()
		{
			FITerm = FOutput;
			FLastInput = FInput;
			FLastTime = micros();
		}

	protected:
		virtual void SystemStart() override
		{
			FInput = InitialValue;
			Initialize();
//			inherited::SystemStart();
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
//			inherited::SystemLoopBegin( currentMicros );
			if( ! Enabled ) 
				return;

			unsigned long timeChange = ( currentMicros - FLastTime );
			float ANormalizedTime = float( timeChange ) / 1000000;

			// Compute all the working error variables
			double error = SetPoint - FInput;
//			ITerm += ( ki * error ) * ANormalizedTime;
//			Serial.println( FITerm + ( IntegralGain * error ) * ANormalizedTime );

			FITerm = constrain( FITerm + ( IntegralGain * error ) * ANormalizedTime, -10000.0f, 10000.0f );
//			FITerm += ( IntegralGain * error ) * ANormalizedTime;

			double dInput = ( FInput - FLastInput ) * ANormalizedTime;
 
			// Compute PID Output
			float AOutput = constrain( ProportionalGain * error + FITerm - DerivativeGain * dInput, 0.0f, 1.0f );
	  
			// Remember some variables for next time
			FLastInput = FInput;
			FLastTime = currentMicros;

			if( AOutput == FOutput )
				return;

			FOutput = AOutput;

			if( ClockInputPin.IsConnected() )
				return;

			OutputPin.Notify( &FOutput );
		}

	public:
		PIDController()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&PIDController::DoReceive );
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&PIDController::DoClockReceive );
			ManualControlInputPin.SetCallback( MAKE_CALLBACK( PIDController::DoManualControlReceive ));
		}

	};
//---------------------------------------------------------------------------
}

#endif
