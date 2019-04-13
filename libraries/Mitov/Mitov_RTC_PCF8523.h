////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_RTC_PCF8523_h
#define _MITOV_RTC_PCF8523_h

#include <Mitov.h>
#include <Mitov_RTC_PCF8563.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	enum TArduinoPCF8523TimerClockFrequency { pcf23t4096Hz, pcf23t64Hz, pcf23t1Hz, pcf23t1_60th_Hz, pcf23t1_3600th_Hz, pcf23tAuto };
//---------------------------------------------------------------------------
	class TArduinoPCF8523TimerBElement
	{
	public:
#ifdef _MITOV_PCF8523_RTC_TIMER_CLOCK_PIN_
		OpenWire::SourcePin	TimerOutputPin;
#endif

	public:
		bool	Enabled : 1;
		bool	InterruptEnabled : 1;
		bool	PulseInterruptEnabled : 1;
		TArduinoPCF8523TimerClockFrequency	ClockFrequency : 3;
		float	Period = 1;
		float	LowPulseWidth = 46.875;

	public:
		TArduinoPCF8523TimerBElement() : 
			Enabled( true ),
			InterruptEnabled( false ),
			PulseInterruptEnabled( false ),
			ClockFrequency( pcf23tAuto )
		{
		}
	};
//---------------------------------------------------------------------------
	class TArduinoPCF8523SecondAndTimerAElement
	{
	public:
#ifdef _MITOV_PCF8523_RTC_TIMER_CLOCK_PIN_
		OpenWire::SourcePin	TimerOutputPin;
#endif

#ifdef _MITOV_PCF8523_RTC_SECOND_CLOCK_PIN_
		OpenWire::SourcePin	SecondClockOutputPin;
#endif

	public:
		bool	Enabled : 1;
		bool	InterruptEnabled : 1;
		bool	PulseInterruptEnabled : 1;
		bool	IsWatchdog : 1;
		bool	SecondInterruptEnabled : 1;
		TArduinoPCF8523TimerClockFrequency	ClockFrequency : 3;
		float	Period = 1;

	public:
		TArduinoPCF8523SecondAndTimerAElement() : 
			Enabled( true ),
			InterruptEnabled( false ),
			PulseInterruptEnabled( false ),
			IsWatchdog( false ),
			SecondInterruptEnabled( false ),
			ClockFrequency( pcf23tAuto )
		{
		}
	};
//---------------------------------------------------------------------------
	enum TArduinoPCF8523ClockOutFrequency { pcf23co32768Hz, pcf23co16384Hz, pcf23co8192Hz, pcf23co4096Hz, pcf23co1024Hz, pcf23co32Hz, pcf23co1Hz };
//---------------------------------------------------------------------------
	class TArduinoPCF8523ClockOutElement
	{
	public:
		bool	Enabled : 1;
		TArduinoPCF8523ClockOutFrequency	Frequency : 3;

	public:
		TArduinoPCF8523ClockOutElement() : 
			Enabled( true ),
			Frequency( pcf23co32768Hz )
		{
		}

	};
//---------------------------------------------------------------------------
	class TArduinoRTCPCF8523Offset
	{
	public:
		bool	PerMinute	: 1;
		int8_t	Value		: 6;
		bool	CorrectionInterruptEnabled = false;

	public:
		TArduinoRTCPCF8523Offset() :
			PerMinute( false ),
			Value( 0 )
		{
		}
	};
//---------------------------------------------------------------------------
	enum TArduinoRTCPCF8523PowerSwitchMode { rtc23psmStandard, rtc23psmDirect, rtc23psmSinglePower };
