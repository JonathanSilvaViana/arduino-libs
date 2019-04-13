////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_CONTROLLINO_RTC_h
#define _MITOV_CONTROLLINO_RTC_h

#include <Mitov.h>
#include <Controllino.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	class ControllinoRTCModule : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SourcePin	OutputPin;

#ifdef _MITOV_CONTROLLINO_RTC_MODULE_CLOCK_PIN_
		OpenWire::ConnectSinkPin	ClockInputPin;
#endif

#ifdef _MITOV_CONTROLLINO_RTC_MODULE_SET_PIN_
		OpenWire::SinkPin	SetInputPin;
#endif

	public:
		bool	Enabled = true;

	protected:
		Mitov::TDateTime FLastDateTime;

	public:
		void SetEnabled( bool AValue )
		{
            if( Enabled == AValue )
                return;

            Enabled = AValue;
			if( Enabled )
				StartModule();

//			else
//				StopModule();

		}

		void SetMilliSecond( uint8_t AValue )
		{
		}

		void SetSecond( uint8_t AValue )
		{
			TDateTime ADateTime = GetDateTime();
			uint16_t AYear, AMonth, ADay, aWeekDay, AHour, AMinute, ASecond, AMilliSecond;
			ADateTime.DecodeDateTime( AYear, AMonth, ADay, aWeekDay, AHour, AMinute, ASecond, AMilliSecond );
			Controllino_SetTimeDate( ADay, aWeekDay, AMonth, AYear, AHour, AMinute, AValue );
		}

		void SetMinute( uint8_t AValue )
		{
			TDateTime ADateTime = GetDateTime();
			uint16_t AYear, AMonth, ADay, aWeekDay, AHour, AMinute, ASecond, AMilliSecond;
			ADateTime.DecodeDateTime( AYear, AMonth, ADay, aWeekDay, AHour, AMinute, ASecond, AMilliSecond );
			Controllino_SetTimeDate( ADay, aWeekDay, AMonth, AYear, AHour, AValue, ASecond );
		}

		void SetHour( uint8_t AValue )
		{
			TDateTime ADateTime = GetDateTime();
			uint16_t AYear, AMonth, ADay, aWeekDay, AHour, AMinute, ASecond, AMilliSecond;
			ADateTime.DecodeDateTime( AYear, AMonth, ADay, aWeekDay, AHour, AMinute, ASecond, AMilliSecond );
			Controllino_SetTimeDate( ADay, aWeekDay, AMonth, AYear, AValue, AMinute, ASecond );
		}

		void SetWeekDay( uint8_t AValue )
		{
			TDateTime ADateTime = GetDateTime();
			uint16_t AYear, AMonth, ADay, aWeekDay, AHour, AMinute, ASecond, AMilliSecond;
			ADateTime.DecodeDateTime( AYear, AMonth, ADay, aWeekDay, AHour, AMinute, ASecond, AMilliSecond );
			Controllino_SetTimeDate( ADay, AValue, AMonth, AYear, AHour, AMinute, ASecond );
		}

		void SetYear( uint16_t AValue )
		{
			TDateTime ADateTime = GetDateTime();
			uint16_t AYear, AMonth, ADay, aWeekDay, AHour, AMinute, ASecond, AMilliSecond;
			ADateTime.DecodeDateTime( AYear, AMonth, ADay, aWeekDay, AHour, AMinute, ASecond, AMilliSecond );
			Controllino_SetTimeDate( ADay, aWeekDay, AMonth, AValue, AHour, AMinute, ASecond );
		}

		Mitov::TDateTime GetDateTime()
		{
			unsigned char aDay, aWeekDay, aMonth, aYear, aHour, aMinute, aSecond;
//			Controllino_PrintTimeAndDate();

			Controllino_ReadTimeDate( &aDay, &aWeekDay, &aMonth, &aYear, &aHour, &aMinute, &aSecond );
//			Serial.println( "Test2" );
			uint16_t ALongYear = 2000 + aYear;
//			Serial.println( ALongYear );
//			Serial.println( aMonth );
//			Serial.println( aDay );
			TDateTime ADateTime;
//			ADateTime.TryEncodeDate( 2000 + aYear, aMonth, aDay );
//			ADateTime.TryEncodeTime( aHour, aMinute, aSecond, 0 );
			ADateTime.TryEncodeDateTime( ALongYear, aMonth, aDay, aHour, aMinute, aSecond, 0 );

			return ADateTime;
		}

		void SetDateTime( const Mitov::TDateTime &ADateTime )
		{
			uint16_t AYear, AMonth, ADay, aWeekDay, AHour, AMinute, ASecond, AMilliSecond;
			ADateTime.DecodeDateTime( AYear, AMonth, ADay, aWeekDay, AHour, AMinute, ASecond, AMilliSecond );
			Controllino_SetTimeDate( ADay, aWeekDay, AMonth, AYear, AHour, AMinute, ASecond );
		}


	protected:
		virtual void SystemInit() override
		{
			if( Enabled )
				StartModule();

//			inherited::SystemInit();
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( Enabled )
#ifdef _MITOV_CONTROLLINO_RTC_MODULE_CLOCK_PIN_
				if( ! ClockInputPin.IsConnected() )
#endif
					ReadTime( false );

//			inherited::SystemLoopBegin( currentMicros );
		}

