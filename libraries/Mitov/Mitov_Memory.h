////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_MEMORY_h
#define _MITOV_MEMORY_h

#include <Mitov.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	class BasicMemoryElement;
//---------------------------------------------------------------------------
	class MemoryIntf
	{
	public:
		Mitov::SimpleList<BasicMemoryElement *>	FElements;

	public:
		virtual bool ReadData( uint32_t AAddress, uint32_t ASize, uint8_t *AData ) = 0;
		virtual bool WriteData( uint32_t AAddress, uint32_t ASize, uint8_t *AData ) = 0;

	};
//---------------------------------------------------------------------------
	class BasicMemoryElement : public OpenWire::Object
	{
	protected:
		MemoryIntf &FOwner;

	public:
		virtual void PopulateAddress( uint32_t &AAddress, byte &AOffset ) = 0;
//		virtual void ProcessRead( MemoryIntf *AOwner ) = 0;
		virtual void ProcessWrite( MemoryIntf *AOwner ) = 0;
//		virtual void GetAddressAndSize( uint32_t &AAddress, &ASize ) = 0;

	public:
		BasicMemoryElement( MemoryIntf &AOwner ) :
			FOwner( AOwner )
		{
			AOwner.FElements.push_back( this );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_PIN, typename T> class TypedMemoryElement : public BasicMemoryElement
	{
		typedef	BasicMemoryElement inherited;

	public:
		OpenWire::ValueSimpleClockedFlagSinkPin	RememberInputPin;
		OpenWire::ConnectSinkPin				RecallInputPin;
		OpenWire::ValueSimpleModifiedSinkPin<T>	InputPin;
		OpenWire::SourcePin						OutputPin;

	public:
		bool		RecallOnRemember = false;

	protected:
		uint32_t	FAddress;

	public:
		virtual void PopulateAddress( uint32_t &AAddress, byte &AOffset ) override
		{
			if( AOffset )
			{
				++ AAddress;
				AOffset = 0;
			}

			FAddress = AAddress;
			AAddress += sizeof( T );
		}

		void DoRecallReceive( void *_Data )
		{
			T AValue = 0;
			if( FOwner.ReadData( FAddress, sizeof( T ), (uint8_t *)&AValue ))
				OutputPin.Notify( &AValue );
		}

		void ProcessWrite( MemoryIntf *AOwner ) override
		{
			if( RememberInputPin.IsConnected() )
			{
				if( ! RememberInputPin.Clocked )
					return;

				RememberInputPin.Clocked = false;
			}

			else if( ! InputPin.Modified )
				return;

			InputPin.Modified = false;

//			Serial.println( "WRITE" );
//			Serial.println( InputPin.Value );
//			Serial.println( sizeof( T ) );

			AOwner->WriteData( FAddress, sizeof( T ), (uint8_t *)&InputPin.Value );

			if( RecallOnRemember || ( ! RecallInputPin.IsConnected() ))
				DoRecallReceive( nullptr );
		}

/*
		virtual void GetAddressAndSize( uint32_t &AAddress, &ASize ) override
		{
			AAddress = FAddress;
			ASize = sizeof( T );
		}
*/
	public:
		TypedMemoryElement( MemoryIntf &AOwner ) :
			inherited( AOwner )
		{
			RecallInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&TypedMemoryElement::DoRecallReceive );
		}

	};
//---------------------------------------------------------------------------
	class TextMemoryElement : public BasicMemoryElement
	{
		typedef	BasicMemoryElement inherited;

	public:
		OpenWire::ValueSimpleClockedFlagSinkPin			RememberInputPin;
		OpenWire::ConnectSinkPin						RecallInputPin;
		OpenWire::ValueSimpleModifiedSinkPin<String>	InputPin;
		OpenWire::SourcePin								OutputPin;

	public:
		uint8_t	MaxSize = 10;

	public:
		bool		RecallOnRemember = false;

	protected:
		uint32_t	FAddress;

	public:
		virtual void PopulateAddress( uint32_t &AAddress, byte &AOffset ) override
		{
			if( AOffset )
			{
				++ AAddress;
				AOffset = 0;
			}

			FAddress = AAddress;
			AAddress += MaxSize + 1;
		}

		virtual void ProcessWrite( MemoryIntf *AOwner ) override
		{
			if( RememberInputPin.IsConnected() )
			{
				if( ! RememberInputPin.Clocked )
					return;

				RememberInputPin.Clocked = false;
			}

			else if( ! InputPin.Modified )
				return;

			InputPin.Modified = false;

//			Serial.println( "WRITE" );
//			Serial.println( InputPin.Value );
//			Serial.println( sizeof( T ) );
			String AText = InputPin.Value;
			uint8_t ASize = MitovMin<unsigned int>( MaxSize, AText.length() );

//			Serial.println( ASize );
//			Serial.println( AText );

			AOwner->WriteData( FAddress, 1, (uint8_t *)&ASize );
			AOwner->WriteData( FAddress + 1, ASize, (uint8_t *)AText.c_str() );

			if( RecallOnRemember || ( ! RecallInputPin.IsConnected() ))
				DoRecallReceive( nullptr );
		}

		void DoRecallReceive( void *_Data )
		{
			uint8_t *ABuffer = new uint8_t[ MaxSize ];
			if( FOwner.ReadData( FAddress, MaxSize, ABuffer ))
			{
				String AValue;
				uint8_t ASize = MitovMin( MaxSize, ABuffer[ 0 ] );
				AValue.reserve( ASize );

				for( int i = 0; i < ASize; ++ i )
					AValue += ((char *)ABuffer )[ i + 1 ];

				OutputPin.SendValue( AValue );
			}

			delete [] ABuffer;
		}

	public:
		TextMemoryElement( MemoryIntf &AOwner ) :
			inherited( AOwner )
		{
			RecallInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&TextMemoryElement::DoRecallReceive );
		}

	};
