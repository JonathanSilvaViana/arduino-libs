////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_SMTP_CLIENT_h
#define _MITOV_SMTP_CLIENT_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

//#define __SMTP__DEBUG__

namespace Mitov
{
	class SMTPClient;
	class SMTPEMailAccount;
	class SMTPEMail;
//---------------------------------------------------------------------------
	namespace SMTPEMail_
	{
		struct TEMailTransaction
		{
		public:
			uint8_t		FRefCount = 0;

		public:
			SMTPEMailAccount	*SenderAccount;
			SMTPEMail	*Mail;

		public:
			void Acquire()
			{
				++FRefCount;
			}

			void Release()
			{
				--FRefCount;
				if( ! FRefCount )
					delete this;
			}

		public:
			TEMailTransaction( SMTPEMail *AMail ) :
				Mail( AMail )
			{
			}
		};
	}
//---------------------------------------------------------------------------
	class SMTPEMailAccount : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	MessageInputPin;

	public:
		String	ClientID;
		String	UserName;
		String	Password;
		uint16_t	Timeout = 5000;

	protected:
		SMTPClient &FOwner;

	protected:
		void DoMessageReceive( void *_Data );

	public:
		SMTPEMailAccount( SMTPClient &AOwner ) :
			FOwner( AOwner )
		{			
			MessageInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&SMTPEMailAccount::DoMessageReceive );
		}
	};
//---------------------------------------------------------------------------
	class SMTPEMailBasicFunction // : public OpenWire::Object
	{
	public:
		bool	Disconnect : 1;
		uint8_t RetryCount : 4;
		uint32_t Timeout;
		uint32_t Delay;
		SMTPEMail_::TEMailTransaction *Transaction;

	public:
		virtual bool TryProcessRequestedInput( String ALine, bool &AResponseCompleted, bool &ASuccess ) { return false; }
		virtual void TimedOut() {}

	public:
		SMTPEMailBasicFunction( SMTPEMail_::TEMailTransaction *ATransaction, bool ADisconnect, uint8_t ARetryCount, uint32_t ATimeout, uint32_t ADelay ) :
			Disconnect( ADisconnect ),
			RetryCount( ARetryCount ),
			Timeout( ATimeout ),
			Delay( ADelay ),
			Transaction( ATransaction )
		{
		}

		virtual ~SMTPEMailBasicFunction() {}

	};
//---------------------------------------------------------------------------
/*
	class SMTPEMailDisconnectFunction : public SMTPEMailBasicFunction
	{
		typedef SMTPEMailBasicFunction inherited;

	public:
		SMTPEMailDisconnectFunction() :
			inherited( true, 0, 2000, 0 )
		{
		}
	}
*/
//---------------------------------------------------------------------------
	class SMTPEMailConnectFunction : public SMTPEMailBasicFunction
	{
		typedef SMTPEMailBasicFunction inherited;

	protected:
		SMTPClient *FOwner;

	public:
		virtual bool TryProcessRequestedInput( String ALine, bool &AResponseCompleted, bool &ASuccess ) override
		{ 
			if( ALine.startsWith( "220 " ) )
			{
#ifdef __SMTP__DEBUG__
				Serial.println( "STARTED" );
#endif
				AResponseCompleted = true;
				ASuccess = true;
				return true;
			}

			return false; 
		}

		virtual void TimedOut() override;

	public:
		SMTPEMailConnectFunction( SMTPClient *AOwner, SMTPEMail_::TEMailTransaction *ATransaction ) :
			inherited( ATransaction, false, 5, 2000, 0 ),
			FOwner( AOwner )
		{
		}
	};
//---------------------------------------------------------------------------
	class SMTPEMailSendWaitResponse : public SMTPEMailBasicFunction
	{
		typedef SMTPEMailBasicFunction inherited;

	protected:
		int FResponse;

	public:
		virtual bool TryProcessRequestedInput( String ALine, bool &AResponseCompleted, bool &ASuccess ) override
		{ 
//			if( ALine.startsWith( "250 " ) )
			if( ALine.startsWith( String( FResponse ) + " " ) )
			{
#ifdef __SMTP__DEBUG__
				Serial.print( "RES: " );
				Serial.println( FResponse );
#endif
				AResponseCompleted = true;
				ASuccess = true;
				return true;
			}

			return false; 
		}

	public:
		SMTPEMailSendWaitResponse( SMTPEMail_::TEMailTransaction *ATransaction, int AResponse ) :
			inherited( ATransaction, false, 5, 2000, 0 ),
			FResponse( AResponse )
		{
		}

		SMTPEMailSendWaitResponse( SMTPEMail_::TEMailTransaction *ATransaction, int AResponse, int ACountReply ) :
			inherited( ATransaction, false, ACountReply, 2000, 0 ),
			FResponse( AResponse )
		{
		}
	};
