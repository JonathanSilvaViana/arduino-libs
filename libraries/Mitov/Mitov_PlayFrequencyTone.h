////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_PLAY_FREQUENCY_TONE_h
#define _MITOV_PLAY_FREQUENCY_TONE_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	class PlayFrequencyTone : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;

	public:
		float	InitialFrequency = 0.0; //100000.0;
		bool	Enabled : 1; // = true;

	protected:
		uint8_t	FPin : 7;

	public:
		void UpdateEnabled()
		{
			if( Enabled )
				UpdateFrequency();

			else
				noTone( FPin );
		}

	protected:
		void UpdateFrequency()
		{
			if( InitialFrequency == 0.0 )
				noTone( FPin );

			else
				tone( FPin, InitialFrequency );
		}

	protected:
		void DoReceive( void *_Data )
		{
			if( ! Enabled )
				return;

			float AValue = *((float *)_Data);
			if( AValue < 0.0 )
				AValue = 0.0;

			if( InitialFrequency == AValue )
				return;

			InitialFrequency = AValue;
			UpdateFrequency();
//			FServo.write( AValue );
		}

	protected:
		virtual void SystemStart() override
		{
//			inherited::SystemStart();
			UpdateFrequency();
		}

	public:
		PlayFrequencyTone( uint8_t APin ) :
			Enabled( true ),
			FPin( APin )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&PlayFrequencyTone::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	class PlayFrequencyToneDuration : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;

	public:
		float	InitialFrequency = 0.0; //100000.0;
		bool	Enabled : 1; // = true;

	protected:
		uint8_t		FPin : 7;
		uint32_t	FCurentDuration = 0;

	public:
		void UpdateEnabled()
		{
			if( Enabled )
				UpdateFrequency();

			else
				noTone( FPin );
		}

	protected:
		void UpdateFrequency()
		{
			if( InitialFrequency == 0.0 )
				noTone( FPin );

			else if( FCurentDuration )
				tone( FPin, InitialFrequency, FCurentDuration );

			else
				tone( FPin, InitialFrequency );
		}

	public:
		void ExternalSetValue( float AFrequency, uint32_t ADuration )
		{
			if( ! ADuration )
				if( ( InitialFrequency == AFrequency ) && ( ! FCurentDuration ))
					return;

			InitialFrequency = AFrequency;
			FCurentDuration = ADuration;
			UpdateEnabled();
		}

	protected:
		void DoReceive( void *_Data )
		{
			if( ! Enabled )
				return;

			float AValue = *((float *)_Data);
			if( AValue < 0.0 )
				AValue = 0.0;

			if( InitialFrequency == AValue )
				return;

			InitialFrequency = AValue;
			FCurentDuration = 0;
			UpdateFrequency();
//			FServo.write( AValue );
		}

	public:
		void SystemStart()
		{
//			inherited::SystemStart();
			UpdateFrequency();
		}

	public:
		PlayFrequencyToneDuration( uint8_t APin ) :
			Enabled( true ),
			FPin( APin )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&PlayFrequencyToneDuration::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER> class PlayFrequencyToneSetValueElement : OpenWire::Object
	{
	public:
		OpenWire::SinkPin	InputPin;

	public:
		float		Frequency = 0.0;
		uint32_t	Duration = 0;

	protected:
		T_OWNER &FOwner;

	protected:
		void DoClockReceive( void *_Data )
		{
			FOwner.ExternalSetValue( Frequency, Duration );
		}

	public:
		PlayFrequencyToneSetValueElement( T_OWNER &AOwner ) :
			FOwner( AOwner )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&PlayFrequencyToneSetValueElement::DoClockReceive );
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER> class PlayFrequencyToneStateSetValueElement : OpenWire::Object
	{
	public:
		OpenWire::SinkPin	InputPin;

	public:
		float		Frequency = 0.0;
		uint32_t	Duration = 0;

	protected:
		T_OWNER &FOwner;
		bool	FInputValue = false;

	public:
		bool TryActive()
		{
			if( FInputValue )
			{
				FOwner.ExternalSetValue( Frequency, Duration );
				return true;
			}

			return false;
		}

	protected:
		void DoReceive( void *_Data )
		{
			if( FInputValue == *(bool *)_Data )
				return;

			FInputValue = *(bool *)_Data;
			FOwner.ResetValue();
		}

	public:
		PlayFrequencyToneStateSetValueElement( T_OWNER &AOwner ) :
			FOwner( AOwner )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&PlayFrequencyToneStateSetValueElement::DoReceive );
			FOwner.RegisterElement( this );
		}
	};
//---------------------------------------------------------------------------
	class PlayFrequencyToneStates : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;

	public:
		float	InitialFrequency = 0.0; //100000.0;
		bool	Enabled : 1; // = true;

	protected:
		uint8_t	FPin : 7;

	protected:
		float		FCurrentFrequency;
		uint32_t	FCurentDuration = 0;

	protected:
		Mitov::SimpleList<PlayFrequencyToneStateSetValueElement<PlayFrequencyToneStates> *>	FElements;

	public:
		void ResetValue()
		{
			for( int i = FElements.size(); i -- ; )
				if( FElements[ i ]->TryActive() )
					return;

			FCurrentFrequency = InitialFrequency;
			FCurentDuration = 0;
			UpdateEnabled();
		}

		void ExternalSetValue( float AFrequency, uint32_t ADuration )
		{
			if( ! ADuration )
				if( ( InitialFrequency == AFrequency ) && ( ! FCurentDuration ))
					return;

			FCurrentFrequency = AFrequency;
			FCurentDuration = ADuration;
			UpdateEnabled();
		}

	public:
		inline void RegisterElement( PlayFrequencyToneStateSetValueElement<PlayFrequencyToneStates> *AValue )
		{
			FElements.push_back( AValue );
		}

	public:
		void UpdateEnabled()
		{
			if( Enabled )
				UpdateFrequency();

			else
				noTone( FPin );
		}

	protected:
		void UpdateFrequency()
		{
			if( FCurrentFrequency == 0.0 )
				noTone( FPin );

			else if( FCurentDuration )
				tone( FPin, FCurrentFrequency, FCurentDuration );

			else
				tone( FPin, FCurrentFrequency );
		}

	protected:
		void DoReceive( void *_Data )
		{
			if( ! Enabled )
				return;

			float AValue = *((float *)_Data);
			if( AValue < 0.0 )
				AValue = 0.0;

			if( InitialFrequency == AValue )
				return;

			InitialFrequency = AValue;
			ResetValue();
//			FServo.write( AValue );
		}

	public:
		void SystemStart()
		{
//			inherited::SystemStart();
			UpdateFrequency();
		}

	public:
		PlayFrequencyToneStates( uint8_t	APin ) :
			Enabled( true ),
			FPin( APin )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&PlayFrequencyToneStates::DoReceive );
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
