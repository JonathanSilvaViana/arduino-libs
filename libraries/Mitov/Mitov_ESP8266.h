////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ESP8266_h
#define _MITOV_ESP8266_h

#include <Mitov.h>
#include <Mitov_BasicEthernet.h>
#include <Mitov_StringPrint.h>

//#define __ESP8266__DEBUG__

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	class ESP8266;
//---------------------------------------------------------------------------
	class ESP8266RemoteAccessPoint
	{
	public:
		bool	Enabled = true;
		String	SSID;
	    String	Password;
	};
//---------------------------------------------------------------------------
	class ESP8266Encription
	{
	public:
		bool	WPA : 1;
		bool	WPA2 : 1;

	public:
		ESP8266Encription() :
			WPA( false ),
			WPA2( false )
		{
		}

	};
//---------------------------------------------------------------------------
	class ESP8266AccessPoint
	{
	public:
		bool	Enabled = true;
		String	SSID;
		String	Password;

		unsigned long		Channel = 1;
		ESP8266Encription	Encription;
	};
//---------------------------------------------------------------------------
	class BasicESP8266Socket : public Mitov::BasicSocket
	{
		typedef Mitov::BasicSocket inherited;

	public:
#ifdef _MITOV_ESP8266_SOCKET_CONNECTED_PIN_
		OpenWire::SourcePin	ConnectedOutputPin;
#endif
#ifdef _MITOV_ESP8266_SOCKET_DISCONNECT_PIN_
		OpenWire::SinkPin	DisconnectInputPin;
#endif

	protected:
		ESP8266 &FModule;
		bool	FRunning : 1; // = false;
		bool	FConnecting : 1; // = false;

	protected:
		BufferPrint	FStringPrint;

	protected:
		void DoConnectCompleted( bool ASuccess )
		{
#ifdef __ESP8266__DEBUG__
			Serial.println( "DoConnectCompleted" );
#endif
			FConnecting = false;
			if( ASuccess )
				FRunning = true;
		}

	public:
		virtual void Connected( bool AValue ) 
		{
#ifdef _MITOV_ESP8266_SOCKET_CONNECTED_PIN_
			ConnectedOutputPin.Notify( &AValue );
#endif
		}

		virtual bool IsEnabled()
		{
			return Enabled && FRunning; //FModule.Enabled;
		}

		virtual Print *GetPrint()
		{
			return &FStringPrint;
		}

		virtual void BeginPacket()
		{
			FStringPrint.Value.clear();
		}

		virtual void DataReceived( int8_t connectionId, uint8_t AData ) = 0;

	protected:
#ifdef _MITOV_ESP8266_SOCKET_DISCONNECT_PIN_
		void DoReceiveDisconnect( void * )
		{
			SetEnabled( false );
		}
#endif

	public:
		BasicESP8266Socket( ESP8266 &AModule );
	};
//---------------------------------------------------------------------------
	class ESP8266SerialBasicFunction // : public OpenWire::Object
	{
	public:
		virtual bool TryProcessRequestedInput( String ALine, bool &AResponseCompleted, bool &ASuccess ) { return false; }
		virtual void TimedOut() {}

	public:
		virtual ~ESP8266SerialBasicFunction() {}

	};
//---------------------------------------------------------------------------
	class ESP8266BasicExpectOKFunction : public ESP8266SerialBasicFunction
	{
		typedef ESP8266SerialBasicFunction inherited;

	protected:
		bool	FEmptyLineDetected = false;

	public:
		void Reset()
		{
			FEmptyLineDetected = false;
		}

	public:
		virtual bool TryProcessRequestedInput( String ALine, bool &AResponseCompleted, bool &ASuccess ) override
		{
//			Serial.println( "TryProcessRequestedInput" );
//			Serial.println( ALine );
			if( FEmptyLineDetected )
			{
				ALine.trim();
				if( ALine != "" )
				{
					ASuccess = ( ALine == "OK" );
/*
					if( ASuccess )
					{
//						Serial.println( "OK Processed" );
		//					Serial.println( "ALockInput = false" );
						OKReceived();
					}
*/
					AResponseCompleted = true;
					FEmptyLineDetected = false;
					return true;
				}
			}

			else if( ALine == "" )
			{
//				Serial.println( "FEmptyLineDetected" );
				FEmptyLineDetected = true;
				return true;
			}

			return false;
		}
	};
//---------------------------------------------------------------------------
	class ESP8266BasicExpectSendOKFunction : public ESP8266SerialBasicFunction
	{
		typedef ESP8266SerialBasicFunction inherited;

	protected:
		bool	FEmptyLineDetected = false;

	public:
		void Reset()
		{
			FEmptyLineDetected = false;
		}

	public:
		virtual bool TryProcessRequestedInput( String ALine, bool &AResponseCompleted, bool &ASuccess ) override
		{
//			Serial.println( "TryProcessRequestedInput" );
//			Serial.println( ALine );
			if( FEmptyLineDetected )
			{
				ALine.trim();
				if( ALine != "" )
				{
					ASuccess = ( ALine == "SEND OK" );
/*
					if( ASuccess )
					{
//						Serial.println( "OK Processed" );
		//					Serial.println( "ALockInput = false" );
						OKReceived();
					}
*/
					AResponseCompleted = true;
					FEmptyLineDetected = false;
					return true;
				}
			}

			else if( ALine == "" )
			{
//				Serial.println( "FEmptyLineDetected" );
				FEmptyLineDetected = true;
				return true;
			}

			return false;
		}
	};
//---------------------------------------------------------------------------
	class ESP8266BasicExpectCommandOKFunction : public ESP8266SerialBasicFunction
	{
		typedef ESP8266SerialBasicFunction inherited;

	protected:
		String	FQuery;
		bool	FCommandDetected : 1; // = false;
		bool	FEmptyLineDetected : 1; // = false;

	public:
		void Reset()
		{
			FEmptyLineDetected = false;
			FCommandDetected = false;
		}

	public:
		virtual bool TryProcessRequestedInput( String ALine, bool &AResponseCompleted, bool &ASuccess ) override
		{
//			Serial.println( "CommandOK: TryProcessRequestedInput" );
//			Serial.println( ALine );
			if( FEmptyLineDetected )
			{
				ALine.trim();
				if( ALine != "" )
				{
					ASuccess = ( ALine == "OK" );
/*
					if( ASuccess )
					{
//						Serial.println( "OK Processed" );
		//					Serial.println( "ALockInput = false" );
						OKReceived();
					}
*/
					AResponseCompleted = true;
					FEmptyLineDetected = false;
					return true;
				}
			}

			else if( FCommandDetected && ( ALine == "" ))
			{
//				Serial.println( "FEmptyLineDetected" );
				FEmptyLineDetected = true;
				return true;
			}

			else
			{
				ALine.trim();
				if( ALine == FQuery )
					FCommandDetected = true;
			}


			return false;
		}

	public:
		ESP8266BasicExpectCommandOKFunction( String	AQuery ) :
			FQuery( AQuery ),
			FCommandDetected( false ),
			FEmptyLineDetected( false )
		{
		}
	};
//---------------------------------------------------------------------------
	template<typename T> class ESP8266BasicExpectOKCallbackFunction : public ESP8266SerialBasicFunction
	{
		typedef ESP8266SerialBasicFunction inherited;

	public:
		typedef void (T::*TOnCallback)( bool );

	protected:
		String		FQuery;
		bool		FCommandDetected : 1; // = false;
		bool		FEmptyLineDetected : 1; // = false;
		T			*FOwner;
		TOnCallback	FCallback;

	public:
		virtual bool TryProcessRequestedInput( String ALine, bool &AResponseCompleted, bool &ASuccess ) override
		{
//			Serial.println( "TryProcessRequestedInput" );
//			Serial.println( ALine );
			if( FEmptyLineDetected )
			{
				ALine.trim();
				if( ALine != "" )
				{
					ASuccess = ( ALine == "OK" );

					( FOwner->*FCallback )( ASuccess );
//					if( ASuccess )
//					{
//						Serial.println( "OK Processed" );
		//					Serial.println( "ALockInput = false" );
//						OKReceived();
//					}

					AResponseCompleted = true;
					FEmptyLineDetected = false;
					return true;
				}
			}

			if( FCommandDetected && ( ALine == "" ))
			{
//				Serial.println( "FEmptyLineDetected" );
				FEmptyLineDetected = true;
				return true;
			}

			else
			{
				ALine.trim();
				if( ALine == FQuery )
					FCommandDetected = true;
			}

			return false;
		}

	public:
		ESP8266BasicExpectOKCallbackFunction( T *AOwner, String AQuery, TOnCallback ACallback ) :
			FQuery( AQuery ),
			FCommandDetected( false ),
			FEmptyLineDetected( false ),
			FOwner( AOwner ),
			FCallback( ACallback )
		{
		}

	};
