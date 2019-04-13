////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_PWM_PCA9685_h
#define _MITOV_PWM_PCA9685_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	class PCA9685PWMChannel : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::IndexedSinkPin<uint8_t>	InputPin;

	public:
		float	InitialValue = 0.0;
		float	OnValue = 0.0;

	};
//---------------------------------------------------------------------------
	class PCA9685ServoChannel : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::IndexedSinkPin<uint8_t>	InputPin;

	public:
		float	InitialValue = 0.0;

	};
//---------------------------------------------------------------------------
	enum TArduinoAnalogPCA9685OutputLogicMode { olmZero, olmOne, olmHighImpedance };
//---------------------------------------------------------------------------
	class TArduinoAnalogPCA9685OutputLogic
	{
	public:
		bool	Inverted : 1;
		TArduinoAnalogPCA9685OutputLogicMode	Mode : 2;

	public:
		TArduinoAnalogPCA9685OutputLogic() :
			Inverted( false ),
			Mode( olmZero )
		{
		}
	};
//---------------------------------------------------------------------------
	class PCA9685PWM : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

#ifdef _MITOV_PWM_PCA9685_CLOCK_PIN_
	public:
		OpenWire::ConnectSinkPin		ClockInputPin;
#endif

	public:
		PCA9685PWMChannel	Channels[ 16 ];

	public:
		TArduinoAnalogPCA9685OutputLogic	OutputLogic;
		float	OscilatorFrequency = 25000000;
		float	Frequency = 1000;
		bool	Enabled : 1;
		bool	OpenDrain : 1;
		bool	Sleep : 1;
		bool	UpdateOnAck : 1;
		bool	ExternalClock : 1;
		uint16_t	FChangedChannels = 0xFFFF;

	protected:
		TwoWire &FWire;
		uint8_t	FAddress;

	public:
		static const uint8_t	PCA9685_MODE1 = 0x0;
		static const uint8_t	PCA9685_MODE2 = 0x1;

		static const uint8_t	PCA9685_SUBADR1 = 0x2;
		static const uint8_t	PCA9685_SUBADR2 = 0x3;
		static const uint8_t	PCA9685_SUBADR3 = 0x4;

		static const uint8_t	PCA9685_PRESCALE = 0xFE;

		static const uint8_t	LED0_ON_L = 0x6;
		static const uint8_t	LED0_ON_H = 0x7;
		static const uint8_t	LED0_OFF_L = 0x8;
		static const uint8_t	LED0_OFF_H = 0x9;

		static const uint8_t	ALLLED_ON_L = 0xFA;
		static const uint8_t	ALLLED_ON_H = 0xFB;
		static const uint8_t	ALLLED_OFF_L = 0xFC;
		static const uint8_t	ALLLED_OFF_H = 0xFD;

	public:
		void UpdateFrequency()
		{
			float Afreq = Frequency * 0.9;  // Correct for overshoot in the frequency setting (see issue #11).
			float prescaleval = OscilatorFrequency;
			prescaleval /= 4096;
			prescaleval /= Afreq;
			prescaleval -= 1;
			uint8_t prescale = floor(prescaleval + 0.5);

			uint8_t oldmode = Read8(PCA9685_MODE1);
			uint8_t newmode = (oldmode&0x7F) | 0x10; // sleep
			Write8(PCA9685_MODE1, newmode); // go to sleep
			Write8(PCA9685_PRESCALE, prescale); // set the prescaler
//			Serial.println( prescale );
			Write8(PCA9685_MODE1, oldmode);
			delay(1);
			Write8(PCA9685_MODE1, oldmode | 0xA0);  //  This sets the MODE1 register to turn on auto increment.
													// This is why the beginTransmission below was not working.
		}

		void UpdateConfig()
		{
			UpdateConfigOnly();
			UpdateAll();
		}

	protected:
		uint8_t Read8(uint8_t addr) 
		{
			FWire.beginTransmission( FAddress );
			FWire.write(addr);
			FWire.endTransmission();

			FWire.requestFrom( FAddress, (uint8_t)1);
			return FWire.read();
		}

		void Write8(uint8_t addr, uint8_t d) 
		{
//  Serial.print( "addr: " ); Serial.print( addr, HEX );
//  Serial.print( " = " ); Serial.println( d, HEX );
			FWire.beginTransmission( FAddress );
			FWire.write(addr);
			FWire.write(d);
			FWire.endTransmission();
		}

	protected:
		void DoIndexReceive( int AIndex, void *_Data )
		{
			float AOnValue = *(float *)_Data;
			AOnValue = constrain( AOnValue, 0.0, 1.0 );
			if( Channels[ AIndex ].InitialValue == AOnValue )
				return;

			Channels[ AIndex ].InitialValue = AOnValue;
			if( Sleep || ( ! Enabled ))
			{
				FChangedChannels |= ( 1 << AIndex );
				return;
			}

#ifdef _MITOV_PWM_PCA9685_CLOCK_PIN_
			if( ClockInputPin.IsConnected() )
			{
				FChangedChannels |= ( 1 << AIndex );
				return;
			}
#endif

			UpdateChannelValue( AIndex, AOnValue );
		}

		void UpdateConfigOnly()
		{
			uint8_t	AValue = 0b100000;

			if( ExternalClock )
				AValue |= 0b1000000;

			if( Sleep )
				AValue |= 0b10000;

//			Serial.print( "PCA9685_MODE1: " );
//			Serial.println( AValue, BIN );

			Write8( PCA9685_MODE1, AValue );

			AValue = 0;

			if( OutputLogic.Inverted )
				AValue |= 0b10000;

			if( UpdateOnAck )
				AValue |= 0b1000;

			if( ! OpenDrain )
				AValue |= 0b100;

			AValue |= OutputLogic.Mode;

//			Serial.print( "PCA9685_MODE2: " );
//			Serial.println( AValue, BIN );

			Write8( PCA9685_MODE2, AValue );
		}

		void UpdateAll()
		{
			if( ! Enabled )
				return;

			if( Sleep )
				return;

			for( int i = 0; i < 16; ++i )
				if( FChangedChannels & ( 1 << i ) )
					UpdateChannelValue( i, Channels[ i ].InitialValue );

		}

		void UpdateChannelValue( int AIndex, float AValue )
		{
			uint16_t	on = Channels[ AIndex ].OnValue * 4095 + 0.5;
			uint16_t	off = AValue * 4095 + 0.5;

//			Serial.print( "AIndex: " );
//			Serial.println( AIndex );
//			Serial.println( on );
//			Serial.println( off );

			FWire.beginTransmission( FAddress );
			FWire.write(LED0_ON_L + 4 * AIndex );
			FWire.write(on);
			FWire.write(on>>8);
			FWire.write(off);
			FWire.write(off>>8);
			FWire.endTransmission();

			FChangedChannels &= ~( 1 << AIndex );
		}

