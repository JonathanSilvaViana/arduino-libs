////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_BASIC_SPI_h
#define _MITOV_BASIC_SPI_h

#include <Mitov.h>
#include <SPI.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	class BasicSPI : public OpenWire::Component
	{
	public:
		virtual uint16_t transfer16(uint16_t data) = 0;
		virtual uint8_t transfer(uint8_t data) = 0;
		virtual void transfer(void *buf, size_t count) = 0;
		virtual void beginTransaction(SPISettings settings) = 0;
		virtual void endTransaction() = 0;
		virtual void setDataMode( uint8_t dataMode ) = 0;
//		virtual void setBitOrder( uint8_t bitOrder ) = 0;
		virtual void setClockDivider( uint8_t clockDiv ) = 0;
		virtual void usingInterrupt( uint8_t interruptNumber ) = 0;
	};
//---------------------------------------------------------------------------
	class Basic_SPIChannel;
//---------------------------------------------------------------------------
	class Basic_MultiChannel_SourceSPI : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SourcePin			ChipSelectOutputPin;
		OpenWire::ConnectSinkPin	ClockInputPin;

	public:
		bool	Enabled : 1;

	public:
		bool	FModified : 1;

	public:
		BasicSPI								&FSPI;
		Mitov::SimpleList<Basic_SPIChannel *>	FChannels;

	protected:
		virtual void SystemInit();

	protected:
		virtual void DoClockReceive( void * );

	public:
		Basic_MultiChannel_SourceSPI( BasicSPI &ASPI ) :
			FSPI( ASPI ),
			Enabled( true ),
			FModified( false )
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&Basic_MultiChannel_SourceSPI::DoClockReceive );
		}

	};
//---------------------------------------------------------------------------
	class Basic_SPIChannel : public OpenWire::Object
	{
		typedef OpenWire::Object	inherited;

	public:
		OpenWire::SinkPin	InputPin;

	public:
		virtual void InitChannel() {}
		virtual void SendOutput() = 0;

	protected:
		virtual void DoReceive( void *_Data ) = 0;

	public:
		Basic_SPIChannel()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&Basic_SPIChannel::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER> class Basic_Typed_SPIChannel : public Mitov::Basic_SPIChannel
	{
		typedef Mitov::Basic_SPIChannel	inherited;

	public:
		float	InitialValue = 0.0f;

	protected:
		float	FValue = 0.0f;
		float	FNewValue = 0.0f;

		int		FIndex;

	protected:
		T_OWNER	&FOwner;

	protected:
		virtual void DoReceive( void *_Data )
		{
			FNewValue = constrain( *((float *)_Data), 0.0f, 1.0f );
			if( FNewValue == FValue )
				return;

			if( FOwner.ClockInputPin.IsConnected() )
				FOwner.FModified = true;

			else
				SendOutput();

		}

	public:
		Basic_Typed_SPIChannel( T_OWNER &AOwner, int AIndex ) :
			FOwner( AOwner ),
			FIndex( AIndex )
		{
			AOwner.FChannels.push_back( this );
		}

	};
//---------------------------------------------------------------------------
	void Basic_MultiChannel_SourceSPI::DoClockReceive( void * )
	{
		if( ! FModified )
			return;

		for( int i =0; i < FChannels.size(); ++i )
			FChannels[ i ]->SendOutput();
	}
//---------------------------------------------------------------------------
	void Basic_MultiChannel_SourceSPI::SystemInit()
	{
		inherited::SystemInit();
		ChipSelectOutputPin.SendValue( true );

		for( int i =0; i < FChannels.size(); ++i )
			FChannels[ i ]->InitChannel();
	}
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
