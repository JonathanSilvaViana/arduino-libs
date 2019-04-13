////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_RTC_DS3231_h
#define _MITOV_RTC_DS3231_h

#include <Mitov.h>
#include <Wire.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
//	enum DS3231ClockFrequency { rtc3231fDisabled, rtc3231f1Hz, rtc3231f1024Hz, rtc3231f4096Hz, rtc3231f8192Hz };
//---------------------------------------------------------------------------
	enum TArduinoRTCDS3231Alarm1Mode { rtc3231a1mOncePerSecond, rtc3231a1mOnSeconds, rtc3231a1mOnMinutesSeconds, rtc3231a1mOnHoursMinutesSeconds, rtc3231a1mOnDateHoursMinutesSeconds, rtc3231a1mOnWeekDayHoursMinutesSeconds };
//---------------------------------------------------------------------------
	enum TArduinoRTCDS3231Alarm2Mode { rtc3231a2mOncePerMinute, rtc3231a2mOnMinutes, rtc3231a2mOnHoursMinutes, rtc3231a2mOnDateHoursMinutes, rtc3231a2mOnWeekDayHoursMinutes };
//---------------------------------------------------------------------------
	class TArduinoRTCDS3231AlarmOne
	{
	public:
		OpenWire::SourcePin	AlarmOutputPin;

	public:
		bool	Enabled : 1;
		bool	InterruptEnabled : 1;
		TArduinoRTCDS3231Alarm1Mode	Mode : 3;
		uint8_t	Second : 6;
		uint8_t	Minute : 6;
		uint8_t	Hour : 5;
		uint8_t	Date : 5;
		uint8_t	WeekDay : 3;

	public:
		TArduinoRTCDS3231AlarmOne() :
			Enabled( false ),
			InterruptEnabled( false ),
			Mode( rtc3231a1mOnHoursMinutesSeconds ),
			Second( 0 ),
			Minute( 0 ),
			Hour( 0 ),
			Date( 1 ),
			WeekDay( 0 )
		{
		}
	};
//---------------------------------------------------------------------------
	class TArduinoRTCDS3231AlarmTwo
	{
	public:
		OpenWire::SourcePin	AlarmOutputPin;

	public:
		bool	Enabled : 1;
		bool	InterruptEnabled : 1;
		TArduinoRTCDS3231Alarm2Mode	Mode : 3;
		uint8_t	Minute : 6;
		uint8_t	Hour : 5;
		uint8_t	Date : 5;
		uint8_t	WeekDay : 3;

	public:
		TArduinoRTCDS3231AlarmTwo() :
			Enabled( false ),
			InterruptEnabled( false ),
			Mode( rtc3231a2mOnHoursMinutes ),
			Minute( 0 ),
			Hour( 0 ),
			Date( 1 ),
			WeekDay( 0 )
		{
		}
	};
//---------------------------------------------------------------------------
	class RTC_DS3231 : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

		enum Register 
		{
			kSecondReg			= 0x00,
			kMinuteReg			= 0x01,
			kHourReg			= 0x02,
			kWeekDayReg			= 0x03,
			kDateReg			= 0x04,
			kMonthReg			= 0x05,
			kYearReg			= 0x06,
			kAlarm1SecondReg	= 0x07,
			kAlarm1MinuteReg	= 0x08,
			kAlarm1HourReg		= 0x09,
			kAlarm1DayReg		= 0x0A,
			kAlarm2MinuteReg	= 0x0B,
			kAlarm2HourReg		= 0x0C,
			kAlarm2DayReg		= 0x0D,
			kControlReg			= 0x0E
//			kClockReg	= 7,

			// The RAM register space follows the clock register space.
//			kRamAddress0     = 8
		};

		static const uint8_t	DS3231_ADDRESS  = 0x68;

		static const uint8_t	RTC_STATUS = 0x0F;

//Status register bits
		static const uint8_t	OSF		= 7;
		static const uint8_t	BB32KHZ = 6;
		static const uint8_t	CRATE1	= 5;
		static const uint8_t	CRATE0	= 4;
		static const uint8_t	EN32KHZ = 3;
		static const uint8_t	BSY		= 2;
		static const uint8_t	A2F		= 1;
		static const uint8_t	A1F		= 0;

	public:
		OpenWire::SourcePin	OutputPin;

