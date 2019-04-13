////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ROTARY_ENCODER_SENSOR_h
#define _MITOV_ROTARY_ENCODER_SENSOR_h

#include <Mitov.h>

namespace Mitov
{
	class RotaryEncoderSensor : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	AInputPin;
//		OpenWire::SinkPin	BInputPin;

		OpenWire::SourcePin	UpOutputPin;
		OpenWire::SourcePin	DownOutputPin;

	public:
		bool	Enabled : 1;

	protected:
		bool	FAValue : 1;
//		bool	FBValue : 1;
//		bool    FAPopulated : 1;
//		bool    FBPopulated : 1;

		Mitov::BasicPinRead *FBPin;

	protected:
		void DoReceiveA( bool AValue )
		{
//			FAPopulated = true;
			if( FAValue == AValue )
				return;

			FAValue = AValue;
			if( Enabled )
				if( AValue )
				{
					if( FBPin->DigitalRead() )
						DownOutputPin.Notify( nullptr );

					else
						UpOutputPin.Notify( nullptr );
				}
				else
				{
					if( FBPin->DigitalRead() )
						UpOutputPin.Notify( nullptr );

					else
						DownOutputPin.Notify( nullptr );
				}

		}

/*
		void DoReceiveB( bool AValue )
		{
			FBPopulated = true;
			if( FBValue == AValue )
				return;

			FBValue = AValue;
		}
*/
	public:
		RotaryEncoderSensor( Mitov::BasicPinRead *ABPin ) :
			Enabled( true ),
//			FAPopulated( false ),
//			FBPopulated( false ),
			FAValue( false ),
//			FBValue( false ),
			FBPin( ABPin )
		{
			AInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&RotaryEncoderSensor::DoReceiveA );
//			BInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&RotaryEncoderSensor::DoReceiveB );
		}
	};
//---------------------------------------------------------------------------
	class RotaryEncoderSensor_Interrupt : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
//		OpenWire::SinkPin	BInputPin;

		OpenWire::SourcePin	UpOutputPin;
		OpenWire::SourcePin	DownOutputPin;

	public:
		bool	Enabled : 1;

	protected:
		uint8_t FUpCount : 7;

		bool	FAValue : 1;
//		bool	FBValue : 1;
//		bool    FAPopulated : 1;
//		bool    FBPopulated : 1;
		uint8_t FDownCount : 7;

		Mitov::BasicPinRead *FBPin;

	public:
		void InterruptHandlerA( bool AValue )
		{
//			FAPopulated = true;
			if( FAValue == AValue )
				return;

			FAValue = AValue;
			if( Enabled )
				if( AValue )
				{
					if( FBPin->DigitalRead() )
						++FDownCount;

					else
						++FUpCount;
				}
				else
				{
					if( FBPin->DigitalRead() )
						++FUpCount;

					else
						++FDownCount;
				}
		}

/*
	protected:
		void DoReceiveB( void *_Data )
		{
			bool AValue = *( bool *)_Data;
			FBPopulated = true;
			if( FBValue == AValue )
				return;

			FBValue = AValue;
		}
*/
	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros )
		{
			noInterrupts();
			uint8_t ADownCount = FDownCount;
			FDownCount = 0;

			uint8_t AUpCount = FUpCount;
			FUpCount = 0;

			interrupts();

			while( ADownCount )
			{
				--ADownCount;
				DownOutputPin.Notify( nullptr );
			}

			while( AUpCount )
			{
				--AUpCount;
				UpOutputPin.Notify( nullptr );
			}

//			inherited::SystemLoopBegin( currentMicros );
		}

	public:
		RotaryEncoderSensor_Interrupt( Mitov::BasicPinRead *ABPin ) :
			Enabled( true ),
			FUpCount( 0 ),
//			FAPopulated( false ),
//			FBPopulated( false ),
			FAValue( false ),
			FDownCount( 0 ),
//			FBValue( false ),
			FBPin( ABPin )
		{
//			BInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&RotaryEncoderSensor_Interrupt::DoReceiveB );
		}

	};
//---------------------------------------------------------------------------
	class RotaryEncoderSensor_Debounce : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	AInputPin;
//		OpenWire::SinkPin	BInputPin;

		OpenWire::SourcePin	UpOutputPin;
		OpenWire::SourcePin	DownOutputPin;

	public:
		uint32_t	DebounceInterval = 0;
		bool	Enabled : 1;

	protected:
		bool	FAValue : 1;
//		bool	FBValue : 1;
//		bool    FAPopulated : 1;
//		bool    FBPopulated : 1;
		bool	FALastValue : 1;
