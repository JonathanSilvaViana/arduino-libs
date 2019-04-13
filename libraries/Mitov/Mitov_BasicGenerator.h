////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_BASIC_GENERATOR_h
#define _MITOV_BASIC_GENERATOR_h

#include <Mitov.h>

namespace Mitov
{
	template<typename T> class BasicGenerator : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::ConnectSinkPin	ClockInputPin;
		OpenWire::SourcePin	OutputPin;

	public:
		bool	Enabled = true;

	public: // Needs to be public due to compiler bug :-(
		T	FValue = T( 0 );

	protected:
		virtual void DoClockReceive( void *_Data ) = 0;

	protected:
		inline void SendOutput() // Needed to be due to compiler bug :-(
		{
			 OutputPin.Notify( &FValue );
		}

	public:
		BasicGenerator()
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&BasicGenerator::DoClockReceive );
		}

	};
//---------------------------------------------------------------------------
	template<typename T> class BasicFrequencyGenerator : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::ConnectSinkPin	ClockInputPin;
		OpenWire::SourcePin	OutputPin;

	public:
		bool	Enabled = true;
		float	Frequency = 1.0f;
		T	Amplitude;
		T	Offset;

		// 0 - 1
		float	Phase = 0.0f;

	public: // Needs to be public due to compiler bug :-(
		T	FValue = T( 0 );

	public:
		void SetFrequency( float AValue )
		{
			if( Frequency == AValue )
				return;

			Frequency = AValue;
//			inherited::CalculateFields();
		}

		void SetPhase( float AValue )
		{
			if( Phase == AValue )
				return;

			Phase = AValue;
			FPhase = Phase;
		}

	protected:
		float	FPhase = 0.0f;
		unsigned long FLastTime = 0;
		
	protected:
		virtual void SystemStart() override
		{
			FPhase = Phase;

//			inherited::SystemStart();
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( ! ClockInputPin.IsConnected() )
				Generate( currentMicros );

//			inherited::SystemLoopBegin( currentMicros );
		}

		void Generate( unsigned long currentMicros )
		{
			if( Enabled && ( Frequency != 0 ) )
			{
				float APeriod = 1000000 / Frequency;

				float ATime = ( currentMicros - FLastTime );
				ATime /= APeriod;
				FPhase += ATime;
				FPhase = V_FMOD( FPhase, 1 );

				CalculateValue();
			}

			FLastTime = currentMicros;
			OutputPin.Notify( &FValue );
		}

	protected:
		virtual void CalculateValue() = 0;

		void DoClockReceive( void *_Data )
		{
			Generate( micros() );
		}

	public:
		BasicFrequencyGenerator( T AAmplitude, T AOffset ) :
			Amplitude( AAmplitude ),
			Offset( AOffset )
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&BasicFrequencyGenerator::DoClockReceive );
		}

	};
//---------------------------------------------------------------------------
}

#endif
