////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_DFROBOT_SERIAL_MP3_h
#define _MITOV_DFROBOT_SERIAL_MP3_h

#include <Mitov.h>
//#include <DFRobotDFPlayerMini.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	enum TArduinoDFRobotMP3PlayerEqualizerMode { emNormal, emPop, emRock, emJazz, emClassic, emBass };
//---------------------------------------------------------------------------
	enum TArduinoDFRobotMP3PlayerOutputDevice { odUDisk, odSD, odAux, odSleep, odFlash };
//---------------------------------------------------------------------------
	class TArduinoDFRobotMP3PlayerGain
	{
	public:
		bool	Enabled : 1;
		uint8_t Value : 7;

	public:
		TArduinoDFRobotMP3PlayerGain() :
			Enabled( false ),
			Value( 0 )
		{
		}
	};
//---------------------------------------------------------------------------
	class MitovDFRobotSerialMP3 : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
#ifdef _MITOV_DFROBOT_SERIAL_MP3_RESET_PIN_
		OpenWire::SinkPin	ResetInputPin;
#endif

#ifdef _MITOV_DFROBOT_SERIAL_MP3_NEXT_PIN_
		OpenWire::SinkPin	NextInputPin;
#endif

#ifdef _MITOV_DFROBOT_SERIAL_MP3_PREVIOUS_PIN_
		OpenWire::SinkPin	PreviousInputPin;
#endif

#ifdef _MITOV_DFROBOT_SERIAL_MP3_CARD_INSERTED_PIN_
		OpenWire::SourcePin	CardInsertedOutputPin;
#endif

#ifdef _MITOV_DFROBOT_SERIAL_MP3_CARD_REMOVED_PIN_
		OpenWire::SourcePin	CardRemovedOutputPin;
#endif

#ifdef _MITOV_DFROBOT_SERIAL_MP3_CARD_ONLINE_PIN_
		OpenWire::SourcePin	CardOnlineOutputPin;
#endif

#ifdef _MITOV_DFROBOT_SERIAL_MP3_CARD_REMOVED_PIN_
		OpenWire::SourcePin	CardRemovedOutputPin;
#endif

#ifdef _MITOV_DFROBOT_SERIAL_MP3_BUSY_ERROR_PIN_
		OpenWire::SourcePin	BusyErrorOutputPin;
#endif

#ifdef _MITOV_DFROBOT_SERIAL_MP3_SLEEPING_PIN_
		OpenWire::SourcePin	SleepingOutputPin;
#endif

#ifdef _MITOV_DFROBOT_SERIAL_MP3_CHECK_SUM_ERROR_PIN_
		OpenWire::SourcePin	CheckSumErrorOutputPin;
#endif

#ifdef _MITOV_DFROBOT_SERIAL_MP3_FILE_INDEX_ERROR_PIN_
		OpenWire::SourcePin	FileIndexErrorOutputPin;
#endif

#ifdef _MITOV_DFROBOT_SERIAL_MP3_FILE_MISMATCH_ERROR_PIN_
		OpenWire::SourcePin	FileMismatchErrorOutputPin;
#endif

#ifdef _MITOV_DFROBOT_SERIAL_MP3_ADVERTISEMENT_ERROR_PIN_
		OpenWire::SourcePin	InAdvertiseErrorOutputPin;
#endif

#ifdef _MITOV_DFROBOT_SERIAL_MP3_TIMEOUT_ERROR_PIN_
		OpenWire::SourcePin	TimeoutErrorOutputPin;
#endif

#ifdef _MITOV_DFROBOT_SERIAL_MP3_ERROR_PIN_
		OpenWire::SourcePin	ErrorOutputPin;
#endif

	protected:
		static const uint8_t MP3Busy = 1;
		static const uint8_t MP3Sleeping = 2;
		static const uint8_t MP3SerialWrongStack = 3;
		static const uint8_t MP3CheckSumNotMatch = 4;
		static const uint8_t MP3FileIndexOut = 5;
		static const uint8_t MP3FileMismatch = 6;
		static const uint8_t MP3Advertise = 7;

		static const uint8_t Stack_Header = 0;
		static const uint8_t Stack_Version = 1;
		static const uint8_t Stack_Length = 2;
		static const uint8_t Stack_Command = 3;
		static const uint8_t Stack_ACK = 4;
		static const uint8_t Stack_Parameter = 5;
		static const uint8_t Stack_CheckSum = 7;
		static const uint8_t Stack_End = 9;

	public:
		float	Volume = 0.5;
		unsigned long TimeOutPeriod = 500;
		bool	Loop : 1; //= false;
		bool	EnableDAC : 1; // = true;
		TArduinoDFRobotMP3PlayerEqualizerMode	EqualizerMode : 3; //= emNormal;
		TArduinoDFRobotMP3PlayerOutputDevice	OutputDevice : 3; // = odSD;
		TArduinoDFRobotMP3PlayerGain			Gain;

	public:
//		DFRobotDFPlayerMini FPlayer;
		Mitov::BasicSerialPort &FSerial;

	public:
		void SetVolume( float AValue )
		{
			if( Volume == AValue )
				return;

			Volume = AValue;
			SetIntVolume( Volume * 30 + 0.5 );
		}

		void SetLoop( bool AValue )
		{
			if( Loop == AValue )
				return;

			Loop = AValue;
			if( Loop )
				EnableLoop( 0x01 );

			else
				EnableLoop( 0x00 );

		}

		void SetEnableDAC( bool AValue )
		{
			if( EnableDAC == AValue )
				return;

			EnableDAC = AValue;
			if( EnableDAC )
				SendCommand( 0x1A, 0x00 );

			else
				SendCommand( 0x1A, 0x01 );

		}

		void SetEqualizerMode( TArduinoDFRobotMP3PlayerEqualizerMode AValue )
		{
			if( EqualizerMode == AValue )
				return;

			EqualizerMode = AValue;
			SendCommand( 0x07, EqualizerMode );
		}

		void SetOutputDevice( TArduinoDFRobotMP3PlayerOutputDevice AValue )
		{
			if( OutputDevice == AValue )
				return;

			OutputDevice = AValue;
			SendCommand( 0x09, OutputDevice );
			delay(200);
		}

		void UpdateGain()
		{
			if( Gain.Enabled )
				SendCommand( 0x10, 1, Gain.Value );

			else
				SendCommand( 0x10, 0, Gain.Value );

		}

	protected:
		uint8_t FReceiveBuffer[ 10 ];
		uint8_t FSendBuffer[ 10 ] = {0x7E, 0xFF, 06, 00, 01, 00, 00, 00, 00, 0xEF};
		unsigned long FTimeOutTimer;
		bool	FIsSending = false;
		bool	FIsAvailable = false;
		bool	FProcessed = false;
		uint8_t FReceivedIndex = 0;
		uint16_t FHandleParameter = 0;

	protected:
		inline uint8_t	ReadByte()
		{
			uint8_t	AResult = FSerial.GetStream().read();
//			Serial.println( AResult, HEX );
			return AResult;
		}

		void uint16ToArray(uint16_t value, uint8_t *array)
		{
			*array = (uint8_t)(value>>8);
			*(array+1) = (uint8_t)(value);
		}

		void SendBuffer()
		{
			while ( FIsSending ) 
				ProcessInput();

#ifdef _DFROBOT_SERIAL_MP3_DEBUG
			Serial.println();
			Serial.print(F("sending:"));
			for (int i=0; i<sizeof( FSendBuffer ); i++) 
			{
				Serial.print( FSendBuffer[i],HEX);
				Serial.print(F(" "));
			}
			Serial.println();
#endif
			FSerial.GetStream().write( FSendBuffer, sizeof( FSendBuffer ));
			FTimeOutTimer = millis();
			FIsSending = true; //_sending[Stack_ACK];
		}

		uint16_t CalculateCheckSum(uint8_t *buffer)
		{
			uint16_t sum = 0;
			for (int i=Stack_Version; i<Stack_CheckSum; i++)
				sum += buffer[i];
  
			return -sum;
		}

		void ProcessInput()
		{
			while( FSerial.GetStream().available()) 
			{
				if ( FReceivedIndex == 0) 
				{
					FReceiveBuffer[Stack_Header] = ReadByte();
#ifdef _DFROBOT_SERIAL_MP3_DEBUG
					Serial.print(F("received:"));
					Serial.print(FReceiveBuffer[FReceivedIndex],HEX);
					Serial.print(F(" "));
#endif
					if (FReceiveBuffer[Stack_Header] == 0x7E) 
					{
						FIsAvailable = false;
						FProcessed = false;
						FReceivedIndex ++;
					}
				}
				else
				{
					FReceiveBuffer[FReceivedIndex] = ReadByte();
#ifdef _DFROBOT_SERIAL_MP3_DEBUG
					Serial.print(FReceiveBuffer[FReceivedIndex],HEX);
					Serial.print(F(" "));
#endif
					switch( FReceivedIndex )
					{
						case Stack_Version:
							if (FReceiveBuffer[FReceivedIndex] != 0xFF) 
								ErrorWrongBuffer();

							break;

						case Stack_Length:
							if (FReceiveBuffer[FReceivedIndex] != 0x06) 
								ErrorWrongBuffer();

							break;

						case Stack_End:
#ifdef _DFROBOT_SERIAL_MP3_DEBUG
							Serial.println();
#endif
							if (FReceiveBuffer[FReceivedIndex] != 0xEF)
							{
								ErrorWrongBuffer();
								return;
							}
							else
							{
								if ( ValidateStack()) 
								{
									FReceivedIndex = 0;
									ParseStack();
									FProcessed = true;
//									if ( FIsAvailable && !FSendBuffer[Stack_ACK]) 
//										FIsSending = false;

									return; // _isAvailable;
								}
								else
								{
									ErrorWrongBuffer();
									return;
								}
							}
							break;

					}
					FReceivedIndex++;
				}
			}
  
			if ( FIsSending && (millis() - FTimeOutTimer >= TimeOutPeriod)) 
			{
				ErrorTimeout();
				FProcessed = true;
			}
  
//			return _isAvailable;
		}


		void ParseStack()
		{
			uint8_t		AHandleCommand = *(FReceiveBuffer + Stack_Command);
			FHandleParameter =  ArrayToUint16(FReceiveBuffer + Stack_Parameter);

			FIsAvailable = true;
			switch (AHandleCommand) 
			{
				case 0x3D:
					PlayFinished( FHandleParameter );
//					handleMessage(DFPlayerPlayFinished, FHandleParameter);
					break;

				case 0x3F:
					if ( FHandleParameter & 0x02) 
						PlayerCardOnline();
//						handleMessage(DFPlayerCardOnline, FHandleParameter);

					break;

				case 0x3A:
					if ( FHandleParameter & 0x02)
						PlayerCardInserted();
//						handleMessage(DFPlayerCardInserted, FHandleParameter);

					break;

				case 0x3B:
					if ( FHandleParameter & 0x02)
						PlayerCardRemoved();
//						handleMessage(DFPlayerCardRemoved, FHandleParameter);

					break;

				case 0x40:
					HandleError( FHandleParameter );
//					handleMessage(DFPlayerError, FHandleParameter);
					break;

				case 0x41:
					FIsSending = false;
					break;

				case 0x3C:
				case 0x3E:
				case 0x42:
				case 0x43:
				case 0x44:
				case 0x45:
				case 0x46:
				case 0x47:
				case 0x48:
				case 0x49:
				case 0x4B:
				case 0x4C:
				case 0x4D:
				case 0x4E:
				case 0x4F:
					FIsAvailable = true;
					break;

				default:
					ErrorWrongBuffer();
			}
		}

		void HandleError( uint16_t AError )
		{
			FIsSending = false;

#ifdef _MITOV_DFROBOT_SERIAL_MP3_ERROR_PIN_
			ErrorOutputPin.SendValue<uint32_t>( AError );
#endif
		}

		void PlayFinished( uint16_t ANumber )
		{
		}

		void ErrorWrongBuffer()
		{
#ifdef _MITOV_DFROBOT_SERIAL_MP3_ERROR_PIN_
			ErrorOutputPin.SendValue<uint32_t>( MP3SerialWrongStack );
#endif
		}

		void ErrorTimeout()
		{
			FIsSending = false;
#ifdef _MITOV_DFROBOT_SERIAL_MP3_TIMEOUT_ERROR_PIN_
			TimeoutErrorOutputPin.Notify( nullptr );
#endif
		}

		inline void PlayerCardOnline()
		{
#ifdef _MITOV_DFROBOT_SERIAL_MP3_CARD_ONLINE_PIN_
			CardOnlineOutputPin.Notify( nullptr );
#endif
		}

		inline void PlayerCardInserted()
		{
#ifdef _MITOV_DFROBOT_SERIAL_MP3_CARD_INSERTED_PIN_
			CardInsertedOutputPin.Notify( nullptr );
#endif
		}

		inline void PlayerCardRemoved()
		{
#ifdef _MITOV_DFROBOT_SERIAL_MP3_CARD_REMOVED_PIN_
			CardRemovedOutputPin.Notify( nullptr );
#endif
		}

		bool ValidateStack()
		{
			return CalculateCheckSum( FReceiveBuffer ) == ArrayToUint16( FReceiveBuffer + Stack_CheckSum );
		}

		uint16_t ArrayToUint16(uint8_t *array)
		{
			uint16_t value = *array;
			value <<=8;
			value += *(array+1);
			return value;
		}

	public:
		void SendCommand(uint8_t command, uint16_t argument)
		{
			FSendBuffer[Stack_Command] = command;
			uint16ToArray(argument, FSendBuffer + Stack_Parameter);
			uint16ToArray( CalculateCheckSum( FSendBuffer ), FSendBuffer + Stack_CheckSum );
			SendBuffer();
		}

		void SendCommand(uint8_t command, uint8_t argumentHigh, uint8_t argumentLow)
		{
			uint16_t buffer = argumentHigh;
			buffer <<= 8;
			SendCommand(command, buffer | argumentLow);
		}

		void Play( int fileNumber )
		{
			SendCommand( 0x03, fileNumber );
//			if( Loop )
//				EnableLoop( 0x01 );

//			else
//				EnableLoop( 0x00 );

		}

		inline void LoopFile( int fileNumber )
		{
			SendCommand( 0x08, fileNumber );
		}

		inline void Next()
		{
			SendCommand( 0x01, 0 );
		}

		inline void Previous()
		{
			SendCommand( 0x02, 0 );
		}

		inline void PlayFolder(uint8_t folderNumber, uint8_t fileNumber)
		{
			SendCommand(0x0F, folderNumber, fileNumber);
		}

		inline void PlayLargeFolder(uint8_t folderNumber, uint16_t fileNumber)
		{
			SendCommand(0x14, (((uint16_t)folderNumber) << 12) | fileNumber);
		}

