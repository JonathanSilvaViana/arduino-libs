////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ESP8266_PROCESSOR_h
#define _MITOV_ESP8266_PROCESSOR_h

#include <Mitov.h>
#include <Esp.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	class ESP8266ProcessorReset : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	ClockInputPin;

	protected:
		void DoClockReceive( void * )
		{
			ESP.restart();
		}

	public:
		ESP8266ProcessorReset()
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ESP8266ProcessorReset::DoClockReceive );
		}

	};
//---------------------------------------------------------------------------
	class ESP8266GetProcessorResetReason : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SourcePin OutputPin;

	protected:
		virtual void SystemStart() override
		{
//			inherited::SystemStart();
			OutputPin.SendValue( ESP.getResetReason());
		}

	};
//---------------------------------------------------------------------------
	class ESP8266GetProcessorResetReasonClocked : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	ClockInputPin;
		OpenWire::SourcePin OutputPin;

	protected:
		void DoClockReceive( void * )
		{
			OutputPin.SendValue( ESP.getResetReason());
		}

	public:
		ESP8266GetProcessorResetReasonClocked()
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ESP8266GetProcessorResetReasonClocked::DoClockReceive );
		}

	};
//---------------------------------------------------------------------------
	class ESP8266GetProcessorResetInfo : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SourcePin OutputPin;

	protected:
		virtual void SystemStart() override
		{
//			inherited::SystemStart();
			OutputPin.SendValue( ESP.getResetInfo());
		}

	};
//---------------------------------------------------------------------------
	class ESP8266GetProcessorResetInfoClocked : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	ClockInputPin;
		OpenWire::SourcePin OutputPin;

	protected:
		void DoClockReceive( void * )
		{
			OutputPin.SendValue( ESP.getResetInfo());
		}

	public:
		ESP8266GetProcessorResetInfoClocked()
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ESP8266GetProcessorResetInfoClocked::DoClockReceive );
		}

	};
//---------------------------------------------------------------------------
	enum TArduinoESP8266ProcessorModuleDeepSleepMode { dsmDefault = int( RF_DEFAULT ), dsmCalibration = int( RF_CAL ), dsmNoCalibration = int( RF_NO_CAL ), dsmDisableRF = int( RF_DISABLED ) };
//---------------------------------------------------------------------------
	class ESP8266ProcessorDeepSleep : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	ClockInputPin;

	public:
		TArduinoESP8266ProcessorModuleDeepSleepMode	Mode = dsmDefault;
		uint32_t	Period = 1000000;

	protected:
		void DoClockReceive( void * )
		{
			ESP.deepSleep( Period, RFMode( Mode ));
		}

	public:
		ESP8266ProcessorDeepSleep()
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ESP8266ProcessorDeepSleep::DoClockReceive );
		}

	};
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