//---------------------------------------------------------------------------
	class ESP8266ConnectAccessPointFunction : public ESP8266SerialBasicFunction
	{
	protected:
		ESP8266	*FOwner;
		uint8_t FIndex : 7;
		bool	FEmptyLineDetected : 1;

	public:
		virtual void TimedOut() override;
		virtual bool TryProcessRequestedInput( String ALine, bool &AResponseCompleted, bool &ASuccess ) override;

	public:
		ESP8266ConnectAccessPointFunction( ESP8266 *AOwner, uint8_t AIndex ) :
			FOwner( AOwner ),
			FIndex( AIndex ),
			FEmptyLineDetected( false )
		{
		}
	};
//---------------------------------------------------------------------------
	class ESP8266AccessPointAddressFunction : public ESP8266SerialBasicFunction
	{
	protected:
		ESP8266	*FOwner;
//		bool	FEmptyLineDetected = false;
		bool	FDataReceived = false;

	public:
		virtual void TimedOut() override
		{
		}

		virtual bool TryProcessRequestedInput( String ALine, bool &AResponseCompleted, bool &ASuccess ) override;

	public:
		ESP8266AccessPointAddressFunction( ESP8266 *AOwner ) :
			FOwner( AOwner )
		{
		}
	};
//---------------------------------------------------------------------------
	class ESP8266SendPacketFunction : public ESP8266SerialBasicFunction
	{
	protected:
		ESP8266	*FOwner;
		Mitov::SimpleList<uint8_t, uint16_t> FData;
//		bool	FEmptyLineDetected = false;

	public:
		virtual bool TryProcessRequestedInput( String ALine, bool &AResponseCompleted, bool &ASuccess ) override;

	public:
		ESP8266SendPacketFunction( ESP8266 *AOwner, Mitov::SimpleList<uint8_t, uint16_t> &AData ) :
			FOwner( AOwner ),
			FData( AData )
		{
#ifdef __ESP8266__DEBUG__
			Serial.print( "DATA : \"" );	Serial.print( Func::ByteListToString( FData )); Serial.println( "\"" );
#endif
		}

	};
