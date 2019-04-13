////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_SERVO_h
#define _MITOV_SERVO_h

#include <Mitov.h>

#include <Servo.h>

namespace Mitov
{
	template<int PIN_NUMBER, int PULSE_MIN, int PULSE_MAX> class MitovServo : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;

	public:
		bool	Enabled = true;
		float	InitialValue = 0.5;

	public:
		void SetEnabled( bool AValue )
		{
			if( Enabled == AValue )
				return;

			Enabled = AValue;
			if( Enabled )
			{
				FServo.attach( PIN_NUMBER, PULSE_MIN, PULSE_MAX );
				DoReceive( &InitialValue );
			}
			else
				FServo.detach();
		}

	protected:
		Servo	FServo;
		
	protected:
		void DoReceive( void *_Data )
		{
			InitialValue = *((float *)_Data);
			if( ! Enabled )
				return;

			float AValue = constrain( InitialValue, 0.0f, 1.0f ) * 180;
			FServo.write( AValue );
		}

	public:
		void SystemStart()
		{
			if( Enabled )
			{
				FServo.attach( PIN_NUMBER, PULSE_MIN, PULSE_MAX );
				DoReceive( &InitialValue );
			}
//			float AValue = constrain( InitialValue, 0.0f, 1.0f ) * 180;
//			FServo.write( AValue );
		}

	public:
		MitovServo()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&MitovServo::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	template<int PIN_NUMBER, int PULSE_MIN, int PULSE_MAX> class ServoShieldElement
#ifdef _MITOV_SHIELD_SERVO_PIN_
		: public OpenWire::Object
#endif
	{
#ifdef _MITOV_SHIELD_SERVO_PIN_
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
#endif

	protected:
		Servo	FServo;

	public:
		void SystemInit( float AInitialValue )
		{
			FServo.attach( PIN_NUMBER, PULSE_MIN, PULSE_MAX );
			DoReceive( &AInitialValue );
		}

	protected:
		void DoReceive( void *_Data )
		{
			float AValue = constrain( *((float *)_Data), 0.0f, 1.0f ) * 180;
			FServo.write( AValue );
		}

#ifdef _MITOV_SHIELD_SERVO_PIN_

	public:
		ServoShieldElement()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ServoShieldElement::DoReceive );
		}
#endif
	};
//---------------------------------------------------------------------------
}

#endif
