////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_CONTROLLINO_RS485_h
#define _MITOV_CONTROLLINO_RS485_h

#include <Mitov.h>
#include <Controllino.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	template<typename T_SERIAL_TYPE, T_SERIAL_TYPE *T_SERIAL> class ControllinoRS48SerialPort : public Mitov::BasicTypedSerialPort<T_SERIAL_TYPE, T_SERIAL>
	{
		typedef	Mitov::BasicTypedSerialPort<T_SERIAL_TYPE, T_SERIAL> inherited;

	public:
		uint32_t				Speed = 9600;
		TArduinoSerialParity	Parity	 : 4;
		unsigned int			StopBits : 4;
		unsigned int			DataBits : 4;

	protected:
#ifdef _MITOV_SERIAL_SENDING_PIN_
		virtual uint32_t GetByteSpeed() override
		{ 
			return Speed / ( StopBits + DataBits + 1 ); 
		}
#endif

#if ((!defined _MITOV_CONTROLLINO_RS485_TRANSMIT_PIN_ ) || (!defined _MITOV_CONTROLLINO_RS485_RECEIVE_PIN_ ))
		virtual void UldateSending() override
		{
//			Serial.print( "UldateSending: " );
//			Serial.println( inherited::FSending );
#ifndef _MITOV_CONTROLLINO_RS485_TRANSMIT_PIN_
			Controllino_SwitchRS485DE( inherited::FSending );
#endif

#ifndef _MITOV_CONTROLLINO_RS485_RECEIVE_PIN_
			Controllino_SwitchRS485RE( inherited::FSending );
#endif
//			Controllino_SwitchRS485DE( 1 );
//			Controllino_SwitchRS485RE( 1 );

			inherited::UldateSending();
		}
#endif

	protected:
		void ResetSerial()
		{
			T_SERIAL->end();
			StartPort();
		}

	public:
		void SetSpeed( unsigned int AValue )
		{
            if( Speed == AValue )
                return;

            Speed = AValue;
            inherited::RestartPort();
		}

		void SetParity( TArduinoSerialParity AValue )
		{
            if( Parity == AValue )
                return;

            Parity = AValue;
            inherited::RestartPort();
		}

		void SetStopBits( unsigned int AValue )
		{
            if( StopBits == AValue )
                return;

            StopBits = AValue;
            inherited::RestartPort();
		}

		void SetDataBits( unsigned int AValue )
		{
            if( DataBits == AValue )
                return;

            DataBits = AValue;
            inherited::RestartPort();
		}

	protected:
		virtual void StartPort() override
		{
			int AIndex = ((int)Parity) * 8 + ( StopBits - 1 ) * 4 + ( DataBits - 5);
			T_SERIAL->begin( Speed, CSerialInits[ AIndex ] );
		}

	public:
		ControllinoRS48SerialPort() :
			Parity( spNone ),
			StopBits( 1 ),
			DataBits( 8 )
		{
		}
	};
//---------------------------------------------------------------------------
	class ControllinoRS485Module : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
#ifdef _MITOV_CONTROLLINO_RS485_TRANSMIT_PIN_
		OpenWire::SinkPin	TransmitInputPin;
#endif

#ifdef _MITOV_CONTROLLINO_RS485_RECEIVE_PIN_
		OpenWire::SinkPin	ReceiveInputPin;
#endif

	public:
		bool	FTransmit : 1;
		bool	FReceive : 1;

	public:
		bool	Enabled : 1;

	public:
		void SetEnabled( bool AValue )
		{
			if( Enabled == AValue )
				return;

			Enabled = AValue;
			if( Enabled )
				StartModule();

			else
				StopModule();

		}

	protected:
		virtual void SystemInit()
		{
			if( Enabled )
				StartModule();

			inherited::SystemInit();
		}

		void StopModule()
		{
		}

		void StartModule()
		{
//			Serial.println( "START" );
			Controllino_RS485Init();
#ifdef _MITOV_CONTROLLINO_RS485_TRANSMIT_PIN_
			Controllino_SwitchRS485DE( FTransmit );
#endif
#ifdef _MITOV_CONTROLLINO_RS485_RECEIVE_PIN_
			Controllino_SwitchRS485RE( ! FReceive );
#endif
		}

	protected:
#ifdef _MITOV_CONTROLLINO_RS485_TRANSMIT_PIN_
		void DoTransmitInputPinReceive( void *_Data )
		{
			FTransmit = *(bool *)_Data;
			if( ! Enabled )
				return;

			Controllino_SwitchRS485DE( FTransmit );
		}
#endif

#ifdef _MITOV_CONTROLLINO_RS485_RECEIVE_PIN_
		void DoReceiveInputPinReceive( void *_Data )
		{
			FReceive = *(bool *)_Data;
			if( ! Enabled )
				return;

			Controllino_SwitchRS485RE( ! FReceive );
		}
#endif

	public:
		ControllinoRS485Module() :
			Enabled( true ),
			FTransmit( false ),
			FReceive( false )
		{
#ifdef _MITOV_CONTROLLINO_RS485_TRANSMIT_PIN_
			TransmitInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ControllinoRS485Module::DoTransmitInputPinReceive );
#endif

#ifdef _MITOV_CONTROLLINO_RS485_RECEIVE_PIN_
			ReceiveInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ControllinoRS485Module::DoReceiveInputPinReceive );
#endif
		}

	};
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