#ifdef _MITOV_DS3231_RTC_CLOCK_PIN_
		OpenWire::ConnectSinkPin	ClockInputPin;
#endif

#ifdef _MITOV_DS3231_RTC_SET_PIN_
		OpenWire::SinkPin	SetInputPin;
#endif	// _MITOV_DS3231_RTC_SET_PIN_

#ifdef _MITOV_DS3231_RTC_TEMPERATURE_PIN_
		OpenWire::TypedStartSourcePin<float>	TemperatureOutputPin;
#endif	// _MITOV_DS3231_RTC_TEMPERATURE_PIN_

	public:
		bool	Enabled : 1;
		bool	Halt : 1;

#ifdef _MITOV_DS3231_RTC_ALARM_ONE_
		TArduinoRTCDS3231AlarmOne	AlarmOne;
#endif	// _MITOV_DS3231_RTC_ALARM_ONE_

#ifdef _MITOV_DS3231_RTC_ALARM_TWO_
		TArduinoRTCDS3231AlarmTwo	AlarmTwo;
#endif	// _MITOV_DS3231_RTC_ALARM_TWO_

	public:
		void UpdateHalt()
		{
			uint8_t sec = readRegister( kSecondReg );

			sec &= ~(1 << 7);
			sec |= ( ( Halt & 1 ) << 7 );
			writeRegister( kSecondReg, sec );
		}

#ifdef _MITOV_DS3231_RTC_ALARM_ONE_
		void UpdateAlarmOneSecond()
		{
			uint8_t	AValue = Func::FromDecToBcd( AlarmOne.Second );
			if( AlarmOne.Mode == rtc3231a1mOncePerSecond )
				AValue |= 0x80;

			writeRegister( kAlarm1SecondReg, AValue );
		}

		void UpdateAlarmOneMinute()
		{
			uint8_t	AValue = Func::FromDecToBcd( AlarmOne.Minute );
			if( AlarmOne.Mode <= rtc3231a1mOnSeconds )
				AValue |= 0x80;

			writeRegister( kAlarm1MinuteReg, AValue );
		}

		void UpdateAlarmOneHour()
		{
			uint8_t	AValue = Func::FromDecToBcd( AlarmOne.Hour );
			if( AlarmOne.Mode <= rtc3231a1mOnMinutesSeconds )
				AValue |= 0x80;

			writeRegister( kAlarm1HourReg, AValue );
		}

		void UpdateAlarmOneDate()
		{
			if( AlarmOne.Mode == rtc3231a1mOnWeekDayHoursMinutesSeconds )
				return;

			uint8_t	AValue = Func::FromDecToBcd( AlarmOne.Date );
			if( AlarmOne.Mode <= rtc3231a1mOnHoursMinutesSeconds )
				AValue |= 0x80;

			writeRegister( kAlarm1DayReg, AValue );
		}

		void UpdateAlarmOneWeekDay()
		{
			if( AlarmOne.Mode != rtc3231a1mOnWeekDayHoursMinutesSeconds )
				return;

			uint8_t	AValue = AlarmOne.WeekDay + 0x41;
			writeRegister( kAlarm1DayReg, AValue );
		}

		void UpdateAlarmOne()
		{
			uint8_t	ASecond = Func::FromDecToBcd( AlarmOne.Second );
			if( AlarmOne.Mode == rtc3231a1mOncePerSecond )
				ASecond |= 0x80;

			uint8_t	AMinute = Func::FromDecToBcd( AlarmOne.Minute );
			if( AlarmOne.Mode <= rtc3231a1mOnSeconds )
				AMinute |= 0x80;

			uint8_t	AHour = Func::FromDecToBcd( AlarmOne.Hour );
			if( AlarmOne.Mode <= rtc3231a1mOnMinutesSeconds )
				AHour |= 0x80;

			uint8_t	ADay;
			if( AlarmOne.Mode == rtc3231a1mOnWeekDayHoursMinutesSeconds )
				ADay = AlarmOne.WeekDay + 0x41;

			else
			{
				ADay = Func::FromDecToBcd( AlarmOne.Date );
				if( AlarmOne.Mode <= rtc3231a1mOnHoursMinutesSeconds )
					ADay |= 0x80;			
			}

			FWire.beginTransmission(DS3231_ADDRESS);
			FWire.write( kAlarm1SecondReg );	

			FWire.write( ASecond );
			FWire.write( AMinute );
			FWire.write( AHour );
			FWire.write( ADay );

			FWire.endTransmission();
		}
