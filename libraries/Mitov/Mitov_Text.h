////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_TEXT_h
#define _MITOV_TEXT_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	class TextStateValue;
//---------------------------------------------------------------------------
	class TextValue : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::ConnectSinkPin	ClockInputPin;
		OpenWire::SourcePin			OutputPin;

	public:
		String Value;

	public:
		virtual void ExternalSetValue( String AValue )
		{
			if( Value == AValue )
				return;

			Value = AValue;
			OutputPin.Notify( (void *)Value.c_str() );
		}

	protected:
		virtual void SystemStart() override
		{
//			inherited::SystemStart();
			if( ! ClockInputPin.IsConnected() )
				OutputPin.Notify( (void *)Value.c_str() );

		}

	protected:
		virtual void DoClockReceive( void *_Data )
		{
			OutputPin.Notify( (void *)Value.c_str() );
		}

	public:
		TextValue( char *AValue ) :
			Value( AValue )
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&TextValue::DoClockReceive );
		}

	};
//---------------------------------------------------------------------------
	class BindableTextValue : public TextValue
	{
		typedef TextValue inherited;

	protected:
		String OldValue = inherited::Value;

	protected:
		virtual void SystemInit()
		{
			inherited::SystemInit();
			OldValue = inherited::Value;
		}

		virtual void SystemLoopBegin( unsigned long currentMicros )
		{
			if( inherited::Value == OldValue )
				return;

			OldValue = inherited::Value;
			inherited::OutputPin.Notify( (void *)OldValue.c_str() );
		}

	public:
		BindableTextValue( char *AValue ) :
			inherited( AValue ),
			OldValue( AValue )
		{
		}

	};
//---------------------------------------------------------------------------
	class ArduinoSetTextValueElement : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	InputPin;

	protected:
		TextValue &FOwner;

	public:
		String	Value;

	protected:
		void DoReceived( void *_Data )
		{
			FOwner.ExternalSetValue( Value );
		}

	public:
		ArduinoSetTextValueElement( TextValue &AOwner ) :
			FOwner( AOwner )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ArduinoSetTextValueElement::DoReceived );
		}

	};
//---------------------------------------------------------------------------
	class ArduinoStateSetTextValueElement : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	InputPin;

	protected:
		TextStateValue &FOwner;
		bool	FInputValue = false;

	public:
		String	Value;

	public:
		bool TryActive();

	protected:
		void DoReceived( void *_Data );

	public:
		ArduinoStateSetTextValueElement( TextStateValue &AOwner );

	};
