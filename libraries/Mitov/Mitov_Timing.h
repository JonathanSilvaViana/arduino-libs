////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_TIMING_h
#define _MITOV_TIMING_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	class ClockGenerator : public Mitov::CommonGenerator
	{
	protected:
		virtual void Clock() override
		{
			OutputPin.Notify(nullptr);
		}

	};
//---------------------------------------------------------------------------
	class Timer : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	StartInputPin;
		OpenWire::SinkPin	ResetInputPin;
		OpenWire::SourcePin	OutputPin;

	public:
		uint32_t	Interval = 1000000;
		bool		AutoRepeat : 1;
		bool		CanRestart : 1;
		bool		Enabled : 1;

	public:
		void	UpdateEnabled()
		{
			if (Enabled)
			{
				if (AutoRepeat)
					DoReceiveStart(nullptr);
			}

			else
			{
				if (FValue)
				{
					FValue = false;
					OutputPin.SendValue(FValue);
				}
			}
		}

	protected:
		bool		FValue : 1;

		uint32_t	FStartTime;

	protected:
		virtual void SystemStart() override
		{
			//			inherited::SystemStart();
			OutputPin.SendValue(FValue);
		}

		virtual void SystemLoopBegin(unsigned long currentMicros) override
		{
			if (Enabled)
			{
				if (!AutoRepeat)
					if (!FValue)
					{
						inherited::SystemLoopBegin(currentMicros);
						return;
					}

				if (FValue)
					if (!Enabled)
					{
						FValue = false;
						OutputPin.SendValue(FValue);
						//						inherited::SystemLoopBegin( currentMicros );
						return;
					}

				currentMicros = micros(); // Use micros(); to make sure the DoReceiveStart is not from the same clock event!
				if (currentMicros - FStartTime >= Interval)
				{
					FValue = !FValue;
					OutputPin.SendValue(FValue);
					if (AutoRepeat)
						FStartTime += Interval;
				}
			}

			//			inherited::SystemLoopBegin( currentMicros );
		}

	protected:
		virtual void DoReceiveStart(void *_Data)
		{
			if (!Enabled)
				return;

			if (!CanRestart)
				if (FValue)
					return;

			FStartTime = micros();
			if (FValue)
				return;

			FValue = true;
			OutputPin.SendValue(FValue);
		}

		void DoReceiveClear(void *_Data)
		{
			if (FValue)
			{
				FValue = false;
				OutputPin.SendValue(FValue);

				if (AutoRepeat)
					FStartTime = micros();
			}
		}

	public:
		Timer() :
			AutoRepeat(false),
			CanRestart(true),
			Enabled(true),
			FValue(false)
		{
			StartInputPin.SetCallback(this, (OpenWire::TOnPinReceive)&Timer::DoReceiveStart);
			ResetInputPin.SetCallback(this, (OpenWire::TOnPinReceive)&Timer::DoReceiveClear);
		}
	};
//---------------------------------------------------------------------------
	class Repeat : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool	Enabled = true;

	protected:
		virtual void SystemLoopBegin(unsigned long currentMicros) override
		{
			if (Enabled)
				OutputPin.Notify(nullptr);

			//			inherited::SystemLoopBegin( currentMicros );
		}

	};
//---------------------------------------------------------------------------
	class Start : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	protected:
		virtual void SystemStart() override
		{
			OutputPin.Notify(nullptr);
			//			inherited::SystemStart();
		}

	};
//---------------------------------------------------------------------------
	class MultiSource : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		Mitov::SimpleList<OpenWire::SourcePin> OutputPins;

	public:
		OpenWire::SinkPin	InputPin;

	protected:
		void DoReceive(void *_Data)
		{
			for (int i = 0; i < OutputPins.size(); ++i)
				OutputPins[i].Notify(_Data);
		}

	public:
		MultiSource()
		{
			InputPin.SetCallback(this, (OpenWire::TOnPinReceive)&MultiSource::DoReceive);
		}
	};