//---------------------------------------------------------------------------
	class SMTPEMailSendDataResponse : public SMTPEMailBasicFunction
	{
		typedef SMTPEMailBasicFunction inherited;

	protected:
		SMTPClient *FOwner;

	public:
		virtual bool TryProcessRequestedInput( String ALine, bool &AResponseCompleted, bool &ASuccess ) override;

	public:
		SMTPEMailSendDataResponse( SMTPClient *AOwner, SMTPEMail_::TEMailTransaction *ATransaction ) :
			inherited( ATransaction, false, 5, 2000, 0 ),
			FOwner( AOwner )
		{
			Transaction->Acquire();
		}

		virtual ~SMTPEMailSendDataResponse()
		{
			Transaction->Release();
		}

	};
//---------------------------------------------------------------------------
	class TArduinoSMTPEMailAddress
	{
	public:
		String	Name;
		String	Address;

	public:
		String GetText()
		{
			if( Name == "" )
				return "<" + Address + ">";

			return Name + " <" + Address + ">";
		}
	};
//---------------------------------------------------------------------------	
	class SMTPEMailMessageBasic
	{
	public:
		virtual void SendLines( SMTPClient *AClient ) = 0;

	};
//---------------------------------------------------------------------------	
	class SMTPEMailMessageText : public SMTPEMailMessageBasic
	{
	public:
		String	Text;

	public:
		virtual void SendLines( SMTPClient *AClient ) override;

	};
//---------------------------------------------------------------------------	
	class SMTPEMailMessageHTML : public SMTPEMailMessageBasic
	{
	public:
		String	Text;

	public:
		virtual void SendLines( SMTPClient *AClient ) override;

	};
//---------------------------------------------------------------------------	
	class SMTPEMailMessageTextTHML : public SMTPEMailMessageBasic
	{
	public:
		String	Boundary;
		String	PlainText;
		String	HTMLText;

	public:
		virtual void SendLines( SMTPClient *AClient ) override;

	};
//---------------------------------------------------------------------------	
	class SMTPEMail : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	ClockInputPin;
		OpenWire::SourcePin	MessageOutputPin;

	public:
		TArduinoSMTPEMailAddress	From;
		Mitov::SimpleList<TArduinoSMTPEMailAddress *>	To;
		Mitov::SimpleList<TArduinoSMTPEMailAddress *>	CC;
		Mitov::SimpleList<TArduinoSMTPEMailAddress *>	BCC;
		String	Subject;
		SMTPEMailMessageBasic	*Message;

	public:
		void SendLines( SMTPClient *AClient );

	protected:
		void DoClockReceive( void *_Data )
		{
			if( ! To.size() )
				return;

			SMTPEMail_::TEMailTransaction *ATransaction = new SMTPEMail_::TEMailTransaction( this );

			ATransaction->Acquire();
			MessageOutputPin.Notify( ATransaction );
			ATransaction->Release();
		}

	public:
		SMTPEMail()
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&SMTPEMail::DoClockReceive );
		}

	};