//---------------------------------------------------------------------------
	class TextStateValue : public TextValue
	{
		typedef Mitov::TextValue inherited;

	protected:
		String FCurrentValue;

	protected:
		Mitov::SimpleList<ArduinoStateSetTextValueElement *>	FElements;

	public:
		virtual void ExternalSetValue( String AValue ) override
		{
			if( FCurrentValue == AValue )
				return;

			FCurrentValue = AValue;
			OutputPin.Notify( (void *)FCurrentValue.c_str() );
		}

		void ResetValue()
		{
			for( int i = FElements.size(); i -- ; )
				if( FElements[ i ]->TryActive() )
					return;

			FCurrentValue = Value;
			OutputPin.Notify( (void *)FCurrentValue.c_str() );
		}

	public:
		void RegisterElement( ArduinoStateSetTextValueElement *AValue )
		{
			FElements.push_back( AValue );
		}

	protected:
		virtual void SystemStart() override
		{
			FCurrentValue = Value;
			inherited::SystemStart();
//			if( ! ClockInputPin.IsConnected() )
//				OutputPin.Notify( (void *)FCurrentValue.c_str() );

		}

		virtual void DoClockReceive( void *_Data ) override
		{
			OutputPin.Notify( (void *)FCurrentValue.c_str() );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class BindableTextStateValue : public TextStateValue
	{
		typedef TextValue inherited;

	protected:
		String OldValue;

	protected:
		virtual void SystemInit()
		{
			inherited::SystemInit();
			OldValue = inherited::Value;
		}

		virtual void SystemLoopBegin( unsigned long currentMicros )
		{
			if( inherited::Value == OldValue )
				return;

			OldValue = inherited::Value;
			ResetValue();
//			inherited::OutputPin.Notify( (void *)OldValue.c_str() );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class BasicFormattedText;
//---------------------------------------------------------------------------
	class FormattedTextElementBasic : public OpenWire::Object // : public OpenWire::Component
	{
//		typedef OpenWire::Component inherited;
	protected:
		BasicFormattedText &FOwner;

	public:
		virtual String GetText() = 0;

		virtual void ClearModified() {}
		virtual bool GetModified() { return false; }
		virtual void SystemStart()
		{
		}

	public:
		FormattedTextElementBasic( BasicFormattedText &AOwner );

	};
//---------------------------------------------------------------------------
	class BasicFormattedText : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::ConnectSinkPin	ClockInputPin;
		OpenWire::SourcePin			OutputPin;

	public:
		Mitov::SimpleList<FormattedTextElementBasic *>	FElements;

	protected:
		virtual void DoClockReceive( void *_Data ) = 0;

	public:
		BasicFormattedText()
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&BasicFormattedText::DoClockReceive );
		}

	};
//---------------------------------------------------------------------------
	template <typename T_STRING> class BasicTypedFormattedText : public Mitov::BasicFormattedText
	{
		typedef Mitov::BasicFormattedText inherited;

	protected:
		struct TStringItem
		{
			T_STRING	Text;
			FormattedTextElementBasic *Element;
		};

	protected:
		Mitov::SimpleList<TStringItem *>	FReadyElements;

	public:
		void AddReadyElement( T_STRING ATextItem, int AIndex )
		{
//			Serial.print( "AddElement: " ); Serial.print( ATextItem ); Serial.println( AIndex );

			TStringItem	*AItem = new TStringItem;
			AItem->Text = ATextItem;
//			Serial.print( "AddElement: " ); Serial.println( AItem->Text );
			if( AIndex < FElements.size() )
				AItem->Element = FElements[ AIndex ];

			else
				AItem->Element = nullptr;

			FReadyElements.push_back( AItem );

//			Serial.println( FReadyElements[ FReadyElements.size() - 1 ]->Text );
//			Serial.println( "DEBUG>>" );
//			for( Mitov::SimpleList<TStringItem *>::iterator Iter = FReadyElements.begin(); Iter != FReadyElements.end(); ++Iter )
//				Serial.println( ( *Iter )->Text );

//			Serial.println( "<<DEBUG" );
		}

	protected:
		void ProcessSendOutput()
		{
//			Serial.println( "ProcessSendOutput" );
			String AText;
			for( int i = 0; i < FReadyElements.size(); ++i )
			{
//				Serial.println( ( *Iter )->Text );
				AText += FReadyElements[ i ]->Text;
				FormattedTextElementBasic *AElemen = FReadyElements[ i ]->Element;
				if( AElemen )
				{
					AText += AElemen->GetText();
					AElemen->ClearModified();
				}
			}

//			Serial.println( AText );
			inherited::OutputPin.Notify( (void *)AText.c_str() );
		}

	protected:
		virtual void SystemLoopEnd() override
		{
			bool AModified = false;
			for( int i = 0; i < FReadyElements.size(); ++i )
			{
				FormattedTextElementBasic *AElemen = FReadyElements[ i ]->Element;
				if( AElemen )
					if( AElemen->GetModified() )
						AModified= true;
			}

			if( AModified )
				if( ! ClockInputPin.IsConnected() )
					ProcessSendOutput();

			inherited::SystemLoopEnd();
		}

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			ProcessSendOutput();
		}

	};