#endif	// _MITOV_DS3231_RTC_ALARM_ONE_

#ifdef _MITOV_DS3231_RTC_ALARM_TWO_
		void UpdateAlarmTwoMinute()
		{
			uint8_t	AValue = Func::FromDecToBcd( AlarmTwo.Minute );
			if( AlarmTwo.Mode == rtc3231a2mOncePerMinute )
				AValue |= 0x80;

			writeRegister( kAlarm2MinuteReg, AValue );
		}

		void UpdateAlarmTwoHour()
		{
			uint8_t	AValue = Func::FromDecToBcd( AlarmTwo.Hour );
			if( AlarmTwo.Mode <= rtc3231a2mOnMinutes )
				AValue |= 0x80;

			writeRegister( kAlarm2HourReg, AValue );
		}

		void UpdateAlarmTwoDate()
		{
			if( AlarmTwo.Mode == rtc3231a2mOnWeekDayHoursMinutes )
				return;

			uint8_t	AValue = Func::FromDecToBcd( AlarmTwo.Date );
			if( AlarmTwo.Mode <= rtc3231a2mOnHoursMinutes )
				AValue |= 0x80;

			writeRegister( kAlarm2DayReg, AValue );
		}

		void UpdateAlarmTwoWeekDay()
		{
			if( AlarmTwo.Mode != rtc3231a2mOnWeekDayHoursMinutes )
				return;

			uint8_t	AValue = AlarmTwo.WeekDay + 0x41;
			writeRegister( kAlarm2DayReg, AValue );
		}

		void UpdateAlarmTwo()
		{
			uint8_t	AMinute = Func::FromDecToBcd( AlarmTwo.Minute );
			if( AlarmTwo.Mode == rtc3231a2mOncePerMinute )
				AMinute |= 0x80;

			uint8_t	AHour = Func::FromDecToBcd( AlarmTwo.Hour );
			if( AlarmTwo.Mode <= rtc3231a2mOnMinutes )
				AHour |= 0x80;

			uint8_t	ADay;
			if( AlarmTwo.Mode == rtc3231a2mOnWeekDayHoursMinutes )
				ADay = AlarmTwo.WeekDay + 0x41;

			else
			{
				ADay = Func::FromDecToBcd( AlarmTwo.Date );
				if( AlarmTwo.Mode <= rtc3231a2mOnHoursMinutes )
					ADay |= 0x80;
			}

			FWire.beginTransmission(DS3231_ADDRESS);
			FWire.write( kAlarm2MinuteReg );	

			FWire.write( AMinute );
			FWire.write( AHour );
			FWire.write( ADay );

			FWire.endTransmission();
		}