//---------------------------------------------------------------------------
	class RepeatSource : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

	public:
		bool	Enabled = true;
		uint32_t	Count = 1;

	protected:
		void DoReceive(void *_Data)
		{
			OutputPin.Notify(_Data);
			if (!Enabled)
				return;

			for (uint32_t i = 0; i < Count; ++i)
				OutputPin.Notify(_Data);
		}

	public:
		RepeatSource()
		{
			InputPin.SetCallback(this, (OpenWire::TOnPinReceive)&RepeatSource::DoReceive);
		}
	};
//---------------------------------------------------------------------------
	class RepeatSource_Interval : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

	public:
		bool	Enabled = true;
		uint32_t	Count = 1;
		uint32_t	Interval = 0;

	protected:
		uint32_t	FRemainingCount = 0;
		uint32_t	FStartTime;

	protected:
		void DoReceive(void *_Data)
		{
			OutputPin.Notify(_Data);
			if (!Enabled)
				return;

			if (!Interval)
			{
				for (uint32_t i = 0; i < Count; ++i)
					OutputPin.Notify(_Data);

				return;
			}

			FStartTime = micros();
			FRemainingCount = Count;
		}

	protected:
		virtual void SystemLoopBegin(unsigned long currentMicros) override
		{
			if (!Enabled)
				return;

			if (!FRemainingCount)
				return;

			uint32_t ACurrentTime = micros();
			if (ACurrentTime - FStartTime < Interval)
				return;

			OutputPin.Notify(nullptr);
			--FRemainingCount;
			FStartTime = ACurrentTime;

			//			inherited::SystemLoopBegin( currentMicros );
		}
	public:
		RepeatSource_Interval()
		{
			InputPin.SetCallback(this, (OpenWire::TOnPinReceive)&RepeatSource_Interval::DoReceive);
		}
	};
//---------------------------------------------------------------------------
	class Delay : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	StartInputPin;
		OpenWire::SinkPin	ResetInputPin;
		OpenWire::SourcePin	OutputPin;

	public:
		unsigned long	Interval = 1000000;
		bool			CanRestart : 1;
		bool			Enabled : 1;


	protected:
		bool			FValue : 1;
		unsigned long	FStartTime;

	protected:
		virtual void SystemLoopBegin(unsigned long currentMicros) override
		{
			if (FValue)
			{
				if (!Enabled)
					FValue = false;

				else
				{
					unsigned long ACurrentMicros = micros(); // Use micros(); to make sure the DoReceiveStart is not from the same clock event!
					if (ACurrentMicros - FStartTime >= Interval)
					{
						FValue = false;
						OutputPin.Notify(nullptr);
					}
				}
			}

			//			inherited::SystemLoopBegin( currentMicros );
		}

	protected:
		virtual void DoReceiveStart(void *_Data)
		{
			if (!Enabled)
				return;

			if (!CanRestart)
				if (FValue)
					return;

			FStartTime = micros();
			if (FValue)
				return;

			FValue = true;
		}

		void DoReceiveClear(void *_Data)
		{
			FValue = false;
		}

	public:
		Delay() :
			CanRestart(true),
			Enabled(true),
			FValue(false)
		{
			StartInputPin.SetCallback(this, (OpenWire::TOnPinReceive)&Delay::DoReceiveStart);
			ResetInputPin.SetCallback(this, (OpenWire::TOnPinReceive)&Delay::DoReceiveClear);
		}
	};
