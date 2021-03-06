////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_AK8963_h
#define _MITOV_COMPASS_AK8963_h

#include <Mitov.h>
#include <Wire.h> //I2C Arduino Library
#include <Mitov_Basic_I2C.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	enum CompassAK8963Mode { cmSingleMeasurement, cm8Hz, cm100Hz, cmExternalTrigger };
//---------------------------------------------------------------------------
	namespace CompassAK8963Const
	{
		static const byte CompassModes[] =
		{
			0b00000001,
			0b00000010,
			0b00000110,
			0b00000100
		};
	}
//---------------------------------------------------------------------------
	class CompassAK8963 : public OpenWire::Component
	{
		typedef	OpenWire::Component	inherited;

	protected:
		static const byte MPU9150_RA_MAG_ST1		= 0x02;
		static const byte MPU9150_RA_MAG_XOUT_L		= 0x03;
		static const byte MPU9150_RA_MAG_XOUT_H		= 0x04;
		static const byte MPU9150_RA_MAG_YOUT_L		= 0x05;
		static const byte MPU9150_RA_MAG_YOUT_H		= 0x06;
		static const byte MPU9150_RA_MAG_ZOUT_L		= 0x07;
		static const byte MPU9150_RA_MAG_ZOUT_H		= 0x08;
		static const byte MPU9150_RA_MAG_ST2		= 0x09;
		static const byte MPU9150_RA_MAG_CNTL1		= 0x0A;
		static const byte MPU9150_RA_MAG_CNTL2		= 0x0B;
		static const byte MPU9150_RA_MAG_ASTC		= 0x0C;

		static const byte MPU9150_RA_MAG_ASAX		= 0x10;
		static const byte MPU9150_RA_MAG_ASAY		= 0x11;
		static const byte MPU9150_RA_MAG_ASAZ		= 0x12;

	public:
		OpenWire::ConnectSinkPin	ClockInputPin;
		OpenWire::SourcePin			OutputPins[ 3 ];

		OpenWire::SinkPin			ResetInputPin;
		OpenWire::SourcePin			OverflowOutputPin;

	public:
		CompassAK8963Mode	Mode : 2;
		bool Enabled : 1;
		bool SelfTest : 1; // Added to save space as bitfield
		byte Address : 2;
		bool HighResolution : 1;

	protected:
		TwoWire &FWire;

	public:
		void SetMode( CompassAK8963Mode AValue )
		{
			if( Mode == AValue )
				return;

			Mode = AValue;
			UpdateCompassControlReg();
		}

		void SetEnabled( bool AValue )
		{
			if( Enabled == AValue )
				return;

			Enabled = AValue;
			UpdateCompassControlReg();
		}

		void SetSelfTest( bool AValue )
		{
			if( SelfTest == AValue )
				return;

			SelfTest = AValue;
			UpdateCompassSelfTestReg();
		}

		void SetHighResolution( bool AValue )
		{
			if( HighResolution == AValue )
				return;

			HighResolution = AValue;
			UpdateCompassControlReg();
		}

	protected:
		float CompassAdjustmentValues[ 3 ];

	protected:
		void PerformRead()
		{
			static const float CompassCoefficients[] =
			{
				10.0f *4219.0f / 8190.0f,
				10.0f *4219.0f / 32760.0f
			};

			uint8_t AIntValues[ 7 ];
			if( ReadBytes( MPU9150_RA_MAG_XOUT_L, 7, AIntValues ))
				for( int i = 0; i < 3; ++i )
				{
					int16_t AIntValue = (((int16_t)AIntValues[ i * 2 + 1 ] ) << 8 ) | AIntValues[ i * 2 ];
					float AValue = AIntValue * CompassCoefficients[ HighResolution & 1 ] * CompassAdjustmentValues[ i ];
					OutputPins[ i ].Notify( &AValue );
				}

		}

		void ReadSensors()
		{
			uint8_t AIntValue;

			if( Mode == cmSingleMeasurement )
			{
				UpdateCompassControlReg();
				delay(10);
				PerformRead();
			}

			if( ReadBytes( MPU9150_RA_MAG_ST1, 1, &AIntValue ))
			{
				OverflowOutputPin.SendValue( AIntValue & 0b00000010 );
				if( AIntValue & 0b00000001 )
					PerformRead();
			}

		}

		void UpdateCompassControlReg()
		{
			byte AValue;
			if( Enabled )
				AValue = CompassAK8963Const::CompassModes[ Mode ];

			else
				AValue = 0;

			AValue |=	( HighResolution	? 0b00010000 : 0 );

			WriteByte( MPU9150_RA_MAG_CNTL1, AValue );
		}

		void UpdateCompassSelfTestReg()
		{
			byte AValue = ( SelfTest	? 0b01000000 : 0 );
			WriteByte( MPU9150_RA_MAG_ASTC, AValue );
		}

		void ReagCompassAdjustmentValues()
		{
			uint8_t AValues[ 3 ];

			ReadBytes( MPU9150_RA_MAG_ASAX, sizeof( AValues ), AValues );
			for( int i = 0; i < 3; ++i )
				CompassAdjustmentValues[ i ] = (((float) AValues[ i ] ) - 128.0f) / 256.0f + 1.0f;

		}

	protected:
		inline void WriteByte( byte ARegister, byte AData )
		{
			I2C::WriteByte( FWire, Address + 0x0C, ARegister, AData );
		}

		bool ReadBytes( uint8_t regAddr, uint8_t length, void *data )
		{
			return I2C::ReadBytes( FWire, Address + 0x0C, regAddr, length, data );
		}

	protected:
		virtual void SystemStart() override
		{
			UpdateCompassControlReg();
			UpdateCompassSelfTestReg();
			ReagCompassAdjustmentValues();

//			inherited::SystemStart();
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{

			if( Enabled )
				if( ! ClockInputPin.IsConnected() )
					ReadSensors();

//			inherited::SystemLoopBegin( currentMicros );
		}

	protected:
		void DoClockReceive( void *_Data )
		{
			ReadSensors();
		}

		void DoResetReceive( void *_Data )
		{
			WriteByte( MPU9150_RA_MAG_CNTL2, 0b00000001 );
		}

	public:
		CompassAK8963( TwoWire &AWire ) :
			Mode( cm100Hz ),
			Enabled( true ),
			SelfTest( false ),
			HighResolution( true ),
			Address( 0 ),
			FWire( AWire )
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&CompassAK8963::DoClockReceive );
			ResetInputPin.SetCallback( MAKE_CALLBACK( CompassAK8963::DoResetReceive ));
		}
	};
//---------------------------------------------------------------------------
	class CompassAK8963I2C : public Mitov::CompassAK8963
	{
		typedef	Mitov::CompassAK8963	inherited;
	};
//---------------------------------------------------------------------------
}

#endif
