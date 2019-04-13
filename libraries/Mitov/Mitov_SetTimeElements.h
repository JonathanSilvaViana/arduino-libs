////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_SET_TIME_ELEMENTS_h
#define _MITOV_SET_TIME_ELEMENTS_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	template<typename T> class RTCSetMilliSecond : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	ClockInputPin;

	public:
		bool	AddValue = false;
		int32_t	Value = 0;

	protected:
		T &FOwner;

	protected:
		void DoClockReceive( void *_Data )
		{
			if( AddValue )
			{
				TDateTime ADateTime = FOwner.GetDateTime();
				ADateTime.AddMilliSeconds( Value );
				FOwner.SetDateTime( ADateTime );
				return;
			}

			int32_t AValue = Value % 1000;
			if( AValue < 0 )
				AValue += 1000;

			FOwner.SetMilliSecond( AValue );
		}

	public:
		RTCSetMilliSecond( T &AOwner ) :
			FOwner( AOwner )
		{
			ClockInputPin.SetCallback( MAKE_CALLBACK( RTCSetMilliSecond::DoClockReceive ));
		}
	};
//---------------------------------------------------------------------------
	template<typename T> class RTCSetSecond : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	ClockInputPin;

	public:
		bool	AddValue = false;
		int32_t	Value = 0;

	protected:
		T &FOwner;

	protected:
		void DoClockReceive( void *_Data )
		{
			if( AddValue )
			{
				TDateTime ADateTime = FOwner.GetDateTime();
				ADateTime.AddSeconds( Value );
				FOwner.SetDateTime( ADateTime );
				return;
			}

			int32_t AValue = Value % 60;
			if( AValue < 0 )
				AValue += 60;

			FOwner.SetSecond( AValue );
		}

	public:
		RTCSetSecond( T &AOwner ) :
			FOwner( AOwner )
		{
			ClockInputPin.SetCallback( MAKE_CALLBACK( RTCSetSecond::DoClockReceive ));
		}
	};
//---------------------------------------------------------------------------
	template<typename T> class RTCSetMinute : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	ClockInputPin;

	public:
		bool	AddValue = false;
		int32_t	Value = 0;

	protected:
		T &FOwner;

	protected:
		void DoClockReceive( void *_Data )
		{
			if( AddValue )
			{
				TDateTime ADateTime = FOwner.GetDateTime();
				ADateTime.AddMinutes( Value );
				FOwner.SetDateTime( ADateTime );
				return;
			}

			int32_t AValue = Value % 60;
			if( AValue < 0 )
				AValue += 60;

			FOwner.SetMinute( AValue );
		}

	public:
		RTCSetMinute( T &AOwner ) :
			FOwner( AOwner )
		{
			ClockInputPin.SetCallback( MAKE_CALLBACK( RTCSetMinute::DoClockReceive ));
		}
	};
//---------------------------------------------------------------------------
	template<typename T> class RTCSetHour : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	ClockInputPin;

	public:
		bool	AddValue = false;
		int32_t	Value = 0;

	protected:
		T &FOwner;

	protected:
		void DoClockReceive( void *_Data )
		{
			if( AddValue )
			{
				TDateTime ADateTime = FOwner.GetDateTime();
				ADateTime.AddHours( Value );
				FOwner.SetDateTime( ADateTime );
				return;
			}

			int32_t AValue = Value % 24;
			if( AValue < 0 )
				AValue += 24;

			FOwner.SetHour( AValue );
		}

	public:
		RTCSetHour( T &AOwner ) :
			FOwner( AOwner )
		{
			ClockInputPin.SetCallback( MAKE_CALLBACK( RTCSetHour::DoClockReceive ));
		}
	};
//---------------------------------------------------------------------------
	template<typename T> class RTCSetDay : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	ClockInputPin;

	public:
		bool	AddValue = false;
		int32_t	Value = 1;

	protected:
		T &FOwner;

	protected:
		void DoClockReceive( void *_Data )
		{
			TDateTime ADateTime = FOwner.GetDateTime();
			if( AddValue )
				ADateTime.AddDays( Value );

			else
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
				ADay = Value;
				if( ! ADateTime.TryEncodeDateTime( AYear, AMonth, ADay, AHour, AMinute, ASecond, AMilliSecond ))
					return;

			}

			FOwner.SetDateTime( ADateTime );
		}

	public:
		RTCSetDay( T &AOwner ) :
			FOwner( AOwner )
		{
			ClockInputPin.SetCallback( MAKE_CALLBACK( RTCSetDay::DoClockReceive ));
		}
	};
//---------------------------------------------------------------------------
	template<typename T> class RTCSetMonth : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	ClockInputPin;

	public:
		bool	AddValue = false;
		int32_t	Value = 1;

	protected:
		T &FOwner;

	protected:
		void DoClockReceive( void *_Data )
		{
			TDateTime ADateTime = FOwner.GetDateTime();
			if( AddValue )
				ADateTime.AddDays( Value );

			else
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
				AMonth = Value;
				if( ! ADateTime.TryEncodeDateTime( AYear, AMonth, ADay, AHour, AMinute, ASecond, AMilliSecond ))
					return;

			}

			FOwner.SetDateTime( ADateTime );
		}

	public:
		RTCSetMonth( T &AOwner ) :
			FOwner( AOwner )
		{
			ClockInputPin.SetCallback( MAKE_CALLBACK( RTCSetMonth::DoClockReceive ));
		}
	};
//---------------------------------------------------------------------------
	template<typename T> class RTCSetYear : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	ClockInputPin;

	public:
		bool	AddValue = false;
		int32_t	Value = 0;

	protected:
		T &FOwner;

	protected:
		void DoClockReceive( void *_Data )
		{
			if( AddValue )
			{
				TDateTime ADateTime = FOwner.GetDateTime();
				ADateTime.AddYears( Value );
				FOwner.SetDateTime( ADateTime );
				return;
			}

			FOwner.SetYear( Value );
		}

	public:
		RTCSetYear( T &AOwner ) :
			FOwner( AOwner )
		{
			ClockInputPin.SetCallback( MAKE_CALLBACK( RTCSetYear::DoClockReceive ));
		}
	};
//---------------------------------------------------------------------------
	template<typename T> class RTCSetWeekDay : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	ClockInputPin;

	public:
		int32_t	Value = 0;

	protected:
		T &FOwner;

	protected:
		void DoClockReceive( void *_Data )
		{
			FOwner.SetWeekDay( Value );
		}

	public:
		RTCSetWeekDay( T &AOwner ) :
			FOwner( AOwner )
		{
			ClockInputPin.SetCallback( MAKE_CALLBACK( RTCSetWeekDay::DoClockReceive ));
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
