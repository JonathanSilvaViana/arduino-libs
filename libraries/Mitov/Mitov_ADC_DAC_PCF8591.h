////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ADC_DAC_PCF8591_h
#define _MITOV_ADC_DAC_PCF8591_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	class PCF8591_DAC
	{
	public:
#ifdef _MITOV_PCF8591_CLOCK_PIN_
		OpenWire::ConnectSinkPin		ClockInputPin;
#endif

#ifdef _MITOV_PCF8591_DAC_INPUT_PIN_
		OpenWire::SinkPin	InputPin;
#endif
	public:
		bool	Enabled = true;
		float	InitialValue = 0;
	};
//---------------------------------------------------------------------------
	enum TArduinoPCF8591ADCMode { adcSingleEnded, adc3Differential, adcMixed, adc2Differential };
//---------------------------------------------------------------------------
	class PCF8591 : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
#ifdef _MITOV_PCF8591_CLOCK_PIN_
		OpenWire::ConnectSinkPin		ClockInputPin;
#endif

		OpenWire::TypedSourcePin<float>	OutputPins[ 4 ];

	public:
		PCF8591_DAC	DAC;

	public:
		bool					Enabled : 1;
		TArduinoPCF8591ADCMode	Mode : 2;

	protected:
		uint8_t FStartChannel : 2;
		uint8_t FEndChannel : 2;
		uint8_t	FAddress : 3;

//		uint8_t	FConfig = 0xFF;

		TwoWire &FWire;

	protected:
#ifdef _MITOV_PCF8591_CLOCK_PIN_
		void DoClockReceive( void *_Data )
		{
			ProcessData( false, false );
		}
#endif

#ifdef _MITOV_PCF8591_DAC_CLOCK_PIN_
		void DoDACClockReceive( void *_Data )
		{
			ProcessData( true, false );
		}
#endif

#ifdef _MITOV_PCF8591_DAC_INPUT_PIN_
		void DoDACReceive( void *_Data )
		{
			float AValue = *(float *)_Data;
			if( AValue == DAC.InitialValue )
				return;

			DAC.InitialValue = AValue;
#ifdef _MITOV_PCF8591_CLOCK_PIN_
			if( ! ClockInputPin.IsConnected() )
#endif
				ProcessData( true, false );

		}
#endif
		inline uint8_t GetAddress()
		{
			return 0x48 | FAddress;
		}

		void ProcessData( bool AFromDAC, bool AFirstTime )
		{
			if( ! Enabled )
				return;

			uint8_t	AStartChannel;
			uint8_t AMaxChannel;
			switch( Mode )
			{
				case adcSingleEnded:	AMaxChannel = 4; break;
				case adc2Differential:	AMaxChannel = 2; break;
				default: AMaxChannel = 3;
			}

			if( FStartChannel > AMaxChannel )
				AStartChannel = AMaxChannel;

			else
				AStartChannel = FStartChannel;


//			for( AStartChannel = 0; AStartChannel < AMaxChannel; ++AStartChannel )
//				if( OutputPins[ i ].IsConnected() )
//					break;

/*
			while( AMaxChannel > AStartChannel )
			{
				if( OutputPins[ AMaxChannel ].IsConnected() )
					break;

				--AMaxChannel;
			}
*/

			uint8_t	AConfig =	AStartChannel | 
								0b100 | 
								uint8_t( Mode ) << 4;

			if( DAC.Enabled )
				AConfig |= 0b01000000;

			uint8_t	AOutValue = ( DAC.InitialValue * 255 ) + 0.5;

			FWire.beginTransmission( GetAddress() );
			FWire.write( AConfig );
//			Serial.println( AConfig, BIN );
			if( AFromDAC )
				FWire.write( AOutValue );

			FWire.endTransmission();

			if( AFromDAC )
				if( ! AFirstTime )
					return;

			if( FEndChannel < AStartChannel )
				return;

			int8_t ACount = FEndChannel - AStartChannel + 1;

			FWire.requestFrom(GetAddress(), uint8_t( ACount + 1 ));

			FWire.read(); // Skip old value
			for( int8_t i = 0; i < ACount; ++i )
			{
				bool AIsDiferential = false;
				if((( AStartChannel + i ) < 2 ) && ( Mode == adc2Differential ) )
					AIsDiferential = true;

				else if((( AStartChannel + i ) == 2 ) && ( Mode == adcMixed ) )
					AIsDiferential = true;

				float AValue;
				if( AIsDiferential )
					AValue = ( float( int8_t( FWire.read() )) / 255.0 ) + 0.5;

				else
  					AValue = float( FWire.read() ) / 255.0;

				OutputPins[ AStartChannel + i ].SetValue( AValue, ! AFirstTime );
			}
		}

	protected:
		virtual void SystemStart() override
		{
//			inherited::SystemStart();
			ProcessData( true, true );
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
#ifdef _MITOV_PCF8591_CLOCK_PIN_
			if( ! ClockInputPin.IsConnected() )
#endif
				ProcessData( false, false );

//			inherited::SystemLoopBegin( currentMicros );
		}

	public:
		PCF8591( uint8_t AAddress, TwoWire &AWire, uint8_t AStartChannel, uint8_t AEndChannel ) :
			Enabled( true ),
			Mode( adcSingleEnded ),
			FStartChannel( AStartChannel ),
			FEndChannel( AEndChannel ),
			FAddress( AAddress & 0b111 ),
			FWire( AWire )
		{
#ifdef _MITOV_PCF8591_CLOCK_PIN_
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&PCF8591::DoClockReceive );
#endif

#ifdef _MITOV_PCF8591_DAC_CLOCK_PIN_
			DAC.ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&PCF8591::DoDACClockReceive );
#endif

#ifdef _MITOV_PCF8591_DAC_INPUT_PIN_
			DAC.InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&PCF8591::DoDACReceive );
#endif		

		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif