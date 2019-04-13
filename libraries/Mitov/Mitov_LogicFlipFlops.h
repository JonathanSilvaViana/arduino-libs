////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_LOGIC_FLIP_FLOPS_h
#define _MITOV_LOGIC_FLIP_FLOPS_h

#include <Mitov.h>

namespace Mitov
{
	class BasicFlipFlop : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SourcePin	OutputPin;
		OpenWire::SourcePin	InvertedOutputPin;

	public:
		bool	InitialValue = false;

	protected:
		void SetValue( bool AValue )
		{
			InitialValue = AValue;
			SendOutput();
		}

		void SendOutput()
		{
			OutputPin.SendValue( InitialValue );
			InvertedOutputPin.SendValue( !InitialValue );
		}

	protected:
		virtual void SystemStart() override
		{
//			inherited::SystemStart();
			SendOutput();
		}

	};
//---------------------------------------------------------------------------
	class SRFlipFlop : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SourcePin	OutputPin;
		OpenWire::SourcePin	InvertedOutputPin;

		OpenWire::SinkPin	SetInputPin;
		OpenWire::SinkPin	ResetInputPin;

	public:
		bool	InitialValue = false;

	protected:
		void SetValue( bool AValue )
		{
			InitialValue = AValue;
			SendOutput();
		}

		void SendOutput()
		{
			OutputPin.SendValue( InitialValue );
			InvertedOutputPin.SendValue( !InitialValue );
		}

	protected:
		void DoReceiveSet( void *_Data )
		{
			SetValue( true );
		}

		void DoReceiveReset( void *_Data )
		{
			SetValue( false );
		}

	protected:
		virtual void SystemStart() override
		{
//			inherited::SystemStart();
			SendOutput();
		}

	public:
		SRFlipFlop()
		{
			SetInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&SRFlipFlop::DoReceiveSet );
			ResetInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&SRFlipFlop::DoReceiveReset );
		}
	};
//---------------------------------------------------------------------------
	class ClockableFlipFlop : public Mitov::SRFlipFlop
	{
		typedef Mitov::SRFlipFlop inherited;

	public:
		OpenWire::SinkPin	ClockInputPin;

	protected:
		virtual void DoClock() = 0;

	protected:
		void DoClockReceive( void *_Data )
		{
			DoClock();
		}

	public:
		ClockableFlipFlop()
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ClockableFlipFlop::DoClockReceive );
		}

	};
//---------------------------------------------------------------------------
	class TFlipFlop : public Mitov::ClockableFlipFlop
	{
		typedef Mitov::ClockableFlipFlop inherited;

	public:
		OpenWire::SinkPin	ToggleInputPin;

	protected:
		bool FToggleValue = true;

	protected:
		void DoReceiveToggle( void *_Data )
		{
			FToggleValue = *(bool *)_Data;
		}

	protected:
		virtual void DoClock() override
		{
			if( FToggleValue )
				SetValue( !InitialValue );
		}

	public:
		TFlipFlop()
		{
			ToggleInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&TFlipFlop::DoReceiveToggle );
		}

	};
//---------------------------------------------------------------------------
	class DFlipFlop : public Mitov::ClockableFlipFlop
	{
		typedef Mitov::ClockableFlipFlop inherited;

	public:
		OpenWire::SinkPin	DataInputPin;

	protected:
		bool FData = false;

	protected:
		virtual void DoClock() override
		{
			SetValue( FData );
		}

	protected:
		void DoReceiveData( void *_Data )
		{
			FData = *(bool *)_Data;
		}

	public:
		DFlipFlop()
		{
			DataInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&DFlipFlop::DoReceiveData );
		}

	};
//---------------------------------------------------------------------------
	class JKFlipFlop : public Mitov::BasicFlipFlop
	{
	public:
		OpenWire::SinkPin	JInputPin;
		OpenWire::SinkPin	KInputPin;
		OpenWire::SinkPin	ClockInputPin;

	protected:
		bool FJ : 1;
		bool FK : 1;

	protected:
		void DoReceiveJ( void *_Data )
		{
			FJ = *(bool *)_Data;
		}

		void DoReceiveK( void *_Data )
		{
			FK = *(bool *)_Data;
		}

		void DoClockReceive( void *_Data )
		{
			if( FJ && FK )
				SetValue( !InitialValue );

			else if( FJ )
				SetValue( true );

			else if( FK )
				SetValue( false );

		}

	public:
		JKFlipFlop() :
			FJ( false ),
			FK( false )
		{
			JInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&JKFlipFlop::DoReceiveJ );
			KInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&JKFlipFlop::DoReceiveK );
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&JKFlipFlop::DoClockReceive );
		}

	};
//---------------------------------------------------------------------------
}

#endif