/*
		inline void Sleep()
		{
			SendCommand( 0x0A, 0 );
		}
*/
		inline void Start()
		{
			SendCommand( 0x0A, 0 );
		}

		uint32_t	GetFilesCount()
		{
			switch( OutputDevice ) 
			{
				case odUDisk:
					SendCommand( 0x47, 0 );
					break;

				case odSD:
					SendCommand( 0x48, 0 );
					break;

				case odFlash:
					SendCommand( 0x49, 0 );
					break;

				default:
					return 0;
			}

			return GetResult();
		}

		uint32_t	GetFilesCountInFolder( uint8_t folderNumber )
		{
			SendCommand( 0x4E, folderNumber );
			return GetResult();
		}

		uint32_t	GetResult()
		{
			FProcessed = false;
			while( ! FProcessed )
				ProcessInput();

			return FHandleParameter;
		}

/*
		inline void PlayMP3FolderFile( uint16_t fileNumber )
		{
			SendCommand( 0x12, fileNumber );
		}

		inline void PlayAdvertisementFolderFile( uint16_t fileNumber )
		{
			SendCommand( 0x13, fileNumber );
		}
*/
		inline void SetIntVolume(uint8_t volume)
		{
			SendCommand( 0x06, volume );
		}

		inline void EnableLoop( uint8_t AValue )
		{
			SendCommand( 0x11, AValue );
		}

		inline void Reset()
		{
			TimeOutPeriod += 3000;
			SendCommand( 0x0C, 0 );
			TimeOutPeriod -= 3000;
			delay( 200 );
		}


	protected:
#ifdef _MITOV_DFROBOT_SERIAL_MP3_RESET_PIN_
		void DoResetReceive( void *_Data )
		{
			Reset();
		}
#endif

#ifdef _MITOV_DFROBOT_SERIAL_MP3_NEXT_PIN_
		void DoNextReceive( void *_Data )
		{
			Next();
		}
#endif

#ifdef _MITOV_DFROBOT_SERIAL_MP3_PREVIOUS_PIN_
		void DoPreviousReceive( void *_Data )
		{
			Previous();
		}
#endif

		virtual void SystemInit() override
		{
//			inherited::SystemStart();

//			FPlayer.begin( FSerial.GetStream() );

//			FSerial.GetStream().begin( 9600 );

//			Serial.println( "START" );
			Reset();

//			Serial.println( "DEVICE" );
			SendCommand( 0x09, OutputDevice );
			delay(200);

//			Serial.println( "VOLUME" );
			SetIntVolume( Volume * 30 + 0.5 );

//			Serial.println( "EqualizerMode" );
			SendCommand( 0x07, EqualizerMode );
			UpdateGain();

			if( EnableDAC )
				SendCommand( 0x1A, 0x00 );

			else
				SendCommand( 0x1A, 0x01 );

			if( Loop )
				EnableLoop( 0x01 );

			else
				EnableLoop( 0x00 );

			delay(600);
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
//			inherited::SystemLoopBegin( currentMicros );
			ProcessInput();
		}

	public:
		MitovDFRobotSerialMP3( Mitov::BasicSerialPort &ASerial, OpenWire::StreamPin &ASerialOutputPin ) :
			Loop( false ),
			EnableDAC( true ),
			EqualizerMode( emNormal ),
			OutputDevice( odSD ),
			FSerial( ASerial )
		{
#ifdef _MITOV_DFROBOT_SERIAL_MP3_RESET_PIN_
			ResetInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&MitovDFRobotSerialMP3::DoResetReceive );