//---------------------------------------------------------------------------	
	class SMTPClient : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;
		OpenWire::SinkPin	ConnectedInputPin;
		OpenWire::SourcePin	ConnectOutputPin;

	protected:
		struct TQueryElement
		{
		public:
			String Command;
			SMTPEMailBasicFunction *ResponseElement;

		public:
			TQueryElement() :
				ResponseElement( nullptr )
			{
			}

			TQueryElement( String ACommand, SMTPEMailBasicFunction *AResponseElement ) :
				Command( ACommand ),
				ResponseElement( AResponseElement )
			{
			}

		};

	protected:
		char		FBuffer[ 256 ];
		uint8_t		FIndex = 0;

	protected:
		Mitov::SimpleList<TQueryElement *>	FQueryQueue;
		TQueryElement *FCurrentElement = nullptr;
		uint32_t	FDelay = 0;
		unsigned long FCurrentMillis = 0;

	protected:
		SMTPEMailAccount	*FActiveAccount = nullptr;

	public:
		void SendTextLine( String AText )
		{
#ifdef __SMTP__DEBUG__
			Serial.print( "SEND: " );
			Serial.println( AText );
#endif

			AText += "\r\n";
			Mitov::TDataBlock ABlock( AText.length(), AText.c_str() );
			OutputPin.Notify( &ABlock );

			FCurrentMillis = millis();
		}

		void SendTextLineNoDot( String AText )
		{
			if( AText.startsWith( "." ))
				AText = "." + AText;

			SendTextLine( AText );
		}

		void SendLinesNoDot( String ALines )
		{
			int AStart = 0;
			for( int i = 0; i < ALines.length(); ++i )
			{
				if( ALines[ i ] == '\n' )
				{
					if( AStart != i )
					{
						String ALine = ALines.substring( AStart, i );
//						Serial.println( i - AStart );
						SendTextLineNoDot( ALine );
					}

					AStart = i + 1;
				}

				if( ALines[ i ] == '\r' )
				{
					String ALine = ALines.substring( AStart, i );
//					Serial.println( i - AStart );
					SendTextLineNoDot( ALine );
					AStart = i + 1;
				}

				else if ( i == ALines.length() - 1 )
				{
					String ALine = ALines.substring( AStart );
					SendTextLineNoDot( ALine );
				}
			}
		}

		inline void Disconnect()
		{
			ConnectOutputPin.SendValue( false );
		}

		void QuitConnection()
		{
			TrySendQuery( new TQueryElement( "QUIT", new SMTPEMailSendWaitResponse( nullptr, 221 ) ));
			TrySendQuery( new TQueryElement( "", new SMTPEMailBasicFunction( nullptr, true, 0, 0, 0 )));
			FActiveAccount = nullptr;
		}

		void ProcessTransaction( SMTPEMail_::TEMailTransaction *ATransaction )
		{
			if( FActiveAccount != ATransaction->SenderAccount )
			{
				if( FActiveAccount )
					QuitConnection();

				TrySendQuery( new TQueryElement( "", new SMTPEMailConnectFunction( this, ATransaction )));
//				ConnectOutputPin.SendValue( true );

				TrySendQuery( new TQueryElement( "EHLO " + ATransaction->SenderAccount->ClientID, new SMTPEMailSendWaitResponse( ATransaction, 250 )));
				if( ATransaction->SenderAccount->UserName != "" )
				{
					TrySendQuery( new TQueryElement( "AUTH LOGIN", new SMTPEMailSendWaitResponse( ATransaction, 334 )));
					TrySendQuery( new TQueryElement( Func::Base64Encode( ATransaction->SenderAccount->UserName ), new SMTPEMailSendWaitResponse( ATransaction, 334 )));
					TrySendQuery( new TQueryElement( Func::Base64Encode( ATransaction->SenderAccount->Password ), new SMTPEMailSendWaitResponse( ATransaction, 235 )));
					TrySendQuery( new TQueryElement( "MAIL FROM: <" + ATransaction->Mail->From.Address + ">", new SMTPEMailSendWaitResponse( ATransaction, 250 )));
				}
			}

			for( int i = 0; i < ATransaction->Mail->To.size(); ++i )
				TrySendQuery( new TQueryElement( "RCPT TO: <" + ATransaction->Mail->To[ i ]->Address + ">", new SMTPEMailSendWaitResponse( ATransaction, 250 )));

			for( int i = 0; i < ATransaction->Mail->CC.size(); ++i )
				TrySendQuery( new TQueryElement( "RCPT TO: <" + ATransaction->Mail->CC[ i ]->Address + ">", new SMTPEMailSendWaitResponse( ATransaction, 250 )));

			for( int i = 0; i < ATransaction->Mail->BCC.size(); ++i )
				TrySendQuery( new TQueryElement( "RCPT TO: <" + ATransaction->Mail->BCC[ i ]->Address + ">", new SMTPEMailSendWaitResponse( ATransaction, 250 )));

			TrySendQuery( new TQueryElement( "DATA", new SMTPEMailSendDataResponse( this, ATransaction )));
			FActiveAccount = ATransaction->SenderAccount;

//			TrySendQuery( new SMTPEMailSendMessage
		}

	public:
		void SendData( SMTPEMail_::TEMailTransaction *ATransaction )
		{
			ATransaction->Mail->SendLines( this );

			// Send .
			FQueryQueue.push_front( new TQueryElement( ".", new SMTPEMailSendWaitResponse( ATransaction, 250, 1 )) );
		}

	protected:
		void SendQuery( TQueryElement *AElement )
		{
#ifdef __SMTP__DEBUG__
			Serial.print( "QUERY : \"" );	Serial.print( AElement->Command ); Serial.println( "\"" );
#endif
			if( AElement->Command == "" )
			{
				if( AElement->ResponseElement->Disconnect )
				{
					Disconnect();
					delete AElement;
				}

				else
				{
					FCurrentMillis = millis();
					ConnectOutputPin.SendValue( true );
					FCurrentElement = AElement;
				}

				return;
			}

			FCurrentMillis = millis();

//			FStream.println( AQuery );
//			FStream.write( AElement->Command, AElement->Command.size() );
			SendTextLine( AElement->Command );

			FCurrentElement = AElement;
		}

		void ProcessNextCommand()
		{
			if( FCurrentElement )
			{
				FDelay = FCurrentElement->ResponseElement->Delay;
#ifdef __SMTP__DEBUG__
				Serial.print( "DELAY: " ); Serial.println( FDelay );
#endif
				delete FCurrentElement;
				FCurrentElement = nullptr;
			}

			if( FDelay )
				return;

#ifdef __SMTP__DEBUG__
			Serial.println( "ProcessNextCommand" );
#endif
//				Serial.print( "RESP_QUEUE: " ); Serial.println( FResponseHandlersQueue.size() );
			if( FQueryQueue.size() )
			{
//					Serial.print( "SEND_QUERY: " ); Serial.println( FQueryQueue.size() );
				TQueryElement *AElement = FQueryQueue[ 0 ];
//					Serial.print( "ESTRACT_QUERY: " ); Serial.println( ACommand );
				FQueryQueue.pop_front();

#ifdef __SMTP__DEBUG__
				Serial.println( "QUEUE>>" );
				for( int i = 0; i < FQueryQueue.size(); ++i )
					Serial.println( FQueryQueue[ i ]->Command );

				Serial.println( "<<QUEUE" );
#endif

				SendQuery( AElement );
//				Serial.print( "SEND_QUERY: " ); Serial.println( FQueryQueue.size() );
			}
		}

	public:
		void TrySendQuery( TQueryElement *AElement )
		{
//			if( ( PowerOn || ( AQuery == "AT" ) ) && ( FResponseHandlersQueue.size() == 0 ) && ( !FInPowerSwitch ))
//			if( FResponseHandlersQueue.size() == 0 )
			if( ! FCurrentElement )
				SendQuery( AElement );

			else
			{
//				while( FQueryQueue.size() > 10 )
//					SystemLoopBegin( micros());

#ifdef __SMTP__DEBUG__
				Serial.print( "ADD TO QUERY : \"" );	Serial.print( AElement->Command ); Serial.println( "\"" );
#endif
				FQueryQueue.push_back( AElement );

#ifdef __SMTP__DEBUG__
				Serial.println( "QUEUE>>" );
				for( int i = 0; i < FQueryQueue.size(); ++i )
					Serial.println( FQueryQueue[ i ]->Command );

				Serial.println( "<<QUEUE" );
#endif
			}
		}

	protected:
		void DeleteTransactionElements( SMTPEMail_::TEMailTransaction *ATransaction )
		{
			if( ! ATransaction )
				return;

#ifdef __SMTP__DEBUG__
			Serial.print( "DELETE TRANSACTION: " );
			Serial.print( FQueryQueue.size() );
#endif
			for( int i = FQueryQueue.size(); i--; )
				if( FQueryQueue[ i ]->ResponseElement->Transaction == ATransaction )
					FQueryQueue.Delete( i );

#ifdef __SMTP__DEBUG__
			Serial.print( " -> " );
			Serial.println( FQueryQueue.size() );
#endif
		}

	protected:
		void DoConnectedReceive( void *_Data )
		{
			if( ((bool *)_Data ))
				return;

			FActiveAccount = nullptr;
		}

		void DoReceive( void *_Data )
		{
			Mitov::TDataBlock ABlock = *(Mitov::TDataBlock *)_Data;
			if( ! ABlock.Size )
				return;

			for( int i = 0; i < ABlock.Size; ++i )
			{
				char AChar = ABlock.Data[ i ];
				if( AChar == 13 )
					return;

				if( AChar != 10 )
				{
					FBuffer[ FIndex ++ ] = AChar;
					if( FIndex < 255 )
						return;
				}

				FBuffer[ FIndex ] = '\0';
				FIndex = 0;

				String AString = FBuffer;

#ifdef __SMTP__DEBUG__
				Serial.println( AString );
#endif

/*
				if( AString.startsWith( "220 " ) )
				{
					Serial.println( "STARTED" );
				}
*/
				bool	AResponseCompleted = false;
				if( FCurrentElement )
				{
					bool ASuccess = false;
					if( FCurrentElement->ResponseElement->TryProcessRequestedInput( AString, AResponseCompleted, ASuccess ))
					{
						if( AResponseCompleted )
						{

							if( ( ! ASuccess ) && FCurrentElement->Command && FCurrentElement->ResponseElement->RetryCount )
							{
#ifdef __SMTP__DEBUG__
								Serial.println( "RETRY2" );
								Serial.println( FCurrentElement->Command );
#endif
								-- FCurrentElement->ResponseElement->RetryCount;
								SendQuery( FCurrentElement );
							}

							else
							{
								if( ! ASuccess )
									DeleteTransactionElements( FCurrentElement->ResponseElement->Transaction );

#ifdef __SMTP__DEBUG__
								Serial.println( "Queue Delete 2" );
								Serial.print( "DELETING: " ); Serial.println( FCurrentElement->Command );
		//							Serial.print( "RESP_QUEUE: " ); Serial.println( FResponseHandlersQueue.size() );
#endif
		//							Serial.println( "ProcessNextCommand 2" );
								ProcessNextCommand();
							}
						}
					}
				}
			}
		}


	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