//---------------------------------------------------------------------------
	class OnOffDelay : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

	public:
		unsigned long	OnInterval = 1000000;
		unsigned long	OffInterval = 1000000;
		bool			Enabled : 1;
		bool			InitialValue : 1;

	protected:
		bool			FInputValue : 1;
		unsigned long	FStartTime;

	protected:
		void DoReceive(void *_Data)
		{
			bool AValue = *(bool *)_Data;
			if (FInputValue == AValue)
				return;

			FInputValue = AValue;
			FStartTime = micros();
		}

	protected:
		virtual void SystemStart() override
		{
			FInputValue = InitialValue;
			OutputPin.SendValue(FInputValue);
			//			inherited::SystemStart();
		}

		virtual void SystemLoopBegin(unsigned long ACurrentMicros) override
		{
			if (FInputValue == InitialValue)
				return;

			unsigned long AInterval = (FInputValue) ? OnInterval : OffInterval;
			if (micros() - FStartTime < AInterval) // Use micros() to avoid problems when started from the same loop!
				return;

			InitialValue = FInputValue;
			OutputPin.SendValue(FInputValue);
		}

	public:
		OnOffDelay() :
			Enabled(true),
			InitialValue(false)
			//			FInputValue( false )

		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&OnOffDelay::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	class DetectEdge : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

	public:
		bool	Enabled : 1;
		bool	Rising : 1;

	protected:
		bool	FLastValue : 1;

	protected:
		void DoReceive(void *_Data)
		{
			bool AValue = *(bool *)_Data;
			if (AValue == FLastValue)
				return;

			FLastValue = AValue;
			if (Rising == AValue)
				OutputPin.Notify(nullptr);

		}

	public:
		DetectEdge() :
			Enabled(true),
			Rising(true),
			FLastValue(false)
		{
			InputPin.SetCallback(this, (OpenWire::TOnPinReceive)&DetectEdge::DoReceive);
		}

	};
//---------------------------------------------------------------------------
	class CPUTimeMicrosMillis : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SourcePin	MicroSecondsOutputPin;
		OpenWire::SourcePin	MilliSecondsOutputPin;

	protected:
		void SendOutputs()
		{
			MicroSecondsOutputPin.SendValue<uint32_t>(micros());
			MilliSecondsOutputPin.SendValue<uint32_t>(millis());
		}

	protected:
		virtual void SystemStart() override
		{
			SendOutputs();
			//			inherited::SystemStart();
		}

		virtual void SystemLoopBegin(unsigned long ACurrentMicros) override
		{
			SendOutputs();
		}
	};
//---------------------------------------------------------------------------
	class CPUTimeMicros : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SourcePin	MicroSecondsOutputPin;

	protected:
		inline void SendOutputs()
		{
			MicroSecondsOutputPin.SendValue<uint32_t>(micros());
		}

	protected:
		virtual void SystemStart() override
		{
			SendOutputs();
			//			inherited::SystemStart();
		}

		virtual void SystemLoopBegin(unsigned long ACurrentMicros) override
		{
			SendOutputs();
		}
	};
//---------------------------------------------------------------------------
	class CPUTimeMillis : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SourcePin	MilliSecondsOutputPin;

	protected:
		inline void SendOutputs()
		{
			MilliSecondsOutputPin.SendValue<uint32_t>(millis());
		}

	protected:
		virtual void SystemStart() override
		{
			SendOutputs();
			//			inherited::SystemStart();
		}

		virtual void SystemLoopBegin(unsigned long ACurrentMicros) override
		{
			SendOutputs();
		}
	};
//---------------------------------------------------------------------------
	class CPUTimeMicrosMillis_Enable : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SourcePin	MicroSecondsOutputPin;
		OpenWire::SourcePin	MilliSecondsOutputPin;

	public:
		bool	Enabled = true;

	protected:
		void SendOutputs()
		{
			if (Enabled)
			{
				MicroSecondsOutputPin.SendValue<uint32_t>(micros());
				MilliSecondsOutputPin.SendValue<uint32_t>(millis());
			}
		}

	protected:
		virtual void SystemStart() override
		{
			SendOutputs();
			//			inherited::SystemStart();
		}

		virtual void SystemLoopBegin(unsigned long ACurrentMicros) override
		{
			SendOutputs();
		}
	};
//---------------------------------------------------------------------------
	class CPUTimeMicros_Enable : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SourcePin	MicroSecondsOutputPin;

	public:
		bool	Enabled = true;

	protected:
		inline void SendOutputs()
		{
			if (Enabled)
				MicroSecondsOutputPin.SendValue<uint32_t>(micros());

		}

	protected:
		virtual void SystemStart() override
		{
			SendOutputs();
			//			inherited::SystemStart();
		}

		virtual void SystemLoopBegin(unsigned long ACurrentMicros) override
		{
			SendOutputs();
		}
	};