#ifdef _MITOV_PWM_PCA9685_CLOCK_PIN_
		void DoClockReceive( void *_Data )
		{
			UpdateAll();
//			ProcessData( false, false );
		}
#endif
	protected:
/*
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
//			inherited::SystemLoopBegin( currentMicros );
		}
*/
		virtual void SystemStart() override
		{
//			inherited::SystemStart();
			Write8(PCA9685_MODE1, 0x0);
			UpdateConfigOnly();
			UpdateFrequency();
			UpdateAll();
		}

	public:
		PCA9685PWM( uint8_t AAddress, TwoWire &AWire ) :
			Enabled( true ),
			OpenDrain( false ),
			Sleep( false ),
			UpdateOnAck( false ),
			ExternalClock( false ),
			FWire( AWire ),
			FAddress( AAddress )
		{
			for( int i = 0; i < 16; ++i )
			{
				Channels[ i ].InputPin.Index = i;
				Channels[ i ].InputPin.OnReceiveObject = this;
				Channels[ i ].InputPin.OnIndexedReceive = (OpenWire::TOnPinIndexedReceive)&PCA9685PWM::DoIndexReceive;
			}
				
#ifdef _MITOV_PWM_PCA9685_CLOCK_PIN_
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&PCA9685PWM::DoClockReceive );
#endif
		}

	};
//---------------------------------------------------------------------------
	class PCA9685Servo : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

#ifdef _MITOV_SERVO_PCA9685_CLOCK_PIN_
	public:
		OpenWire::ConnectSinkPin		ClockInputPin;