//			inherited::SystemLoopBegin( currentMicros );
			if( FCurrentElement )
			{
//				Serial.println( "test2" );
				unsigned long AMillis = millis();
				if( ( AMillis - FCurrentMillis ) > FCurrentElement->ResponseElement->Timeout )
				{
#ifdef __SMTP__DEBUG__
					Serial.print( "TIMEOUT: " ); Serial.println( FCurrentElement->Command );
					Serial.println( FCurrentElement->ResponseElement->Timeout );
					Serial.println( AMillis );
					Serial.println( FCurrentMillis );
#endif
//					FLockRequestedInputIndex = 0;

					if( FCurrentElement->Command && FCurrentElement->ResponseElement->RetryCount )
					{
#ifdef __SMTP__DEBUG__
						Serial.println( "RETRY3" );
						Serial.println( FCurrentElement->Command );
#endif
						-- FCurrentElement->ResponseElement->RetryCount;
						SendQuery( FCurrentElement );
					}

					else
					{			
						FCurrentElement->ResponseElement->TimedOut();
						DeleteTransactionElements( FCurrentElement->ResponseElement->Transaction );
	//					Serial.println( "ProcessNextCommand 3" );
						ProcessNextCommand();
					}

				}
			}

			else if( FActiveAccount )
			{

//				Serial.println( "test1" );
				unsigned long AMillis = millis();
				if( ( AMillis - FCurrentMillis ) > FActiveAccount->Timeout )
				{
#ifdef __SMTP__DEBUG__
					Serial.println( "CLOSING!!!" );
#endif
					QuitConnection();
				}
			}

			if( FDelay )
			{
				unsigned long AMillis = millis();
				if( ( AMillis - FCurrentMillis ) > FDelay )
				{
#ifdef __SMTP__DEBUG__
					Serial.println( "DELAY COMPLETED" );
#endif
					FDelay = 0;
					ProcessNextCommand();
				}
			}
		}