//---------------------------------------------------------------------------

	class ESP8266 : public OpenWire::Component, public ESP8266SerialBasicFunction
	{
		typedef OpenWire::Component inherited;

	public:
#ifdef _MITOV_ESP8266_RESET_PIN_
		OpenWire::SinkPin	ResetInputPin;
#endif

#ifdef _MITOV_ESP8266_ACCESS_POINT_IP_PIN_
		OpenWire::SourcePin	AccessPointAddressOutputPin;
#endif

#ifdef _MITOV_ESP8266_ACCESS_POINT_MAC_PIN_
		OpenWire::SourcePin	AccessPointMACOutputPin;
#endif

#ifdef _MITOV_ESP8266_STATION_IP_PIN_
		OpenWire::SourcePin	StationAddressOutputPin;
#endif

#ifdef _MITOV_ESP8266_STATION_MAC_PIN_
		OpenWire::SourcePin	StationMACOutputPin;
#endif

	public:
		ESP8266AccessPoint	AccessPoint;
		Mitov::SimpleObjectList<ESP8266RemoteAccessPoint*>	RemoteAccessPoints;

	public:
		Mitov::SimpleList<BasicESP8266Socket*>	Sockets;

	protected:
		struct TResponseElement
		{
		public:
			ESP8266SerialBasicFunction *Handler;
			bool Delete : 1;
			uint8_t RetryCount : 4;
			uint32_t Timeout;
			uint32_t Delay;
//			String Command;
			Mitov::SimpleList<uint8_t, uint16_t> Command;

		public:
/*
			TResponseElement() :
				Handler( nullptr ),
				Delete( false ),
				Timeout( 0 ),
				ADelay( 0 ),
				RetryCount( 0 )
			{
			}
*/
			TResponseElement( ESP8266SerialBasicFunction *AHandler, bool ADelete, uint32_t ATimeout, Mitov::SimpleList<uint8_t, uint16_t> &ACommand, uint8_t ARetryCount, uint32_t ADelay ) :
				Handler( AHandler ),
				Delete( ADelete ),
				Command( ACommand ),
				RetryCount( ARetryCount ),
				Timeout( ATimeout ),
				Delay( ADelay )
			{
//				Command.append( ACommand.c_str(), ACommand.length() );
#ifdef __ESP8266__DEBUG__
				Serial.print( "TResponseElement: " ); Serial.println( Func::ByteListToString( Command ));
#endif
			}

			~TResponseElement()
			{
				if( Delete )
					delete Handler;
			}

		};

		struct TQueryElement
		{
		public:
//			String Command;
			Mitov::SimpleList<uint8_t, uint16_t> Command;
			TResponseElement *ResponseElement;

		public:
			TQueryElement() :
				ResponseElement( nullptr )
			{
			}

			TQueryElement( Mitov::SimpleList<uint8_t, uint16_t> &ACommand, TResponseElement *AResponseElement ) :
				Command( ACommand ),
				ResponseElement( AResponseElement )
			{
			}

		};

	protected:
		Stream &FStream;
//		Mitov::BasicSerialPort &FSerial;

	protected:
//		Mitov::SimpleList<TResponseElement *>	FResponseHandlersQueue;
		Mitov::SimpleList<TQueryElement>		FQueryQueue;

		char		FBuffer[ 256 ];
		uint8_t		FIndex = 0;

//		uint8_t		FLockRequestedInputIndex = 0;
//		uint8_t		FLockInputIndex = 0;


		TResponseElement *FCurrentElement = nullptr;
		uint32_t	FDelay = 0;
		unsigned long FLastTime = 0;
		unsigned long FCurrentMillis = 0;

		bool	FEmptyLineDetected : 1; // = false;
//		bool	FRestarted : 1; // = false;

		bool	FInSocketReceiving : 1;
		bool	FInSocketProcessing : 1;
		uint8_t FReadConnectionId : 2; //= 0;
		uint16_t FReadLength = 0;


		Mitov::BasicESP8266Socket	*FServerInstance;

	public:
		Mitov::BasicESP8266Socket	*FConnections[ 4 ];


	public:
		virtual bool TryProcessRequestedInput( String ALine, bool &AResponseCompleted, bool &ASuccess ) 
		{ 
			// nready
			if( FEmptyLineDetected )
			{
				ALine.trim();
				if( ALine == "ready" )
				{
//					Serial.println( "READY !!!" );

					AResponseCompleted = true;
					ASuccess = true;
					return true;
				}
			}

			else if( ALine == "" )
			{
//				Serial.println( "FEmptyLineDetected" );
				FEmptyLineDetected = true;
				return true;
			}

			return false;
		}

		virtual void TimedOut() override
		{
			FEmptyLineDetected = false;
		}

	protected:
		void SendQuery( Mitov::SimpleList<uint8_t, uint16_t> &AQuery, TResponseElement *AElement )
		{
#ifdef __ESP8266__DEBUG__
			Serial.print( "QUERY : \"" );	Serial.print( Func::ByteListToString( AQuery )); Serial.println( "\"" );
#endif
			FCurrentMillis = millis();

//			FStream.println( AQuery );
			FStream.write( AQuery, AQuery.size() );
			FCurrentElement = AElement;
		}

		void TrySendQuery( Mitov::SimpleList<uint8_t, uint16_t> &AQuery, TResponseElement *AElement )
		{
//			if( ( PowerOn || ( AQuery == "AT" ) ) && ( FResponseHandlersQueue.size() == 0 ) && ( !FInPowerSwitch ))
//			if( FResponseHandlersQueue.size() == 0 )
			if( ! FCurrentElement )
				SendQuery( AQuery, AElement );

			else
			{
				while( FQueryQueue.size() > 10 )
					SystemLoopBegin( micros());

#ifdef __ESP8266__DEBUG__
				Serial.print( "ADD TO QUERY : \"" );	Serial.print( Func::ByteListToString( AQuery )); Serial.println( "\"" );
#endif
				FQueryQueue.push_back( TQueryElement( AQuery, AElement ));

#ifdef __ESP8266__DEBUG__
				Serial.println( "QUEUE>>" );
				for( int i = 0; i < FQueryQueue.size(); ++i )
					Serial.println( Func::ByteListToString( FQueryQueue[ i ].Command ));

				Serial.println( "<<QUEUE" );
#endif
			}
		}

		void ProcessNextCommand()
		{
			if( FCurrentElement )
			{
				FDelay = FCurrentElement->Delay;
#ifdef __ESP8266__DEBUG__
				Serial.print( "DELAY: " ); Serial.println( FDelay );
#endif
				delete FCurrentElement;
				FCurrentElement = nullptr;
			}

			if( FDelay )
				return;

#ifdef __ESP8266__DEBUG__
			Serial.println( "ProcessNextCommand" );
#endif
//				Serial.print( "RESP_QUEUE: " ); Serial.println( FResponseHandlersQueue.size() );
			if( FQueryQueue.size() )
			{
//					Serial.print( "SEND_QUERY: " ); Serial.println( FQueryQueue.size() );
				TQueryElement AElement = FQueryQueue[ 0 ];
//					Serial.print( "ESTRACT_QUERY: " ); Serial.println( ACommand );
				FQueryQueue.pop_front();

#ifdef __ESP8266__DEBUG__
				Serial.println( "QUEUE>>" );
				for( int i = 0; i < FQueryQueue.size(); ++i )
					Serial.println( Func::ByteListToString( FQueryQueue[ i ].Command ));

				Serial.println( "<<QUEUE" );
#endif

				SendQuery( AElement.Command, AElement.ResponseElement );
//				Serial.print( "SEND_QUERY: " ); Serial.println( FQueryQueue.size() );
			}
		}

	public:
		void SendQueryRegisterOKResponse( String AQuery, uint8_t ARetryCount = 0, uint32_t ATimeout = 10000, uint32_t ADelay = 0 )
		{
			SendQueryRegisterResponse( new ESP8266BasicExpectOKFunction, AQuery, true, ARetryCount, ATimeout );
		}

		void SendQueryRegisterCommandOKResponse( String AQuery, uint8_t ARetryCount = 0, uint32_t ATimeout = 10000, uint32_t ADelay = 0 )
		{
			SendQueryRegisterResponse( new ESP8266BasicExpectCommandOKFunction( AQuery ), AQuery, true, ARetryCount, ATimeout );
		}

		void SendQueryRegisterResponseFront( ESP8266SerialBasicFunction *ASender, Mitov::SimpleList<uint8_t, uint16_t> &AQuery, bool ADelete = false, uint8_t ARetryCount = 0, uint32_t ATimeout = 10000, uint32_t ADelay = 0 )
		{
#ifdef __ESP8266__DEBUG__
			Serial.print( "PUSHING: \"" ); Serial.print( Func::ByteListToString( AQuery )); Serial.print( "\" TIMEOUT: " ); Serial.println( ATimeout );
#endif
			TResponseElement *AElement = new TResponseElement( ASender, ADelete, ATimeout, AQuery, ARetryCount, ADelay );
			if( ! FCurrentElement )
				SendQuery( AQuery, AElement );

			else
			{
#ifdef __ESP8266__DEBUG__
				Serial.print( "ADD TO QUERY FRONT : \"" );	Serial.print( Func::ByteListToString( AQuery )); Serial.println( "\"" );
#endif
				FQueryQueue.push_front( TQueryElement( AQuery, AElement ));

#ifdef __ESP8266__DEBUG__
				Serial.println( "QUEUE>>" );
				for( int i = 0; i < FQueryQueue.size(); ++i )
					Serial.println( Func::ByteListToString( FQueryQueue[ i ].Command ));

				Serial.println( "<<QUEUE" );
#endif
			}

//			FResponseHandlersQueue.push_back( AElement );
			
//			SendQuery( AQuery );
//			Serial.print( "PUSHING: " ); Serial.println( AQuery );
//			Serial.println( "PUSHED" );
		}


		void SendQueryRegisterResponse( ESP8266SerialBasicFunction *ASender, Mitov::SimpleList<uint8_t, uint16_t> &AQuery, bool ADelete = false, uint8_t ARetryCount = 0, uint32_t ATimeout = 10000, uint32_t ADelay = 0 )
		{
#ifdef __ESP8266__DEBUG__
			Serial.print( "PUSHING: \"" ); Serial.print( Func::ByteListToString( AQuery )); Serial.print( "\" TIMEOUT: " ); Serial.println( ATimeout );
#endif
			TResponseElement *AElement = new TResponseElement( ASender, ADelete, ATimeout, AQuery, ARetryCount, ADelay );
			TrySendQuery( AQuery, AElement );
//			FResponseHandlersQueue.push_back( AElement );
			
//			SendQuery( AQuery );
//			Serial.print( "PUSHING: " ); Serial.println( AQuery );
//			Serial.println( "PUSHED" );
		}

		void SendQueryRegisterResponse( ESP8266SerialBasicFunction *ASender, String AQuery, bool ADelete = false, uint8_t ARetryCount = 0, uint32_t ATimeout = 10000, uint32_t ADelay = 0 )
		{
			AQuery += "\r\n";
			Mitov::SimpleList<uint8_t, uint16_t> AListQuery;
			AListQuery.append( (uint8_t *)AQuery.c_str(), AQuery.length() );
			SendQueryRegisterResponse( ASender, AListQuery, ADelete, ARetryCount, ATimeout, ADelay );
		}

		void SendPacket( int8_t ASocketID, Mitov::SimpleList<uint8_t, uint16_t> &AData )
		{
			SendQueryRegisterResponse( new ESP8266SendPacketFunction( this, AData ), "AT+CIPSEND=" + String( ASocketID ) + "," + String( AData.size()), false, 0, 10000 );
		}

	public:
		void SendAccessPointIPAddress( String AValue )
		{
#ifdef _MITOV_ESP8266_ACCESS_POINT_IP_PIN_
			AccessPointAddressOutputPin.SendValue( AValue );
#endif
		}

		void SendAccessPointMACAddress( String AValue )
		{
#ifdef _MITOV_ESP8266_ACCESS_POINT_MAC_PIN_
			AccessPointMACOutputPin.SendValue( AValue );
#endif
		}

		void SendStationIPAddress( String AValue )
		{
#ifdef _MITOV_ESP8266_STATION_IP_PIN_
			StationAddressOutputPin.SendValue( AValue );
#endif
		}

		void SendStationMACAddress( String AValue )
		{
#ifdef _MITOV_ESP8266_STATION_MAC_PIN_
			StationMACOutputPin.SendValue( AValue );
#endif
		}

		bool AssignServerID( BasicESP8266Socket *ASocket )
		{
			FServerInstance = ASocket;
/*
			bool AResult = false;
			for( int i = 0; i < 4; ++i )
				if( ! FConnections[ i ] )
				{
					FConnections[ i ] = ASocket;
					FServerInstance = ASocket;
					AResult = true;
				}

			return AResult;
*/
			return true;
		}

		void ReleaseServerID( BasicESP8266Socket *ASocket )
		{
			if( FServerInstance == ASocket )
				FServerInstance = nullptr;

			for( int i = 0; i < 4; ++i )
				if( FConnections[ i ] == ASocket )
				{
					ClearSocketQueue( i );
					FConnections[ i ] = nullptr;
				}
		}

		bool AssignConnectionID( BasicESP8266Socket *ASocket, uint8_t &ASocketID )
		{
			for( uint8_t i = 0; i < 4; ++i )
				if( ! FConnections[ i ] )
				{
					FConnections[ i ] = ASocket;
					ASocketID = i;
					return true;
				}

			return false;
		}

		void ReleaseConnectionID( uint8_t ASocketID )
		{
			FConnections[ ASocketID ] = FServerInstance;
		}

		void ClearSocketQueue( uint8_t ASocketID )
		{
			String ACommand = "AT+CIPSEND=" + String( ASocketID ) + ",";
#ifdef __ESP8266__DEBUG__
			Serial.print( "CLEAR QUEUE: " ); Serial.println( ACommand );
#endif
			for( int i = FQueryQueue.size(); i--; )
				if( memcmp( (uint8_t *)FQueryQueue[ i ].Command, ACommand.c_str(), 13 ) == 0 )
					FQueryQueue.Delete( i );
			
#ifdef __ESP8266__DEBUG__
				Serial.println( "QUEUE>>" );
				for( int i = 0; i < FQueryQueue.size(); ++i )
					Serial.println( Func::ByteListToString( FQueryQueue[ i ].Command ));

				Serial.println( "<<QUEUE" );
#endif
		}

/*
		void SendDirect(String data )
		{
#ifdef __ESP8266__DEBUG__
			Serial.println( data );
#endif
			FStream.print( data );
			WaitSentOK( 2000 );
		}

		void SendDirect( uint8_t *AData, int ASize )
		{
#ifdef __ESP8266__DEBUG__
			Serial.write( AData, ASize );
#endif
			FStream.write( AData, ASize );
			WaitSentOK( 2000 );
		}

		void WaitSentOK( const int timeout )
		{
			String response = "";
			long int time = millis();
			 
			while( timeout > millis() - time)
			{
			  while(FStream.available())
			  {     
				// The esp has data so collecti it 
				char c = FStream.read(); // read the next character.
				response += c;
			  }  

			  if( response.indexOf( "SEND OK\r\n" ) >= 0 )
				  break;

			}
		}

		String SendData(String command, String response_key, const int timeout)
		{
			String response = "";
    
#ifdef __ESP8266__DEBUG__
			Serial.println( command );
#endif
			FStream.print( command + "\r\n" ); // send the read character to the esp8266
    
			long int time = millis();
    
			while( timeout > millis() - time)
			{
			  while(FStream.available())
			  {     
				// The esp has data so collecti it 
				char c = FStream.read(); // read the next character.
				response += c;
			  }  

			  if( response.indexOf( response_key ) >= 0 )
				  break;

			}
    
#ifdef __ESP8266__DEBUG__
			Serial.println( response );
#endif
			return response;
		}

		bool SendData(String command, const int timeout, String &AResponse )
		{
			AResponse = "";
    
#ifdef __ESP8266__DEBUG__
			Serial.println( command );
#endif

			FStream.print( command + "\r\n" ); // send the read character to the esp8266
    
			long int time = millis();
    
			while( timeout > millis() - time)
			{
				while(FStream.available())
				{     
					// The esp has data so collecti it 
					char c = FStream.read(); // read the next character.
					AResponse += c;
				}  

				if( AResponse.indexOf( "\r\nOK\r\n" ) >= 0 )
				{
#ifdef __ESP8266__DEBUG__
					Serial.println( AResponse );
#endif
					return true;
				}

				if( AResponse.indexOf( "\r\nERROR\r\n" ) >= 0 )
				{
#ifdef __ESP8266__DEBUG__
					Serial.println( AResponse );
#endif
					return false;
				}

				if( AResponse.indexOf( "\r\nFAIL\r\n" ) >= 0 )
				{
#ifdef __ESP8266__DEBUG__
					Serial.println( AResponse );
#endif
					return false;
				}

			}
    
#ifdef __ESP8266__DEBUG__
			Serial.println( AResponse );
#endif
			return false;
		}

		bool SendData( String command, const int timeout )
		{
			String AResponse;
			return SendData( command, timeout, AResponse );
		}
*/
		void ReadSerial()
		{
//			if( FInPowerSwitch )
//				return;

			int AChar = FStream.read();
//			Serial.print( AChar );
			if( AChar < 0 )
				return;


//			Serial.print( (char)AChar );

			if( FInSocketReceiving )
			{
#ifdef __ESP8266__DEBUG__
				Serial.print( "FInSocketReceiving: " ); Serial.println( FReadConnectionId );
				Serial.print( "FReadLength: " ); Serial.println( FReadLength );
#endif
				if( FConnections[ FReadConnectionId ] )
					FConnections[ FReadConnectionId ]->DataReceived( FReadConnectionId, AChar );

				--FReadLength;
				if( ! FReadLength )
					FInSocketReceiving = false;

				return;
			}

			if( FInSocketProcessing )
			{
				if( AChar != ':' )
				{
					FBuffer[ FIndex ++ ] = AChar;
					if( FIndex < 255 )
						return;
				}

				FBuffer[ FIndex ] = '\0';
				FIndex = 0;
				String AString = FBuffer;

//				Serial.println( AString );
				String AValue;
				if( Func::ExtractOptionallyQuotedCommaText( AString, AValue ))
				{
//					Serial.println( AValue );
					FReadConnectionId = AValue.toInt();
					if( Func::ExtractOptionallyQuotedCommaText( AString, AValue ))
					{
						FReadLength = AValue.toInt();
//						Serial.println( FReadLength );
						FInSocketReceiving = true;
						FInSocketProcessing = false;
					}
				}

			return;
			}

/*
		if( FLockInputIndex )
		{
			bool ASuccess = false;
			if( FFunctions[ FLockInputIndex - 1 ]->ProcessCharacter( AChar, ASuccess ))
			{
				if( ASuccess )
					FLockInputIndex = 0;

				return;
			}
		}
*/
//			Serial.print( (char)AChar );
//			return;
//			if( AChar < ' ' )
//				Serial.println( AChar );

//		if( FIndex == 0 && AChar == ">" )
//		{
//		}

		if( AChar == 13 )
			return;

		if( AChar != 10 )
		{
			FBuffer[ FIndex ++ ] = AChar;
			if( ! ( FIndex == 1 && AChar == '>' ))
			{
				if( FIndex == 5 )
				{
					if( memcmp( FBuffer, "+IPD,", 5 ) == 0 )
					{
#ifdef __ESP8266__DEBUG__
						Serial.println( "+IPD, detected" );
#endif
						FInSocketProcessing = true;
						FIndex = 0;
						return;
					}
				}

				if( FIndex < 255 )
					return;
			}
		}

		FBuffer[ FIndex ] = '\0';
		FIndex = 0;

		String AString = FBuffer;

#ifdef __ESP8266__DEBUG__
		Serial.print( "LINE: " ); Serial.println( AString );

//			Serial.print( "QUEUE: " ); Serial.println( FResponseHandlersQueue.size() );
#endif
/*
		if( AString.startsWith("+IPD,"))
		{
			AString.remove( 0, 5 );
			Serial.println( AString );
			String AValue;
			if( Func::ExtractOptionallyQuotedCommaText( AString, AValue ))
			{
				Serial.println( AValue );
				int AReadConnectionId = AValue.toInt();
				if( Func::ExtractOptionallyQuotedCommaText( AString, AValue ))
				{
					int ALengthStr = AValue.toInt();
					Serial.println( ALengthStr );
				}
			}

			return;
		}
*/
//			bool	AResponseCompleted = false;

//			Serial.print( "FLockRequestedInputIndex : " ); Serial.println( FLockRequestedInputIndex );
//			Serial.print( "FLockInputIndex : " ); Serial.println( FLockInputIndex );
/*
		if( FLockRequestedInputIndex )
		{
			ALockInput = true;
			bool ASuccess = false;
			if( FResponseHandlersQueue[ FLockRequestedInputIndex - 1 ]->Handler->TryProcessRequestedInput( AString, ALockInput, AResponseCompleted, ASuccess ))
			{
				if( AResponseCompleted )
				{
					if( FCurrentElement == FResponseHandlersQueue[ FLockRequestedInputIndex - 1 ] )
						FCurrentElement = nullptr;

					if( ( ! ASuccess ) && FResponseHandlersQueue[ FLockRequestedInputIndex - 1 ]->Command && FResponseHandlersQueue[ FLockRequestedInputIndex - 1 ]->RetryCount )
					{
#ifdef __ESP8266__DEBUG__
						Serial.println( "RETRY1" );
						Serial.println( FResponseHandlersQueue[ FLockRequestedInputIndex - 1 ]->Command );
#endif
						-- FResponseHandlersQueue[ FLockRequestedInputIndex - 1 ]->RetryCount;
						SendQuery( FResponseHandlersQueue[ FLockRequestedInputIndex - 1 ]->Command, FResponseHandlersQueue[ FLockRequestedInputIndex - 1 ] );
					}

					else
					{
#ifdef __ESP8266__DEBUG__
						Serial.println( "Queue Delete 1" );
//							Serial.println( ALockInput );
//							Serial.print( "RESP_QUEUE: " ); Serial.println( FResponseHandlersQueue.size() );
#endif
						delete FResponseHandlersQueue[ FLockRequestedInputIndex - 1 ];
						FResponseHandlersQueue.Delete( FLockRequestedInputIndex - 1 );
//							Serial.println( "ProcessNextCommand 1" );
						ProcessNextCommand();
					}
				}

				if( ! ALockInput )
					FLockRequestedInputIndex = 0;
			}

			return;
		}
*/
		if( AString.length() )
		{
			if( AString[ 0 ] >= '0' && AString[ 0 ] <= '3' )
			{
				int AIndex = AString[ 0 ] - '0';
				if( AString.endsWith( ",CLOSED" ))
				{
					if( FConnections[ AIndex ] )
						FConnections[ AIndex ]->Connected( false );

					FConnections[ AIndex ] = nullptr;
					ClearSocketQueue( AIndex );
//						FConnections[ AIndex ]->StopSocket();

					return;
				}

				else if( AString.endsWith( ",CONNECT" ))
				{
					if( FConnections[ AIndex ] )
						FConnections[ AIndex ]->Connected( true );

					else
					{
						FConnections[ AIndex ] = FServerInstance;
//						FServerInstance->FClientCurrentID = AIndex;
						FServerInstance->Connected( true );
					}

					return;
				}
			}
		}

		bool	AResponseCompleted = false;
		if( FCurrentElement )
		{
			bool ASuccess = false;
			if( FCurrentElement->Handler->TryProcessRequestedInput( AString, AResponseCompleted, ASuccess ))
			{
				if( AResponseCompleted )
				{

					if( ( ! ASuccess ) && FCurrentElement->Command && FCurrentElement->RetryCount )
					{
#ifdef __ESP8266__DEBUG__
						Serial.println( "RETRY2" );
						Serial.println( Func::ByteListToString( FCurrentElement->Command ));
#endif
						-- FCurrentElement->RetryCount;
						SendQuery( FCurrentElement->Command, FCurrentElement );
					}

					else
					{
#ifdef __ESP8266__DEBUG__
						Serial.println( "Queue Delete 2" );
						Serial.print( "DELETING: " ); Serial.println( Func::ByteListToString( FCurrentElement->Command ));
//							Serial.print( "RESP_QUEUE: " ); Serial.println( FResponseHandlersQueue.size() );
#endif
//							Serial.println( "ProcessNextCommand 2" );
						ProcessNextCommand();
					}
				}

				return;
			}
		}

/*
			if( FLockInputIndex )
			{
//				Serial.println( "FLockInputIndex" );
				ALockInput = true;
				FFunctions[ FLockInputIndex - 1 ]->TryProcessInput( AString, ALockInput );
				if( ! ALockInput )
					FLockInputIndex = 0;

				return;
			}

//			Serial.println( "*****" );
			ALockInput = false;
			for( int i = 0; i < FFunctions.size(); ++i )
				if( FFunctions[ i ]->TryProcessInput( AString, ALockInput ))
				{
					if( ALockInput )
					{
						FLockInputIndex = i + 1;
#ifdef __ESP8266__DEBUG__
						Serial.print( "FLockInputIndex = " ); Serial.println( FLockInputIndex );
#endif
					}

					return;
				}
*/
		}

	protected:
		void Reset()
		{
			if( FCurrentElement )
				if( FCurrentElement->Handler == this )
					return;

			FEmptyLineDetected = false;
//			FRestarted = false;
			SendQueryRegisterResponse( this, "AT+RST", false, 5, 10000 );
		}

		void DoReset( void * )
		{
			Reset();
		}

	public:
		void TryConnectAccesPointIndex( uint8_t AIndex )
		{
			if( AIndex >= RemoteAccessPoints.size() )
				return;

			SendQueryRegisterResponse( new ESP8266ConnectAccessPointFunction( this, AIndex + 1 ), "AT+CWJAP=\"" + RemoteAccessPoints[ AIndex ]->SSID + "\",\"" + RemoteAccessPoints[ AIndex ]->Password + "\"", true, 2, 20000, 1000 );
		}

	protected:
		virtual void SystemStart()
		{
//			inherited::SystemStart();
#ifdef __ESP8266__DEBUG__
			Serial.println( "SystemStart" );
#endif
//			SendData( "AT+RST", "\r\nready\r\n", 5000 );
			Reset();
//			String AResponse = SendData( "AT+RST", "\r\nready\r\n", 5000 );
//			Serial.println( AResponse );

#ifdef __ESP8266__DEBUG__
			Serial.println( "INIT" );
#endif

//			int AMode = 0;
			int AMode = 1; // Allow scanning
/*
			for( int i = 0; i < RemoteAccessPoints.size(); ++i )
				if( RemoteAccessPoints[ i ]->Enabled )
				{
					AMode = 1;
					break;
				}
*/
			if( AMode == 0 || AccessPoint.Enabled )
				AMode |= 2; // Set it as access point so others can connect

//			AMode = 3;
//			SendData( "AT+CWMODE=" + String( AMode ), 5000 );
			SendQueryRegisterOKResponse( "AT+CWMODE=" + String( AMode ), 5, 10000 );

//			Serial.println( "TTT" );
//			return;

			if( AccessPoint.Enabled )
//				if( AccessPoint.Encription )
			{
				String ASSID;
				if( AccessPoint.SSID != "" )
					ASSID = AccessPoint.SSID;

				else
					ASSID = "Arduino";

				String AEncoding;
				if( AccessPoint.Encription.WPA )
				{
					if( AccessPoint.Encription.WPA2 )
						AEncoding = "4";

					else
						AEncoding = "2";
				}

				else
				{
					if( AccessPoint.Encription.WPA2 )
						AEncoding = "3";

					else
						AEncoding = "0";
				}

//				SendData( "AT+CWSAP=\"" + ASSID + "\",\"" + AccessPoint.Password + "\"," + AccessPoint.Channel + "," + AEncoding, 1000 );
				SendQueryRegisterOKResponse( "AT+CWSAP=\"" + ASSID + "\",\"" + AccessPoint.Password + "\"," + AccessPoint.Channel + "," + AEncoding, 5, 1000 );
			}

			TryConnectAccesPointIndex( 0 );

/*
			for( int i = 0; i < RemoteAccessPoints.size(); ++i )
				if( RemoteAccessPoints[ i ]->Enabled )
					if(RemoteAccessPoints[ i ]->SSID != "" )
						if( SendData( "AT+CWJAP=\"" + RemoteAccessPoints[ i ]->SSID + "\",\"" + RemoteAccessPoints[ i ]->Password + "\"", 20000 ))
							break;	
*/

#if ( defined( _MITOV_ESP8266_ACCESS_POINT_IP_PIN_ ) || defined( _MITOV_ESP8266_ACCESS_POINT_MAC_PIN_ ) || defined( _MITOV_ESP8266_STATION_IP_PIN_ ) || defined( _MITOV_ESP8266_STATION_MAC_PIN_ ) )
			bool AProcess = false;

#ifdef _MITOV_ESP8266_ACCESS_POINT_IP_PIN_
			if( AccessPointAddressOutputPin.IsConnected() )
				AProcess = true;
#endif

#ifdef _MITOV_ESP8266_ACCESS_POINT_MAC_PIN_
			if( AccessPointMACOutputPin.IsConnected() )
				AProcess = true;
#endif

#ifdef _MITOV_ESP8266_STATION_IP_PIN_
			if( StationAddressOutputPin.IsConnected() )
				AProcess = true;
#endif

#ifdef _MITOV_ESP8266_STATION_MAC_PIN_
			if( StationMACOutputPin.IsConnected() )
				AProcess = true;
#endif
			if( AProcess )
			{
//				String AResponse = SendData( "AT+CIFSR", "\r\nOK\r\n",1000 );
				SendQueryRegisterResponse( new ESP8266AccessPointAddressFunction( this ), "AT+CIFSR", true, 5, 10000 );
			}
#endif

/*
			if( AccessPointAddressOutputPin.IsConnected() || AccessPointMACOutputPin.IsConnected() || StationAddressOutputPin.IsConnected() || StationMACOutputPin.IsConnected() )
			{
//				Serial.println( "ADDRESS:" );
				String AResponse = SendData( "AT+CIFSR", "\r\nOK\r\n",1000 );
#ifdef __ESP8266__DEBUG__
				Serial.println( AResponse );
#endif

				int AAddressPos = AResponse.indexOf( "+CIFSR:APIP,\"" );

				int AAddressEndPos;
				if( AAddressPos >= 0 )
				{
					AAddressEndPos = AResponse.indexOf( "\"", AAddressPos + 13 );
					if( AAddressEndPos >= 0 )
					{
						String Addresses = AResponse.substring( AAddressPos + 13, AAddressEndPos );
						AccessPointAddressOutputPin.Notify( (void *)Addresses.c_str() );
					}

					else
						AAddressEndPos = 0;

				}

				else
					AAddressEndPos = 0;

				AAddressPos = AResponse.indexOf( "+CIFSR:APMAC,\"", AAddressEndPos );
				if( AAddressPos >= 0 )
				{
					AAddressEndPos = AResponse.indexOf( "\"", AAddressPos + 14 );
					if( AAddressEndPos >= 0 )
					{
						String Addresses = AResponse.substring( AAddressPos + 14, AAddressEndPos );
						AccessPointMACOutputPin.Notify( (void *)Addresses.c_str() );
					}

					else
						AAddressEndPos = 0;

				}				
				else
					AAddressEndPos = 0;

				AAddressPos = AResponse.indexOf( "+CIFSR:STAIP,\"", AAddressEndPos );
				if( AAddressPos >= 0 )
				{
					AAddressEndPos = AResponse.indexOf( "\"", AAddressPos + 14 );
					if( AAddressEndPos >= 0 )
					{
						String Addresses = AResponse.substring( AAddressPos + 14, AAddressEndPos );
						StationAddressOutputPin.Notify( (void *)Addresses.c_str() );
					}

					else
						AAddressEndPos = 0;

				}				
				else
					AAddressEndPos = 0;

				AAddressPos = AResponse.indexOf( "+CIFSR:STAMAC,\"", AAddressEndPos );
				if( AAddressPos >= 0 )
				{
					AAddressEndPos = AResponse.indexOf( "\"", AAddressPos + 15 );
					if( AAddressEndPos >= 0 )
					{
						String Addresses = AResponse.substring( AAddressPos + 15, AAddressEndPos );
						StationMACOutputPin.Notify( (void *)Addresses.c_str() );
					}

					else
						AAddressEndPos = 0;

				}				
				else
					AAddressEndPos = 0;

			}
*/
//			SendData( "AT+CIPMUX=1",1000 );
//			SendQueryRegisterOKResponse( "AT+CIPDOMAIN=\"esptst\"", 5, 1000 );

			SendQueryRegisterOKResponse( "AT+CIPMUX=1", 5, 1000 );

			for( int i = 0; i < Sockets.size(); ++i )
				Sockets[ i ]->TryStartSocket();

		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) 
		{
/*
			if( FStream.available() )
			{
#ifdef __ESP8266__DEBUG__
//				Serial.println( "AVALIABLE" );
#endif
				if( ! FReadLength )
					if(FStream.find("+IPD,"))
					{
#ifdef __ESP8266__DEBUG__
						Serial.println( "DATA" );
#endif
//					int connectionId = FStream.read() - '0';
						String AIDStr = FStream.readStringUntil( ',' );
						FReadConnectionId = AIDStr.toInt();
#ifdef __ESP8266__DEBUG__
						Serial.println( AIDStr );
						Serial.println( FReadConnectionId );
#endif
//					FStream.find("," ); // Skip ','
//					FStream.read(); // Skip ','
						String ALengthStr = FStream.readStringUntil( ':' );
#ifdef __ESP8266__DEBUG__
						Serial.println( "Length = " + ALengthStr );
#endif
						FReadLength = ALengthStr.toInt();
					}

				if( FReadLength )
					if( FReadConnectionId >= 0 && FReadConnectionId < 4 )
					{
						while( FReadLength )
						{
							if( ! FStream.available() )
								break;

							int AData = FStream.read();
//#ifdef __ESP8266__DEBUG__
//							Serial.println( (char)AData );
//#endif
							if( FConnections[ FReadConnectionId ] )
								FConnections[ FReadConnectionId ]->DataReceived( FReadConnectionId, AData );

							--FReadLength;
						}
					}

			}
*/
			ReadSerial();
			if( FCurrentElement )
			{
				unsigned long AMillis = millis();
				if( ( AMillis - FCurrentMillis ) > FCurrentElement->Timeout )
				{
#ifdef __ESP8266__DEBUG__
					Serial.print( "TIMEOUT: " ); Serial.println( Func::ByteListToString( FCurrentElement->Command ));
					Serial.println( FCurrentElement->Timeout );
					Serial.println( AMillis );
					Serial.println( FCurrentMillis );
#endif
//					FLockRequestedInputIndex = 0;

					if( FCurrentElement->Command && FCurrentElement->RetryCount )
					{
#ifdef __ESP8266__DEBUG__
						Serial.println( "RETRY3" );
						Serial.println( Func::ByteListToString( FCurrentElement->Command ));
#endif
						-- FCurrentElement->RetryCount;
						SendQuery( FCurrentElement->Command, FCurrentElement );
					}

					else
					{			
						FCurrentElement->Handler->TimedOut();
	//					Serial.println( "ProcessNextCommand 3" );
						ProcessNextCommand();
					}

				}
			}

			if( FDelay )
			{
				unsigned long AMillis = millis();
				if( ( AMillis - FCurrentMillis ) > FDelay )
				{
#ifdef __ESP8266__DEBUG__
					Serial.println( "DELAY COMPLETED" );
#endif
					FDelay = 0;
					ProcessNextCommand();
				}
			}

//			inherited::SystemLoopBegin( currentMicros );
		}

	public:
		ESP8266( Mitov::BasicSerialPort &ASerial ) :
			FStream( ASerial.GetStream() ),
			FEmptyLineDetected( false ),
			FInSocketReceiving( false ),
			FInSocketProcessing( false ),
			FReadConnectionId( 0 )
		{
#ifdef _MITOV_ESP8266_RESET_PIN_
			ResetInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ESP8266::DoReset );
#endif
			memset( FConnections, 0, sizeof( FConnections ) );
		}

	};