#endif

	public:
		PCA9685ServoChannel	Channels[ 16 ];

	public:
		TArduinoAnalogPCA9685OutputLogic	OutputLogic;
		float	OscilatorFrequency = 25000000;
		bool	Enabled : 1;
		bool	OpenDrain : 1;
		bool	Sleep : 1;
		bool	UpdateOnAck : 1;
		bool	ExternalClock : 1;
		uint16_t	FChangedChannels = 0xFFFF;

	protected:
		TwoWire &FWire;
		uint8_t	FAddress;

	public:
		void UpdateFrequency()
		{
			float Afreq = 60; // * 0.9;  // Correct for overshoot in the frequency setting (see issue #11).
			float prescaleval = OscilatorFrequency;
			prescaleval /= 4096;
			prescaleval /= Afreq;
			prescaleval -= 1;
			uint8_t prescale = floor(prescaleval + 0.5);

			uint8_t oldmode = Read8( PCA9685PWM::PCA9685_MODE1);
			uint8_t newmode = (oldmode&0x7F) | 0x10; // sleep
			Write8( PCA9685PWM::PCA9685_MODE1, newmode); // go to sleep
			Write8( PCA9685PWM::PCA9685_PRESCALE, prescale); // set the prescaler
//			Serial.println( prescale );
			Write8( PCA9685PWM::PCA9685_MODE1, oldmode);
			delay(1);
			Write8( PCA9685PWM::PCA9685_MODE1, oldmode | 0xA0);  //  This sets the MODE1 register to turn on auto increment.
													// This is why the beginTransmission below was not working.
		}

		void UpdateConfig()
		{
			UpdateConfigOnly();
			UpdateAll();
		}

	protected:
		uint8_t Read8(uint8_t addr) 
		{
			FWire.beginTransmission( FAddress );
			FWire.write(addr);
			FWire.endTransmission();

			FWire.requestFrom( FAddress, (uint8_t)1);
			return FWire.read();
		}

		void Write8(uint8_t addr, uint8_t d) 
		{
//  Serial.print( "addr: " ); Serial.print( addr, HEX );
//  Serial.print( " = " ); Serial.println( d, HEX );
			FWire.beginTransmission( FAddress );
			FWire.write(addr);
			FWire.write(d);
			FWire.endTransmission();
		}

	protected:
		void DoIndexReceive( int AIndex, void *_Data )
		{
			float AOnValue = *(float *)_Data;
			AOnValue = constrain( AOnValue, 0.0, 1.0 );
			if( Channels[ AIndex ].InitialValue == AOnValue )
				return;

			Channels[ AIndex ].InitialValue = AOnValue;
			if( Sleep || ( ! Enabled ))
			{
				FChangedChannels |= ( 1 << AIndex );
				return;
			}

#ifdef _MITOV_SERVO_PCA9685_CLOCK_PIN_
			if( ClockInputPin.IsConnected() )
			{
				FChangedChannels |= ( 1 << AIndex );
				return;
			}
#endif

			UpdateChannelValue( AIndex, AOnValue );
		}

		void UpdateConfigOnly()
		{
			uint8_t	AValue = 0b100000;

			if( ExternalClock )
				AValue |= 0b1000000;

			if( Sleep )
				AValue |= 0b10000;

//			Serial.print( "PCA9685_MODE1: " );
//			Serial.println( AValue, BIN );

			Write8( PCA9685PWM::PCA9685_MODE1, AValue );

			AValue = 0;

			if( OutputLogic.Inverted )
				AValue |= 0b10000;

			if( UpdateOnAck )
				AValue |= 0b1000;

			if( ! OpenDrain )
				AValue |= 0b100;

			AValue |= OutputLogic.Mode;

//			Serial.print( "PCA9685_MODE2: " );
//			Serial.println( AValue, BIN );

			Write8( PCA9685PWM::PCA9685_MODE2, AValue );
		}

		void UpdateAll()
		{
			if( ! Enabled )
				return;

			if( Sleep )
				return;

			for( int i = 0; i < 16; ++i )
				if( FChangedChannels & ( 1 << i ) )
					UpdateChannelValue( i, Channels[ i ].InitialValue );

		}

		void UpdateChannelValue( int AIndex, float AValue )
		{
//			uint16_t	off = AValue * 4095 + 0.5;
			uint16_t	off = AValue * ( 600 - 150 ) + 150.5;

//			Serial.print( "AIndex: " );
//			Serial.println( AIndex );
//			Serial.println( on );
//			Serial.println( off );

			FWire.beginTransmission( FAddress );
			FWire.write( PCA9685PWM::LED0_OFF_L + 4 * AIndex );
			FWire.write(off);
			FWire.write(off>>8);
			FWire.endTransmission();

			FChangedChannels &= ~( 1 << AIndex );
		}

#ifdef _MITOV_SERVO_PCA9685_CLOCK_PIN_
		void DoClockReceive( void *_Data )
		{
			UpdateAll();
//			ProcessData( false, false );
		}
#endif
	protected:
/*
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
//			inherited::SystemLoopBegin( currentMicros );
		}
*/
		virtual void SystemStart() override
		{
//			inherited::SystemStart();
			Write8( PCA9685PWM::PCA9685_MODE1, 0x0);
			UpdateConfigOnly();
			UpdateFrequency();
			UpdateAll();
		}

	public:
		PCA9685Servo( uint8_t AAddress, TwoWire &AWire ) :
			Enabled( true ),
			OpenDrain( false ),
			Sleep( false ),
			UpdateOnAck( false ),
			ExternalClock( false ),
			FWire( AWire ),
			FAddress( AAddress )
		{
			for( int i = 0; i < 16; ++i )
			{
				Channels[ i ].InputPin.Index = i;
				Channels[ i ].InputPin.OnReceiveObject = this;
				Channels[ i ].InputPin.OnIndexedReceive = (OpenWire::TOnPinIndexedReceive)&PCA9685Servo::DoIndexReceive;
			}
				
#ifdef _MITOV_SERVO_PCA9685_CLOCK_PIN_
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&PCA9685Servo::DoClockReceive );
#endif
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