/*
		virtual void SystemStart() override
		{
//			inherited::SystemStart();
		}
*/
	public:
		SMTPClient()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&SMTPClient::DoReceive );
			ConnectedInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&SMTPClient::DoConnectedReceive );
		}

	};
//---------------------------------------------------------------------------	
//---------------------------------------------------------------------------	
//---------------------------------------------------------------------------	
//---------------------------------------------------------------------------	
	void SMTPEMailAccount::DoMessageReceive( void *_Data )
	{
		SMTPEMail_::TEMailTransaction *ATransaction = (SMTPEMail_::TEMailTransaction *)_Data;
//		ATransaction->Acquire();
		ATransaction->SenderAccount = this;

		FOwner.ProcessTransaction( ATransaction );
	}
//---------------------------------------------------------------------------	
//---------------------------------------------------------------------------	
//---------------------------------------------------------------------------	
//---------------------------------------------------------------------------	
	void SMTPEMailConnectFunction::TimedOut()
	{
		FOwner->Disconnect();
	}
//---------------------------------------------------------------------------	
	bool SMTPEMailSendDataResponse::TryProcessRequestedInput( String ALine, bool &AResponseCompleted, bool &ASuccess )
	{ 
//			if( ALine.startsWith( "250 " ) )
		if( ALine.startsWith( "354 " ) )
		{
#ifdef __SMTP__DEBUG__
			Serial.println( "DATA!" );
#endif
			FOwner->SendData( Transaction );
			AResponseCompleted = true;
			ASuccess = true;
			return true;
		}

		return false; 
	}