//---------------------------------------------------------------------------
	class ESP8266ScanNetworksOperation : public OpenWire::Component, public ESP8266SerialBasicFunction
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	ScanInputPin;

#ifdef _MITOV_ESP8266_SCAN_NETWORK_SCANNING_PIN_
		OpenWire::SourcePin	ScanningOutputPin;
#endif

#ifdef _MITOV_ESP8266_SCAN_NETWORK_COUNT_PIN_
		OpenWire::SourcePin	CountOutputPin;
#endif

#ifdef _MITOV_ESP8266_SCAN_NETWORK_FAILED_PIN_
		OpenWire::SourcePin	FailedOutputPin;
#endif

#ifdef _MITOV_ESP8266_SCAN_NETWORK_ENCRIPTION_PIN_
		OpenWire::SourcePin	EncryptionOutputPin;
#endif

#ifdef _MITOV_ESP8266_SCAN_NETWORK_SSID_PIN_
		OpenWire::SourcePin	SSIDOutputPin;
#endif

#ifdef _MITOV_ESP8266_SCAN_NETWORK_SIGNAL_PIN_
		OpenWire::SourcePin	SignalStrengthOutputPin;
#endif

#ifdef _MITOV_ESP8266_SCAN_NETWORK_MAC_PIN_
		OpenWire::SourcePin	MACAddressOutputPin;
