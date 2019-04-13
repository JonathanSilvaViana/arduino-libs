////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_BNO055_h
#define _MITOV_BNO055_h

#include <Mitov.h>
#include <Wire.h> //I2C Arduino Library
#include <Mitov_Basic_I2C.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	namespace BNO055
	{
		const uint8_t CAdresses[ 2 ] = { 0x28, 0x29 };
	}
//---------------------------------------------------------------------------
	namespace TArduinoBNO0Mode
	{
		enum Type
		{
			Accelerometer,
			Compass,
			Gyroscope,
			AccelerometerCompass,
			AccelerometerGyroscope,
			GyroscopeCompass,
			AccelerometerGyroscopeCompass,

			FusionAccelerometerGyroscopeRelative,
			FusionAccelerometerCompassAbsolute,
			FusionAccelerometerCompassRelative,

			FusionFull,
			FusionFullCalibrated,

			Auto
		};

	}
//---------------------------------------------------------------------------
	namespace TArduinoBNO0PowerMode
	{
		enum Type { Normal, LowPower, Suspend };
	}
//---------------------------------------------------------------------------
	namespace TArduinoBNO0AxisOrientation
	{
		enum Type 
		{ 
			X = 0b000,
			Y = 0b001,
			Z = 0b010,
			
			InvertedX = 0b100,
			InvertedY = 0b101,
			InvertedZ = 0b110
		};
	}
//---------------------------------------------------------------------------
	class TArduinoBNO0Orientation
	{
	public:
		TArduinoBNO0AxisOrientation::Type	X : 3;
		TArduinoBNO0AxisOrientation::Type	Y : 3;
		bool	InvertZ : 1;

	public:
		TArduinoBNO0Orientation() :
			X( TArduinoBNO0AxisOrientation::X ),
			Y( TArduinoBNO0AxisOrientation::Y ),
			InvertZ( false )
		{
		}

	};
//---------------------------------------------------------------------------
	class TArduinoBNO055Thermometer
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool	Enabled : 1;
		bool	FromGyroscope : 1;
		bool	InFahrenheit : 1;

	public:
		TArduinoBNO055Thermometer() :
			Enabled( true ),
			FromGyroscope( false ),
			InFahrenheit( false )
		{
		}

	};
//---------------------------------------------------------------------------
	class TArduinoBNO055Offsets
	{
	public:
		float	X = 0;
		float	Y = 0;
		float	Z = 0;
	};
//---------------------------------------------------------------------------
	namespace TArduinoBNO0AccelerometerMode
	{
		enum Type
		{
			Normal,
			Suspend,
			LowPower1,
			Standby,
			LowPower2,
			DeepSuspend
		};
	}
//---------------------------------------------------------------------------
	class TArduinoBNO055Accelerometer
	{
	public:
		OpenWire::SourcePin	OutputPins[ 3 ];

	public:
		uint8_t	Range : 4;
		TArduinoBNO0AccelerometerMode::Type	Mode : 3;
		bool	Enabled : 1;

		float	Bandwidth = 62.5;

#ifdef _MITOV_BNO055_ACCELEROMETER_OFFSETS_
		TArduinoBNO055Offsets	Offsets;
#endif

	public:
		void SendData( int16_t *AIntData )
		{
			for( int i = 0; i < 3; ++ i)
				OutputPins[ i ].SendValue<float>( float( AIntData[ i ] ) / 100.0 ); // in m/s^2
		}

	public:
		TArduinoBNO055Accelerometer() :
			Range( 4 ),
			Mode( TArduinoBNO0AccelerometerMode::Normal ),
			Enabled( true )
		{
		}

	};
//---------------------------------------------------------------------------
	namespace TArduinoBNO0GyroscopeMode
	{
		enum Type
		{
			Normal,
			FastPowerUp,
			DeepSuspend,
			Suspend,
			AdvancedPowersave
		};
	}
//---------------------------------------------------------------------------
	class TArduinoBNO055Gyroscope
	{
	public:
		OpenWire::SourcePin	OutputPins[ 3 ];

	public:
		uint16_t	Range : 11; // 2000 max
		uint16_t	Bandwidth : 9; // 523 max
		TArduinoBNO0GyroscopeMode::Type	Mode : 3;
		bool	Enabled : 1;

#ifdef _MITOV_BNO055_GYROSCOPE_OFFSETS_
		TArduinoBNO055Offsets	Offsets;
#endif

	public:
		void SendData( int16_t *AIntData )
		{
			for( int i = 0; i < 3; ++ i)
				OutputPins[ i ].SendValue<float>( float( AIntData[ i ] ) / 16.0 ); // in Dps
		}

	public:
		TArduinoBNO055Gyroscope() :
			Range( 2000 ),
			Bandwidth( 32 ),
			Mode( TArduinoBNO0GyroscopeMode::Normal ),
			Enabled( true )
		{
		}

	};
//---------------------------------------------------------------------------
	namespace TArduinoBNO055CompassOperationMode
	{
		enum Type
		{
			LowPower,
			Regular,
			EnhancedRegular,
			HighAccuracy
		};
	}
//---------------------------------------------------------------------------
	namespace TArduinoBNO055CompassPowerMode
	{
		enum Type
		{
			Normal,
			Sleep,
			Suspend,
			ForceMode
		};
	}
