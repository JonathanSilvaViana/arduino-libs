////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_MICROCHIP_TCN75A_h
#define _MITOV_MICROCHIP_TCN75A_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	class TArduinoMicrochipTCN75AAlert
	{
	public:
		uint8_t QueueSize : 3;
		bool	InterruptMode : 1;
		bool	ActiveLow : 1;
		float	Temperature = 80.0;
		float	Hysteresis = 5.0;

	public:
		TArduinoMicrochipTCN75AAlert() :
			QueueSize( 1 ),
			InterruptMode( false ),
			ActiveLow( true )
		{
		}
	};
//---------------------------------------------------------------------------
	class MicrochipTCN75A : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
#if( defined( _MITOV_MICROCHIP_TCN75A_CLOCK_PIN_ ) || defined( _MITOV_MICROCHIP_SHIELD_TCN75A_CLOCK_PIN_ ))
		OpenWire::ConnectSinkPin	ClockInputPin;
#endif
		OpenWire::TypedStartSourcePin<float>	OutputPin;

	protected:
#if( defined( _MITOV_MICROCHIP_TCN75A_CLOCK_PIN_ ) || defined( _MITOV_MICROCHIP_SHIELD_TCN75A_CLOCK_PIN_ ))
		void DoClockReceive( void *_Data )
		{
			ReadData();
		}
#endif

	public:
		bool Enabled : 1;
		uint8_t Resolution : 4;
		bool InFahrenheit : 1;
		bool ShutDown : 1;
		TArduinoMicrochipTCN75AAlert Alert;

	protected:
		uint8_t FAddress;

		TwoWire &FWire;

	protected:
		 static const uint8_t TEMPERATURE_REG = 0x00;
		 static const uint8_t CONFIG_REG = 0x01;
		 static const uint8_t HYSTERESIS_REG = 0x02;
		 static const uint8_t LIMIT_REG = 0x03;

	public:
		void	UpdateLimits()
		{
			WriteRegister16Temp( LIMIT_REG, Alert.Temperature );
			UpdateHysteresis();
		}

		void	UpdateHysteresis()
		{
			float AMinValue = Alert.Temperature - Alert.Hysteresis;
			if( AMinValue < -273.15 )
				AMinValue = -273.15;

			WriteRegister16Temp( HYSTERESIS_REG, AMinValue );
		}

		void	UpdateConfig()
		{
			uint8_t	AValue =	(( Resolution - 9 ) & 0b11 << 5 );
			switch( Alert.QueueSize )
			{
				case 2:		AValue |= 0b01000; break;
				case 4:		AValue |= 0b10000; break;
				default:	AValue |= 0b11000;
			}

			if( ! Alert.ActiveLow )
				AValue |= 0b100;

			if( Alert.InterruptMode )
				AValue |= 0b10;

			if( ShutDown )
				AValue |= 0b1;

//			Serial.println( AValue, BIN );

			WriteRegister8( CONFIG_REG, AValue );
		}

	public:
		void ReadData()
		{
			if( ! Enabled )
				 return;

			FWire.beginTransmission( FAddress );
			FWire.write( TEMPERATURE_REG );
			FWire.endTransmission();

			FWire.requestFrom( FAddress, uint8_t( 2 ));
			if (Wire.available() != 2 )
				return;

			int32_t AValue = FWire.read();
			AValue <<= 8;
			AValue |= Wire.read();

//			AValue = 0x7FFFFFFF;
//			AValue = 0x80000000;

//			AValue /= ( 16 << ( 3 - ( Resolution - 9 )));
			AValue /= 16;
//			Serial.println( AValue );

			float ATemp = AValue * 0.0625;
			if( InFahrenheit )
				ATemp = ATemp * ( 9.0/5.0 ) + 32.0;

//			AValue = ( ATemp / 0.0625 ) + 0.5;
//			AValue << 16;
//			Serial.println( AValue );
//			Serial.println( "" );

			OutputPin.Notify( &ATemp );
		}

	protected:
		void WriteRegister8( uint8_t ARegister, uint8_t AValue )
		{
			 FWire.beginTransmission( FAddress );
			 FWire.write( ARegister );
			 FWire.write( AValue );
			 FWire.endTransmission();
		}

		void WriteRegister16Temp( uint8_t ARegister, float ATemp )
		{
			int16_t AValue = ( ATemp / 0.0625 ) + 0.5;
			AValue << 16;
			AValue &= 0xFF80;
			WriteRegister16( ARegister, AValue );
		}

		void WriteRegister16( uint8_t ARegister, uint16_t AValue )
		{
			 FWire.beginTransmission( FAddress );
			 FWire.write( ARegister );
			 FWire.write( uint8_t( AValue >> 8 ) );
			 FWire.write( uint8_t( AValue ) );
			 FWire.endTransmission();
		}

	protected:
		virtual void SystemStart() override
		{
//			inherited::SystemStart();

			UpdateConfig();
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
#if( defined( _MITOV_MICROCHIP_TCN75A_CLOCK_PIN_ ) || defined( _MITOV_MICROCHIP_SHIELD_TCN75A_CLOCK_PIN_ ))
			if( ! ClockInputPin.IsConnected() )
#endif
				ReadData();

//			inherited::SystemLoopBegin( currentMicros );
		}

	public:
		MicrochipTCN75A( uint8_t AAddress, TwoWire &AWire ) :
			Enabled( true ),
			Resolution( 12 ),
			InFahrenheit( false ),
			ShutDown( false ),
			FAddress( AAddress ),
			FWire( AWire )
		{
#if( defined( _MITOV_MICROCHIP_TCN75A_CLOCK_PIN_ ) || defined( _MITOV_MICROCHIP_SHIELD_TCN75A_CLOCK_PIN_ ))
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&MicrochipTCN75A::DoClockReceive );
#endif
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