//---------------------------------------------------------------------------
	class CPUTimeMillis_Enable : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SourcePin	MilliSecondsOutputPin;

	public:
		bool	Enabled = true;

	protected:
		inline void SendOutputs()
		{
			if (Enabled)
				MilliSecondsOutputPin.SendValue<uint32_t>(millis());

		}

	protected:
		virtual void SystemStart() override
		{
			SendOutputs();
			//			inherited::SystemStart();
		}

		virtual void SystemLoopBegin(unsigned long ACurrentMicros) override
		{
			SendOutputs();
		}
	};
//---------------------------------------------------------------------------
	class CPUTimeMicrosMillis_Clocked : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SourcePin	MicroSecondsOutputPin;
		OpenWire::SourcePin	MilliSecondsOutputPin;

		OpenWire::SinkPin	ClockInputPin;

	protected:
		void DoClockReceive(void *_Data)
		{
			MicroSecondsOutputPin.SendValue<uint32_t>(micros());
			MilliSecondsOutputPin.SendValue<uint32_t>(millis());
		}

	public:
		CPUTimeMicrosMillis_Clocked()
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&CPUTimeMicrosMillis_Clocked::DoClockReceive );
		}
	};
//---------------------------------------------------------------------------
	class CPUTimeMicros_Clocked : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SourcePin	MicroSecondsOutputPin;

		OpenWire::SinkPin	ClockInputPin;

	protected:
		void DoClockReceive(void *_Data)
		{
			MicroSecondsOutputPin.SendValue<uint32_t>(micros());
		}

	public:
		CPUTimeMicros_Clocked()
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&CPUTimeMicros_Clocked::DoClockReceive );
		}
	};
//---------------------------------------------------------------------------
	class CPUTimeMillis_Clocked : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SourcePin	MilliSecondsOutputPin;

		OpenWire::SinkPin	ClockInputPin;

	protected:
		void DoClockReceive(void *_Data)
		{
			MilliSecondsOutputPin.SendValue<uint32_t>(millis());
		}

	public:
		CPUTimeMillis_Clocked()
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&CPUTimeMillis_Clocked::DoClockReceive );
		}
	};
//---------------------------------------------------------------------------
	class CPUTimeMicrosMillis_Enable_Clocked : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SourcePin	MicroSecondsOutputPin;
		OpenWire::SourcePin	MilliSecondsOutputPin;

		OpenWire::SinkPin	ClockInputPin;

	public:
		bool	Enabled = true;

	protected:
		void DoClockReceive(void *_Data)
		{
			if (Enabled)
			{
				MicroSecondsOutputPin.SendValue<uint32_t>(micros());
				MilliSecondsOutputPin.SendValue<uint32_t>(millis());
			}
		}

	public:
		CPUTimeMicrosMillis_Enable_Clocked()
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&CPUTimeMicrosMillis_Enable_Clocked::DoClockReceive );
		}
	};
//---------------------------------------------------------------------------
	class CPUTimeMicros_Enable_Clocked : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SourcePin	MicroSecondsOutputPin;

		OpenWire::SinkPin	ClockInputPin;

	public:
		bool	Enabled = true;

	protected:
		void DoClockReceive(void *_Data)
		{
			if (Enabled)
				MicroSecondsOutputPin.SendValue<uint32_t>(micros());
		}

	public:
		CPUTimeMicros_Enable_Clocked()
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&CPUTimeMicros_Enable_Clocked::DoClockReceive );
		}
	};
//---------------------------------------------------------------------------
	class CPUTimeMillis_Enable_Clocked : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SourcePin	MilliSecondsOutputPin;

		OpenWire::SinkPin	ClockInputPin;

	public:
		bool	Enabled = true;

	protected:
		void DoClockReceive(void *_Data)
		{
			if (Enabled)
				MilliSecondsOutputPin.SendValue<uint32_t>(millis());
		}

	public:
		CPUTimeMillis_Enable_Clocked()
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&CPUTimeMillis_Enable_Clocked::DoClockReceive );
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