//---------------------------------------------------------------------------
	class TArduinoRTCPCF8523PowerManagement
	{
	public:
#ifdef _MITOV_PCF8523_RTC_LOW_VOLTAGE_PIN_
		OpenWire::TypedStartSourcePin<bool>	LowVoltageOutputPin;
#endif

#ifdef _MITOV_PCF8523_RTC_POWER_SWITCHED_PIN_
		OpenWire::SourcePin	PowerSwitchedOutputPin;
#endif

	public:
		TArduinoRTCPCF8523PowerSwitchMode	SwitchMode : 2;
		bool	BatteryLowInterruptEnabled : 1;
		bool	PowerSwitchInterruptEnabled : 1;

	public:
		TArduinoRTCPCF8523PowerManagement() :
			SwitchMode( rtc23psmSinglePower ),
			BatteryLowInterruptEnabled( false ),
			PowerSwitchInterruptEnabled( false )
		{
		}
	};
//---------------------------------------------------------------------------
	class RTC_PCF8523 : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	protected:
		const uint8_t	PCF_ADDRESS = ( uint8_t( 0xD0 ) >> 1 );

		// register addresses in the rtc
		const uint8_t	RTCC_STAT1_ADDR				= 0x00;
		const uint8_t	RTCC_STAT2_ADDR				= 0x01;
		const uint8_t	RTCC_STAT3_ADDR				= 0x02;
		const uint8_t	RTCC_SEC_ADDR				= 0x03;
		const uint8_t	RTCC_MIN_ADDR				= 0x04;
		const uint8_t	RTCC_HOUR_ADDR				= 0x05;
		const uint8_t	RTCC_DAY_ADDR				= 0x06;
		const uint8_t	RTCC_WEEKDAY_ADDR			= 0x07;
		const uint8_t	RTCC_MONTH_ADDR				= 0x08;
		const uint8_t	RTCC_YEAR_ADDR				= 0x09;
		const uint8_t	RTCC_ALRM_MIN_ADDR			= 0x0A;
		const uint8_t	RTCC_ALRM_HOUR_ADDR			= 0x0B;
		const uint8_t	RTCC_ALRM_DAY_ADDR			= 0x0C;
		const uint8_t	RTCC_ALRM_WEEKDAY_ADDR		= 0x0D;
		const uint8_t	RTCC_OFFSET_ADDR			= 0x0E;
		const uint8_t	RTCC_TIMER_CLOCKOUT_ADDR	= 0x0F;
		const uint8_t	RTCC_TIMER1_ADDR_1			= 0x10;
		const uint8_t	RTCC_TIMER1_ADDR_2			= 0x11;
		const uint8_t	RTCC_TIMER2_ADDR_1			= 0x12;
		const uint8_t	RTCC_TIMER2_ADDR_2			= 0x13;

	public:
#ifdef _MITOV_PCF8523_RTC_CLOCK_PIN_
		OpenWire::ConnectSinkPin	ClockInputPin;
#endif

#ifdef _MITOV_PCF8523_RTC_SET_PIN_
		OpenWire::SinkPin			SetInputPin;
#endif
		OpenWire::SourcePin	OutputPin;

#ifdef _MITOV_PCF8523_RTC_INTEGRITY_PIN_
		OpenWire::SourcePin	IntegrityCompromisedOutputPin;
#endif

#ifdef _MITOV_PCF8523_RTC_ALARM_PIN_
		OpenWire::SourcePin	AlarmOutputPin;
