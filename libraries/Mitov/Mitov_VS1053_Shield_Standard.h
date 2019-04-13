////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_VS1053_SHIELD_STANDARD_h
#define _MITOV_VS1053_SHIELD_STANDARD_h

#include <Mitov.h>
#include <SdFat.h>
//#include <SdFatUtil.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

SdFat	sd;

namespace Mitov
{
	class TArduinoVS1053BMP3Volume
	{
	public:
		float	Left = 0.8;
		float	Right = 0.8;

	};
//---------------------------------------------------------------------------
	class TArduinoVS1053BMP3VUMeter
	{
	public:
		OpenWire::SourcePin	OutputPins[ 2 ];

	public:
		bool	Enabled = true;

	};
//---------------------------------------------------------------------------
	class VS1053ShieldStandard : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
#ifdef _MITOV_VS1053_SHIELD_CLASIC_POSITION_PIN_
		OpenWire::SourcePin	PositionOutputPin;
#endif
#ifdef _MITOV_VS1053_SHIELD_CLASIC_STOP_PIN_
		OpenWire::SinkPin	StopInputPin;
#endif
/*
#ifdef _MITOV_VS1053_SHIELD_CLASIC_PAUSE_PIN_
		OpenWire::SinkPin	PauseInputPin;
#endif
#ifdef _MITOV_VS1053_SHIELD_CLASIC_RESUME_PIN_
		OpenWire::SinkPin	ResumeInputPin;
#endif
*/
	public:
		TArduinoVS1053BMP3Volume	Volume;
		TArduinoVS1053BMP3VUMeter	VUMeter;
		bool	Paused : 1;
		bool	Stereo : 1;

	public:
		SFEMP3Shield FPlayer;

	public:
		void UpdateVolume()
		{
			FPlayer.setVolume( ( 1.0 - Volume.Left ) * 254 + 0.5, ( 1.0 - Volume.Right ) * 254 + 0.5 );
//			FPlayer.setVolume( 1, 1 );
		}

		void UpdatePaused()
		{
			if( Paused )
				FPlayer.pauseMusic();

			else
				FPlayer.resumeMusic();
		}

		void UpdateMono()
		{
			if( Stereo )
				FPlayer.setMonoMode( 1 );

			else
				FPlayer.setMonoMode( 0 );
		}

	protected:
#ifdef _MITOV_VS1053_SHIELD_CLASIC_STOP_PIN_
		void DoReceiveStop( void *_Data )
		{
			FPlayer.stopTrack();
		}
#endif

/*
#ifdef _MITOV_VS1053_SHIELD_CLASIC_PAUSE_PIN_
		void DoReceivePause( void *_Data )
		{
			FPlayer.pauseMusic();
		}
#endif

#ifdef _MITOV_VS1053_SHIELD_CLASIC_RESUME_PIN_
		void DoReceiveResume( void *_Data )
		{
			FPlayer.resumeMusic();
		}
#endif
*/
	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
//			inherited::SystemLoopBegin( currentMicros );

			if( VUMeter.Enabled )
			{
/*
				FPlayer.setVUmeter( 1 );
				int16_t AValue = FPlayer.getVUlevel();
				
				Serial.println( AValue );
*/
				//				VUMeter.OutputPins[ 0 ]
			}

		}

		virtual void SystemInit() override
		{
//			inherited::SystemInit();
			sd.begin( SD_SEL, SPI_FULL_SPEED );
			FPlayer.begin();
		}

		virtual void SystemStart() override
		{
//			inherited::SystemStart();
			UpdatePaused();
			UpdateMono();
			UpdateVolume();
//			FPlayer.setVUmeter( 1 );
		}

	public:
		VS1053ShieldStandard() :
			Paused( false ),
			Stereo( true )
		{
#ifdef _MITOV_VS1053_SHIELD_CLASIC_STOP_PIN_
			StopInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&VS1053ShieldStandard::DoReceiveStop );
#endif

/*
#ifdef _MITOV_VS1053_SHIELD_CLASIC_PAUSE_PIN_
			PauseInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&VS1053ShieldStandard::DoReceivePause );
#endif

#ifdef _MITOV_VS1053_SHIELD_CLASIC_RESUME_PIN_
			ResumeInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&VS1053ShieldStandard::DoReceiveResume );
#endif
*/
		}

	};
//---------------------------------------------------------------------------
	class VS1053PlayFile : OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	ClockInputPin;

	public:
		String	FileName;

	protected:
		VS1053ShieldStandard &FOwner;

	protected:
		void DoReceiveClock( void *_Data )
		{
			FOwner.FPlayer.stopTrack();
			FOwner.FPlayer.playMP3( (char *)FileName.c_str() );
		}

	public:
		VS1053PlayFile( VS1053ShieldStandard &AOwner ) :
			FOwner( AOwner )
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&VS1053PlayFile::DoReceiveClock );
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
