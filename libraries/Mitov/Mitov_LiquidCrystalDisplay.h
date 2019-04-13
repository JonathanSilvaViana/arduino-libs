////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_IQUID_CRYSTAL_DISPLAY_h
#define _MITOV_IQUID_CRYSTAL_DISPLAY_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	class LiquidCrystalDisplayBasic;
//---------------------------------------------------------------------------
	class LiquidCrystalElementBasic : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public: // Public for the print access
		Mitov::LiquidCrystalDisplayBasic &FOwner;

	public:
		Mitov::LiquidCrystalDisplayBasic *GetDisplay() { return &FOwner; }

	public:
		virtual void DisplayInit() {}
		virtual void DisplayStart() {}

	public:
		LiquidCrystalElementBasic( Mitov::LiquidCrystalDisplayBasic &AOwner );

	};
//---------------------------------------------------------------------------
	// TODO: Implement setCursor() and createChar()
	class LiquidCrystalDisplayBasic : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public: // Public for the print access
		Mitov::SimpleList<LiquidCrystalElementBasic *>	FElements;

	public:
#ifdef _MITOV_LCD_SCROLL_LEFT_PIN_
		OpenWire::SinkPin	ScrollLeftInputPin;
#endif

#ifdef _MITOV_LCD_SCROLL_RIGHT_PIN_
		OpenWire::SinkPin	ScrollRightInputPin;
#endif

#ifdef _MITOV_LCD_CLEAR_PIN_
		OpenWire::SinkPin	ClearInputPin;
#endif

#ifdef _MITOV_LCD_HOME_PIN_
		OpenWire::SinkPin	HomeInputPin;
#endif

	public:
		bool Enabled : 1;
		bool AutoScroll : 1;
		bool RightToLeft : 1;
		bool ShowCursor : 1;
		bool Blink : 1;
		bool Hight10Pixels : 1;
		bool Backlight : 1; // = true; // In base class to save memory
		bool BacklightPositive : 1; // = true; // In base class to save memory

	protected:
		uint8_t	FWidth;
		uint8_t	FHeight;

	protected:
		uint8_t FCursorLine = 0;
		uint8_t FCursorPos = 0;

	public:
		static const uint8_t LCD_8BITMODE	= 0x10;
//		static const uint8_t LCD_4BITMODE	= 0x00;
		static const uint8_t LCD_2LINE		= 0x08;
//		static const uint8_t LCD_1LINE		= 0x00;
		static const uint8_t LCD_5x10DOTS	= 0x04;
//		static const uint8_t LCD_5x8DOTS	= 0x00;

		static const uint8_t LCD_ENTRYLEFT				= 0x02;
		static const uint8_t LCD_ENTRYSHIFTINCREMENT	= 0x01;

		static const uint8_t LCD_DISPLAYON	= 0x04;
		static const uint8_t LCD_CURSORON	= 0x02;
		static const uint8_t LCD_BLINKON	= 0x01;

		static const uint8_t LCD_DISPLAYMOVE	= 0x08;
		static const uint8_t LCD_MOVELEFT       = 0x00;
		static const uint8_t LCD_MOVERIGHT		= 0x04;

		static const uint16_t HOME_CLEAR_EXEC	= 2000;