#endif

#ifdef _MITOV_DFROBOT_SERIAL_MP3_NEXT_PIN_
			NextInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&MitovDFRobotSerialMP3::DoNextReceive );
#endif

#ifdef _MITOV_DFROBOT_SERIAL_MP3_PREVIOUS_PIN_
			PreviousInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&MitovDFRobotSerialMP3::DoPreviousReceive );
#endif
//			FPlayer.begin( ASerial.GetStream() );
		}

	};
//---------------------------------------------------------------------------
	class DFRobotMP3PlayerElementPlayFolderFile : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	ClockInputPin;

	public:
		uint8_t		Folder : 4;
		uint16_t	File : 12;

	protected:
		MitovDFRobotSerialMP3 &FOwner;

	protected:
		void DoClockReceive( void *_Data )
		{
			FOwner.PlayFolder( Folder, File );
		}

	public:
		DFRobotMP3PlayerElementPlayFolderFile( MitovDFRobotSerialMP3 &AOwner ) :
			Folder( 1 ),
			File( 1 ),
			FOwner( AOwner )
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&DFRobotMP3PlayerElementPlayFolderFile::DoClockReceive );
		}

	};
//---------------------------------------------------------------------------
	class DFRobotMP3PlayerElementPlayLargeFolderFile : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	ClockInputPin;

	public:
		uint8_t		Folder : 4;
		uint16_t	File : 12;

	protected:
		MitovDFRobotSerialMP3 &FOwner;

	protected:
		void DoClockReceive( void *_Data )
		{
			FOwner.PlayLargeFolder( Folder, File );
		}

	public:
		DFRobotMP3PlayerElementPlayLargeFolderFile( MitovDFRobotSerialMP3 &AOwner ) :
			Folder( 1 ),
			File( 1 ),
			FOwner( AOwner )
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&DFRobotMP3PlayerElementPlayLargeFolderFile::DoClockReceive );
		}

	};
//---------------------------------------------------------------------------
	template<uint8_t T_COMMAND> class DFRobotMP3PlayerElementPlayFile : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	ClockInputPin;

	public:
		uint16_t	File = 1;

	protected:
		MitovDFRobotSerialMP3 &FOwner;

	protected:
		void DoClockReceive( void *_Data )
		{
//			Serial.println( T_COMMAND, HEX );
//			FOwner.Play( File );
			FOwner.SendCommand( T_COMMAND, File );
//			if( T_COMMAND == 3 )
//				FOwner.EnableLoop( 0x01 );

//			FOwner.EnableLoop( 0x00 );
//			Serial.println( "Playing" );
		}

	public:
		DFRobotMP3PlayerElementPlayFile( MitovDFRobotSerialMP3 &AOwner ) :
			FOwner( AOwner )
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&DFRobotMP3PlayerElementPlayFile::DoClockReceive );
		}

	};
//---------------------------------------------------------------------------
	class DFRobotMP3PlayerElementPlayFileLoop : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	ClockInputPin;

	public:
		uint16_t	File = 1;
		bool		Loop = false;

	protected:
		MitovDFRobotSerialMP3 &FOwner;

	protected:
		void DoClockReceive( void *_Data )
		{
//			Serial.println( T_COMMAND, HEX );
			if( Loop )
				FOwner.LoopFile( File );

			else
				FOwner.Play( File );
//			FOwner.SendCommand( T_COMMAND, File );
//			if( T_COMMAND == 3 )
//				FOwner.EnableLoop( 0x01 );

//			FOwner.EnableLoop( 0x00 );
//			Serial.println( "Playing" );
		}

	public:
		DFRobotMP3PlayerElementPlayFileLoop( MitovDFRobotSerialMP3 &AOwner ) :
			FOwner( AOwner )
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&DFRobotMP3PlayerElementPlayFileLoop::DoClockReceive );
		}

	};