#endif	// _MITOV_DS3231_RTC_ALARM_TWO_

	public:
		void SetMilliSecond( uint8_t AValue )
		{
		}

		void SetSecond( uint8_t AValue )
		{
			writeRegister( kSecondReg, ( ( Halt & 1 ) << 7 ) | Func::FromDecToBcd( AValue ) );
		}

		void SetMinute( uint8_t AValue )
		{
			writeRegister( kMinuteReg, Func::FromDecToBcd( AValue ) );
		}

		void SetHour( uint8_t AValue )
		{
			writeRegister( kHourReg, Func::FromDecToBcd( AValue ) );
		}

		void SetWeekDay( uint8_t AValue )
		{
			writeRegister( kWeekDayReg, Func::FromDecToBcd( AValue ) );
		}

		void SetYear( uint16_t AValue )
		{
			writeRegister( kYearReg, Func::FromDecToBcd( AValue % 100 ) );
		}

		Mitov::TDateTime GetDateTime()
		{
			FWire.beginTransmission(DS3231_ADDRESS);
			FWire.write((byte)0);	
			FWire.endTransmission();

			FWire.requestFrom(DS3231_ADDRESS, (byte)7);
			uint16_t ASecond = Func::FromBcdToDec(FWire.read() & 0x7F);
			uint16_t AMinute = Func::FromBcdToDec(FWire.read());
			uint16_t AHour = hourFromRegisterValue( FWire.read() );
			uint16_t ADay = FWire.read();
			uint16_t ADate = Func::FromBcdToDec(FWire.read());
			uint16_t AMonth = Func::FromBcdToDec(FWire.read());
			uint16_t AYear = Func::FromBcdToDec(FWire.read()) + 2000;

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
			FWire.beginTransmission(DS3231_ADDRESS);

			FWire.write((byte)0); // start at location 0

			FWire.write( ( ( Halt & 1 ) << 7 ) | Func::FromDecToBcd( ASecond ));
			FWire.write(Func::FromDecToBcd( AMinute ));
			FWire.write(Func::FromDecToBcd( AHour ));
			FWire.write(Func::FromDecToBcd( AWeekDay ));
			FWire.write(Func::FromDecToBcd( ADay ));
			FWire.write(Func::FromDecToBcd( AMonth ));
			FWire.write(Func::FromDecToBcd( AYear % 100 ));

			FWire.endTransmission();

			uint8_t s = readRegister(RTC_STATUS);        //read the status register
			writeRegister( RTC_STATUS, s & ~( 1 << OSF) );  //clear the Oscillator Stop Flag
		}

	protected:
		TwoWire	&FWire;

	protected:
		Mitov::TDateTime FLastDateTime;

	protected:
		uint8_t readRegister(const uint8_t reg) 
		{
			FWire.beginTransmission(DS3231_ADDRESS);
			FWire.write( reg );	
			FWire.endTransmission();

			FWire.requestFrom(DS3231_ADDRESS, (uint8_t)1 );

			return FWire.read();
		}

		void writeRegister( const uint8_t reg, const uint8_t value ) 
		{
			FWire.beginTransmission(DS3231_ADDRESS);
			FWire.write( reg );	
			FWire.write( value );	
			FWire.endTransmission();
		}

		uint8_t hourFromRegisterValue(const uint8_t value) 
		{
			uint8_t adj;
			if (value & 128)  // 12-hour mode
				adj = 12 * ((value & 32) >> 5);

			else           // 24-hour mode
				adj = 10 * ((value & (32 + 16)) >> 4);

			return (value & 15) + adj;
		}

		void ReadTime( bool AFromClock )
		{
//			Serial.println( "ReadTime" );
			if( ! OutputPin.IsConnected() )
				return;

			FWire.beginTransmission(DS3231_ADDRESS);
			FWire.write((byte)0);	
			FWire.endTransmission();

			FWire.requestFrom(DS3231_ADDRESS, (byte)7);
			uint16_t ASecond = Func::FromBcdToDec(FWire.read() & 0x7F);
			uint16_t AMinute = Func::FromBcdToDec(FWire.read());
			uint16_t AHour = hourFromRegisterValue( FWire.read() );
			uint16_t ADay = FWire.read();
			uint16_t ADate = Func::FromBcdToDec(FWire.read());
			uint16_t AMonth = Func::FromBcdToDec(FWire.read());
			uint16_t AYear = Func::FromBcdToDec(FWire.read()) + 2000;

#ifdef _MITOV_DS3231_RTC_TEMPERATURE_PIN_
			FWire.beginTransmission(DS3231_ADDRESS);
			FWire.write((byte)0x11);	
			FWire.endTransmission();
			FWire.requestFrom(DS3231_ADDRESS, (byte)2);

			int16_t ATemp = int16_t( FWire.read() ) << 8;
			ATemp |= FWire.read();

			float ATemperature = float( ATemp ) / 256;
			TemperatureOutputPin.SetValue( ATemperature );
//			Serial.println( ATemperature );
#endif

			Mitov::TDateTime ADateTime;

			if( ADateTime.TryEncodeDateTime( AYear, AMonth, ADate, AHour, AMinute, ASecond, 0 ))
			{
				if( AFromClock || ( FLastDateTime != ADateTime ))
				{
					FLastDateTime = ADateTime;
					OutputPin.Notify( &ADateTime );
				}
			}

		}

