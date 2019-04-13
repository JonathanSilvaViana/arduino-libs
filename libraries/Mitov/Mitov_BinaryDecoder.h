////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_BINARY_DECODER_h
#define _MITOV_BINARY_DECODER_h

#include <Mitov.h>

namespace Mitov
{
	template<int C_NUM_OUTPUTS> class BinaryDecoder : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin OutputPins[ C_NUM_OUTPUTS ];

	public:
		uint32_t	InitialValue = 0;

	protected:
		void SetValue( uint32_t AValue )
		{
			InitialValue = AValue;
			if( InitialValue < C_NUM_OUTPUTS )
				OutputPins[ InitialValue ].SendValue( true );
		}

	protected:
		void DoReceive( void *_Data )
		{
			unsigned long AValue = *(uint32_t *)_Data;
			if( InitialValue == AValue )
				return;

			if( InitialValue < C_NUM_OUTPUTS )
				OutputPins[ InitialValue ].SendValue( false );

			SetValue( AValue );
		}

	protected:
		virtual void SystemInit() override
		{
//			inherited::SystemInit();
			for( int i = 0; i < C_NUM_OUTPUTS; ++i )
				OutputPins[ i ].SendValue( i == InitialValue );
		}

	public:
		BinaryDecoder()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&BinaryDecoder::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	template<int C_NUM_OUTPUTS> class BinaryDecoderBar : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin OutputPins[ C_NUM_OUTPUTS ];

	public:
		uint32_t	InitialValue = 0;
		bool		ValueBar = true;

	public:
		void SetValueBar( bool AValue )
		{
			if( AValue == ValueBar )
				return;

			ValueBar = AValue;
			for( int i = 0; i < InitialValue; ++i )
			{
				if( i >= C_NUM_OUTPUTS )
					break;

				OutputPins[ i ].Notify( (void *)&ValueBar );
			}

		}

	protected:
		void SetValue( uint32_t AValue )
		{
			uint32_t AOldValue = InitialValue;
			InitialValue = AValue;
			if( ValueBar )
			{
				if( AOldValue < InitialValue )
				{
					for( int i = AOldValue + 1; i <= InitialValue; ++i )
					{
						if( i >= C_NUM_OUTPUTS )
							break;

						OutputPins[ i ].SendValue( true );
					}
				}

				else
				{
					for( int i = InitialValue + 1; i <= AOldValue; ++i )
					{
						if( i >= C_NUM_OUTPUTS )
							break;

						OutputPins[ i ].SendValue( false );
					}
				}
			}

			else
				if( InitialValue < C_NUM_OUTPUTS )
					OutputPins[ InitialValue ].SendValue( true );
		}

	protected:
		void DoReceive( void *_Data )
		{
			unsigned long AValue = *(uint32_t *)_Data;
			if( InitialValue == AValue )
				return;

			if( ! ValueBar )
				if( InitialValue < C_NUM_OUTPUTS )
					OutputPins[ InitialValue ].SendValue( false );

			SetValue( AValue );
		}

	protected:
		virtual void SystemInit() override
		{
//			inherited::SystemInit();
			if( ! ValueBar )
			{
				for( int i = 0; i < C_NUM_OUTPUTS; ++i )
					OutputPins[ i ].SendValue( i == InitialValue );
			}

			else
			{
				for( int i = 0; i < C_NUM_OUTPUTS; ++i )
					OutputPins[ i ].SendValue( i <= InitialValue );
			}
		}

	public:
		BinaryDecoderBar()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&BinaryDecoderBar::DoReceive );
		}
	};
//---------------------------------------------------------------------------
}

#endif