#endif

	public:
		bool	Enabled : 1; // = true;
		bool	Capacitor12_6 : 1;

	public:
		TArduinoRTCPCF8523Offset	Offset;
		TArduinoRTCAlarm	Alarm;
		TArduinoPCF8523SecondAndTimerAElement	SecondAndTimerA;
		TArduinoPCF8523TimerBElement		TimerB;
		TArduinoPCF8523ClockOutElement		ClockOut;
		TArduinoRTCPCF8523PowerManagement	PowerManagement;

	public:
		void UpdateMinuteAlarm()
		{
			if( Alarm.Minute.Enabled )
				writeRegister( RTCC_ALRM_MIN_ADDR, 0x80 | Func::FromDecToBcd( Alarm.Minute.Value ) );

			else
				writeRegister( RTCC_ALRM_MIN_ADDR, 0 );

		}

		void UpdateHourAlarm()
		{
			if( Alarm.Hour.Enabled )
				writeRegister( RTCC_ALRM_HOUR_ADDR, 0x80 | Func::FromDecToBcd( Alarm.Hour.Value ) );

			else
				writeRegister( RTCC_ALRM_HOUR_ADDR, 0 );

		}

		void UpdateDayAlarm()
		{
			if( Alarm.Day.Enabled )
				writeRegister( RTCC_ALRM_DAY_ADDR, 0x80 | Func::FromDecToBcd( Alarm.Day.Value ) );

			else
				writeRegister( RTCC_ALRM_DAY_ADDR, 0 );

		}

		void UpdateWeekDayAlarm()
		{
			if( Alarm.WeekDay.Enabled )
				writeRegister( RTCC_ALRM_WEEKDAY_ADDR, 0x80 | Func::FromDecToBcd( Alarm.WeekDay.Value ) );

			else
				writeRegister( RTCC_ALRM_WEEKDAY_ADDR, 0 );

		}

		void UpdateClockOut()
		{
			uint8_t	AValue =	(( SecondAndTimerA.PulseInterruptEnabled ) ? 0x80 : 0 ) |
								(( TimerB.PulseInterruptEnabled ) ? 0b01000000 : 0 );

			if( ClockOut.Enabled )
				AValue |= ( uint8_t( ClockOut.Frequency ) << 3 );

			else
				AValue |= 0b11000;
							
			if( SecondAndTimerA.Enabled )
				AValue |= ( SecondAndTimerA.IsWatchdog ) ? 0b100 : 0b010;

			if( TimerB.Enabled )
				AValue |= 1;

			writeRegister( RTCC_TIMER_CLOCKOUT_ADDR, AValue );
		}

		void UpdateTimerA()
		{
			uint8_t	AValueControl = 0;
			uint8_t	AValue = 0;
			if( SecondAndTimerA.Enabled )
			{
				if( SecondAndTimerA.ClockFrequency != pcftAuto )
				{
					AValueControl = uint8_t( SecondAndTimerA.ClockFrequency );
					switch( SecondAndTimerA.ClockFrequency )
					{
						case pcf23t4096Hz:
							if( SecondAndTimerA.Period < 0.062255859375 )
								AValue = ( SecondAndTimerA.Period * 255 / 0.062255859375 ) + 0.5;

							else
								AValue = 255;

							break;

						case pcf23t64Hz:
							if( SecondAndTimerA.Period < 3.984375 )
								AValue = ( SecondAndTimerA.Period * 255 / 3.9843755 ) + 0.5;

							else
								AValue = 255;

							break;

						case pcf23t1Hz:
							if( SecondAndTimerA.Period < 255 )
								AValue = SecondAndTimerA.Period + 0.5;

							else
								AValue = 255;

							break;

						case pcf23t1_60th_Hz:
							if( SecondAndTimerA.Period < 15300 )
								AValue = ( SecondAndTimerA.Period * 255 / 15300 ) + 0.5;

							else
								AValue = 255;

							break;

						default:						
							if( SecondAndTimerA.Period < 918000 )
								AValue = ( SecondAndTimerA.Period * 255 / 918000 ) + 0.5;

							else
								AValue = 255;

							break;
					}
				}

				else
				{
					if( SecondAndTimerA.Period < 0.062255859375 )
					{
						AValueControl = 0x00;
						AValue = ( SecondAndTimerA.Period * 255 / 0.062255859375 ) + 0.5;
					}
					else if( SecondAndTimerA.Period < 3.984375 )
					{
						AValueControl = 0x01;
						AValue = ( SecondAndTimerA.Period * 255 / 3.984375 ) + 0.5;
					}
					else if( SecondAndTimerA.Period < 255 )
					{
						AValueControl = 0x02;
						AValue = SecondAndTimerA.Period + 0.5;
					}
					else if( SecondAndTimerA.Period <= 15300 )
					{
						AValueControl = 0x03;
						AValue = ( SecondAndTimerA.Period * 255 / 15300 ) + 0.5;
					}
					else
					{
						AValueControl = 0x04;
						if( SecondAndTimerA.Period <= 918000 )
							AValue = ( SecondAndTimerA.Period * 255 / 918000 ) + 0.5;

						else
							AValue = 255;
					}
				}
			}

			FWire.beginTransmission( PCF_ADDRESS );
			FWire.write( RTCC_TIMER1_ADDR_1 );
			FWire.write( AValueControl );
			FWire.write( AValue );
			FWire.endTransmission();
		}

		void UpdateTimerB()
		{
			uint8_t	AValueControl = 0;
			uint8_t	AValue = 0;
			if( TimerB.Enabled )
			{
				if( TimerB.ClockFrequency != pcftAuto )
				{
					AValueControl = uint8_t( TimerB.ClockFrequency );
					switch( TimerB.ClockFrequency )
					{
						case pcf23t4096Hz:
							if( TimerB.Period < 0.062255859375 )
								AValue = ( TimerB.Period * 255 / 0.062255859375 ) + 0.5;

							else
								AValue = 255;

							break;

						case pcf23t64Hz:
							if( TimerB.Period < 3.984375 )
								AValue = ( TimerB.Period * 255 / 3.9843755 ) + 0.5;

							else
								AValue = 255;

							break;

						case pcf23t1Hz:
							if( TimerB.Period < 255 )
								AValue = TimerB.Period + 0.5;

							else
								AValue = 255;

							break;

						case pcf23t1_60th_Hz:
							if( TimerB.Period < 15300 )
								AValue = ( TimerB.Period * 255 / 15300 ) + 0.5;

							else
								AValue = 255;

							break;

						default:						
							if( TimerB.Period < 918000 )
								AValue = ( TimerB.Period * 255 / 918000 ) + 0.5;

							else
								AValue = 255;

							break;
					}
				}

				else
				{
					if( TimerB.Period < 0.062255859375 )
					{
						AValueControl = 0x00;
						AValue = ( TimerB.Period * 255 / 0.062255859375 ) + 0.5;
					}
					else if( TimerB.Period < 3.984375 )
					{
						AValueControl = 0x01;
						AValue = ( TimerB.Period * 255 / 3.984375 ) + 0.5;
					}
					else if( TimerB.Period < 255 )
					{
						AValueControl = 0x02;
						AValue = TimerB.Period + 0.5;
					}
					else if( TimerB.Period <= 15300 )
					{
						AValueControl = 0x03;
						AValue = ( TimerB.Period * 255 / 15300 ) + 0.5;
					}
					else
					{
						AValueControl = 0x04;
						if( TimerB.Period <= 918000 )
							AValue = ( TimerB.Period * 255 / 918000 ) + 0.5;

						else
							AValue = 255;
					}
				}
			}

			const float CPulseWidth[] = { 46.875, 62.500, 78.125, 93.750,  125.000, 156.250, 187.500 };

			if( TimerB.LowPulseWidth >= 218.750 )
				AValueControl |= 0b1110000;

			else
			{
				for( int i = 0; i < MITOV_ARRAY_SIZE( CPulseWidth ); ++i )
					if( TimerB.LowPulseWidth <= CPulseWidth[ i ] )
					{
						AValueControl |= i << 4;
						break;
					}
			}

			FWire.beginTransmission( PCF_ADDRESS );
			FWire.write( RTCC_TIMER2_ADDR_1 );
			FWire.write( AValueControl );
			FWire.write( AValue );
			FWire.endTransmission();
		}

		void UpdateControl1()
		{
			uint8_t AValue =	(( Capacitor12_6 ) ? 0x80 : 0 ) |
								(( SecondAndTimerA.SecondInterruptEnabled ) ? 0b100 : 0 ) |
								(( Alarm.InterruptEnabled ) ? 0b10 : 0 ) |
								(( Offset.CorrectionInterruptEnabled ) ? 0b1 : 0 );

			writeRegister( RTCC_STAT1_ADDR, AValue );
		}

		void UpdateControl2()
		{
			uint8_t AValue =	(( TimerB.InterruptEnabled ) ? 1 : 0 ) |
								(( SecondAndTimerA.InterruptEnabled ) ? 0b110 : 0 );

			writeRegister( RTCC_STAT2_ADDR, AValue );
		}

		void UpdateControl3()
		{
//			rtc23psmStandard = 0, rtc23psmDirect = 0b00100000, rtc23psmSinglePower = 0b01100000
			const uint8_t CModes[] = { 0, 0b00100000, 0b01100000 };

			uint8_t AValue =	0x80 |
								(( PowerManagement.BatteryLowInterruptEnabled ) ? 1 : 0 ) |
								(( PowerManagement.PowerSwitchInterruptEnabled ) ? 0b10 : 0 ) |
								CModes[ PowerManagement.SwitchMode ];

#ifdef _MITOV_PCF8523_RTC_LOW_VOLTAGE_PIN_
			AValue &= 0x7F;
#else // _MITOV_PCF8523_RTC_LOW_VOLTAGE_PIN_
			if( PowerManagement.BatteryLowInterruptEnabled )
				AValue &= 0x7F;
#endif // _MITOV_PCF8523_RTC_LOW_VOLTAGE_PIN_

			writeRegister( RTCC_STAT3_ADDR, AValue );
		}

		void UpdateOffset()
		{
			int8_t	AIntValue = Offset.Value;
			uint8_t	AValue = *((uint8_t *)&AIntValue );
			if( Offset.PerMinute )
				AValue |= 0x80;
			
			writeRegister( RTCC_OFFSET_ADDR, AValue );
		}

	public:
		void SetMilliSecond( uint8_t AValue )
		{
		}

		void SetSecond( uint8_t AValue )
		{
			writeRegister( RTCC_SEC_ADDR, AValue );
		}

		void SetMinute( uint8_t AValue )
		{
			writeRegister( RTCC_MIN_ADDR, Func::FromDecToBcd( AValue ) );
		}

		void SetHour( uint8_t AValue )
		{
			writeRegister( RTCC_HOUR_ADDR, Func::FromDecToBcd( AValue ) );
		}

		void SetWeekDay( uint8_t AValue )
		{
			writeRegister( RTCC_WEEKDAY_ADDR, Func::FromDecToBcd( AValue ) );
		}

		void SetYear( uint16_t AValue )
		{
			writeRegister( RTCC_YEAR_ADDR, Func::FromDecToBcd( AValue % 100 ) );
		}

		Mitov::TDateTime GetDateTime()
		{
			FWire.beginTransmission( PCF_ADDRESS );
			FWire.write( RTCC_SEC_ADDR );
			FWire.endTransmission();

			FWire.requestFrom( PCF_ADDRESS, (uint8_t)7 );

			uint8_t  ASecondLowBattery = FWire.read();
			uint16_t AMinute = FWire.read();
			uint16_t AHour = FWire.read();
			uint16_t ADate = FWire.read();
			uint16_t AWeekDay = FWire.read();
			uint8_t  ACenturyMonth = FWire.read();
			uint16_t AYear = FWire.read(); // + 2000;

			uint16_t ASecond = Func::FromBcdToDec( ASecondLowBattery & 0x7F );
			AMinute = Func::FromBcdToDec( AMinute );
			AHour = Func::FromBcdToDec( AHour );
			ADate = Func::FromBcdToDec( ADate );
			uint16_t AMonth = Func::FromBcdToDec( ACenturyMonth & 0b11111 );
			AYear = Func::FromBcdToDec( AYear ); // + 2000;

			if( ACenturyMonth & 0x80 )
				AYear += 2100;

			else
				AYear += 2000;

			Mitov::TDateTime ADateTime;
			ADateTime.TryEncodeDateTime( AYear, AMonth, ADate, AHour, AMinute, ASecond, 0 );
			return ADateTime;
		}

		void SetDateTime( const Mitov::TDateTime &ADateTime )
		{
			uint16_t AYear;
			uint16_t AMonth;
			uint16_t ADay;
			uint16_t AWeekDay;
			uint16_t AHour;
			uint16_t AMinute;
			uint16_t ASecond;
			uint16_t AMilliSecond;
			ADateTime.DecodeDateTime( AYear, AMonth, ADay, AWeekDay, AHour, AMinute, ASecond, AMilliSecond );

			FWire.beginTransmission( PCF_ADDRESS );
			FWire.write( RTCC_SEC_ADDR );

//			FWire.write( ( ( Halt & 1 ) << 7 ) | ( Func::FromDecToBcd( ASecond ) & 0x7F ));
			FWire.write( Func::FromDecToBcd( ASecond ) & 0x7F );
			FWire.write(Func::FromDecToBcd( AMinute ));
			FWire.write(Func::FromDecToBcd( AHour ));
			FWire.write(Func::FromDecToBcd( ADay ));
			FWire.write(Func::FromDecToBcd( AWeekDay ));
			if( AYear >= 2100 )
				FWire.write( 0x80 | Func::FromDecToBcd( AMonth ));

			else
				FWire.write(Func::FromDecToBcd( AMonth ));

			FWire.write(Func::FromDecToBcd( AYear % 100 ));

			FWire.endTransmission();
		}


	protected:
		TwoWire	&FWire;

	protected:
		Mitov::TDateTime FLastDateTime;

	protected:
#ifdef _MITOV_PCF8523_RTC_CLOCK_PIN_
		void DoClockReceive( void *_Data )
		{
			ReadTime( true );
		}
#endif

	protected:
		uint8_t readRegister(const uint8_t reg) 
		{
			FWire.beginTransmission(PCF_ADDRESS);
			FWire.write( reg );	
			FWire.endTransmission();

			FWire.requestFrom(PCF_ADDRESS, (uint8_t)1 );

			return FWire.read();
		}

		void writeRegister( const uint8_t reg, const uint8_t value ) 
		{
			FWire.beginTransmission(PCF_ADDRESS);
			FWire.write( reg );	
			FWire.write( value );	
			FWire.endTransmission();
		}

		void ReadTime( bool AFromClock )
		{
			if( ! OutputPin.IsConnected() )
				return;

//			Serial.println( "ReadTime" );

			// Start at beginning, read entire memory in one go
			FWire.beginTransmission( PCF_ADDRESS );
			FWire.write( RTCC_SEC_ADDR );
			FWire.endTransmission();

			FWire.requestFrom( PCF_ADDRESS, (uint8_t)7 );

			uint8_t  ASecondLowBattery = FWire.read();
			uint16_t AMinute = FWire.read();
			uint16_t AHour = FWire.read();
			uint16_t ADate = FWire.read();
			uint16_t AWeekDay = FWire.read();
			uint8_t  ACenturyMonth = FWire.read();
			uint16_t AYear = FWire.read(); // + 2000;

			uint16_t ASecond = Func::FromBcdToDec( ASecondLowBattery & 0x7F );
			AMinute = Func::FromBcdToDec( AMinute );
			AHour = Func::FromBcdToDec( AHour );
			ADate = Func::FromBcdToDec( ADate );
			uint16_t AMonth = Func::FromBcdToDec( ACenturyMonth & 0b11111 );
			AYear = Func::FromBcdToDec( AYear ); // + 2000;

			if( ACenturyMonth & 0x80 )
				AYear += 2100;

			else
				AYear += 2000;

			Mitov::TDateTime ADateTime;

//			Serial.println( AMonth );

			if( ADateTime.TryEncodeDateTime( AYear, AMonth, ADate, AHour, AMinute, ASecond, 0 ))
				if( AFromClock || ( FLastDateTime != ADateTime ))
				{
					FLastDateTime = ADateTime;
					OutputPin.Notify( &ADateTime );
				}

#ifdef _MITOV_PCF8523_RTC_INTEGRITY_PIN_
			if( IntegrityCompromisedOutputPin )
				IntegrityCompromisedOutputPin.SendValue<bool>( ASecondLowBattery & 0x80 );

#endif // _MITOV_PCF8523_RTC_INTEGRITY_PIN_

		}


	protected:
#ifdef _MITOV_PCF8523_RTC_SET_PIN_
		void DoSetReceive( void *_Data )
		{
//			if( WriteProtect )
//				return;

			Mitov::TDateTime &ADateTime = *(Mitov::TDateTime *)_Data;
			SetDateTime( ADateTime );
		}
#endif

	protected:
		virtual void SystemInit() override
		{
			FWire.beginTransmission( PCF_ADDRESS );    // Issue I2C start signal
			FWire.write((byte)0x0);        // start address

			FWire.write((byte)0x0);     //control/status1
			FWire.write((byte)0x0);     //control/status2
			FWire.write((byte)0x0);     //control/status3
			FWire.endTransmission();
/*
			FWire.write((byte)0x81);     //set seconds & VL
			FWire.write((byte)0x01);    //set minutes
			FWire.write((byte)0x01);    //set hour
			FWire.write((byte)0x01);    //set day
			FWire.write((byte)0x01);    //set weekday
			FWire.write((byte)0x01);     //set month, century to 1
			FWire.write((byte)0x01);    //set year to 99
			FWire.write((byte)0x80);    //minute alarm value reset to 00
			FWire.write((byte)0x80);    //hour alarm value reset to 00
			FWire.write((byte)0x80);    //day alarm value reset to 00
			FWire.write((byte)0x80);    //weekday alarm value reset to 00
*/
//			Serial.println( "TEST" );
			// Update Alarm

			UpdateOffset();

			FWire.beginTransmission( PCF_ADDRESS );
			FWire.write( RTCC_ALRM_MIN_ADDR );
			if( Alarm.Minute.Enabled )
				FWire.write( Func::FromDecToBcd( Alarm.Minute.Value ) );

			else
				FWire.write( 0x80 );

			if( Alarm.Hour.Enabled )
				FWire.write( Func::FromDecToBcd( Alarm.Hour.Value ) );

			else
				FWire.write( 0x80 );

			if( Alarm.Day.Enabled )
				FWire.write( Func::FromDecToBcd( Alarm.Day.Value ) );

			else
				FWire.write( 0x80 );


			if( Alarm.WeekDay.Enabled )
				FWire.write( Func::FromDecToBcd( Alarm.WeekDay.Value ) );

			else
				FWire.write( 0x80 );

			FWire.endTransmission();

/*
			FWire.beginTransmission( PCF_ADDRESS );    // Issue I2C start signal
			FWire.write( RTCC_SQW_ADDR );
			FWire.write((byte)0x0);     //set SQW, see: setSquareWave
			FWire.write((byte)0x0);     //timer off
			FWire.endTransmission();
*/
			UpdateClockOut();
			UpdateTimerA();
			UpdateTimerB();
			UpdateControl1();
			UpdateControl2();
			UpdateControl3();

//			Serial.println( "TEST2" );
//			inherited::SystemInit();
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( Enabled )
			{
#ifdef _MITOV_PCF8523_RTC_CLOCK_PIN_
				if( ! ClockInputPin.IsConnected() )
#endif // _MITOV_PCF8523_RTC_CLOCK_PIN_
					ReadTime( false );

#if ( defined( _MITOV_PCF8523_RTC_ALARM_PIN_ ) || defined( _MITOV_PCF8523_RTC_SECOND_CLOCK_PIN_ ) || defined( _MITOV_PCF8523_RTC_TIMER_CLOCK_PIN_ ))
				uint8_t  AStatus = readRegister( RTCC_STAT2_ADDR );

#if defined( _MITOV_PCF8523_RTC_TIMER_CLOCK_PIN_ )
				if( SecondAndTimerA.IsWatchdog )
				{
					if( AStatus & 0b10000000 ) // Check Interrupt
					{
						// Clear Interrupt
						writeRegister( RTCC_STAT2_ADDR, AStatus & ( ~0b10000000 ) );
						SecondAndTimerA.TimerOutputPin.Notify( nullptr );
					}
				}
				else
				{
					if( AStatus & 0b01000000 ) // Check Interrupt
					{
						// Clear Interrupt
						writeRegister( RTCC_STAT2_ADDR, AStatus & ( ~0b01000000 ) );
						SecondAndTimerA.TimerOutputPin.Notify( nullptr );
					}
				}

				if( AStatus & 0b00100000 ) // Check Interrupt
				{
					// Clear Interrupt
					writeRegister( RTCC_STAT2_ADDR, AStatus & ( ~0b00100000 ) );
					TimerB.TimerOutputPin.Notify( nullptr );
				}

#endif // _MITOV_PCF8523_RTC_TIMER_CLOCK_PIN_

//				Serial.println( "TEST" );
#if defined( _MITOV_PCF8523_RTC_ALARM_PIN_ )
				if( AStatus & 0b1000 ) // Check Alarm
				{
					// Clear Alarm
					writeRegister( RTCC_STAT2_ADDR, AStatus & ( ~0b1000 ) );
					AlarmOutputPin.Notify( nullptr );
				}
#endif // _MITOV_PCF8523_RTC_ALARM_PIN_

#if defined( _MITOV_PCF8523_RTC_SECOND_CLOCK_PIN_ )
				if( AStatus & 0b10000 ) // Seconds Interrupt
				{
					// Clear Interrupt
					writeRegister( RTCC_STAT2_ADDR, AStatus & ( ~0b10000 ) );
					SecondAndTimerA.SecondClockOutputPin.Notify( nullptr );
				}
#endif // _MITOV_PCF8523_RTC_ALARM_PIN_

#endif // ( defined( _MITOV_PCF8523_RTC_ALARM_PIN_ ) || defined( _MITOV_PCF8523_RTC_SECOND_CLOCK_PIN_ ))

#if ( defined( _MITOV_PCF8523_RTC_LOW_VOLTAGE_PIN_ ) || defined( _MITOV_PCF8523_RTC_POWER_SWITCHED_PIN_ ))
				uint8_t  AStatus3 = readRegister( RTCC_STAT3_ADDR );
				if( AStatus3 & 0b1000 ) // Switch Interrupt
				{
					writeRegister( RTCC_STAT3_ADDR, AStatus3 & ( ~0b1000 ) );
					PowerManagement.PowerSwitchedOutputPin.Notify( nullptr );
				}

				PowerManagement.LowVoltageOutputPin.SetValue( ( AStatus3 & 0b1000 ) != 0 ) // Switch Interrupt
#endif // ( defined( _MITOV_PCF8523_RTC_LOW_VOLTAGE_PIN_ ) || defined( _MITOV_PCF8523_RTC_POWER_SWITCHED_PIN_ ))

			}


//			inherited::SystemLoopBegin( currentMicros );
		}

	public:
		RTC_PCF8523( TwoWire &AWire ) :
			Enabled( true ),
			Capacitor12_6( false ),
			FWire( AWire )
		{
#ifdef _MITOV_PCF8523_RTC_CLOCK_PIN_
				ClockInputPin.SetCallback( MAKE_CALLBACK( RTC_PCF8523::DoClockReceive ));
#endif

#ifdef _MITOV_PCF8523_RTC_SET_PIN_
				SetInputPin.SetCallback( MAKE_CALLBACK( RTC_PCF8523::DoSetReceive ));
#endif
		}

		RTC_PCF8523() :
			Enabled( true ),
			Capacitor12_6( false ),
			FWire( Wire )
		{
#ifdef _MITOV_PCF8523_RTC_CLOCK_PIN_
				ClockInputPin.SetCallback( MAKE_CALLBACK( RTC_PCF8523::DoClockReceive ));
#endif

#ifdef _MITOV_PCF8523_RTC_SET_PIN_
				SetInputPin.SetCallback( MAKE_CALLBACK( RTC_PCF8523::DoSetReceive ));
#endif
		}

	};
//---------------------------------------------------------------------------
	typedef RTC_PCF8523 RTC_PCF8523_Alarm;
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif // _MITOV_RTC_PCF8523_h