/*
		void StopModule()
		{
		}
*/
		void StartModule()
		{
//			Serial.println( "START" );
//			SPI.begin();
//			Controllino_RTC_init( 65 );
			Controllino_RTC_init( 0 );
//			Controllino_RTCSSInit();
//			Controllino_SetRTCSS( true );
		}

#ifdef _MITOV_CONTROLLINO_RTC_MODULE_CLOCK_PIN_
		void DoClockReceive( void *_Data )
		{
			ReadTime( true );
		}
#endif

		void ReadTime( bool AFromClock )
		{
			if( ! OutputPin.IsConnected() )
				return;

			if( ! Enabled )
				return;

//			Serial.println( "Test1" );
			unsigned char aDay, aWeekDay, aMonth, aYear, aHour, aMinute, aSecond;
//			Controllino_PrintTimeAndDate();

			Controllino_ReadTimeDate( &aDay, &aWeekDay, &aMonth, &aYear, &aHour, &aMinute, &aSecond );
//			Serial.println( "Test2" );
			uint16_t ALongYear = 2000 + aYear;
//			Serial.println( ALongYear );
//			Serial.println( aMonth );
//			Serial.println( aDay );
			TDateTime ADateTime;
//			ADateTime.TryEncodeDate( 2000 + aYear, aMonth, aDay );
//			ADateTime.TryEncodeTime( aHour, aMinute, aSecond, 0 );
			if( ADateTime.TryEncodeDateTime( ALongYear, aMonth, aDay, aHour, aMinute, aSecond, 0 ))
			{
				if( AFromClock || ( FLastDateTime != ADateTime ))
				{
					FLastDateTime = ADateTime;
					OutputPin.Notify( &ADateTime );
				}
			}
		}

		void DoSetTimeReceive( void *_Data )
		{
			if( ! Enabled )
				return;

			uint16_t AYear, AMonth, ADay, aWeekDay, AHour, AMinute, ASecond, AMilliSecond;

//			Serial.println( "Test2" );
			((TDateTime *)_Data)->DecodeDateTime( AYear, AMonth, ADay, aWeekDay, AHour, AMinute, ASecond, AMilliSecond );
			AYear %= 100;
			Controllino_SetTimeDate( ADay, aWeekDay, AMonth, AYear, AHour, AMinute, ASecond );

		}

	public:
		ControllinoRTCModule()
		{			
#ifdef _MITOV_CONTROLLINO_RTC_MODULE_CLOCK_PIN_
			ClockInputPin.SetCallback( MAKE_CALLBACK( ControllinoRTCModule::DoClockReceive ));
#endif
			
#ifdef _MITOV_CONTROLLINO_RTC_MODULE_SET_PIN_
			SetInputPin.SetCallback( MAKE_CALLBACK( ControllinoRTCModule::DoSetTimeReceive ));
#endif
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
