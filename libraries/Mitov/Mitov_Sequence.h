////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_SEQUENCE_h
#define _MITOV_SEQUENCE_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	class Sequence;
//---------------------------------------------------------------------------
	class SequenceElement : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		uint32_t	Delay;

	protected:
		Sequence	&FOwner;

#ifdef _SEQUENCE_DIGIAL_ELEMENT_
	public:
		bool		FIsDigital;
#endif // _SEQUENCE_DIGIAL_ELEMENT_

	public:
		SequenceElement( Sequence &AOwner, bool AIsDigital, uint32_t ADelay );

	};
//---------------------------------------------------------------------------
	class SequenceClockedElement : public SequenceElement
	{
		typedef SequenceElement inherited;

	public:
		OpenWire::SinkPin	ClockInputPin;

	public:
		void DoClockReceive( void *_Data );

	public:
		SequenceClockedElement( Sequence &AOwner, bool AIsDigital, uint32_t ADelay );

	};
//---------------------------------------------------------------------------
	class Sequence : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::ConnectSinkPin	StartInputPin;

	public:
		bool	Enabled : 1;
		bool	Repeat : 1;
		bool	CanRestart : 1;

	protected:
		bool	IsEnabled : 1;
//		SequenceClockedElement *FCurrentElement = nullptr;
		int16_t		FCurrentIndex = -1;
		uint32_t	FStartTime;

	public:
		SimpleList<SequenceElement *, uint16_t>	FElements;

	public:
		void SignalElement( SequenceElement *AElement )
		{
			if( ! Enabled )
				return;

			if( FCurrentIndex < 0 )
				return;

			SequenceElement *AElementItem = FElements[ FCurrentIndex ];

//			Serial.print( "SignalElement: " );
//			Serial.println( FCurrentIndex );

			if( AElement != AElementItem )
				return;

//			FElements[ FCurrentIndex ]->DoClockReceive( nullptr );
			AElementItem->OutputPin.SendValue<bool>( false );
			if( ++FCurrentIndex < FElements.size() )
				FStartTime = millis();

			else if( Repeat )
				DoStartReceive( nullptr );

			else
			{
				FCurrentIndex = -1;
				return;
			}

#ifdef _SEQUENCE_DIGIAL_ELEMENT_
			AElementItem = FElements[ FCurrentIndex ];
			if( AElementItem->FIsDigital )
				AElementItem->OutputPin.SendValue<bool>( true );
#endif // _SEQUENCE_DIGIAL_ELEMENT_


//			Serial.print( "FCurrentIndex: " ); Serial.println( FCurrentIndex );
		}

	protected:
		void DoStartReceive( void *_Data )
		{
			if( Enabled )
				if( FElements.size() )
					if( CanRestart || ( FCurrentIndex == -1 ))
					{
						FCurrentIndex = 0;
						FStartTime = millis();
					}

		}

	public:
		void SystemStart()
		{
			if( ! StartInputPin.IsConnected() )
				DoStartReceive( nullptr );
//				OutputPin.Notify( &FValue );

//			inherited::SystemStart();
		}

		void SystemLoopBegin( unsigned long currentMicros )
		{
#ifdef _SEQUENCE_DIGIAL_ELEMENT_
			if( IsEnabled != Enabled )
			{
				SequenceElement *AElementItem = FElements[ FCurrentIndex ];
				if( AElementItem->FIsDigital )
					AElementItem->OutputPin.SendValue<bool>( Enabled );

				IsEnabled = Enabled;
			}
#endif // _SEQUENCE_DIGIAL_ELEMENT_

			if( Enabled )
				if( FCurrentIndex >= 0 )
					if( ( millis() - FStartTime ) >= FElements[ FCurrentIndex ]->Delay )
						SignalElement( FElements[ FCurrentIndex ] );

/*
			if( FDetectTimerRunning )
				if( ( millis() - FDetectStartTime ) >= PacketDetectInterval )
					TryProcessAcumulattor();

			if( FTimeoutTimerRunning )
				if( ( millis() - FTimeoutStartTime ) >= ResponseTimeout )
				{
					// Timeout
					FDetectTimerRunning = false;

					if( FCurrentTransaction )
						FCurrentTransaction->Release();

					FCurrentTransaction = nullptr;
					FReceivedData.clear();
					TrySend();
				}
*/
//			inherited::SystemLoopBegin( currentMicros );
		}

	public:
		Sequence() :
			Enabled( true ),
			IsEnabled( true ),
			Repeat( false ),
			CanRestart( true )
		{
			StartInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&Sequence::DoStartReceive );
		}

	};
//---------------------------------------------------------------------------
	SequenceElement::SequenceElement( Sequence &AOwner, bool AIsDigital, uint32_t ADelay ) :
		FOwner( AOwner ),
		Delay( ADelay )
#ifdef _SEQUENCE_DIGIAL_ELEMENT_
		, FIsDigital( AIsDigital )
#endif // _SEQUENCE_DIGIAL_ELEMENT_
	{
		FOwner.FElements.push_back( this );
	}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
	SequenceClockedElement::SequenceClockedElement( Sequence &AOwner, bool AIsDigital, uint32_t ADelay ) :
		inherited( AOwner, AIsDigital, ADelay )
	{
		ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&SequenceClockedElement::DoClockReceive );
	}
//---------------------------------------------------------------------------
	void SequenceClockedElement::DoClockReceive( void *_Data )
	{
		FOwner.SignalElement( this );		
	}
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
