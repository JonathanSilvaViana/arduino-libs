////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_CONVERTERS_h
#define _MITOV_CONVERTERS_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	template<typename T> class TypedToAnalog : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

	public:
		float	Scale = 1.0;

	protected:
		void DoReceive( void *_Data )
		{
			float AFloatValue = ( *(T*)_Data ) * Scale;
			OutputPin.Notify( &AFloatValue );
		}

	public:
		TypedToAnalog()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&TypedToAnalog::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	class AnalogToInteger : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

	public:
		bool	Round = true;
		float	Scale = 1.0;

	protected:
		void DoReceive( void *_Data )
		{
			float AFloatValue = *(float*)_Data * Scale;
			if( Round )
				AFloatValue += 0.5f;

			long AIntValue = AFloatValue;
			OutputPin.Notify( &AIntValue );
		}

	public:
		AnalogToInteger()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&AnalogToInteger::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	class AnalogToUnsigned : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

	public:
		bool	Round : 1;
		bool	Constrain : 1;
		float	Scale = 1.0;

	protected:
		void DoReceive( void *_Data )
		{
			float AFloatValue = *(float*)_Data * Scale;
			if( Round )
				AFloatValue += 0.5f;

			long AIntValue = AFloatValue;
			if( Constrain )
				AIntValue &= 0x7FFFFFFF;

			OutputPin.Notify( &AIntValue );
		}

	public:
		AnalogToUnsigned() : 
			Round( true ),
			Constrain( true )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&AnalogToUnsigned::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	class AnalogToText : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

	public:
		uint8_t	MinWidth : 5;
		uint8_t	Precision : 5;

	protected:
		void DoReceive( void *_Data )
		{
			float AFloatValue = *(float*)_Data;
			char AText[ 16 ];
			dtostrf( AFloatValue,  MinWidth, Precision, AText );
			OutputPin.Notify( AText );
		}

	public:
		AnalogToText() :
			MinWidth( 1 ),
			Precision( 3 )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&AnalogToText::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	template<typename T> class TypedToText : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

	public:
		uint8_t	Base = 10;

	protected:
		void DoReceive( void *_Data )
		{
			T AValue = *(T*)_Data;
			char AText[ 16 ];
			ltoa( AValue, AText, Base );
			OutputPin.Notify( AText );
		}

	public:
		TypedToText()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&TypedToText::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	class DigitalToText : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

	public:
		String	TrueValue = "true";
		String	FalseValue = "false";

	protected:
		void DoReceive( void *_Data )
		{
			bool AValue = *(bool *)_Data;
//			Serial.println( AValue );
			const char *AText;
			if( AValue )
				AText = TrueValue.c_str();

			else
				AText = FalseValue.c_str();

//			Serial.println( AText );
			OutputPin.Notify( (void*)AText );
		}

	public:
		DigitalToText()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&DigitalToText::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	template <typename T, T T_FALSE, T C_TRUE> class DigitalToType : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

	public:
		T	TrueValue = C_TRUE;
		T	FalseValue = T_FALSE;

	protected:
		void DoReceive( void *_Data )
		{
			if( *(bool *)_Data )
				OutputPin.Notify( &TrueValue );

			else
				OutputPin.Notify( &FalseValue );

		}

	public:
		DigitalToType()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&DigitalToType::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	template <typename T> class DigitalToTypeParams : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

	public:
		T	TrueValue;
		T	FalseValue;

	protected:
		void DoReceive( void *_Data )
		{
			if( *(bool *)_Data )
				OutputPin.Notify( &TrueValue );

			else
				OutputPin.Notify( &FalseValue );

		}

	public:
		DigitalToTypeParams( T ATrueValue, T AFalseValue ) :
			TrueValue( ATrueValue ),
			FalseValue( AFalseValue )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&DigitalToTypeParams::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	class TextToAnalog : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

	protected:
		void DoReceive( void *_Data )
		{
			char * AText = (char*)_Data;
			float AValue = strtod( AText, nullptr );
			OutputPin.Notify( &AValue );
		}

	public:
		TextToAnalog()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&TextToAnalog::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	class TextToInteger : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		uint8_t	Base = 10;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

	protected:
		void DoReceive( void *_Data )
		{
			char * AText = (char*)_Data;
//			long AValue = atoi( AText );
			char *AEnd;
			long AValue = strtol( AText, &AEnd, Base );
			OutputPin.Notify( &AValue );
		}

	public:
		TextToInteger()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&TextToInteger::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	class TextToUnsigned : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

	public:
		uint8_t	Base = 10;

	protected:
		void DoReceive( void *_Data )
		{
			char * AText = (char*)_Data;
//			unsigned long AValue = atoi( AText );
			char *AEnd;
			unsigned long AValue = strtol( AText, &AEnd, Base );
			OutputPin.Notify( &AValue );
		}

	public:
		TextToUnsigned()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&TextToUnsigned::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	class UnsignedToInteger : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

	public:
		bool Constrain = true;

	protected:
		void DoReceive( void *_Data )
		{
			unsigned long AValue = *(unsigned long*)_Data;
			if( Constrain )
				AValue &= 0x7FFFFFFF;

			OutputPin.Notify( &AValue );
		}

	public:
		UnsignedToInteger()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&UnsignedToInteger::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	class IntegerToUnsigned : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

	public:
		bool Constrain = true;

	protected:
		void DoReceive( void *_Data )
		{
			long AValue = *(long*)_Data;
			if( Constrain )
				AValue &= 0x7FFFFFFF;

			OutputPin.Notify( &AValue );
		}

	public:
		IntegerToUnsigned()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&IntegerToUnsigned::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	template<int C_NUM_INPUTS> class UnsignedToDigital : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin OutputPins[ C_NUM_INPUTS ];

	public:
		uint32_t	InitialValue = 0;

	protected:
		uint32_t	FOldValue = 0;

	protected:
		void SetValue( uint32_t AValue, bool AUpdate )
		{
			for( long i = 0; i < C_NUM_INPUTS; ++i )
			{
				uint32_t ABit = uint32_t(1) << i;
				bool AOldBitValue = ( FOldValue & ABit );
				bool ANewBitValue = ( AValue & ABit );
				if( AUpdate || AOldBitValue != ANewBitValue )
					OutputPins[ i ].SendValue( ANewBitValue );

			}

			FOldValue = AValue;
		}

	protected:
		void DoReceive( void *_Data )
		{
			unsigned long AValue = *(unsigned long *)_Data;
			if( FOldValue == AValue )
				return;

			SetValue( AValue, false );
		}

	protected:
		virtual void SystemInit()
		{
//			inherited::SystemInit();
			SetValue( InitialValue, true );
		}

	public:
		UnsignedToDigital()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&UnsignedToDigital::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	class CelsiusToFahrenheit : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool	Enabled : 1;
		bool	Reverse : 1;

	protected:
        float FilterValue( float AValue )
        {
			if( Reverse )
				return ( AValue - 32.0 ) / ( 9.0/5.0 );

			else
				return AValue * ( 9.0/5.0 ) + 32.0;
        }

	protected:
		void DoReceive( void *_Data )
        {
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

            float AOutValue = FilterValue( *(float*)_Data );

			OutputPin.Notify( &AOutValue );
        }

	public:
		CelsiusToFahrenheit() :
			Enabled( true ),
			Reverse( false )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&CelsiusToFahrenheit::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	class CelsiusToKelvin : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool	Enabled : 1;
		bool	Reverse : 1;

	protected:
        float FilterValue( float AValue )
        {
			if( Reverse )
				return AValue - 273.15;

			else
				return AValue + 273.15;
        }

	protected:
		void DoReceive( void *_Data )
        {
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

			float AInValue = *(float*)_Data;

            float AOutValue = FilterValue( AInValue );

			OutputPin.Notify( &AOutValue );
        }

	public:
		CelsiusToKelvin() :
			Enabled( true ),
			Reverse( false )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&CelsiusToKelvin::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	class MetresToFeet : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool	Enabled : 1;
		bool	Reverse : 1;

	protected:
        float FilterValue( float AValue )
        {
			if( Reverse )
				return AValue * 0.3048;

			else
				return AValue / 0.3048;
        }

	protected:
		void DoReceive( void *_Data )
        {
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

			float AInValue = *(float*)_Data;

            float AOutValue = FilterValue( AInValue );

			OutputPin.Notify( &AOutValue );
        }

	public:
		MetresToFeet() :
			Enabled( true ),
			Reverse( false )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&MetresToFeet::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	template<int T_SIZE> class CharToText : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool	Enabled : 1;
		bool	Truncate : 1;
		bool	UpdateOnEachChar : 1;
		uint32_t	MaxLength = 100;

	protected:
		byte	FBuffer[ T_SIZE + 1 ];
		uint32_t	FIndex = 0;

	protected:
		void SendBufferNoReset()
		{
			FBuffer[ FIndex ] = '\0';
			OutputPin.Notify( FBuffer );
		}

		void SendBuffer()
		{
			SendBufferNoReset();
			FIndex = 0;
		}

	protected:
		void DoReceive( void *_Data )
        {
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

			char AValue = *(char *)_Data;
			if( AValue == '\n' )
				return;

			if( AValue == '\r' )
			{
				SendBuffer();
				return;
			}
				
			if( FIndex >= T_SIZE )
			{
				if( Truncate )
					return;

				SendBuffer();
			}

			FBuffer[ FIndex++ ] = AValue;
			if( UpdateOnEachChar )
				SendBufferNoReset();

        }

	public:
		CharToText() :
			Enabled( true ),
			Truncate( false ),
			UpdateOnEachChar( false )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&CharToText::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	class TextToChar : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

	public:
        bool	Enabled : 1;
		bool	AddReturn : 1;
		bool	AddNewLine : 1;

	protected:
		void DoReceive( void *_Data )
        {
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

			char *AValue = (char *)_Data;
			while( *AValue )
			{
				OutputPin.Notify( AValue );
				++AValue;
			}

			if( AddReturn )
				OutputPin.SendValue( '\r' );

			if( AddNewLine )
				OutputPin.SendValue( '\n' );
        }

	public:
		TextToChar() :
			Enabled( true ),
			AddReturn( true ),
			AddNewLine( true )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&TextToChar::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	class PressureToAltitude : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;
		float BaseLinePressure = 0;

	protected:
        float FilterValue( float AValue )
        {
			return ( 44330.0 * ( 1 - pow( AValue / BaseLinePressure, 1 / 5.255 )));
        }

	protected:
		void DoReceive( void *_Data )
        {
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

			float AInValue = *(float*)_Data;

            float AOutValue = FilterValue( AInValue );

			OutputPin.Notify( &AOutValue );
        }

	public:
		PressureToAltitude()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&PressureToAltitude::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	class AltitudePressureToSeaLevelPressure : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;
		float Altitude = 0;

	protected:
        float FilterValue( float AValue )
        {
			return ( AValue / pow( 1 - ( Altitude / 44330.0 ), 5.255 ));
        }

	protected:
		void DoReceive( void *_Data )
        {
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

			float AInValue = *(float*)_Data;

            float AOutValue = FilterValue( AInValue );

			OutputPin.Notify( &AOutValue );
        }

	public:
		AltitudePressureToSeaLevelPressure()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&AltitudePressureToSeaLevelPressure::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	template<int C_NUM_INPUTS> class AnalogToAnalogArray : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::IndexedSinkPin<uint8_t>	InputPins[ C_NUM_INPUTS ];
		OpenWire::ConnectSinkPin	ClockInputPin;
		OpenWire::SourcePin			OutputPin;

	protected:
		bool	FModified = false;
		float	FBufferData[ C_NUM_INPUTS ];

	protected:
		void DoReceive( int AIndex, void *_Data )
		{
			FBufferData[ AIndex ] = *(float *)_Data;
			FModified = true;
		}

		void DoClockReceive( void * )
		{
			if( FModified )
				CalculateSendOutput( false );
		}

		void CalculateSendOutput( bool AFromStart )
		{
			TArray<float> ABuffer( C_NUM_INPUTS, FBufferData );
			OutputPin.Notify( &ABuffer );
		}

	protected:
		virtual void SystemStart() override
		{
			CalculateSendOutput( true );
		}

		virtual void SystemLoopEnd() override
		{
			if( FModified )
				if( ! ClockInputPin.IsConnected() )
					CalculateSendOutput( false );

		}

	public:
		AnalogToAnalogArray()
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&AnalogToAnalogArray::DoClockReceive );

			for( int i = 0; i < C_NUM_INPUTS; ++i )
				InputPins[ i ].SetCallback( i, this, (OpenWire::TOnPinIndexedReceive)&AnalogToAnalogArray::DoReceive );

		}

	};