//		bool	FInTimer : 1;
//		bool	FBLastValue : 1;

		unsigned long	FALastTime = 0;
//		unsigned long	FBLastTime = 0;

		Mitov::BasicPinRead *FBPin;

	protected:
		void DoValueReceiveA( bool AValue )
		{
//			FAPopulated = true;
//			FInTimer = false;
			if( FAValue == AValue )
				return;

			FAValue = AValue;
			if( Enabled )
				if( AValue )
				{
					if( FBPin->DigitalRead() )
						DownOutputPin.Notify( nullptr );

					else
						UpOutputPin.Notify( nullptr );
				}
				else
				{
					if( FBPin->DigitalRead() )
						UpOutputPin.Notify( nullptr );

					else
						DownOutputPin.Notify( nullptr );
				}

		}

/*
		void DoValueReceiveB( bool AValue )
		{
			FBPopulated = true;
			if( FBValue == AValue )
				return;

			FBValue = AValue;
		}
*/
	protected:
		void DoReceiveA( void *_Data )
		{
			bool AValue = *( bool *)_Data;
			if( AValue != FALastValue )
			{
				FALastTime = millis();
//				FInTimer = true;
			}

			FALastValue = AValue;
		}

/*
		void DoReceiveB( void *_Data )
		{
			bool AValue = *( bool *)_Data;
			if( AValue != FBLastValue )
				FBLastTime = millis();

			FBLastValue = AValue;
		}
*/

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros )
		{
			if( FAValue != FALastValue )
//				if(( ! FInTimer ) || ( millis() - FALastTime > DebounceInterval ))
				if( millis() - FALastTime > DebounceInterval )
					DoValueReceiveA( FALastValue );

/*
			if( FBValue != FBLastValue )
				if( millis() - FBLastTime > DebounceInterval )
					DoValueReceiveB( FBLastValue );
*/
//			inherited::SystemLoopBegin( currentMicros );
		}

	public:
		RotaryEncoderSensor_Debounce( Mitov::BasicPinRead *ABPin ) :
			Enabled( true ),
			FAValue( false ),
			FALastValue( false ),
//			FInTimer( false ),
			FBPin( ABPin )
		{
			AInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&RotaryEncoderSensor_Debounce::DoReceiveA );
//			BInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&RotaryEncoderSensor_Debounce::DoReceiveB );
		}
	};
//---------------------------------------------------------------------------
	class RotaryEncoderSensor_Debounce_Interrupt : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SourcePin	UpOutputPin;
		OpenWire::SourcePin	DownOutputPin;

	public:
		uint32_t	DebounceInterval = 0;
		bool		Enabled : 1;

	protected:
		uint8_t FUpCount : 7;
//		bool	FAValue : 1;

		uint8_t FDownCount : 7;
		bool	FInTimer : 1;

		unsigned long	FALastTime = 0;
		Mitov::BasicPinRead *FBPin;

	public:
		void InterruptHandlerA( bool AValue )
		{
			if( ! AValue )
				return;

//			if( FAValue == AValue )
//				return;

			unsigned long	ATime = millis();
			if( FInTimer )
				if(( ATime - FALastTime ) <= DebounceInterval )
					return;

//			FAValue = AValue;
			if( Enabled )
			{
				FInTimer = true;
				FALastTime = ATime;
				if( AValue )
				{
					if( FBPin->DigitalRead() )
						++FDownCount;

					else
						++FUpCount;
				}
				else
				{
					if( FBPin->DigitalRead() )
						++FUpCount;

					else
						++FDownCount;
				}
			}
		}

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros )
		{
			unsigned long ATime = millis();

			noInterrupts();
			if( FInTimer )
				if( ATime - FALastTime > DebounceInterval )
					FInTimer = false;

			uint8_t ADownCount = FDownCount;
			FDownCount = 0;

			uint8_t AUpCount = FUpCount;
			FUpCount = 0;

			interrupts();

			while( ADownCount )
			{
				--ADownCount;
				DownOutputPin.Notify( nullptr );
			}

			while( AUpCount )
			{
				--AUpCount;
				UpOutputPin.Notify( nullptr );
			}

//			inherited::SystemLoopBegin( currentMicros );
		}

	public:
		RotaryEncoderSensor_Debounce_Interrupt( Mitov::BasicPinRead *ABPin ) :
			Enabled( true ),
			FUpCount( 0 ),
//			FAValue( false ),
			FDownCount( 0 ),
			FInTimer( false ),
			FBPin( ABPin )
		{
		}

	};
//---------------------------------------------------------------------------
}

#endif