//---------------------------------------------------------------------------
	class DigitalMemoryElement : public BasicMemoryElement
	{
		typedef	BasicMemoryElement inherited;

	public:
		OpenWire::ValueSimpleClockedFlagSinkPin		RememberInputPin;
		OpenWire::ConnectSinkPin					RecallInputPin;
		OpenWire::ValueSimpleModifiedSinkPin<bool>	InputPin;
		OpenWire::SourcePin							OutputPin;

	public:
		bool		RecallOnRemember = false;

	protected:
		uint32_t	FAddress;

	public:
/*
		virtual void GetAddressAndSize( uint32_t &AAddress, &ASize ) override
		{
			AAddress = FAddress;
			ASize = sizeof( T );
		}
*/
	public:
		uint8_t	FMask;

	public:
		virtual void PopulateAddress( uint32_t &AAddress, byte &AOffset ) override
		{
			FMask = 1 << AOffset;
			FAddress = AAddress;
			if( ++AOffset >= 8 )
			{
				++ AAddress;
				AOffset = 0;
			}
		}

		virtual void DoRecallReceive( void *_Data )
		{
//			Serial.print( "READ: " ); Serial.println( FAddress, HEX );

			uint8_t AByteValue;
			if( FOwner.ReadData( FAddress, 1, &AByteValue ))
			{
				bool AValue = (( AByteValue & FMask ) != 0 );
				OutputPin.Notify( &AValue );
			}
		}

		virtual void ProcessWrite( MemoryIntf *AOwner ) override
		{
			if( RememberInputPin.IsConnected() )
			{
				if( ! RememberInputPin.Clocked )
					return;

				RememberInputPin.Clocked = false;
			}

			else if( ! InputPin.Modified )
				return;

			InputPin.Modified = false;

//			Serial.println( "WRITE" );

			uint8_t AByteValue;
			if( AOwner->ReadData( FAddress, 1, &AByteValue ))
			{
				if( InputPin.Value )
					AByteValue |= FMask;

				else
					AByteValue &= ~FMask;

				AOwner->WriteData( FAddress, 1, &AByteValue );

				if( RecallOnRemember || ( ! RecallInputPin.IsConnected() ))
					DoRecallReceive( nullptr );
			}
		}

	public:
		DigitalMemoryElement( MemoryIntf &AOwner ) :
			inherited( AOwner )
		{
			RecallInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&DigitalMemoryElement::DoRecallReceive );
		}

	};
//---------------------------------------------------------------------------
}

#endif