// LCD Commands
// ---------------------------------------------------------------------------
		static const uint8_t LCD_CLEARDISPLAY       = 0x01;
		static const uint8_t LCD_RETURNHOME         = 0x02;
		static const uint8_t LCD_ENTRYMODESET       = 0x04;
		static const uint8_t LCD_DISPLAYCONTROL     = 0x08;
		static const uint8_t LCD_CURSORSHIFT        = 0x10;
		static const uint8_t LCD_FUNCTIONSET        = 0x20;
		static const uint8_t LCD_SETCGRAMADDR       = 0x40;
		static const uint8_t LCD_SETDDRAMADDR		= 0x80;

	public:
		void SetCursor(uint8_t col, uint8_t row)
		{
		   const byte row_offsetsDef[]   = { 0x00, 0x40, 0x14, 0x54 }; // For regular LCDs
		   const byte row_offsetsLarge[] = { 0x00, 0x40, 0x10, 0x50 }; // For 16x4 LCDs
   
		   if ( row >= FHeight ) 
			  row = FHeight - 1;    // rows start at 0
   
		   // 16x4 LCDs have special memory map layout
		   // ----------------------------------------
		   if ( FWidth == 16 && FHeight == 4 )
			  Command(LCD_SETDDRAMADDR | (col + row_offsetsLarge[row]));

		   else 
			  Command(LCD_SETDDRAMADDR | (col + row_offsetsDef[row]));
   
		}

		size_t Print( const String AValue )
		{
			const char *APtr = AValue.c_str();
			size_t ACount = 0;
			while( *APtr )
			{
				Write( *APtr++ );
				++ ACount;
			}

			return ACount;
		}

		size_t Print( float AValue )
		{
			char AText[ 16 ];
			dtostrf( AValue,  1, 2, AText );
			return Print( String( AText ));
		}

		size_t Print( int32_t AValue )
		{
			char AText[ 16 ];
			ltoa( AValue, AText, 10 );
			return Print( String( AText ));
		}

		size_t Print( uint32_t AValue )
		{
			char AText[ 16 ];
			ltoa( AValue, AText, 10 );
			return Print( String( AText ));
		}

		size_t Print( char AValue )
		{
			Write( (uint8_t)AValue );
			return 1;
		}

		size_t Print( byte AValue )
		{
			Write( AValue );
			return 1;
		}

	public:
		Mitov::LiquidCrystalDisplayBasic *GetDisplay() { return this; }

	public:
		inline void Command(uint8_t value)
		{
			Send( value, true, false );
		}

		void Write(uint8_t value)
		{
			Send( value, false, false );
		}

		virtual void Send( uint8_t value, bool ACommand, bool A4Bits ) {} // = 0;

	public:
		void ClearLine()
		{
			SetCursor( 0, FCursorLine );
			FCursorPos = 0;
//			for( int i = 0; i < FCols; ++ i )
//				Interface::FLcd->print( ' ' );

//			Interface::FLcd->setCursor( 0, FCursorLine );
		}

		void NewLine( int AClearSize )
		{
			for( int i = 0; i < FWidth - FCursorPos - AClearSize; ++ i )
				Print( ' ' );

			++FCursorLine;
			FCursorPos = 0;
			if( FCursorLine >= FHeight )
				FCursorLine = 0;

//			Interface::FLcd->setCursor( 0, FCursorLine );
		}

		void MoveChars( int ACount )
		{
			++FCursorPos;
		}

	protected:
		void UpdateDisplayControl()
		{
			uint8_t AValue = 0;
			if( Enabled )
				AValue |= LCD_DISPLAYON;

			if( ShowCursor )
				AValue |= LCD_CURSORON;

			if( Blink )
				AValue |= LCD_BLINKON;

			Command( LCD_DISPLAYCONTROL | AValue );
		}

		void UpdateEntryModeSet()
		{
			uint8_t AValue = 0;

			if( ! RightToLeft )
				AValue |= LCD_ENTRYLEFT;

			if( AutoScroll )
				AValue |= LCD_ENTRYSHIFTINCREMENT;

			Command( LCD_ENTRYMODESET | AValue );
		}

		void Clear()
		{
			Command(LCD_CLEARDISPLAY);             // clear display, set cursor position to zero
			delayMicroseconds(HOME_CLEAR_EXEC);    // this command is time consuming
		}

#ifdef _MITOV_LCD_SCROLL_LEFT_PIN_
		void DoScrollLeft( void * )
		{
			Command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT );
//			Interface::FLcd->scrollDisplayLeft();
		}
#endif

#ifdef _MITOV_LCD_SCROLL_RIGHT_PIN_
		void DoScrollRight( void * )
		{
			Command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
//			Interface::FLcd->scrollDisplayRight();
		}
#endif

#ifdef _MITOV_LCD_CLEAR_PIN_
		void DoClear( void * )
		{
			Clear();
		}
#endif

#ifdef _MITOV_LCD_HOME_PIN_
		void DoHome( void * )
		{
			Command(LCD_RETURNHOME);             // set cursor position to zero
			delayMicroseconds(HOME_CLEAR_EXEC);  // This command is time consuming
//			Interface::FLcd->home();
		}