//---------------------------------------------------------------------------
	template<int C_NUM_INPUTS> class AnalogArrayToAnalogArray
	{
	};
//---------------------------------------------------------------------------
	template<typename T> class SingleElementArrayPin : public OpenWire::ValueSinkPin<Mitov::TValueArray<T> >
	{
		typedef OpenWire::ValueSinkPin<Mitov::TValueArray<T> > inherited;

	public:
		virtual void Receive( void *_Data ) override
		{
/*
			Serial.print( "Pin: " ); Serial.println( (int)this );
			Serial.println( inherited::Value.Size );
			Serial.println( (int)inherited::Value.Data );
			delay( 200 );
*/
			T AValue = *(T*)_Data;
			*inherited::Value.Data = AValue;
			if( inherited::OnReceive )
				( inherited::OnReceiveObject->*inherited::OnReceive )( _Data );

//			OpenWire::SinkPin::Receive( _Data );
		}

	};
//---------------------------------------------------------------------------
	class FloatArraySinkPinsArray
	{
	protected:
		OpenWire::ValueSinkPin<Mitov::TValueArray<float> >	**FInputPins;

	public:
		OpenWire::ValueSinkPin<Mitov::TValueArray<float> >& operator[] ( int AIndex ) 
		{
          return *( FInputPins[ AIndex ] );
		}

	public:
		inline void SetPin( int AIndex, OpenWire::ValueSinkPin<Mitov::TValueArray<float> > *APin )
		{
			FInputPins[ AIndex ] = APin;
		}

	public:
		FloatArraySinkPinsArray( int ASize )
		{
			FInputPins = new OpenWire::ValueSinkPin<Mitov::TValueArray<float> >*[ ASize ];
		}
	};
