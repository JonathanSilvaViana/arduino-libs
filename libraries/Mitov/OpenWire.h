////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _OPENWIRE_h
#define _OPENWIRE_h

#include <Mitov_SimpleList.h>

#if defined( VISUINO_MAPLE_MINI ) || defined( VISUINO_NUCLEO_F303RE )
	#define VISUINO_STMDUINO
#endif


#if defined( VISUINO_CHIPKIT_DP32 ) || defined( VISUINO_CHIPKIT_LENNY )
	#ifndef digitalPinToInterrupt
		#ifndef NOT_AN_INTERRUPT
			#define NOT_AN_INTERRUPT -1
		#endif

		#define digitalPinToInterrupt(p) ((p) == PIN_INT0 ? 0 : ((p) == PIN_INT1 ? 1 : ((p) == PIN_INT2 ? 2 : ((p) == PIN_INT3 ? 3 : ((p) == PIN_INT4 ? 4 : NOT_AN_INTERRUPT)))))
	#endif
#endif

#if ( defined( VISUINO_STMDUINO ) || defined( VISUINO_TRINKET ))

	#ifdef VISUINO_NUCLEO_F303RE
		#undef digitalPinToInterrupt
	#endif

	#ifndef digitalPinToInterrupt
		#define digitalPinToInterrupt(p) (p)
	#endif

#endif

//		#define digitalPinToInterrupt(p) (p)

#if defined( VISUINO_MAPLE_MINI )
//	char * ltoa(long l, char * buffer, int radix)
//	{
//		return itoa( l, buffer, radix );
//	}
#endif

namespace OpenWire
{
	class StreamPin;
//---------------------------------------------------------------------------	
	class Object
	{
	};
//---------------------------------------------------------------------------	
	class VirtualObject : public Object
	{
	public:
		virtual ~VirtualObject() {}
	};
//---------------------------------------------------------------------------
	class Component : public VirtualObject
	{
	public:
		static void _SystemInit();
		static void _SystemLoop();
		static void _SystemUpdateHardware();

	protected:
		virtual void SystemInit()
		{
//			CalculateFields();
		}

		virtual void SystemStart() {}
		virtual void SystemLoopBegin( unsigned long currentMicros ) {}
		virtual void SystemLoopEnd() {}
		virtual void SystemLoopUpdateHardware() {}
		virtual void SelfDestroy() {}

	public:
		Component();
		virtual ~Component();

	};
//---------------------------------------------------------------------------
	typedef void (Object::*TOnInterrupt)( bool );
//---------------------------------------------------------------------------
	typedef void (Object::*TOnPinReceive) ( void *_Data );
//---------------------------------------------------------------------------
	typedef void (Object::*TOnPinIndexedReceive) ( int AIndex, void *_Data );
//---------------------------------------------------------------------------
/*
	class Pin : public Object
	{
//	public:
//		virtual void InternalConnect( Pin &_other ) {}

	public:
//		virtual void Connect( Pin &_other )
		virtual void Connect( Pin &_other ) // Nneds to be virtual due to compiler bug! :-(
		{
//			Serial.println( "Connect" );
			InternalConnect( _other );
			_other.InternalConnect( *this );
		}
	};
*/
//---------------------------------------------------------------------------
	class ReceivePin : public Object
	{
	public:
		virtual void InternalConnect( StreamPin &_other ) {}

	public:
		virtual void Receive( void *_Data ) {}

	};
//---------------------------------------------------------------------------
	class CallbackPin : public ReceivePin
	{
	protected:
		Object       *OnReceiveObject = nullptr;
		TOnPinReceive OnReceive = nullptr;

	public:
		void SetCallback( Object *AOnReceiveObject, TOnPinReceive AOnReceive )
		{
			OnReceiveObject = AOnReceiveObject;
			OnReceive = AOnReceive;
		}

	public:
		virtual void InternalConnect( StreamPin &_other ) {}

	public:
		virtual void Receive( void *_Data ) override
		{
			if( OnReceive )
				( OnReceiveObject->*OnReceive )( _Data );

		}

	};
//---------------------------------------------------------------------------
	class StreamPin : public Object
	{
	public:
		Mitov::SimpleList<OpenWire::ReceivePin*> Pins;