#endif

	public:
		virtual void SystemInit() override
		{
//			inherited::SystemInit();


//			Interface::FLcd->begin( FWidth, FHeight );
//			UpdateDisplayControl();
//			UpdateEntryModeSet();

			for( int i = 0; i < FElements.size(); ++i )
				FElements[ i ]->DisplayInit();

//			Interface::FLcd->setCursor(0,0);
		}

		virtual void SystemStart() override
		{
//			inherited::SystemStart();

			for( int i = 0; i < FElements.size(); ++i )
				FElements[ i ]->DisplayStart();
		}

	public:
		LiquidCrystalDisplayBasic( uint8_t AWidth, uint8_t AHeight ) :
			Enabled( true ),
			AutoScroll( false ),
			RightToLeft( false ),
			ShowCursor( false ),
			Blink( false ),
			Hight10Pixels( false ),
			Backlight( true ),
			BacklightPositive( true ),
			FWidth( AWidth ),
			FHeight( AHeight )
		{
#ifdef _MITOV_LCD_SCROLL_LEFT_PIN_
			ScrollLeftInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&LiquidCrystalDisplayBasic::DoScrollLeft );
#endif

#ifdef _MITOV_LCD_SCROLL_RIGHT_PIN_
			ScrollRightInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&LiquidCrystalDisplayBasic::DoScrollRight );
#endif

#ifdef _MITOV_LCD_CLEAR_PIN_
			ClearInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&LiquidCrystalDisplayBasic::DoClear );
#endif

#ifdef _MITOV_LCD_HOME_PIN_
			HomeInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&LiquidCrystalDisplayBasic::DoHome );
#endif
		}

	};