/*
		void UpdateClockFrequency()
		{
			const uint8_t CValues [] = { 0x00, 0x80, 0x40, 0x41, 0x42, 0x43 };

			uint8_t AValue = readRegister( kControlReg );

			sec &= ~(1 << 7);
			sec |= ( ( Halt & 1 ) << 7 );
			writeRegister( kSecondReg, sec );

			writeRegister( kControlReg, CValues[ ClockFrequency ] );
		}
*/
	protected:
#ifdef _MITOV_DS3231_RTC_SET_PIN_
		void DoSetReceive( void *_Data )
		{
			Mitov::TDateTime &ADateTime = *(Mitov::TDateTime *)_Data;
			SetDateTime( ADateTime );
		}

#endif // _MITOV_DS3231_RTC_SET_PIN_

#ifdef _MITOV_DS3231_RTC_CLOCK_PIN_
		void DoClockReceive( void *_Data )
		{
			ReadTime( true );
		}
#endif // _MITOV_DS3231_RTC_CLOCK_PIN_

	protected:
		virtual void SystemInit() override
		{
			UpdateHalt();
#ifdef _MITOV_DS3231_RTC_ALARM_ONE_
			UpdateAlarmOne();
#endif	// _MITOV_DS3231_RTC_ALARM_ONE_

#ifdef _MITOV_DS3231_RTC_ALARM_TWO_
			UpdateAlarmTwo();
#endif	// _MITOV_DS3231_RTC_ALARM_TWO_
//			UpdateClockFrequency();
//			inherited::SystemInit();
		}

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( Enabled )
#ifdef _MITOV_DS3231_RTC_CLOCK_PIN_
				if( ! ClockInputPin.IsConnected() )
#endif // _MITOV_DS3231_RTC_CLOCK_PIN_
					ReadTime( false );

#if defined( _MITOV_DS3231_RTC_ALARM_ONE_ ) || defined( _MITOV_DS3231_RTC_ALARM_TWO_ )
			if( 
#if defined( _MITOV_DS3231_RTC_ALARM_ONE_ )
					AlarmOne.Enabled 
#if defined( _MITOV_DS3231_RTC_ALARM_TWO_ )
				|| 
#endif	// _MITOV_DS3231_RTC_ALARM_TWO_

#endif	// _MITOV_DS3231_RTC_ALARM_ONE_
#if defined( _MITOV_DS3231_RTC_ALARM_TWO_ )
					AlarmTwo.Enabled 
#endif	// _MITOV_DS3231_RTC_ALARM_TWO_
				)
			{
				uint8_t AValue = readRegister( RTC_STATUS );        //read the status register
				if( AValue & 0b11 )
				{
					// Clear the alarm bits
					writeRegister( RTC_STATUS, AValue & ( ~ 0b11 ) );

#ifdef _MITOV_DS3231_RTC_ALARM_ONE_
					if( AValue & 0b01 )
						AlarmOne.AlarmOutputPin.Notify( nullptr );
#endif	// _MITOV_DS3231_RTC_ALARM_ONE_

#ifdef _MITOV_DS3231_RTC_ALARM_TWO_
					if( AValue & 0b10 )
						AlarmTwo.AlarmOutputPin.Notify( nullptr );
#endif	// _MITOV_DS3231_RTC_ALARM_TWO_
				}
			}
#endif	// defined( _MITOV_DS3231_RTC_ALARM_ONE_ ) || defined( _MITOV_DS3231_RTC_ALARM_TWO_ )

//			inherited::SystemLoopBegin( currentMicros );
		}

	public:
		RTC_DS3231( TwoWire &AWire ) :
			FWire( AWire ),
			Enabled( true ),
			Halt( false )
		{
#ifdef _MITOV_DS3231_RTC_CLOCK_PIN_
			ClockInputPin.SetCallback( MAKE_CALLBACK( RTC_DS3231::DoClockReceive ));
#endif

#ifdef _MITOV_DS3231_RTC_SET_PIN_
			SetInputPin.SetCallback( MAKE_CALLBACK( RTC_DS3231::DoSetReceive ));
#endif // _MITOV_DS3231_RTC_SET_PIN_
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif // _MITOV_RTC_DS3231_h