//---------------------------------------------------------------------------
	class DFRobotMP3PlayerElementGetFilesCount : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	ClockInputPin;
		OpenWire::SourcePin	OutputPin;

	protected:
		MitovDFRobotSerialMP3 &FOwner;

	protected:
		void DoClockReceive( void *_Data )
		{
			uint32_t	AResult = FOwner.GetFilesCount();
			OutputPin.Notify( &AResult );
		}

	public:
		DFRobotMP3PlayerElementGetFilesCount( MitovDFRobotSerialMP3 &AOwner ) :
			FOwner( AOwner )
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&DFRobotMP3PlayerElementGetFilesCount::DoClockReceive );
		}

	};
//---------------------------------------------------------------------------
	class DFRobotMP3PlayerElementGetFilesCountInFolder : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	ClockInputPin;
		OpenWire::SourcePin	OutputPin;

	public:
		uint8_t		Folder = 1;

	protected:
		MitovDFRobotSerialMP3 &FOwner;

	protected:
		void DoClockReceive( void *_Data )
		{
			uint32_t	AResult = FOwner.GetFilesCountInFolder( Folder );
			OutputPin.Notify( &AResult );
		}

	public:
		DFRobotMP3PlayerElementGetFilesCountInFolder( MitovDFRobotSerialMP3 &AOwner ) :
			FOwner( AOwner )
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&DFRobotMP3PlayerElementGetFilesCountInFolder::DoClockReceive );
		}

	};
//---------------------------------------------------------------------------
	template<uint8_t T_COMMAND> class DFRobotMP3PlayerElementTypedCommand : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	ClockInputPin;
		OpenWire::SourcePin	OutputPin;

	protected:
		MitovDFRobotSerialMP3 &FOwner;

	protected:
		void DoClockReceive( void *_Data )
		{
			FOwner.SendCommand( T_COMMAND, 0 );
//			FOwner.Sleep();
		}

	public:
		DFRobotMP3PlayerElementTypedCommand( MitovDFRobotSerialMP3 &AOwner ) :
			FOwner( AOwner )
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&DFRobotMP3PlayerElementTypedCommand::DoClockReceive );
		}

	};
//---------------------------------------------------------------------------
	class DFRobotMP3PlayerElementLoopFolder : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	ClockInputPin;

	public:
		uint8_t		Folder = 1;

	protected:
		MitovDFRobotSerialMP3 &FOwner;

	protected:
		void DoClockReceive( void *_Data )
		{
			FOwner.SendCommand( 0x17, Folder );
		}

	public:
		DFRobotMP3PlayerElementLoopFolder( MitovDFRobotSerialMP3 &AOwner ) :
			FOwner( AOwner )
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&DFRobotMP3PlayerElementLoopFolder::DoClockReceive );
		}

	};
//---------------------------------------------------------------------------
/*
	class DFRobotMP3PlayerElementPlayMP3FolderFile : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	ClockInputPin;

	public:
		uint16_t	File = 1;

	protected:
		MitovDFRobotSerialMP3 &FOwner;

	protected:
		void DoClockReceive( void *_Data )
		{
			FOwner.PlayMP3FolderFile( File );
		}

	public:
		DFRobotMP3PlayerElementPlayMP3FolderFile( MitovDFRobotSerialMP3 &AOwner ) :
			FOwner( AOwner )
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&DFRobotMP3PlayerElementPlayMP3FolderFile::DoClockReceive );
		}

	};
//---------------------------------------------------------------------------
	class DFRobotMP3PlayerElementPlayAdvertisementFolderFile : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	ClockInputPin;

	public:
		uint16_t	File = 1;

	protected:
		MitovDFRobotSerialMP3 &FOwner;

	protected:
		void DoClockReceive( void *_Data )
		{
			FOwner.PlayAdvertisementFolderFile( File );
		}

	public:
		DFRobotMP3PlayerElementPlayAdvertisementFolderFile( MitovDFRobotSerialMP3 &AOwner ) :
			FOwner( AOwner )
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&DFRobotMP3PlayerElementPlayAdvertisementFolderFile::DoClockReceive );
		}

	};
*/
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