//---------------------------------------------------------------------------	
	void SMTPEMail::SendLines( SMTPClient *AClient )
	{
		AClient->SendTextLine( "From: " + From.GetText() );

		{ // AToLine Scope
			String AToLine;
			for( int i = 0; i < To.size(); ++i )
			{
				if( i )
					AToLine += ", ";

				AToLine += To[ i ]->GetText();
			}

			AClient->SendTextLine( "To: " + AToLine );

			if( CC.size() )
			{
				AToLine = "";
				for( int i = 0; i < CC.size(); ++i )
				{
					if( i )
						AToLine += ", ";

					AToLine += CC[ i ]->GetText();
				}

				AClient->SendTextLine( "CC: " + AToLine );
			}

			if( CC.size() )
			{
				AToLine = "";
				for( int i = 0; i < BCC.size(); ++i )
				{
					if( i )
						AToLine += ", ";

					AToLine += BCC[ i ]->GetText();
				}

				AClient->SendTextLine( "BCC: " + AToLine );
			}
		}

		AClient->SendTextLine( "Subject: " + Subject );
		AClient->SendTextLine( "Mime-Version: 1.0" );
		Message->SendLines( AClient );
	}
//---------------------------------------------------------------------------	
	void SMTPEMailMessageText::SendLines( SMTPClient *AClient )
	{
		AClient->SendTextLine( "Content-Type: text/plain; charset=\"UTF-8\"" );
		AClient->SendTextLine( "Content-Transfer-Encoding: 7bit" );
		AClient->SendTextLine( "" );
		AClient->SendLinesNoDot( Text );
	}
//---------------------------------------------------------------------------	
	void SMTPEMailMessageHTML::SendLines( SMTPClient *AClient )
	{
		AClient->SendTextLine( "Content-Type: text/html; charset=\"UTF-8\"" );
		AClient->SendTextLine( "Content-Transfer-Encoding: 7bit" );
		AClient->SendTextLine( "" );
		AClient->SendLinesNoDot( Text );
	}
//---------------------------------------------------------------------------	
	void SMTPEMailMessageTextTHML::SendLines( SMTPClient *AClient )
	{
		AClient->SendTextLine( "Content-Type: multipart/alternative; boundary=" + Boundary );
		AClient->SendTextLine( "\r\n--" + Boundary );
		AClient->SendTextLine( "Content-Type: text/plain; charset=\"UTF-8\"" );
		AClient->SendTextLine( "Content-Transfer-Encoding: 7bit" );
		AClient->SendTextLine( "" );
		AClient->SendLinesNoDot( PlainText );

		AClient->SendTextLine( "--" + Boundary );
		AClient->SendTextLine( "Content-Type: text/html; charset=\"UTF-8\"" );
		AClient->SendTextLine( "Content-Transfer-Encoding: 7bit" );
		AClient->SendTextLine( "" );
		AClient->SendLinesNoDot( HTMLText );
		AClient->SendTextLine( "--" + Boundary + "--" );

	}
//---------------------------------------------------------------------------	
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