//---------------------------------------------------------------------------
	template<uint8_t C_PIN_COUNT> class LiquidCrystalDisplayParallel : public LiquidCrystalDisplayBasic
	{
		typedef LiquidCrystalDisplayBasic inherited;

	public:
		uint8_t *__Pins;

	protected:
		static const uint8_t	EXEC_TIME = 37;
		static const uint8_t	RS_PIN_INDEX = 0;
		static const uint8_t	RW_PIN_INDEX = 1;
		static const uint8_t	ENABLE_PIN_INDEX = 2;

	public:
		virtual void Send( uint8_t value, bool ACommand, bool A4Bits ) override 
		{
			digitalWrite( __Pins[ RS_PIN_INDEX ], ( ! ACommand ) );
   
			// if there is a RW pin indicated, set it low to Write
			// ---------------------------------------------------
			if( __Pins[ RW_PIN_INDEX ] != 255 ) 
				digitalWrite( __Pins[ RW_PIN_INDEX ], LOW );
   
			if( ! A4Bits )
				WriteNbits( value >> 4, 4 );

			WriteNbits( value, 4 );

			delayMicroseconds( EXEC_TIME ); // wait for the command to execute by the LCD
		}

		void WriteNbits(uint8_t value, uint8_t numBits) 
		{
			for (uint8_t i = 0; i < numBits; i++) 
				digitalWrite( __Pins[ i + 3 ], (value >> i) & 0x01);

			PulseEnable();
		}

		void PulseEnable(void) 
		{
		   // There is no need for the delays, since the digitalWrite operation
		   // takes longer.
		   digitalWrite( __Pins[ ENABLE_PIN_INDEX ], HIGH );   
		   delayMicroseconds( 1 );          // enable pulse must be > 450ns   
		   digitalWrite( __Pins[ ENABLE_PIN_INDEX ], LOW );
		}

	protected:
		void InitRegisters()
		{
			// 4 bit mode
			// this is according to the hitachi HD44780 datasheet
			// figure 24, pg 46

			// we start in 8bit mode, try to set 4 bit mode
			Send( 0x03, true, true );
//			delayMicroseconds(4500); // wait min 4.1ms
      
			// second try
			Send( 0x03, true, true );
//			delayMicroseconds(4500); // wait min 4.1ms
      
			// third go!
			Send( 0x03, true, true );
//			delayMicroseconds(150);
      
			// finally, set to 4-bit interface
			Send( 0x02, true, true ); 
		}

	protected:
		virtual void SystemInit(void) override
		{
			for (uint8_t i = 0; i < C_PIN_COUNT + 3; i++)
				pinMode( __Pins[ i ], OUTPUT );

			// Now we pull both RS and R/W low to begin commands
			digitalWrite( __Pins[ RS_PIN_INDEX ], LOW);
			digitalWrite( __Pins[ ENABLE_PIN_INDEX ], LOW);
   
			if( __Pins[ RW_PIN_INDEX ] != 255 ) 
				digitalWrite( __Pins[ RW_PIN_INDEX ], LOW);

			// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
			// according to datasheet, we need at least 40ms after power rises above 2.7V
			// before sending commands. Arduino can turn on way before 4.5V so we'll wait 
			// 50
			// ---------------------------------------------------------------------------
			delay( 100 ); // 100ms delay

//			Serial.println( "STEP1" );


			InitRegisters();
      
			// finally, set # lines, font size, etc.
			uint8_t ADisplayFunction = 0;
			if( C_PIN_COUNT == 8 )
				ADisplayFunction |= LCD_8BITMODE;

			if( FHeight > 1 )
				ADisplayFunction |= LCD_2LINE;

			if( Hight10Pixels && ( FHeight == 1 ))
				ADisplayFunction |= LCD_5x10DOTS;

//			Serial.print( "ADisplayFunction: " );
//			Serial.println( ADisplayFunction, HEX );

			Command( LCD_FUNCTIONSET | ADisplayFunction );  
   
//			Serial.println( "UpdateDisplayControl()" );
			UpdateDisplayControl();
   
			// clear the LCD
//			Serial.println( "Clear()" );
			Clear();

//			Serial.println( "UpdateEntryModeSet()" );
			UpdateEntryModeSet();
   
			// Initialize to default text direction (for romance languages)
//			_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
			// set the entry mode
//			command(LCD_ENTRYMODESET | _displaymode);

			inherited::SystemInit();
//			UpdateBacklight();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<> void LiquidCrystalDisplayParallel<8>::InitRegisters()
	{
		uint8_t ADisplayFunction = LCD_8BITMODE;
		if( FHeight > 1 )
			ADisplayFunction |= LCD_2LINE;

		if( Hight10Pixels && ( FHeight == 1 ))
			ADisplayFunction |= LCD_5x10DOTS;

		// this is according to the hitachi HD44780 datasheet
		// page 45 figure 23
      
		// Send function set command sequence
		Command(LCD_FUNCTIONSET | ADisplayFunction);
		delayMicroseconds(4500);  // wait more than 4.1ms
      
		// second try
		Command(LCD_FUNCTIONSET | ADisplayFunction);
		delayMicroseconds(150);
      
		// third go
		Command(LCD_FUNCTIONSET | ADisplayFunction);
	}
//---------------------------------------------------------------------------
	template<> void LiquidCrystalDisplayParallel<8>::Send( uint8_t value, bool ACommand, bool A4Bits )
	{
		digitalWrite( __Pins[ RS_PIN_INDEX ], ( ! ACommand ) );
   
		// if there is a RW pin indicated, set it low to Write
		// ---------------------------------------------------
		if( __Pins[ RW_PIN_INDEX ] != 255 ) 
			digitalWrite( __Pins[ RW_PIN_INDEX ], LOW );
   
		WriteNbits(value, 8); 

		delayMicroseconds( EXEC_TIME ); // wait for the command to execute by the LCD
	}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
	class LiquidCrystalElementDefineCustomCharacter : public LiquidCrystalElementBasic
	{
		typedef Mitov::LiquidCrystalElementBasic inherited;

	public:
		const /*PROGMEM*/ uint8_t *_CharMap;

	protected:

		uint8_t	FIndex;

	public:
		virtual void DisplayInit() override
		{
			uint8_t ACharMap[ 8 ];
			for( int i = 0; i < 8; ++i )
				ACharMap[ i ] = pgm_read_byte( _CharMap + i );

//			FLcd->createChar( FIndex, ACharMap );
//			location &= 0x7;            // we only have 8 locations 0-7
   
			FOwner.Command( LiquidCrystalDisplayBasic::LCD_SETCGRAMADDR | (FIndex << 3));
			delayMicroseconds(30);
   
			for (int i=0; i<8; i++) 
			{
				FOwner.Write(_CharMap[i]);      // call the virtual write method
				delayMicroseconds(40);
			}
		}

	public:
		LiquidCrystalElementDefineCustomCharacter( Mitov::LiquidCrystalDisplayBasic &AOwner, uint8_t AIndex ) :
			inherited( AOwner ),
			FIndex( AIndex )
		{
			AOwner.FElements.push_back( this );
		}

	};
//---------------------------------------------------------------------------
	class LiquidCrystalElementCustomCharacterField : public LiquidCrystalElementBasic
	{
		typedef Mitov::LiquidCrystalElementBasic inherited;

	public:
		uint8_t Column : 7;
		uint8_t Row : 7;

	public:
		uint8_t Index : 3;

	protected:
		bool	FModified : 1;

	public:
		void SetIndex( uint8_t	AValue )
		{
			if( AValue > 7 )
				AValue = 7;

			else if( AValue < 0 )
				AValue = 0;

			if( Index == AValue )
				return;

			Index = AValue;
			FModified = true;
		}

		void SetColumn( uint8_t AValue )
		{
			if( AValue < 0 )
				AValue = 0;

			if( Column == AValue )
				return;

			Column = AValue;
			FModified = true;
		}

		void SetRow( uint8_t AValue )
		{
			if( AValue < 0 )
				AValue = 0;

			if( Row == AValue )
				return;

			Row = AValue;
			FModified = true;
		}

	public:
		virtual void DisplayStart() override
		{
			DisplayCharacter();
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			inherited::SystemLoopBegin( currentMicros );
			if( FModified )
			{
				DisplayCharacter();
				FModified = false;
			}
		}

	protected:
		void DisplayCharacter()
		{
//			FLcd->setCursor( Column, Row );
//			FLcd->write( (uint8_t) Index );
			FOwner.SetCursor( Column, Row );
			FOwner.Write( (uint8_t) Index );
		}

	public:
		LiquidCrystalElementCustomCharacterField( Mitov::LiquidCrystalDisplayBasic &AOwner ) :
			inherited( AOwner ),
			Column( 0 ),
			Row( 0 ),
			Index( 0 ),
			FModified( false )
		{
			AOwner.FElements.push_back( this );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_LCD, T_LCD *T_LCD_INSTANCE, typename T> class LiquidCrystalDisplayInput : public OpenWire::Object
	{
		typedef OpenWire::Object	inherited;

	public:
		OpenWire::SinkPin	InputPin;

	protected:
		void DoReceive( void *_Data )
		{
			T_LCD_INSTANCE->ClearLine();
			int AClearSize = T_LCD_INSTANCE->GetDisplay()->Print( *(T*)_Data );
			T_LCD_INSTANCE->NewLine( AClearSize );
		}

	public:
		LiquidCrystalDisplayInput()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&LiquidCrystalDisplayInput::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	template<typename T_LCD, T_LCD *T_LCD_INSTANCE> class LiquidCrystalDisplayCharInput : public OpenWire::Component
	{
		typedef OpenWire::Component	inherited;

	public:
		OpenWire::SinkPin	InputPin;

	protected:
		void DoReceive( void *_Data )
		{
//			Serial.println( "DoReceive" );
//			Serial.print( (char*)_Data );
//			if( *(byte*)_Data < ' ' )
//				return;

			if( *(char*)_Data == '\r' )
				T_LCD_INSTANCE->NewLine( 0 );

			else if( *(char*)_Data == '\n' )
				T_LCD_INSTANCE->ClearLine();

			else if( *(char*)_Data >= ' ' )
			{
				int AClearSize = T_LCD_INSTANCE->GetDisplay()->Print( *(char*)_Data );
				T_LCD_INSTANCE->MoveChars( AClearSize );
			}
//			int AClearSize = T_LCD_INSTANCE->FLcd->print( (char*)_Data );
//			T_LCD_INSTANCE->NewLine( AClearSize );
		}

	public:
		LiquidCrystalDisplayCharInput()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&LiquidCrystalDisplayCharInput::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	template<typename T_LCD, T_LCD *T_LCD_INSTANCE> class LiquidCrystalDisplayStringInput : public OpenWire::Object
	{
		typedef OpenWire::Object	inherited;

	public:
		OpenWire::SinkPin	InputPin;

	protected:
		void DoReceive( void *_Data )
		{
			T_LCD_INSTANCE->ClearLine();
			int AClearSize = T_LCD_INSTANCE->GetDisplay()->Print( (char*)_Data );
			T_LCD_INSTANCE->NewLine( AClearSize );
		}

	public:
		LiquidCrystalDisplayStringInput()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&LiquidCrystalDisplayStringInput::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	template<typename T_LCD, T_LCD *T_LCD_INSTANCE> class LiquidCrystalDisplayClockInput : public OpenWire::Object
	{
		typedef OpenWire::Object	inherited;

	public:
		OpenWire::SinkPin	InputPin;

	protected:
		void DoReceive( void *_Data )
		{
			T_LCD_INSTANCE->ClearLine();
			int AClearSize = T_LCD_INSTANCE->GetDisplay()->Print( "(Clock)" );
			T_LCD_INSTANCE->NewLine( AClearSize );
		}

	public:
		LiquidCrystalDisplayClockInput()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&LiquidCrystalDisplayClockInput::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	template<typename T_LCD, T_LCD *T_LCD_INSTANCE, typename T_OBJECT> class LiquidCrystalDisplayObjectInput : public OpenWire::Object
	{
		typedef OpenWire::Object	inherited;

	public:
		OpenWire::SinkPin	InputPin;

	protected:
		void DoReceive( void *_Data )
		{
			T_LCD_INSTANCE->ClearLine();
			int AClearSize = T_LCD_INSTANCE->GetDisplay()->Print( ((T_OBJECT *)_Data)->ToString().c_str() );
			T_LCD_INSTANCE->NewLine( AClearSize );
		}

	public:
		LiquidCrystalDisplayObjectInput()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&LiquidCrystalDisplayObjectInput::DoReceive );
		}
	};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
	class LiquidCrystalElementBasicPositionedWidthField : public LiquidCrystalElementBasic
	{
		typedef Mitov::LiquidCrystalElementBasic inherited;

	public:
		uint8_t Column : 7;
		uint8_t Row : 7;

	public:
		uint8_t Width : 7;
		bool	AllignLeft : 1;
		char	FillCharacter = ' ';

	public:
		void ClearLine()
		{
			FOwner.SetCursor( Column, Row );
		}

		void NewLine( int AClearSize )
		{
			for( int i = 0; i < (int)Width - AClearSize; ++ i )
				FOwner.Print( FillCharacter );
		}

	public:
		LiquidCrystalElementBasicPositionedWidthField( Mitov::LiquidCrystalDisplayBasic &AOwner ) :
			inherited( AOwner ),
			Column( 0 ),
			Row( 0 ),
			Width( 16 ),
			AllignLeft( false )
		{
		}

	};
//---------------------------------------------------------------------------
	class LiquidCrystalElementTextField : public LiquidCrystalElementBasicPositionedWidthField
	{
		typedef Mitov::LiquidCrystalElementBasicPositionedWidthField inherited;

	public:
		String	InitialValue;

	protected:
		virtual void SystemStart() override
		{
			inherited::SystemStart();
			ClearLine();
			int AClearSize = FOwner.Print( InitialValue );
			NewLine( AClearSize );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T> class LiquidCrystalElementBasicPositionedWidthTypedField : public LiquidCrystalElementBasicPositionedWidthField
	{
		typedef Mitov::LiquidCrystalElementBasicPositionedWidthField inherited;

	public:
		T	InitialValue;

	public:
		OpenWire::SinkPin	InputPin;

	protected:
		virtual int PrintValue( T AValue ) = 0;

		void ClearPrintValue( T AValue )
		{
			InitialValue = AValue;
//			Serial.println( AValue );
			ClearLine();
			int AClearSize = PrintValue( AValue );
			NewLine( AClearSize );
		}

	protected:
		virtual void SystemStart() override
		{
			inherited::SystemStart();
			ClearPrintValue( InitialValue );
		}

		void DoReceiveData( void *_Data )
		{
			ClearPrintValue( *(T*)_Data );
		}

	public:
		LiquidCrystalElementBasicPositionedWidthTypedField( Mitov::LiquidCrystalDisplayBasic &AOwner, T AInitialValue ) :
			inherited( AOwner ),
			InitialValue( AInitialValue )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&LiquidCrystalElementBasicPositionedWidthTypedField::DoReceiveData );
		}

	};
//---------------------------------------------------------------------------
	class LiquidCrystalElementIntegerField : public LiquidCrystalElementBasicPositionedWidthTypedField<int32_t>
	{
		typedef Mitov::LiquidCrystalElementBasicPositionedWidthTypedField<int32_t> inherited;

	public:
		uint8_t	Base = 10;

	protected:
		virtual int PrintValue( int32_t AValue ) override
		{
			char AText[ 16 ];
			ltoa( AValue, AText, Base );
			if( Width < 15 )
				AText[ Width ] = '\0';

			if( AllignLeft )
				return FOwner.Print( AText );

			for( int i = 0; i < Width - strlen( AText ); ++i )
				FOwner.Print( FillCharacter );

			FOwner.Print( AText );
			return Width;
		}

	public:
		LiquidCrystalElementIntegerField( Mitov::LiquidCrystalDisplayBasic &AOwner ) :
			inherited( AOwner, 0 )
		{
		}

	};
//---------------------------------------------------------------------------
	class LiquidCrystalElementUnsignedField : public LiquidCrystalElementBasicPositionedWidthTypedField<uint32_t>
	{
		typedef Mitov::LiquidCrystalElementBasicPositionedWidthTypedField<uint32_t> inherited;

	public:
		uint8_t	Base = 10;

	protected:
		virtual int PrintValue( uint32_t AValue ) override
		{
			char AText[ 16 ];
			ltoa( AValue, AText, Base );
			if( Width < 15 )
				AText[ Width ] = '\0';

			return FOwner.Print( AText );
		}

	public:
		LiquidCrystalElementUnsignedField( Mitov::LiquidCrystalDisplayBasic &AOwner ) :
			inherited( AOwner, 0 )
		{
		}

	};
//---------------------------------------------------------------------------
	class LiquidCrystalElementAnalogField : public LiquidCrystalElementBasicPositionedWidthTypedField<float>
	{
		typedef Mitov::LiquidCrystalElementBasicPositionedWidthTypedField<float> inherited;

	public:
		uint8_t	MinWidth = 1;
		uint8_t	Precision = 3;

	protected:
		virtual int PrintValue( float AValue ) override
		{
			char AText[ 16 ];
			dtostrf( AValue,  MinWidth, Precision, AText );
			if( Width < 15 )
				AText[ Width ] = '\0';

			return FOwner.Print( AText );
		}

	public:
		LiquidCrystalElementAnalogField( Mitov::LiquidCrystalDisplayBasic &AOwner ) :
			inherited( AOwner, 0 )
		{
		}

	};
//---------------------------------------------------------------------------
	class LiquidCrystalElementDigitalField : public LiquidCrystalElementBasicPositionedWidthTypedField<bool>
	{
		typedef Mitov::LiquidCrystalElementBasicPositionedWidthTypedField<bool> inherited;

	public:
		String	TrueValue = "true";
		String	FalseValue = "false";

	protected:
		virtual int PrintValue( bool AValue ) override
		{
			if( AValue )
				return FOwner.Print( TrueValue );

			return FOwner.Print( FalseValue );
		}

	public:
		LiquidCrystalElementDigitalField( Mitov::LiquidCrystalDisplayBasic &AOwner ) :
			inherited( AOwner, 0 )
		{
		}

	};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
	LiquidCrystalElementBasic::LiquidCrystalElementBasic( LiquidCrystalDisplayBasic &AOwner ) :
		FOwner( AOwner )
	{
	}
//---------------------------------------------------------------------------
}


#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