	public:
		template<typename T> void SendValue( T AValue )
		{
			Notify( &AValue );
		}
/*
		void SendValue( String AValue )
		{
			Notify( (char *)AValue.c_str() );
		}
*/
	public:
		void Connect( OpenWire::ReceivePin &_other )
		{
//			Serial.println( "InternalConnect" );
			Pins.push_back( &_other );
			_other.InternalConnect( *this );
//			Serial.println( Pins.size() );
		}

	public:
		bool IsConnected()
		{
			return ( Pins.size() > 0 );
		}

	public:
		void Notify( void *_data )
		{
			for( int i = 0; i < Pins.size(); ++i )
				Pins[ i ]->Receive( _data );

		}
	};
//---------------------------------------------------------------------------
	template<> void StreamPin::SendValue<String>( String AValue )
	{
		Notify( (char *)AValue.c_str() );
	}
//---------------------------------------------------------------------------
	typedef StreamPin SourcePin;
/*
	class SourcePin : public StreamPin
	{
	};
*/
//---------------------------------------------------------------------------
	typedef CallbackPin SinkPin;
/*
	class SinkPin : public CallbackPin
	{
	};
*/
//---------------------------------------------------------------------------
	class ConnectSinkPin : public CallbackPin
	{
		typedef CallbackPin inherited;

	protected:
		bool	FIsConnected = false;

	public:
		virtual void InternalConnect( StreamPin &_other ) override
		{
			inherited::InternalConnect( _other );
			FIsConnected = true;
		}

		virtual bool IsConnected()
		{
			return FIsConnected;
		}

	};
//---------------------------------------------------------------------------
	template<typename T> class TypedSourcePin : public SourcePin
	{
		typedef SourcePin inherited;

	public:
		T	Value;

	public:
		void SetValue( T AValue, bool AChangeOnly )
		{
			if( AChangeOnly )
				if( AValue == Value )
					return;

			Value = AValue;
			Notify( &Value );
		}

	};
//---------------------------------------------------------------------------
	template<> class TypedSourcePin<String> : public SourcePin
	{
		typedef SourcePin inherited;

	public:
		String	Value;

	public:
		void SetValue( String AValue, bool AChangeOnly )
		{
			if( AChangeOnly )
				if( AValue == Value )
					return;

			Value = AValue;
			SendValue( Value );
		}

	};
//---------------------------------------------------------------------------
	template<typename T> class TypedStartSourcePin : public SourcePin
	{
		typedef SourcePin inherited;

	public:
		T	Value;

	protected:
		bool	FStarted = false;

	public:
		void SetValue( T AValue, bool ASendAll = false )
		{
			if( ! FStarted )
				FStarted = true;

			else
			{
				if( ! ASendAll )
					if( AValue == Value )
						return;
			}

			Value = AValue;
			Notify( &Value );
		}

	};
//---------------------------------------------------------------------------
	template<typename T> class ValueSimpleSinkPin : public ReceivePin
	{
	public:
		T Value = T( 0 );

	public:
		virtual void Receive( void *_Data ) override
		{
			Value = *(T*)_Data;
		}
	};
//---------------------------------------------------------------------------
	template<> class ValueSimpleSinkPin<String> : public ReceivePin
	{
	public:
		String Value;

	public:
		virtual void Receive( void *_Data ) override
		{
			Value = (char *)_Data;
		}
	};
//---------------------------------------------------------------------------
	template<> class ValueSimpleSinkPin<char *> : public ReceivePin
	{
	public:
		String Value;

	public:
		virtual void Receive( void *_Data ) override
		{
			Value = (char*)_Data;
		}

	};
//---------------------------------------------------------------------------
	template<typename T> class ValueSimpleModifiedSinkPin : public ReceivePin
	{
	public:
		T Value = T( 0 );
		bool	Populated : 1;
		bool	Modified : 1;

	public:
		virtual void InternalConnect( StreamPin &_other ) override 
		{
			Populated = false;
		}

	protected:
		virtual void Receive( void *_Data ) override
		{
			T AValue = *(T*)_Data;
			Populated = true;
			if( AValue == Value )
				return;

			Value = AValue;
			Modified = true;
		}

	public:
		ValueSimpleModifiedSinkPin() :
			Populated( true ),
			Modified( false )
		{
		}

	};
//---------------------------------------------------------------------------
	template<> class ValueSimpleModifiedSinkPin<String> : public ReceivePin
	{
	public:
		String Value;
		bool	Populated : 1;
		bool	Modified : 1;

	protected:
		virtual void Receive( void *_Data ) override
		{
			String AValue = String((char*)_Data );
			Populated = true;
			if( AValue == Value )
				return;

			Value = AValue;
			Modified = true;
		}

	public:
		ValueSimpleModifiedSinkPin() :
			Populated( false ),
			Modified( false )
		{
		}
	};
//---------------------------------------------------------------------------
	template<> class ValueSimpleModifiedSinkPin<char *> : public ReceivePin
	{
	public:
		String Value;
		bool	Populated : 1;
		bool	Modified : 1;

	protected:
		virtual void Receive( void *_Data ) override
		{
			String AValue = String((char*)_Data );
			Populated = true;
			if( AValue == Value )
				return;

			Value = AValue;
			Modified = true;
		}

	public:
		ValueSimpleModifiedSinkPin() :
			Populated( false ),
			Modified( false )
		{
		}
	};
//---------------------------------------------------------------------------
	class ValueSimpleClockedFlagSinkPin : public ReceivePin
	{
		typedef ReceivePin inherited;