//---------------------------------------------------------------------------
	class FormattedText_Fixed : public Mitov::BasicTypedFormattedText<char *>
	{
		typedef Mitov::BasicTypedFormattedText<char *> inherited;

	public:
		void AddNullElement( const char *ATextItem )
		{
			TStringItem	*AItem = new TStringItem;

			AItem->Text = (char *)ATextItem;
			AItem->Element = nullptr;

			inherited::FReadyElements.push_back( AItem );
		}

	protected:
		virtual void SystemStart() override
		{
			for( int i = 0; i < FElements.size(); ++i )
				FElements[ i ]->SystemStart();
			
			inherited::SystemStart();
			ProcessSendOutput();
		}

	};
//---------------------------------------------------------------------------
	class FormattedText : public Mitov::BasicTypedFormattedText<String>
	{
		typedef Mitov::BasicTypedFormattedText<String> inherited;

	public:
		String Text;

	public:
		void InitElements()
		{
			FReadyElements.clear();
			String	ATextItem;
			String	AIndexText;
			bool	AInEscape = false;

//			Serial.println( "INIT" );
//			Serial.println( Text );
//			delay( 1000 );

			for( unsigned int i = 0; i < Text.length(); ++ i )
			{
				char AChar = Text[ i ];
				if( AInEscape )
				{
					if( AChar >= '0' && AChar <= '9' )
						AIndexText += AChar;

					else
					{
						if( AChar == '%' )
						{
							if( AIndexText.length() == 0 )
							{
								ATextItem += '%';
								AInEscape = false;
							}

							else
							{
								AddReadyElement( ATextItem, AIndexText.toInt() );
								ATextItem = "";
								AIndexText = "";
							}

						}

						else
						{
							if( AIndexText.length() == 0 )
								ATextItem += '%';

							else
							{
								AddReadyElement( ATextItem, AIndexText.toInt() );
								ATextItem = "";
							}

							ATextItem += AChar;
						}

						if( AChar != '%' )
							AInEscape = false;
					}
				}

				else
				{
					if( AChar == '%' )
					{
						AInEscape = true;
						AIndexText = "";
					}

					else
						ATextItem += AChar;

				}

			}

			if( AInEscape )
				AddReadyElement( ATextItem, AIndexText.toInt() );

			else if( ATextItem.length() )
			{
				TStringItem	*AItem = new TStringItem;

				AItem->Text = ATextItem;
				AItem->Element = nullptr;

				FReadyElements.push_back( AItem );
			}

//			Serial.println( "DEBUG>>" );
//			for( Mitov::SimpleList<TStringItem *>::iterator Iter = FReadyElements.begin(); Iter != FReadyElements.end(); ++Iter )
//				Serial.println( ( *Iter )->Text );

//			Serial.println( "<<DEBUG" );
		}

	protected:
		virtual void SystemStart() override
		{
			for( int i = 0; i < FElements.size(); ++i )
				FElements[ i ]->SystemStart();
			
			InitElements();
			inherited::SystemStart();
			ProcessSendOutput();
		}

	};