//---------------------------------------------------------------------------
	template<int C_NUM_INPUTS> class MixedAnalogToAnalogArray : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		FloatArraySinkPinsArray		InputPins = FloatArraySinkPinsArray( C_NUM_INPUTS );
		OpenWire::ConnectSinkPin	ClockInputPin;
		OpenWire::SourcePin			OutputPin;

	protected:
		bool	FModified = false;

	protected:
		void DoReceive( void * )
		{
			FModified = true;
		}

		void DoClockReceive( void * )
		{
			if( FModified )
				CalculateSendOutput( false );
		}

		void CalculateSendOutput( bool AFromStart )
		{
//			TArray<float> ABuffer( C_NUM_INPUTS, FBufferData );
//			float *AData = 
//			TArray<float> ABuffer( C_NUM_INPUTS, FBufferData );
			int ASize = 0;

			for( int i = 0; i < C_NUM_INPUTS; ++i )
				ASize += InputPins[ i ].Value.Size;

//			Serial.println( "Size" );
//			Serial.println( ASize );
//			delay( 1000 );

			float *AData = new float[ ASize ];
			float *APtr = AData;
			for( int i = 0; i < C_NUM_INPUTS; ++i )
			{
				int ABlockSize = InputPins[ i ].Value.Size;
				memcpy( APtr, InputPins[ i ].Value.Data, ABlockSize * sizeof( APtr[ 0 ] ));
				APtr += ABlockSize;
			}

			TArray<float> ABuffer( ASize, AData );
			OutputPin.Notify( &ABuffer );

			delete [] AData;
		}

	protected:
		virtual void SystemStart() override
		{
			for( int i = 0; i < C_NUM_INPUTS; ++i )
				InputPins[ i ].SetCallback( this, (OpenWire::TOnPinReceive)&MixedAnalogToAnalogArray::DoReceive );

			CalculateSendOutput( true );
		}

		virtual void SystemLoopEnd() override
		{
			if( FModified )
				if( ! ClockInputPin.IsConnected() )
					CalculateSendOutput( false );

		}

	public:
		MixedAnalogToAnalogArray()
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&MixedAnalogToAnalogArray::DoClockReceive );
		}

	};
//---------------------------------------------------------------------------
	template<int C_NUM_INPUTS> class AnalogArrayToAnalog : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin				InputPin;
		OpenWire::TypedSourcePin<float>	OutputPins[ C_NUM_INPUTS ];

	protected:
		bool	FStarted = false;

	protected:
		void DoReceive( void *_Data )
		{
			int ASize = (( TArray<float> *)_Data )->Size;
			float *AData = (( TArray<float> *)_Data )->Data;

			for( int i = 0; i < C_NUM_INPUTS; ++i )
				OutputPins[ i ].SetValue( *AData++, FStarted );

			FStarted = true;
		}

	public:
		AnalogArrayToAnalog()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&AnalogArrayToAnalog::DoReceive );
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