//---------------------------------------------------------------------------
	class TArduinoBNO055Compass
	{
	public:
		OpenWire::SourcePin	OutputPins[ 3 ];

	public:
		uint8_t	Rate : 5; // max 32
		TArduinoBNO055CompassOperationMode::Type	OperationMode : 2;
		TArduinoBNO055CompassPowerMode::Type	PowerMode : 2;
		bool	Enabled : 1;

#ifdef _MITOV_BNO055_COMPASS_OFFSETS_
		TArduinoBNO055Offsets	Offsets;
#endif

	public:
		void SendData( int16_t *AIntData )
		{
			for( int i = 0; i < 3; ++ i)
				OutputPins[ i ].SendValue<float>( float( AIntData[ i ] ) / 16.0 ); // in uT
		}

	public:
		TArduinoBNO055Compass() :
			Rate( 20 ),
			OperationMode( TArduinoBNO055CompassOperationMode::Regular ),
			PowerMode( TArduinoBNO055CompassPowerMode::Normal ),
			Enabled( true )
		{
		}

	};
//---------------------------------------------------------------------------
	class TArduinoBNO055Fused
	{
	public:
		OpenWire::SourcePin	OutputPins[ 3 ];

	public:
		bool	Enabled : 1;
		bool	Calibration : 1;

	public:
		void SendData( int16_t *AIntData )
		{
			for( int i = 0; i < 3; ++ i)
				OutputPins[ i ].SendValue<float>( float( AIntData[ i ] ) / 16.0 ); // in m/s^2
		}

	public:
		TArduinoBNO055Fused() :
			Enabled( true ),
			Calibration( true )
		{
		}

	};