//---------------------------------------------------------------------------
	template<typename T_LCD, T_LCD *T_LCD_INSTANCE, typename T> class TextFormatElementInput : public OpenWire::Object
	{
		typedef OpenWire::Object	inherited;

	public:
		OpenWire::SinkPin	InputPin;

	protected:
		void DoReceive( void *_Data )
		{
			T_LCD_INSTANCE->SetValue( String( *(T*)_Data ));
		}

	public:
		TextFormatElementInput()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&TextFormatElementInput::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	template<typename T_LCD, T_LCD *T_LCD_INSTANCE> class TextFormatElementStringInput : public OpenWire::Object
	{
		typedef OpenWire::Object	inherited;

	public:
		OpenWire::SinkPin	InputPin;

	protected:
		void DoReceive( void *_Data )
		{
			T_LCD_INSTANCE->SetValue( (char*)_Data );
		}

	public:
		TextFormatElementStringInput()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&TextFormatElementStringInput::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	template<typename T_LCD, T_LCD *T_LCD_INSTANCE> class TextFormatElementClockInput : public OpenWire::Object
	{
		typedef OpenWire::Object	inherited;

	public:
		OpenWire::SinkPin	InputPin;

	protected:
		void DoReceive( void *_Data )
		{
			T_LCD_INSTANCE->SetValue( "(Clock)" );
		}

	public:
		TextFormatElementClockInput()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&TextFormatElementClockInput::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	template<typename T_LCD, T_LCD *T_LCD_INSTANCE, typename T_OBJECT> class TextFormatElementObjectInput : public OpenWire::Object
	{
		typedef OpenWire::Object	inherited;

	public:
		OpenWire::SinkPin	InputPin;

	protected:
		void DoReceive( void *_Data )
		{
			T_LCD_INSTANCE->SetValue( ((T_OBJECT *)_Data)->ToString() );
		}

	public:
		TextFormatElementObjectInput()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&TextFormatElementObjectInput::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	class FormattedTextElementText : public FormattedTextElementBasic
	{
		typedef Mitov::FormattedTextElementBasic inherited;

	public:
		String	InitialValue;
		String	FValue;
		bool	FModified = false;

	public:
		void SetValue( String AValue )
		{
			FModified = true;
			FValue = AValue;
		}

	public:
		virtual String GetText() override
		{
//			Serial.print( "TEST1: " );
//			Serial.println( FValue );
			return FValue;
		}

		virtual void ClearModified() override
		{
			FModified = false;
		}

		virtual bool GetModified() override
		{ 
			return FModified;
		}

	public:
		virtual void SystemStart() override
		{
//			inherited::SystemStart();
			if( ! FModified )
				FValue = InitialValue;

//			Serial.print( "TEST0: " );
//			Serial.println( InitialValue );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T> class FormattedTextElementTyped : public FormattedTextElementBasic
	{
		typedef Mitov::FormattedTextElementBasic inherited;

	public:
		T	InitialValue;

	public:
		OpenWire::ValueSimpleModifiedSinkPin<T>	InputPin;

		virtual void ClearModified() override
		{
			InputPin.Modified = false;
		}

		virtual bool GetModified() override
		{ 
			return InputPin.Modified;
		}

	public:
		virtual void SystemStart() override
		{
//			inherited::SystemStart();
			InputPin.Value = InitialValue;
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T> class FormattedTextLengthElementTyped : public T
	{
		typedef T inherited;

	public:
		uint8_t	Length = 0;
		char	FillCharacter = ' ';

	public:
		virtual String GetText() override
		{
			String AText = inherited::GetText();
			for( int i = AText.length(); i < Length; ++i )
				AText = FillCharacter + AText;

			return AText;
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class FormattedTextElementInteger : public Mitov::FormattedTextElementTyped<int32_t>
	{
		typedef Mitov::FormattedTextElementTyped<int32_t> inherited;

	public:
		uint8_t	Base = 10;

	public:
		virtual String GetText() override
		{
			char AText[ 16 ];
			ltoa( InputPin.Value, AText, Base );

			return AText;
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class FormattedTextElementUnsigned : public Mitov::FormattedTextElementTyped<uint32_t>
	{
		typedef Mitov::FormattedTextElementTyped<uint32_t> inherited;

	public:
		uint8_t	Base = 10;

	public:
		virtual String GetText() override
		{
			char AText[ 16 ];
			itoa( InputPin.Value, AText, Base );

			return AText;
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class FormattedTextElementAnalog : public Mitov::FormattedTextElementTyped<float>
	{
		typedef Mitov::FormattedTextElementTyped<float> inherited;

	public:
		uint8_t	MinWidth : 5;
		uint8_t	Precision : 5;

	public:
		virtual String GetText() override
		{
			char AText[ 16 ];
			dtostrf( InputPin.Value,  MinWidth, Precision, AText );
//			Serial.println( AText );

			return AText;
		}

	public:
		FormattedTextElementAnalog( BasicFormattedText &AOwner ) :
			inherited( AOwner ),		
			MinWidth( 1 ),
			Precision( 3 )
		{
		}

	};
//---------------------------------------------------------------------------
	class FormattedTextElementDigital : public Mitov::FormattedTextElementTyped<bool>
	{
		typedef Mitov::FormattedTextElementTyped<bool> inherited;

	public:
		String	TrueValue = "true";
		String	FalseValue = "false";

	public:
		virtual String GetText() override
		{
			if( InputPin.Value )
				return TrueValue;

			return FalseValue;
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class TextLength : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;

	protected:
		void DoReceive( void *_Data )
        {
            if( ! Enabled )
                return;

			int32_t AValue = strlen( (char *)_Data );

			OutputPin.Notify( &AValue );
        }

	public:
		TextLength()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&TextLength::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	class FindTextPosition : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool	Enabled : 1;
		bool	IgnoreCase : 1;
		String	Text;

	protected:
		void DoReceive( void *_Data )
        {
            if( ! Enabled )
                return;

			String		AText = Text;
			String		ATextValue = String( (char *)_Data );

			if( IgnoreCase )
			{
				AText.toUpperCase();
				ATextValue.toUpperCase();
			}

			int32_t AValue = ATextValue.indexOf( AText );

			OutputPin.Notify( &AValue );
        }

	public:
		FindTextPosition() :
			Enabled( true ),
			IgnoreCase( false )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&FindTextPosition::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	class FindLastTextPosition : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool	Enabled : 1;
		bool	IgnoreCase : 1;
		String	Text;

	protected:
		void DoReceive( void *_Data )
        {
            if( ! Enabled )
                return;

			String		AText = Text;
			String		ATextValue = String( (char *)_Data );

			if( IgnoreCase )
			{
				AText.toUpperCase();
				ATextValue.toUpperCase();
			}

			int32_t AValue = ATextValue.lastIndexOf( AText );

			OutputPin.Notify( &AValue );
        }

	public:
		FindLastTextPosition() :
			Enabled( true ),
			IgnoreCase( false )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&FindLastTextPosition::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	class ContainsText : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool	Enabled : 1;
		bool	IgnoreCase : 1;
		String	Text;

	protected:
		void DoReceive( void *_Data )
        {
            if( ! Enabled )
                return;

			String		AText = Text;
			String		ATextValue = String( (char *)_Data );

			if( IgnoreCase )
			{
				AText.toUpperCase();
				ATextValue.toUpperCase();
			}

			bool AValue = ( ATextValue.indexOf( AText ) >= 0 );

			OutputPin.Notify( &AValue );
        }

	public:
		ContainsText() :
			Enabled( true ),
			IgnoreCase( false )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ContainsText::DoReceive );
		}

	protected:
	};
//---------------------------------------------------------------------------
	class TextStartsWith : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled : 1;

	public:
		bool	IgnoreCase : 1;
		String	Text;

	protected:
		void DoReceive( void *_Data )
        {
            if( ! Enabled )
                return;

			String		AText = Text;
			String		ATextValue = String( (char *)_Data );

			if( IgnoreCase )
			{
				AText.toUpperCase();
				ATextValue.toUpperCase();
			}

			bool AValue = ATextValue.startsWith( AText );

			OutputPin.Notify( &AValue );
        }

	public:
		TextStartsWith() :
			Enabled( true ),
			IgnoreCase( false )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&TextStartsWith::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	class TextEndsWith : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool	Enabled : 1;
		bool	IgnoreCase : 1;
		String	Text;

	protected:
		void DoReceive( void *_Data )
        {
            if( ! Enabled )
                return;

			String		AText = Text;
			String		ATextValue = String( (char *)_Data );

			if( IgnoreCase )
			{
				AText.toUpperCase();
				ATextValue.toUpperCase();
			}

			bool AValue = ATextValue.endsWith( AText );

			OutputPin.Notify( &AValue );
        }

	public:
		TextEndsWith() :
			Enabled( true ),
			IgnoreCase( false )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&TextEndsWith::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	class SubText : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;

	public:
		uint32_t	Length = 1;
		uint32_t	Position = 0;

	protected:
		void DoReceive( void *_Data )
        {
            if( ! Enabled )
                return;

			String AValue = String( (char *)_Data ).substring( Position, Position + Length );

			OutputPin.SendValue( AValue );
        }

	public:
		SubText()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&SubText::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	class LeftSubText : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;

	public:
		uint32_t	Length = 1;

	protected:
		void DoReceive( void *_Data )
        {
            if( ! Enabled )
                return;

			String AValue = String( (char *)_Data ).substring( 0, Length );

			OutputPin.SendValue( AValue );
        }

	public:
		LeftSubText()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&LeftSubText::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	class RightSubText : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;

	public:
		uint32_t	Length = 1;

	protected:
		void DoReceive( void *_Data )
        {
            if( ! Enabled )
                return;

			String AValue = String( (char *)_Data );
			AValue = AValue.substring( AValue.length() - Length );

			OutputPin.SendValue( AValue );
        }

	public:
		RightSubText()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&RightSubText::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	class DeleteSubText : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;

	public:
		uint32_t	Length = 1;
		uint32_t	Position = 0;

	protected:
		void DoReceive( void *_Data )
        {
            if( ! Enabled )
                return;

			String AValue = String( (char *)_Data );
			AValue.remove( Position, Length );

			OutputPin.SendValue( AValue );
        }

	public:
		DeleteSubText()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&DeleteSubText::DoReceive );
		}

	protected:
	};
//---------------------------------------------------------------------------
	class DeleteLeftSubText : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;

	public:
		uint32_t	Length = 1;

	protected:
		void DoReceive( void *_Data )
        {
            if( ! Enabled )
                return;

			String AValue = String( (char *)_Data );
			AValue.remove( 0, Length );

			OutputPin.SendValue( AValue );
        }

	public:
		DeleteLeftSubText()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&DeleteLeftSubText::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	class DeleteRightSubText : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;

	public:
		uint32_t	Length = 1;

	protected:
		void DoReceive( void *_Data )
        {
            if( ! Enabled )
                return;

			String AValue = String( (char *)_Data );
			AValue.remove( AValue.length() - Length );

			OutputPin.SendValue( AValue );
        }

	public:
		DeleteRightSubText()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&DeleteRightSubText::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	class ToUpperCase : public OpenWire::Object	
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;

	protected:
		void DoReceive( void *_Data )
        {
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

			String AValue = String( (char *)_Data );
			AValue.toUpperCase();

			OutputPin.SendValue( AValue );
        }

	public:
		ToUpperCase()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ToUpperCase::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	class ToLowerCase : public OpenWire::Object	
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;

	protected:
		void DoReceive( void *_Data )
        {
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

			String AValue = String( (char *)_Data );
			AValue.toLowerCase();

			OutputPin.SendValue( AValue );
        }

	public:
		ToLowerCase()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ToLowerCase::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	class TrimText : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;

	protected:
		void DoReceive( void *_Data )
        {
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

			String AValue = String( (char *)_Data );
			AValue.trim();

			OutputPin.SendValue( AValue );
        }

	public:
		TrimText()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&TrimText::DoReceive );
		}
	};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
	FormattedTextElementBasic::FormattedTextElementBasic( BasicFormattedText &AOwner ) :
		FOwner( AOwner )
	{
		AOwner.FElements.push_back( this );
	}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
	ArduinoStateSetTextValueElement::ArduinoStateSetTextValueElement( TextStateValue &AOwner ) :
		FOwner( AOwner )
	{
		InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ArduinoStateSetTextValueElement::DoReceived );
		FOwner.RegisterElement( this );
	}
//---------------------------------------------------------------------------
	bool ArduinoStateSetTextValueElement::TryActive()
	{
		if( FInputValue )
		{
			FOwner.ExternalSetValue( Value );
			return true;
		}

		return false;
	}
//---------------------------------------------------------------------------
	void ArduinoStateSetTextValueElement::DoReceived( void *_Data )
	{
		if( FInputValue == *(bool *)_Data )
			return;

		FInputValue = *(bool *)_Data;
		if( FInputValue )
			FOwner.ExternalSetValue( Value );

		else
			FOwner.ResetValue();
	}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