#endif

#ifdef _MITOV_ESP8266_SCAN_NETWORK_CHANNEL_PIN_
		OpenWire::SourcePin	ChannelOutputPin;
#endif

#ifdef _MITOV_ESP8266_SCAN_NETWORK_FOUND_PIN_
		OpenWire::SourcePin	FoundNetworkOutputPin;
#endif

	protected:
		ESP8266	&FOwner;

	protected:
		bool	FEmptyLineDetected : 1; // = false;
		bool	FScanRequest : 1; // = false;
		uint8_t	FCount : 6;


	protected:
		void Reset()
		{
			FCount = 0;
			FEmptyLineDetected = false;
			FScanRequest = false;
#ifdef _MITOV_ESP8266_SCAN_NETWORK_SCANNING_PIN_
			ScanningOutputPin.SendValue( false );
#endif
		}

	public:
		virtual void TimedOut() override
		{
#ifdef _MITOV_ESP8266_SCAN_NETWORK_FAILED_PIN_
			FailedOutputPin.Notify( nullptr );
#endif

#ifdef _MITOV_ESP8266_SCAN_NETWORK_COUNT_PIN_
			CountOutputPin.SendValue<uint32_t>( FCount );
#endif
			Reset();
		}

		virtual bool TryProcessRequestedInput( String ALine, bool &AResponseCompleted, bool &ASuccess ) override
		{ 
			if( FEmptyLineDetected )
			{
				ALine.trim();
				if( ALine != "" )
				{
					ASuccess = ( ALine == "OK" );
					if( ! ASuccess )
					{
#ifdef _MITOV_ESP8266_SCAN_NETWORK_FAILED_PIN_
						FailedOutputPin.Notify( nullptr );
#endif

					}

#ifdef _MITOV_ESP8266_SCAN_NETWORK_COUNT_PIN_
					CountOutputPin.SendValue<uint32_t>( FCount );
#endif
					AResponseCompleted = true;
					Reset();
					return true;
				}
			}

			else if( ALine == "" )
			{
//				Serial.println( "FEmptyLineDetected" );
				FEmptyLineDetected = true;
				return true;
			}

			else
			{
				String AShortLine = ALine;
				AShortLine.trim();
	//			Serial.print( "SHORT_LINE: " ); Serial.println( AShortLine );
				if( AShortLine.startsWith( "+CWLAP:" ) )
				{
					int APos = 7;
					for( ; APos < AShortLine.length(); ++APos )
						if( AShortLine[ APos ] == '(' )
							break;

					AShortLine.remove( 0, APos + 1 );
//					Serial.println( AShortLine );
					String AValue;
					if( ! Func::ExtractOptionallyQuotedCommaText( AShortLine, AValue ))
						return true;

#ifdef _MITOV_ESP8266_SCAN_NETWORK_ENCRIPTION_PIN_
					EncryptionOutputPin.SendValue<uint32_t>( AValue.toInt() );
#endif

					if( ! Func::ExtractOptionallyQuotedCommaText( AShortLine, AValue ))
						return true;

#ifdef _MITOV_ESP8266_SCAN_NETWORK_SSID_PIN_
					SSIDOutputPin.SendValue( AValue );
#endif

					if( ! Func::ExtractOptionallyQuotedCommaText( AShortLine, AValue ))
						return true;

#ifdef _MITOV_ESP8266_SCAN_NETWORK_SIGNAL_PIN_
					SignalStrengthOutputPin.SendValue<int32_t>( AValue.toInt() );
#endif

					if( ! Func::ExtractOptionallyQuotedCommaText( AShortLine, AValue ))
						return true;

#ifdef _MITOV_ESP8266_SCAN_NETWORK_MAC_PIN_
					MACAddressOutputPin.SendValue( AValue );
#endif

					if( ! Func::ExtractOptionallyQuotedCommaText( AShortLine, AValue ))
						return true;

#ifdef _MITOV_ESP8266_SCAN_NETWORK_CHANNEL_PIN_
					ChannelOutputPin.SendValue<int32_t>( AValue.toInt() );
#endif
					
//					Serial.println( "CH:" );
//					Serial.println( AValue );

#ifdef _MITOV_ESP8266_SCAN_NETWORK_FOUND_PIN_
					FoundNetworkOutputPin.Notify( nullptr );
#endif
					++ FCount;
					return true;
				}
			}

			return false;
		}

	protected:
		void DoScanNetworks( void *_Data )
		{
			if( FScanRequest )
				return;

			FScanRequest = true;
			FOwner.SendQueryRegisterResponse( this, "AT+CWLAP" );
#ifdef _MITOV_ESP8266_SCAN_NETWORK_SCANNING_PIN_
			ScanningOutputPin.SendValue( true );
#endif
/*
			if( WiFi.scanComplete() != WIFI_SCAN_RUNNING )
			{
				WiFi.scanNetworks( true );
				FScanRequest = true;
			}
*/
		}

	protected:
#ifdef _MITOV_ESP8266_SCAN_NETWORK_SCANNING_PIN_
		virtual void SystemInit() override
		{
//			inherited::SystemInit();
			ScanningOutputPin.SendValue( false );
		}
#endif

	public:
		ESP8266ScanNetworksOperation( ESP8266 &AOwner ) :
			FOwner( AOwner ),
			FEmptyLineDetected( false ),
			FScanRequest( false ),
			FCount( 0 )
		{			
			ScanInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ESP8266ScanNetworksOperation::DoScanNetworks );
		}
	};
//---------------------------------------------------------------------------
	class ESP8266TCPServerSocket : public BasicESP8266Socket
	{
		typedef BasicESP8266Socket inherited;

//		EthernetServer	*FServer;
//		EthernetClient	FClient;

	public:
		int8_t	FClientCurrentID = -1;

	protected:
		virtual void StartSocket() override
		{
			if( FConnecting )
				return;

#ifdef __ESP8266__DEBUG__
			Serial.println( "SERVER::StartSocket" );
#endif
//			if( FModule.AssignConnectionID( this ) )
			if( FModule.AssignServerID( this ) )
			{
//				FModule.SendData( "AT+CIPSERVER=1," + String( Port ), 2000 );
				String ACommand = "AT+CIPSERVER=1," + String( Port );
				FModule.SendQueryRegisterResponse( new ESP8266BasicExpectOKCallbackFunction<ESP8266TCPServerSocket>( this, ACommand, &ESP8266TCPServerSocket::DoConnectCompleted ), ACommand, false, 5, 2000 );
				FRunning = true;
			}

		}

		virtual void DataReceived( int8_t connectionId, uint8_t AData ) override
		{
#ifdef __ESP8266__DEBUG__
			Serial.print( "RECEIVED: " );
			Serial.println( connectionId );
#endif
			FClientCurrentID = connectionId;
			OutputPin.SendValue( Mitov::TDataBlock( 1, &AData ));
//			FClientCurrentID = -1;
		}

	public:
		virtual void EndPacket() override
		{
//			Serial.println( "EndPacket" );
//			Serial.println( FClientCurrentID );
//			Serial.println( FRunning );
			if( FRunning )
				if( FClientCurrentID >= 0 )
					FModule.SendPacket( FClientCurrentID, FStringPrint.Value );

/*
			if( FStringPrint.Value.size() == 0 )
				return;

			if( FClientCurrentID >= 0 )
			{
				if( FModule.SendData( "AT+CIPSEND=" + String( FClientCurrentID ) + "," + String( FStringPrint.Value.size()), "\r\n> ", 2000 ))
					FModule.SendDirect( (uint8_t *)FStringPrint.Value, FStringPrint.Value.size() );

//				Serial.println( FStringPrint.Value );
			}

			else
			{
/ *
				for( int i = 0; i < 4; ++ i )
					if( FModule.FConnections[ i ] == this )
					{
						if( FModule.SendData( "AT+CIPSEND=" + String( i ) + "," + String( FStringPrint.Value.size()), "\r\n> ", 2000 ))
							FModule.SendDirect( (uint8_t *)FStringPrint.Value, FStringPrint.Value.size() );

					}
* /
			}
*/
//			FStringPrint.Value.clear();
		}

	public:
		virtual void Connected( bool AValue ) override
		{
			if( ! AValue )
				FClientCurrentID = -1;

			inherited::Connected( AValue );
		}

		virtual void StopSocket() override
		{
			if( FRunning )
			{
//				FModule.SendData( "AT+CIPSERVER=0," + String( Port ), 2000 );
				FModule.SendQueryRegisterOKResponse( "AT+CIPSERVER=0," + String( Port ), 5, 2000 );
				FModule.ReleaseServerID( this );
				Connected( false );
				FRunning = false;
			}
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class ESP8266TCPClientSocket : public BasicESP8266Socket
	{
		typedef BasicESP8266Socket inherited;

	public:
		String		Host;
		uint8_t		FID;

	protected:
		virtual void StartSocket() override
		{
			if( FConnecting )
				return;

#ifdef __ESP8266__DEBUG__
				Serial.println( "CLIENT::StartSocket" );
#endif
			if( FModule.AssignConnectionID( this, FID ) )
			{
				FConnecting = true;
//				FModule.SendData( "AT+CIPSTART=" + String( FID ) +",\"TCP\",\"" + Host + "\"," + String( Port ), 2000 );
				String ACommand = "AT+CIPSTART=" + String( FID ) +",\"TCP\",\"" + Host + "\"," + String( Port );
				FModule.SendQueryRegisterResponse( new ESP8266BasicExpectOKCallbackFunction<ESP8266TCPClientSocket>( this, ACommand, &ESP8266TCPClientSocket::DoConnectCompleted ), ACommand, false, 5, 2000 );
//				FModule.SendQueryRegisterOKResponse( "AT+CIPSTART=" + String( FID ) +",\"TCP\",\"" + Host + "\"," + String( Port ), 5, 2000 );
//				FRunning = true;
			}

//			Serial.println( "StartSocket" );
		}

		virtual void StopSocket() override
		{
			if( FRunning )
			{
				FModule.ReleaseConnectionID( FID );
//				FModule.SendData( "AT+CIPCLOSE=" + String( FID ), 2000 );
				FModule.SendQueryRegisterOKResponse( "AT+CIPCLOSE=" + String( FID ), 5, 2000 );
				Connected( false );
				FRunning = false;
			}
//			FClient.stop();

		}

		virtual void DataReceived( int8_t connectionId, uint8_t AData ) override
		{
//			OutputPin.Notify( &AData );
			OutputPin.SendValue( Mitov::TDataBlock( 1, &AData ));
		}

	public:
		virtual void EndPacket() override
		{
			if( FRunning )
				FModule.SendPacket( FID, FStringPrint.Value );
/*
			if( FStringPrint.Value.size() == 0 )
				return;

			if( FModule.SendData( "AT+CIPSEND=" + String( FID ) + "," + String( FStringPrint.Value.size()), "\r\n> ", 2000 ))
				FModule.SendDirect( (uint8_t *)FStringPrint.Value, FStringPrint.Value.size() );
*/
//			FStringPrint.Value.clear();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
	BasicESP8266Socket::BasicESP8266Socket( ESP8266 &AModule ) :
		FModule( AModule ),
		FRunning( false ),
		FConnecting( false )
	{
		AModule.Sockets.push_back( this );
#ifdef _MITOV_ESP8266_SOCKET_DISCONNECT_PIN_
		DisconnectInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&BasicESP8266Socket::DoReceiveDisconnect );
#endif
	}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
	void ESP8266ConnectAccessPointFunction::TimedOut() 
	{
//		FEmptyLineDetected = false;
		FOwner->TryConnectAccesPointIndex( FIndex );
	}
//---------------------------------------------------------------------------
	bool ESP8266ConnectAccessPointFunction::TryProcessRequestedInput( String ALine, bool &AResponseCompleted, bool &ASuccess )
	{ 
		// WIFI GOT IP

		ALine.trim();
		if( FEmptyLineDetected )
		{
			ALine.trim();
			if( ALine != "" )
			{
				ASuccess = ( ALine == "OK" );
				if( ! ASuccess )
					FOwner->TryConnectAccesPointIndex( FIndex );

				AResponseCompleted = true;
				return true;
			}
		}

		else if( ALine == "WIFI GOT IP" )
		{
			FEmptyLineDetected = true;
			return true;
		}

		return false;
	}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
	bool ESP8266AccessPointAddressFunction::TryProcessRequestedInput( String ALine, bool &AResponseCompleted, bool &ASuccess )
	{ 
		if( FDataReceived && ( ALine == "OK" ) )
		{
			ASuccess = true;
//			ALine.trim();
//			if( ALine != "" )
//			{
//				ASuccess = FDataReceived && ( ALine == "OK" );
				AResponseCompleted = true;
				return true;
//			}
		}

/*
		else if( ALine == "" )
		{
//				Serial.println( "FEmptyLineDetected" );
			FEmptyLineDetected = true;
			return true;
		}
*/
		else
		{
			String AShortLine = ALine;
			AShortLine.trim();
//			Serial.print( "SHORT_LINE: " ); Serial.println( AShortLine );
			if( AShortLine.startsWith( "+CIFSR:APIP," ) )
			{
				AShortLine.remove( 0, 12 );

				String AValue;
				if( Func::ExtractOptionallyQuotedCommaText( AShortLine, AValue ))
				{
					FDataReceived = true;
					FOwner->SendAccessPointIPAddress( AValue );
				}
			}

			else if( AShortLine.startsWith( "+CIFSR:APMAC," ) )
			{
				AShortLine.remove( 0, 13 );

				String AValue;
				if( Func::ExtractOptionallyQuotedCommaText( AShortLine, AValue ))
				{
					FDataReceived = true;
					FOwner->SendAccessPointMACAddress( AValue );
				}
			}

			else if( AShortLine.startsWith( "+CIFSR:STAIP," ) )
			{
				AShortLine.remove( 0, 13 );

				String AValue;
				if( Func::ExtractOptionallyQuotedCommaText( AShortLine, AValue ))
				{
					FDataReceived = true;
					FOwner->SendStationIPAddress( AValue );
				}
			}

			else if( AShortLine.startsWith( "+CIFSR:STAMAC," ) )
			{
				AShortLine.remove( 0, 13 );

				String AValue;
				if( Func::ExtractOptionallyQuotedCommaText( AShortLine, AValue ))
				{
					FDataReceived = true;
					FOwner->SendStationMACAddress( AValue );
				}
			}
		}
	}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
	bool ESP8266SendPacketFunction::TryProcessRequestedInput( String ALine, bool &AResponseCompleted, bool &ASuccess )
	{
//		if( FEmptyLineDetected )
//		{
			ALine.trim();
//			Serial.println( "TEST" );
//			Serial.println( ALine );
			if( ALine == ">" )
			{
//				Serial.println( "ADD DATA!!!" );
				ASuccess = true;
#ifdef __ESP8266__DEBUG__
				Serial.print( "ADD DATA : \"" );	Serial.print( Func::ByteListToString( FData )); Serial.println( "\"" );
#endif
				FOwner->SendQueryRegisterResponseFront( new ESP8266BasicExpectSendOKFunction, FData, true, 1, 10000 );

//					FModule.SendDirect( (uint8_t *)FStringPrint.Value, FStringPrint.Value.size() );
//					FOwner->SendQueryRegisterOKResponse(  , false, 0, 2000 );
//						Serial.println( "OK Processed" );
//					Serial.println( "ALockInput = false" );
//					OKReceived();

				AResponseCompleted = true;
//				FEmptyLineDetected = false;
				return true;
			}
//		}

//		else if( ALine == "" )
//		{
//				Serial.println( "FEmptyLineDetected" );
//			FEmptyLineDetected = true;
//			return true;
//		}

		return false;
	}
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