//---------------------------------------------------------------------------
	class BNO055I2C : public OpenWire::Object
	{
		typedef	OpenWire::Object	inherited;

	protected:
		static const uint8_t BNO055_ID = 0xA0;

    enum ERegisters
    {
      // Page id register definition
      BNO055_PAGE_ID_ADDR                                     = 0X07,

      // PAGE0 REGISTER DEFINITION START
      BNO055_CHIP_ID_ADDR                                     = 0x00,
      BNO055_ACCEL_REV_ID_ADDR                                = 0x01,
      BNO055_MAG_REV_ID_ADDR                                  = 0x02,
      BNO055_GYRO_REV_ID_ADDR                                 = 0x03,
      BNO055_SW_REV_ID_LSB_ADDR                               = 0x04,
      BNO055_SW_REV_ID_MSB_ADDR                               = 0x05,
      BNO055_BL_REV_ID_ADDR                                   = 0X06,

      // Accel data register
      BNO055_ACCEL_DATA_X_LSB_ADDR                            = 0X08,
      BNO055_ACCEL_DATA_X_MSB_ADDR                            = 0X09,
      BNO055_ACCEL_DATA_Y_LSB_ADDR                            = 0X0A,
      BNO055_ACCEL_DATA_Y_MSB_ADDR                            = 0X0B,
      BNO055_ACCEL_DATA_Z_LSB_ADDR                            = 0X0C,
      BNO055_ACCEL_DATA_Z_MSB_ADDR                            = 0X0D,

      // Mag data register
      BNO055_MAG_DATA_X_LSB_ADDR                              = 0X0E,
      BNO055_MAG_DATA_X_MSB_ADDR                              = 0X0F,
      BNO055_MAG_DATA_Y_LSB_ADDR                              = 0X10,
      BNO055_MAG_DATA_Y_MSB_ADDR                              = 0X11,
      BNO055_MAG_DATA_Z_LSB_ADDR                              = 0X12,
      BNO055_MAG_DATA_Z_MSB_ADDR                              = 0X13,

      // Gyro data registers
      BNO055_GYRO_DATA_X_LSB_ADDR                             = 0X14,
      BNO055_GYRO_DATA_X_MSB_ADDR                             = 0X15,
      BNO055_GYRO_DATA_Y_LSB_ADDR                             = 0X16,
      BNO055_GYRO_DATA_Y_MSB_ADDR                             = 0X17,
      BNO055_GYRO_DATA_Z_LSB_ADDR                             = 0X18,
      BNO055_GYRO_DATA_Z_MSB_ADDR                             = 0X19,

      // Euler data registers
      BNO055_EULER_H_LSB_ADDR                                 = 0X1A,
      BNO055_EULER_H_MSB_ADDR                                 = 0X1B,
      BNO055_EULER_R_LSB_ADDR                                 = 0X1C,
      BNO055_EULER_R_MSB_ADDR                                 = 0X1D,
      BNO055_EULER_P_LSB_ADDR                                 = 0X1E,
      BNO055_EULER_P_MSB_ADDR                                 = 0X1F,

      // Quaternion data registers
      BNO055_QUATERNION_DATA_W_LSB_ADDR                       = 0X20,
      BNO055_QUATERNION_DATA_W_MSB_ADDR                       = 0X21,
      BNO055_QUATERNION_DATA_X_LSB_ADDR                       = 0X22,
      BNO055_QUATERNION_DATA_X_MSB_ADDR                       = 0X23,
      BNO055_QUATERNION_DATA_Y_LSB_ADDR                       = 0X24,
      BNO055_QUATERNION_DATA_Y_MSB_ADDR                       = 0X25,
      BNO055_QUATERNION_DATA_Z_LSB_ADDR                       = 0X26,
      BNO055_QUATERNION_DATA_Z_MSB_ADDR                       = 0X27,

      // Linear acceleration data registers
      BNO055_LINEAR_ACCEL_DATA_X_LSB_ADDR                     = 0X28,
      BNO055_LINEAR_ACCEL_DATA_X_MSB_ADDR                     = 0X29,
      BNO055_LINEAR_ACCEL_DATA_Y_LSB_ADDR                     = 0X2A,
      BNO055_LINEAR_ACCEL_DATA_Y_MSB_ADDR                     = 0X2B,
      BNO055_LINEAR_ACCEL_DATA_Z_LSB_ADDR                     = 0X2C,
      BNO055_LINEAR_ACCEL_DATA_Z_MSB_ADDR                     = 0X2D,

      // Gravity data registers
      BNO055_GRAVITY_DATA_X_LSB_ADDR                          = 0X2E,
      BNO055_GRAVITY_DATA_X_MSB_ADDR                          = 0X2F,
      BNO055_GRAVITY_DATA_Y_LSB_ADDR                          = 0X30,
      BNO055_GRAVITY_DATA_Y_MSB_ADDR                          = 0X31,
      BNO055_GRAVITY_DATA_Z_LSB_ADDR                          = 0X32,
      BNO055_GRAVITY_DATA_Z_MSB_ADDR                          = 0X33,

      // Temperature data register
      BNO055_TEMP_ADDR                                        = 0X34,

      // Status registers
      BNO055_CALIB_STAT_ADDR                                  = 0X35,
      BNO055_SELFTEST_RESULT_ADDR                             = 0X36,
      BNO055_INTR_STAT_ADDR                                   = 0X37,

      BNO055_SYS_CLK_STAT_ADDR                                = 0X38,
      BNO055_SYS_STAT_ADDR                                    = 0X39,
      BNO055_SYS_ERR_ADDR                                     = 0X3A,

      // Unit selection register
      BNO055_UNIT_SEL_ADDR                                    = 0X3B,
      BNO055_DATA_SELECT_ADDR                                 = 0X3C,

      // Mode registers
      BNO055_OPR_MODE_ADDR                                    = 0X3D,
      BNO055_PWR_MODE_ADDR                                    = 0X3E,

      BNO055_SYS_TRIGGER_ADDR                                 = 0X3F,
      BNO055_TEMP_SOURCE_ADDR                                 = 0X40,

      // Axis remap registers
      BNO055_AXIS_MAP_CONFIG_ADDR                             = 0X41,
      BNO055_AXIS_MAP_SIGN_ADDR                               = 0X42,

      // SIC registers
      BNO055_SIC_MATRIX_0_LSB_ADDR                            = 0X43,
      BNO055_SIC_MATRIX_0_MSB_ADDR                            = 0X44,
      BNO055_SIC_MATRIX_1_LSB_ADDR                            = 0X45,
      BNO055_SIC_MATRIX_1_MSB_ADDR                            = 0X46,
      BNO055_SIC_MATRIX_2_LSB_ADDR                            = 0X47,
      BNO055_SIC_MATRIX_2_MSB_ADDR                            = 0X48,
      BNO055_SIC_MATRIX_3_LSB_ADDR                            = 0X49,
      BNO055_SIC_MATRIX_3_MSB_ADDR                            = 0X4A,
      BNO055_SIC_MATRIX_4_LSB_ADDR                            = 0X4B,
      BNO055_SIC_MATRIX_4_MSB_ADDR                            = 0X4C,
      BNO055_SIC_MATRIX_5_LSB_ADDR                            = 0X4D,
      BNO055_SIC_MATRIX_5_MSB_ADDR                            = 0X4E,
      BNO055_SIC_MATRIX_6_LSB_ADDR                            = 0X4F,
      BNO055_SIC_MATRIX_6_MSB_ADDR                            = 0X50,
      BNO055_SIC_MATRIX_7_LSB_ADDR                            = 0X51,
      BNO055_SIC_MATRIX_7_MSB_ADDR                            = 0X52,
      BNO055_SIC_MATRIX_8_LSB_ADDR                            = 0X53,
      BNO055_SIC_MATRIX_8_MSB_ADDR                            = 0X54,

      // Accelerometer Offset registers
      ACCEL_OFFSET_X_LSB_ADDR                                 = 0X55,
      ACCEL_OFFSET_X_MSB_ADDR                                 = 0X56,
      ACCEL_OFFSET_Y_LSB_ADDR                                 = 0X57,
      ACCEL_OFFSET_Y_MSB_ADDR                                 = 0X58,
      ACCEL_OFFSET_Z_LSB_ADDR                                 = 0X59,
      ACCEL_OFFSET_Z_MSB_ADDR                                 = 0X5A,

      // Magnetometer Offset registers
      MAG_OFFSET_X_LSB_ADDR                                   = 0X5B,
      MAG_OFFSET_X_MSB_ADDR                                   = 0X5C,
      MAG_OFFSET_Y_LSB_ADDR                                   = 0X5D,
      MAG_OFFSET_Y_MSB_ADDR                                   = 0X5E,
      MAG_OFFSET_Z_LSB_ADDR                                   = 0X5F,
      MAG_OFFSET_Z_MSB_ADDR                                   = 0X60,

      // Gyroscope Offset registers
      GYRO_OFFSET_X_LSB_ADDR                                  = 0X61,
      GYRO_OFFSET_X_MSB_ADDR                                  = 0X62,
      GYRO_OFFSET_Y_LSB_ADDR                                  = 0X63,
      GYRO_OFFSET_Y_MSB_ADDR                                  = 0X64,
      GYRO_OFFSET_Z_LSB_ADDR                                  = 0X65,
      GYRO_OFFSET_Z_MSB_ADDR                                  = 0X66,

      // Radius registers
      ACCEL_RADIUS_LSB_ADDR                                   = 0X67,
      ACCEL_RADIUS_MSB_ADDR                                   = 0X68,
      MAG_RADIUS_LSB_ADDR                                     = 0X69,
      MAG_RADIUS_MSB_ADDR                                     = 0X6A,

      // PAGE1 REGISTER DEFINITION START
      ACC_CONFIG_ADDR										  = 0X08,
      MAG_CONFIG_ADDR										  = 0X09,
      GYR_CONFIG_0_ADDR										  = 0X0A,
      GYR_CONFIG_1_ADDR										  = 0X0B
    };

	enum EOperationModes
    {
      /* Operation mode settings*/
      OPERATION_MODE_CONFIG                                   = 0X00,
      OPERATION_MODE_ACCONLY                                  = 0X01,
      OPERATION_MODE_MAGONLY                                  = 0X02,
      OPERATION_MODE_GYRONLY                                  = 0X03,
      OPERATION_MODE_ACCMAG                                   = 0X04,
      OPERATION_MODE_ACCGYRO                                  = 0X05,
      OPERATION_MODE_MAGGYRO                                  = 0X06,
      OPERATION_MODE_AMG                                      = 0X07,
      OPERATION_MODE_IMUPLUS                                  = 0X08,
      OPERATION_MODE_COMPASS                                  = 0X09,
      OPERATION_MODE_M4G                                      = 0X0A,
      OPERATION_MODE_NDOF_FMC_OFF                             = 0X0B,
      OPERATION_MODE_NDOF                                     = 0X0C
    };

    enum EPowerMode
    {
      POWER_MODE_NORMAL                                       = 0X00,
      POWER_MODE_LOWPOWER                                     = 0X01,
      POWER_MODE_SUSPEND                                      = 0X02
    };


	public:
#ifdef _MITOV_BNO055_CLOCK_PIN_
		OpenWire::ConnectSinkPin	ClockInputPin;
#endif

#ifdef _MITOV_BNO055_RESET_PIN_
		OpenWire::SinkPin			ResetInputPin;
#endif

	public:
#ifdef _MITOV_BNO055_ORIENTATION_
		TArduinoBNO0Orientation	Orientation;
#endif

#ifdef _MITOV_BNO055_THERMOMETER_
		TArduinoBNO055Thermometer	Thermometer;
#endif

#ifdef _MITOV_BNO055_ACCELEROMETER_
		TArduinoBNO055Accelerometer	Accelerometer;
#endif

#ifdef _MITOV_BNO055_GYROSCOPE_
		TArduinoBNO055Gyroscope	Gyroscope;
#endif

#ifdef _MITOV_BNO055_COMPASS_
		TArduinoBNO055Compass	Compass;
#endif

#ifdef _MITOV_BNO055_FUSED_
		TArduinoBNO055Fused	Fused;
#endif

	public:
		bool	Enabled : 1;
		bool	Address : 1;
		bool	ExternalOscillator : 1;

		TArduinoBNO0Mode::Type	Mode : 4;
		TArduinoBNO0PowerMode::Type	PowerMode : 2;

	public:
		void UpdatePowerMode()
		{
			WriteTo( BNO055_OPR_MODE_ADDR, OPERATION_MODE_CONFIG );
			UpdatePowerModeInternal();
			UpdateMode();
		}

#ifdef _MITOV_BNO055_ORIENTATION_
		void UpdateOrientation()
		{
			WriteTo( BNO055_OPR_MODE_ADDR, OPERATION_MODE_CONFIG );
			UpdateOrientationInternal();
			UpdateMode();
		}
#endif

		void UpdateMode()
		{
			uint8_t	AValue;
			if( Mode != TArduinoBNO0Mode::Auto )
				AValue = Mode;

			else
			{
				AValue = 0;
				bool ACompas = false;
				bool AGyroscope = false;
				bool AAccelerometer = false;
				bool AFused	= false;
				bool ACalibration = false;
				bool ARelative = false;

#ifdef _MITOV_BNO055_ACCELEROMETER_
				AAccelerometer = Accelerometer.Enabled;
#endif

#ifdef _MITOV_BNO055_GYROSCOPE_
				AGyroscope = Gyroscope.Enabled;
#endif

#ifdef _MITOV_BNO055_COMPASS_
				ACompas = Compass.Enabled;
#endif

#ifdef _MITOV_BNO055_FUSED_
				if( Fused.Enabled )
				{
					AFused = true;
					ACalibration = Fused.Calibration;
				}
#endif
				if( AFused )
				{
					if( ACompas )
					{
						if( AGyroscope )
						{
							if( ACalibration )
								AValue = TArduinoBNO0Mode::FusionFullCalibrated;

							else
								AValue = TArduinoBNO0Mode::FusionFull;
						}
						else
						{
							if( ARelative )
								AValue = TArduinoBNO0Mode::FusionAccelerometerCompassRelative;

							else
								AValue = TArduinoBNO0Mode::FusionAccelerometerCompassAbsolute;
						}
					}

					else
						AValue = TArduinoBNO0Mode::FusionAccelerometerGyroscopeRelative;
				}
				else
				{
//					Serial.println( "TEST1" );
					if( AAccelerometer )
					{
						if( ACompas )
						{
							if( AGyroscope )
								AValue = TArduinoBNO0Mode::AccelerometerGyroscopeCompass;

							else
								AValue = TArduinoBNO0Mode::AccelerometerCompass;
						}

						else
						{
							if( AGyroscope )
								AValue = TArduinoBNO0Mode::AccelerometerGyroscope;

							else
								AValue = TArduinoBNO0Mode::Accelerometer;
						}
					}
					else
					{
						if( ACompas )
						{
							if( AGyroscope )
								AValue = TArduinoBNO0Mode::GyroscopeCompass;

							else
								AValue = TArduinoBNO0Mode::Compass;
						}
						else
						{
							if( AGyroscope )
								AValue = TArduinoBNO0Mode::Gyroscope;

							else
								AValue = TArduinoBNO0Mode::Compass; // The default mode
						}
					}
				}
			}

//			Serial.println( "OPR_MOD" );
//			Serial.println( AValue + 1, HEX );

			WriteTo( BNO055_OPR_MODE_ADDR, AValue + 1 ); //OPERATION_MODE_NDOF ); // OPERATION_MODE_NDOF
		}

		void UpdateTemperatureSource()
		{
			WriteTo( BNO055_OPR_MODE_ADDR, OPERATION_MODE_CONFIG );
			UpdateTemperatureSourceInternal();
			UpdateMode();
		}

		void UpdateAccelerometerMode()
		{
			WriteTo( BNO055_OPR_MODE_ADDR, OPERATION_MODE_CONFIG );
			UpdateAccelerometerModeInternal();
			UpdateMode();
		}

		void UpdateAccelerometerOffsets()
		{
			WriteTo( BNO055_OPR_MODE_ADDR, OPERATION_MODE_CONFIG );
			UpdateAccelerometerOffsetsInternal();
			UpdateMode();
		}

		void UpdateGyroscopeOffsets()
		{
			WriteTo( BNO055_OPR_MODE_ADDR, OPERATION_MODE_CONFIG );
			UpdateGyroscopeOffsetsInternal();
			UpdateMode();
		}

		void UpdateCompassOffsets()
		{
			WriteTo( BNO055_OPR_MODE_ADDR, OPERATION_MODE_CONFIG );
			UpdateCompassOffsetsInternal();
			UpdateMode();
		}

		void UpdateGyroscopeMode()
		{
			WriteTo( BNO055_OPR_MODE_ADDR, OPERATION_MODE_CONFIG );
			UpdateGyroscopeModeInternal();
			UpdateMode();
		}

		void UpdateCompassMode()
		{
			WriteTo( BNO055_OPR_MODE_ADDR, OPERATION_MODE_CONFIG );
			UpdateCompassModeInternal();
			UpdateMode();
		}

		void UpdateInits()
		{
			uint8_t AValue = 0; //0b00010000; // Temp always in C
			// TODO: Add other units
			WriteTo( BNO055_UNIT_SEL_ADDR, AValue );
		}

	protected:
		TwoWire &FWire;

	protected:
		void DoClockReceive( void *_Data )
		{
			ReadSensors();
		}

		void DoResetReceive( void *_Data )
		{
			Reset();
//			UpdatePowerManagementReg1( true );
		}

	protected:
		void UpdatePowerModeInternal()
		{
			WriteTo( BNO055_PWR_MODE_ADDR, PowerMode );
			delay(10);
		}

		void UpdateAccelerometerOffsetsInternal()
		{
#ifdef _MITOV_BNO055_ACCELEROMETER_OFFSETS_
			int16_t	ABuffer[ 3 ];
			ABuffer[ 0 ] = Accelerometer.Offsets.X * 100 + 0.5;
			ABuffer[ 1 ] = Accelerometer.Offsets.Y * 100 + 0.5;
			ABuffer[ 2 ] = Accelerometer.Offsets.Z * 100 + 0.5;

			WriteVector( ACCEL_OFFSET_X_LSB_ADDR, ABuffer );
#endif
		}

		void UpdateGyroscopeOffsetsInternal()
		{
#ifdef _MITOV_BNO055_GYROSCOPE_OFFSETS_
			int16_t	ABuffer[ 3 ];
			ABuffer[ 0 ] = Gyroscope.Offsets.X * 16 + 0.5;
			ABuffer[ 1 ] = Gyroscope.Offsets.Y * 16 + 0.5;
			ABuffer[ 2 ] = Gyroscope.Offsets.Z * 16 + 0.5;

			WriteVector( GYRO_OFFSET_X_LSB_ADDR, ABuffer );
#endif
		}

		void UpdateCompassOffsetsInternal()
		{
#ifdef _MITOV_BNO055_COMPASS_OFFSETS_
			int16_t	ABuffer[ 3 ];
			ABuffer[ 0 ] = Compass.Offsets.X * 16 + 0.5;
			ABuffer[ 1 ] = Compass.Offsets.Y * 16 + 0.5;
			ABuffer[ 2 ] = Compass.Offsets.Z * 16 + 0.5;

			WriteVector( MAG_OFFSET_X_LSB_ADDR, ABuffer );
#endif
		}

		void UpdateOrientationInternal()
		{
#ifdef _MITOV_BNO055_ORIENTATION_
			uint8_t	AXMap = Orientation.X & 0b11;
			uint8_t	AYMap = Orientation.Y & 0b11;
			uint8_t	AZMap;
			for( AZMap = 0; AZMap < 3; ++AZMap )
			{
				if( AZMap == AXMap )
					continue;

				if( AZMap == AYMap )
					continue;

				break;
			}

			uint8_t	AValue = AXMap |
				( AYMap << 2 ) |
				( AZMap << 4 );

			WriteTo( BNO055_AXIS_MAP_CONFIG_ADDR, AValue );

			AValue = 0;
			if( Orientation.X & 0b100 )
				AValue |= 0b001;

			if( Orientation.Y & 0b100 )
				AValue |= 0b010;

			if( Orientation.InvertZ )
				AValue |= 0b100;

			WriteTo( BNO055_AXIS_MAP_SIGN_ADDR, AValue );

#else
			WriteTo( BNO055_AXIS_MAP_CONFIG_ADDR, 0x24 );
			WriteTo( BNO055_AXIS_MAP_SIGN_ADDR, 0 );
#endif
		}

		void UpdateCompassModeInternal()
		{
/*
			WriteTo(BNO055_PAGE_ID_ADDR, 1);
			Serial.println( "MAG_CONFIG" );
			Serial.println( ReadByte( MAG_CONFIG_ADDR ), HEX );
//			WriteTo( MAG_CONFIG_ADDR, 0x6D );
////			WriteTo( MAG_CONFIG_ADDR, 0x6D );
			WriteTo( MAG_CONFIG_ADDR, 0xB );
			WriteTo(BNO055_PAGE_ID_ADDR, 0);
			return;
*/
			WriteTo(BNO055_PAGE_ID_ADDR, 1);
#ifdef _MITOV_BNO055_COMPASS_
			static const uint8_t CRate[] =
			{
				2,
				6,
				8,
				10,
				15,
				20,
				25,
				30
			};

			uint8_t ARateOrder = MITOV_ARRAY_SIZE( CRate ) - 1;
			for( int i = MITOV_ARRAY_SIZE( CRate ); i--; )
			{
				if( Compass.Rate > CRate[ i ] )
					break;

				ARateOrder = i;

				if( Compass.Rate == CRate[ i ] )
					break;

			}

			uint8_t AValue = ARateOrder | ( uint8_t( Compass.OperationMode ) << 3 ) | ( uint8_t( Compass.PowerMode ) << 5 );

//			Serial.println( "MAG_CONFIG" );
//			Serial.println( AValue, HEX );

			WriteTo( MAG_CONFIG_ADDR, AValue );

#else
			WriteTo( MAG_CONFIG_ADDR, 0x6D );
#endif
			WriteTo(BNO055_PAGE_ID_ADDR, 0);
		}

		void UpdateGyroscopeModeInternal()
		{
			WriteTo(BNO055_PAGE_ID_ADDR, 1);
#ifdef _MITOV_BNO055_GYROSCOPE_
			static const uint16_t CRange[] =
			{
				125,
				250,
				500,
				1000,
				2000
			};

			uint8_t ARangeOrder = MITOV_ARRAY_SIZE( CRange ) - 1;
			for( int i = MITOV_ARRAY_SIZE( CRange ); i--; )
			{
				if( Gyroscope.Range > CRange[ i ] )
					break;

				ARangeOrder = i;

				if( Gyroscope.Range == CRange[ i ] )
					break;

			}

			static const uint16_t CBandwidth[] =
			{
				12,
				23,

				32,

				47,

				64,

				116,
				230,
				523
			};

			static const uint8_t CBandwidthOrders[] =
			{
				0b00101000, // 12
				0b00100000, // 23

				0b00111000, // 32

				0b00011000, // 47

				0b00110000, // 64

				0b00010000, // 116
				0b00001000, // 230
				0b00000000  // 523
			};

			uint8_t ABandwidthOrder = MITOV_ARRAY_SIZE( CBandwidth ) - 1;
			for( int i = MITOV_ARRAY_SIZE( CBandwidth ); i--; )
			{
				if( Gyroscope.Bandwidth > CBandwidth[ i ] )
					break;

				ABandwidthOrder = i;

				if( Gyroscope.Bandwidth == CBandwidth[ i ] )
					break;

			}

			uint8_t AValue = ARangeOrder | CBandwidthOrders[ ABandwidthOrder ];
			WriteTo( GYR_CONFIG_0_ADDR, AValue );

			WriteTo( GYR_CONFIG_1_ADDR, uint8_t( Gyroscope.Mode ) );
#else
			WriteTo( GYR_CONFIG_0_ADDR, 0x38 );
			WriteTo( GYR_CONFIG_1_ADDR, 0x00 );
#endif
			WriteTo(BNO055_PAGE_ID_ADDR, 0);
		}

		void UpdateAccelerometerModeInternal()
		{
			WriteTo(BNO055_PAGE_ID_ADDR, 1);
#ifdef _MITOV_BNO055_ACCELEROMETER_
			static const float CBandwidth[] =
			{
				7.81,
				15.63,
				31.25,
				62.5,
				125,
				250,
				500,
				1000
			};

			uint8_t ABandwidthOrder = MITOV_ARRAY_SIZE( CBandwidth ) - 1;
			for( int i = MITOV_ARRAY_SIZE( CBandwidth ); i--; )
			{
				if( Accelerometer.Bandwidth > CBandwidth[ i ] )
					break;

				ABandwidthOrder = i;

				if( Accelerometer.Bandwidth == CBandwidth[ i ] )
					break;

			}

			uint8_t ARangeOrder = Mitov::MitovIlog2( Accelerometer.Range ) - 1;

			uint8_t AValue = ARangeOrder | ( ABandwidthOrder << 2 ) | ( uint8_t( Accelerometer.Mode ) << 5 );
			WriteTo( ACC_CONFIG_ADDR, AValue );
#else
			WriteTo( ACC_CONFIG_ADDR, 0x0D );
#endif
			WriteTo(BNO055_PAGE_ID_ADDR, 0);
		}

		void UpdateTemperatureSourceInternal()
		{
#ifdef _MITOV_BNO055_THERMOMETER_
			WriteTo( BNO055_TEMP_SOURCE_ADDR, Thermometer.FromGyroscope ? 1 : 0 );
#endif
		}

	protected:
		void Reset()
		{
			WriteTo( BNO055_OPR_MODE_ADDR, OPERATION_MODE_CONFIG );

			WriteTo(BNO055_SYS_TRIGGER_ADDR, 0x20);
			while (ReadByte(BNO055_CHIP_ID_ADDR) != BNO055_ID)
				delay(10);

			delay(50);

// Set to normal power mode
			WriteTo( BNO055_PWR_MODE_ADDR, POWER_MODE_NORMAL );
			delay(10);

			WriteTo(BNO055_PAGE_ID_ADDR, 0);

// Set the output units
/*
uint8_t unitsel = (0 << 7) | // Orientation = Android
                (0 << 4) | // Temperature = Celsius
                (0 << 2) | // Euler = Degrees
                (1 << 1) | // Gyro = Rads
                (0 << 0);  // Accelerometer = m/s^2
write8(BNO055_UNIT_SEL_ADDR, unitsel);
*/

// Configure axis mapping (see section 3.4)
/*
write8(BNO055_AXIS_MAP_CONFIG_ADDR, REMAP_CONFIG_P2); // P0-P7, Default is P1
delay(10);
write8(BNO055_AXIS_MAP_SIGN_ADDR, REMAP_SIGN_P2); // P0-P7, Default is P1
delay(10);
*/
  
			WriteTo(BNO055_SYS_TRIGGER_ADDR, 0x0);
			delay(10);

			UpdateInits();
			if( ExternalOscillator )
				WriteTo( BNO055_SYS_TRIGGER_ADDR, 0x80 );

			UpdateTemperatureSourceInternal();
			UpdatePowerModeInternal();
			UpdateOrientationInternal();
			UpdateAccelerometerModeInternal();
			UpdateGyroscopeModeInternal();
			UpdateCompassModeInternal();
			UpdateAccelerometerOffsetsInternal();
			UpdateGyroscopeOffsetsInternal();
			UpdateCompassOffsetsInternal();
			UpdateMode();

//			Serial.println( "Reset Completed" );
		}

		void ReadSensors()
		{
//			Serial.println( "ReadSensors" );
#ifdef _MITOV_BNO055_THERMOMETER_
			if( Thermometer.Enabled )
			{
				float AValue = (int8_t)ReadByte(BNO055_TEMP_ADDR);
				if( Thermometer.InFahrenheit )
					AValue = Func::ConvertCtoF( AValue );

				Thermometer.OutputPin.Notify( &AValue );
			}
#endif // _MITOV_BNO055_THERMOMETER_

#ifdef _MITOV_BNO055_ACCELEROMETER_
			if( GetAccelerometerDataAvailable() )
			{
				int16_t AIntData[ 6 ];
				ReadVector( BNO055_ACCEL_DATA_X_LSB_ADDR, AIntData );
				Accelerometer.SendData( AIntData );
			}
#endif

#ifdef _MITOV_BNO055_GYROSCOPE_
			if( GetGyroscopeDataAvailable() )
			{
				int16_t AIntData[ 6 ];
				ReadVector( BNO055_GYRO_DATA_X_LSB_ADDR, AIntData );
				Gyroscope.SendData( AIntData );
			}
#endif

#ifdef _MITOV_BNO055_COMPASS_
			if( GetCompassDataAvailable() )
			{
				int16_t AIntData[ 6 ];
				ReadVector( BNO055_MAG_DATA_X_LSB_ADDR, AIntData );
				Compass.SendData( AIntData );
			}
#endif

#ifdef _MITOV_BNO055_FUSED_
			if( GetFusedDataAvailable() )
			{
				int16_t AIntData[ 6 ];
				ReadVector( BNO055_EULER_H_LSB_ADDR, AIntData );
				Fused.SendData( AIntData );
			}
#endif
		}

	protected:
		bool GetAccelerometerDataAvailable()
		{
#ifdef _MITOV_BNO055_ACCELEROMETER_
			if( Mode == TArduinoBNO0Mode::Auto )
				return Accelerometer.Enabled;
#endif
			if( Mode == TArduinoBNO0Mode::Accelerometer )
				return true;

			if( Mode == TArduinoBNO0Mode::AccelerometerCompass )
				return true;

			if( Mode == TArduinoBNO0Mode::AccelerometerGyroscope )
				return true;

			if( Mode >= TArduinoBNO0Mode::AccelerometerGyroscopeCompass )
				return true;

			return false;
		}

		bool GetGyroscopeDataAvailable()
		{
#ifdef _MITOV_BNO055_GYROSCOPE_
			if( Mode == TArduinoBNO0Mode::Auto )
				return Gyroscope.Enabled;
#endif
			if( Mode == TArduinoBNO0Mode::Gyroscope )
				return true;

			if( Mode == TArduinoBNO0Mode::AccelerometerGyroscope )
				return true;

			if( Mode == TArduinoBNO0Mode::GyroscopeCompass )
				return true;

			if( Mode == TArduinoBNO0Mode::AccelerometerGyroscopeCompass )
				return true;

			if( Mode == TArduinoBNO0Mode::FusionAccelerometerGyroscopeRelative )
				return true;

			if( Mode >= TArduinoBNO0Mode::FusionFull )
				return true;

			return false;
		}

		bool GetCompassDataAvailable()
		{
#ifdef _MITOV_BNO055_COMPASS_
			if( Mode == TArduinoBNO0Mode::Auto )
				return Compass.Enabled;
#endif
			if( Mode == TArduinoBNO0Mode::Compass )
				return true;

			if( Mode == TArduinoBNO0Mode::GyroscopeCompass )
				return true;

			if( Mode == TArduinoBNO0Mode::AccelerometerGyroscopeCompass )
				return true;

			if( Mode >= TArduinoBNO0Mode::FusionAccelerometerCompassAbsolute )
				return true;

			return false;
		}

		bool GetFusedDataAvailable()
		{
#ifdef _MITOV_BNO055_FUSED_
			if( Mode == TArduinoBNO0Mode::Auto )
				return Fused.Enabled;
#endif
			return ( Mode >= TArduinoBNO0Mode::FusionAccelerometerGyroscopeRelative );
		}

	protected:
		void ReadVector( uint8_t ARegister, int16_t *AData )
		{
			uint8_t AAddress = BNO055::CAdresses[ Address ];
			FWire.beginTransmission(AAddress);
  			FWire.write( ARegister );
			FWire.endTransmission();
			FWire.requestFrom(AAddress, uint8_t( 6 ));

			for (uint8_t i = 0; i < 3; i++)
			{
				int16_t ALowByte = FWire.read();
				int16_t AHighByte = FWire.read();
				AData[i] = ( ALowByte * 0xFF ) | ( AHighByte << 8 );
			}

//			Serial.println( *AData );

//			for( int i = 0; i < 3; ++i )
//				AData[ i ] = ((int16_t)((uint8_t *) AData )[ i * 2 ]) | (((int16_t)((uint8_t *) AData )[ i * 2 + 1 ]) << 8);

//			readLen((adafruit_bno055_reg_t)vector_type, buffer, 6);
//			return true;
		}

		void WriteVector( uint8_t ARegister, int16_t *AData )
		{
			uint8_t AAddress = BNO055::CAdresses[ Address ];
			FWire.beginTransmission(AAddress);

  			FWire.write( ARegister );
			for (uint8_t i = 0; i < 3; i++)
			{
				FWire.write( uint8_t( AData[ i ] ));
				FWire.write( uint8_t( AData[ i ] >> 8 ));
			}

			FWire.endTransmission();
		}

		void WriteTo( uint8_t ARegister, byte AData )
		{
//			Serial.print( "write8: " ); Serial.print( ARegister, HEX ); Serial.print( " = " ); Serial.println( AData, HEX );

			I2C::WriteByte( FWire, BNO055::CAdresses[ Address ], ARegister, AData );
		}

		bool ReadBytes( uint8_t ARegister, uint8_t length, void *data )
		{
			return I2C::ReadBytes( FWire, BNO055::CAdresses[ Address ], ARegister, length, data );
		}

		uint8_t ReadByte( uint8_t ARegister ) // Do not inline! C++ Bug! :-(
		{
			return I2C::ReadByte( FWire, BNO055::CAdresses[ Address ], ARegister );
		}

	public:
		void SystemStart()
		{
//	BNO055 clock stretches for 500us or more!
#ifdef ESP8266
			FWire.setClockStretchLimit(1000); // Allow for 1000us of clock stretching
#endif
			ReadByte(BNO055_CHIP_ID_ADDR);

// Switch to config mode (just in case since this is the default)
			Reset();


// Set the requested operating mode (see section 3.3)
//			setMode(mode);
//			delay(20);
		}

		void SystemLoopBegin( unsigned long currentMicros )
		{
			if( Enabled )
			{
#ifdef _MITOV_BNO055_CLOCK_PIN_
				if( ! ClockInputPin.IsConnected() )
#endif
					ReadSensors();
			}
		}

	public:
		BNO055I2C( TwoWire &AWire ) :
			Enabled( true ),
			Address( false ),
			ExternalOscillator( false ),
			Mode( TArduinoBNO0Mode::Auto ),
			FWire( AWire )
		{
#ifdef _MITOV_BNO055_CLOCK_PIN_
			ClockInputPin.SetCallback( MAKE_CALLBACK( BNO055I2C::DoClockReceive ));
#endif

#ifdef _MITOV_BNO055_RESET_PIN_
			ResetInputPin.SetCallback( MAKE_CALLBACK( BNO055I2C::DoResetReceive ));
#endif
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