	public:
		bool	Clocked : 1;

	protected:
		bool	FIsConnected : 1;

	protected:
		virtual void Receive( void *_Data ) override
		{
			Clocked = true;
		}

	public:
		virtual void InternalConnect( StreamPin &_other ) override
		{
			inherited::InternalConnect( _other );
			FIsConnected = true;
		}

		virtual bool IsConnected()
		{
			return FIsConnected;
		}

	public:
		ValueSimpleClockedFlagSinkPin() :
			Clocked( false ),
			FIsConnected( false )
		{
		}
	};
//---------------------------------------------------------------------------
	template<typename T> class ValueSinkPin : public SinkPin
	{
		typedef SinkPin inherited;

	public:
		T Value = T( 0 );

	public:
		virtual void Receive( void *_Data ) override
		{
			Value = *(T*)_Data;
			inherited::Receive( _Data );
		}
	};
//---------------------------------------------------------------------------
	template<> class ValueSinkPin<String> : public SinkPin
	{
		typedef SinkPin inherited;

	public:
		String Value;

	public:
		virtual void Receive( void *_Data ) override
		{
			Value = (char *)_Data;
			inherited::Receive( _Data );
		}
	};
//---------------------------------------------------------------------------
	template<> class ValueSinkPin<char *> : public SinkPin
	{
		typedef SinkPin inherited;

	public:
		String Value;

	public:
		virtual void Receive( void *_Data ) override
		{
			Value = (char*)_Data;
			inherited::Receive( _Data );
		}

	};
//---------------------------------------------------------------------------
	template<typename T> class ValueChangeDetectSinkPin : public SinkPin
	{
		typedef SinkPin inherited;

	public:
		T Value = T( 0 );

	public:
		virtual void Receive( void *_Data ) override
		{
			if( Value == *(T*)_Data )
				return;

			Value = *(T*)_Data;
			inherited::Receive( _Data );
		}
	};
//---------------------------------------------------------------------------
	template<> class ValueChangeDetectSinkPin<String> : public SinkPin
	{
		typedef SinkPin inherited;

	public:
		String Value;

	public:
		virtual void Receive( void *_Data ) override
		{
			String AValue = (char *)_Data;
			if( Value == AValue )
				return;

			Value = AValue;
			inherited::Receive( _Data );
		}
	};
//---------------------------------------------------------------------------
	template<> class ValueChangeDetectSinkPin<char *> : public SinkPin
	{
		typedef SinkPin inherited;

	public:
		String Value;

	public:
		virtual void Receive( void *_Data ) override
		{
			String AValue = (char *)_Data;
			if( Value == AValue )
				return;

			Value = AValue;
			inherited::Receive( _Data );
		}

	};
//---------------------------------------------------------------------------
	template<typename T> class ValueChangeSinkPin : public SinkPin
	{
		typedef SinkPin inherited;

	public:
		T Value = 0;
		T OldValue = 0;

	public:
		virtual void Receive( void *_Data ) override
		{
			Value = *(T*)_Data;
			inherited::Receive( _Data );
		}

	};
//---------------------------------------------------------------------------
	template<> class ValueChangeSinkPin<char *> : public SinkPin
	{
		typedef SinkPin inherited;

	public:
		String Value;
		String OldValue;

	public:
		virtual void Receive( void *_Data ) override
		{
			Value = (char *)_Data;
			inherited::Receive( _Data );
		}

	};
//---------------------------------------------------------------------------
	template<> class ValueChangeSinkPin<String> : public SinkPin
	{
		typedef SinkPin inherited;

	public:
		String Value;
		String OldValue;

	public:
		virtual void Receive( void *_Data ) override
		{
			Value = (char *)_Data;
			inherited::Receive( _Data );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_SIZE> class IndexedSinkPin : public ReceivePin
	{
		typedef SinkPin inherited;

	public:
		T_SIZE Index = 0;

	public:
		Object				*OnReceiveObject = nullptr;
		TOnPinIndexedReceive OnIndexedReceive = nullptr;

	public:
		void SetCallback( int AIndex, Object *AOnReceiveObject, TOnPinIndexedReceive AOnReceive )
		{
			Index = AIndex;
			OnReceiveObject = AOnReceiveObject;
			OnIndexedReceive = AOnReceive;
		}

	protected:
		virtual void Receive( void *_Data ) override
		{
			if( OnIndexedReceive )
				( OnReceiveObject->*OnIndexedReceive )( Index, _Data );

		}

	};
//---------------------------------------------------------------------------
	template<typename T> class LiveBindingSink : public OpenWire::ReceivePin
	{
	protected:
		void (*FFunc)( T AData );

	protected:
		virtual void Receive( void *_Data ) override
		{
			FFunc(*(T*)_Data );
		}

	public:
		LiveBindingSink( void (*AFunc)( T AData ) ) :
			FFunc( AFunc )
		{
		}
	};
//---------------------------------------------------------------------------
	template<> void LiveBindingSink<String>::Receive( void *_Data )
	{
		FFunc((char*)_Data );
	}
//---------------------------------------------------------------------------
	static Mitov::SimpleList<OpenWire::Component*, uint16_t> _Components;
//---------------------------------------------------------------------------
	class PinRaisingInterrupt
	{
	protected:
		struct THandlers
		{
			Object			*Instance;
			TOnInterrupt	Callback;

		public:
			THandlers()
			{
			}

			THandlers( Object *AInstance, TOnInterrupt ACallback ) :
				Instance( AInstance ),
				Callback( ACallback )
			{
			}
		};

	protected:
		Mitov::SimpleList<THandlers> FHandlersList;

	public:
		void InterruptHandler()
		{
			for( int i = 0; i < FHandlersList.size(); ++i )
				( FHandlersList[ i ].Instance->*FHandlersList[ i ].Callback )( true );
				
		}

	public:
		void SubscribeRising( Object *AInstance, TOnInterrupt ACallback )
		{
			FHandlersList.push_back( THandlers( AInstance, ACallback ));
		}

	public:
		PinRaisingInterrupt( int AInterruptPin, void (*AInterruptRoutine)() )
		{
			// Add by Adrien van den Bossche <vandenbo@univ-tlse2.fr> for Teensy
			// ARM M4 requires the below. else pin interrupt doesn't work properly.
			// On all other platforms, its innocuous, belt and braces
			pinMode( AInterruptPin, INPUT ); 

			int AInterruptNumber = digitalPinToInterrupt( AInterruptPin );
			attachInterrupt( AInterruptNumber, AInterruptRoutine, RISING );
		}
	};
//---------------------------------------------------------------------------
	class PinFallingInterrupt
	{
	protected:
		struct THandlers
		{
			Object			*Instance;
			TOnInterrupt	Callback;

		public:
			THandlers()
			{
			}

			THandlers( Object *AInstance, TOnInterrupt ACallback ) :
				Instance( AInstance ),
				Callback( ACallback )
			{
			}
		};

	protected:
		Mitov::SimpleList<THandlers> FHandlersList;

	public:
		void InterruptHandler()
		{
			for( int i = 0; i < FHandlersList.size(); ++i )
				( FHandlersList[ i ].Instance->*FHandlersList[ i ].Callback )( false );
				
		}

	public:
		void SubscribeFalling( Object *AInstance, TOnInterrupt ACallback )
		{
			FHandlersList.push_back( THandlers( AInstance, ACallback ));
		}

	public:
		PinFallingInterrupt( int AInterruptPin, void (*AInterruptRoutine)() )
		{
			// Add by Adrien van den Bossche <vandenbo@univ-tlse2.fr> for Teensy
			// ARM M4 requires the below. else pin interrupt doesn't work properly.
			// On all other platforms, its innocuous, belt and braces
			pinMode( AInterruptPin, INPUT ); 

			int AInterruptNumber = digitalPinToInterrupt( AInterruptPin );
			attachInterrupt( AInterruptNumber, AInterruptRoutine, FALLING );
		}
	};
//---------------------------------------------------------------------------
	class PinChangeInterrupt
	{
	protected:
		struct THandlers
		{
			Object			*Instance;
			TOnInterrupt	Callback;

		public:
			THandlers()
			{
			}

			THandlers( Object *AInstance, TOnInterrupt ACallback ) :
				Instance( AInstance ),
				Callback( ACallback )
			{
			}
		};

	protected:
		int FPinNumber;
		Mitov::SimpleList<THandlers> FRisingHandlersList;
		Mitov::SimpleList<THandlers> FFallingHandlersList;

	public:
		void InterruptHandler()
		{
			if( digitalRead( FPinNumber ) == HIGH )
			{
				for( int i = 0; i < FRisingHandlersList.size(); ++i )
					( FRisingHandlersList[ i ].Instance->*FRisingHandlersList[ i ].Callback )( true );
			}
			else
			{
				for( int i = 0; i < FFallingHandlersList.size(); ++i )
					( FFallingHandlersList[ i ].Instance->*FFallingHandlersList[ i ].Callback )( false );
			}
		}

	public:
		void SubscribeRising( Object *AInstance, TOnInterrupt ACallback )
		{
			FRisingHandlersList.push_back( THandlers( AInstance, ACallback ));
		}

		void SubscribeFalling( Object *AInstance, TOnInterrupt ACallback )
		{
			FFallingHandlersList.push_back( THandlers( AInstance, ACallback ));
		}

		void SubscribeChange( Object *AInstance, TOnInterrupt ACallback )
		{
			FRisingHandlersList.push_back( THandlers( AInstance, ACallback ));
			FFallingHandlersList.push_back( THandlers( AInstance, ACallback ));
		}

	public:
		PinChangeInterrupt( int AInterruptPin, void (*AInterruptRoutine)() ) :
			FPinNumber( AInterruptPin )
		{
			// Add by Adrien van den Bossche <vandenbo@univ-tlse2.fr> for Teensy
			// ARM M4 requires the below. else pin interrupt doesn't work properly.
			// On all other platforms, its innocuous, belt and braces
			pinMode( AInterruptPin, INPUT ); 

			int AInterruptNumber = digitalPinToInterrupt( AInterruptPin );
			attachInterrupt( AInterruptNumber, AInterruptRoutine, CHANGE );
		}
	};	 
//---------------------------------------------------------------------------
	Component::Component()
	{
		_Components.push_back(this);
	}
//---------------------------------------------------------------------------
	Component::~Component()
	{
		_Components.erase( this );
	}
//---------------------------------------------------------------------------
	void Component::_SystemInit()
	{
		for( int i = 0; i < OpenWire::_Components.size(); ++ i )
			OpenWire::_Components[ i ]->SystemInit();

		for( int i = 0; i < OpenWire::_Components.size(); ++ i )
			OpenWire::_Components[ i ]->SystemStart();

	}
//---------------------------------------------------------------------------
	void Component::_SystemLoop()
	{
		unsigned long currentMicros = micros();

		for( int i = 0; i < OpenWire::_Components.size(); ++ i )
//		for( int i = OpenWire::_Components.size(); i -- ; )
			OpenWire::_Components[ i ]->SystemLoopBegin( currentMicros );

		for( int i = 0; i < OpenWire::_Components.size(); ++ i )
//		for( int i = OpenWire::_Components.size(); i -- ; )
			OpenWire::_Components[ i ]->SystemLoopEnd();

		for( int i = 0; i < OpenWire::_Components.size(); ++ i )
//		for( int i = OpenWire::_Components.size(); i -- ; )
			OpenWire::_Components[ i ]->SystemLoopUpdateHardware();

		for( int i = OpenWire::_Components.size(); i -- ; )
			OpenWire::_Components[ i ]->SelfDestroy();

	}
//---------------------------------------------------------------------------
	void Component::_SystemUpdateHardware()
	{
		for( int i = 0; i < OpenWire::_Components.size(); ++ i )
//		for( int i = OpenWire::_Components.size(); i -- ; )
			OpenWire::_Components[ i ]->SystemLoopUpdateHardware();

	}
//---------------------------------------------------------------------------
}

#endif
