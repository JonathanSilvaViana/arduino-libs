////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ARDUINO_SPI_h
#define _MITOV_ARDUINO_SPI_h

#include <Mitov.h>
#include <Mitov_Basic_SPI.h>

namespace Mitov
{
	template<SPIClass &ISPI> class ArduinoSPI : public Mitov::BasicSPI
	{
		typedef Mitov::BasicSPI inherited;

	public:
		virtual uint16_t transfer16(uint16_t data) override
		{
			return ISPI.transfer16( data );
		}

		virtual uint8_t transfer(uint8_t data) override
		{
			return ISPI.transfer( data );
		}

		virtual void transfer(void *buf, size_t count) override
		{
#if defined(VISUINO_ESP8266) || defined(VISUINO_ESP32)
			ISPI.writeBytes( (uint8_t*)buf, count );
#else // VISUINO_ESP8266
			ISPI.transfer( buf, count );
#endif // VISUINO_ESP8266
		}

		virtual void beginTransaction(SPISettings settings) override
		{
		#if defined(SPI_HAS_TRANSACTION)
			ISPI.beginTransaction( settings );
		#endif
		}

		virtual void endTransaction() override
		{
		#if defined(SPI_HAS_TRANSACTION)
			ISPI.endTransaction();
		#endif
		}

		virtual void setDataMode( uint8_t dataMode ) override
		{
			ISPI.setDataMode( dataMode );
		}

/*
		virtual void setBitOrder( uint8_t bitOrder ) override
		{
			ISPI.setBitOrder( bitOrder );
		}
*/
		virtual void setClockDivider( uint8_t clockDiv ) override
		{
			ISPI.setClockDivider( clockDiv );
		}

		virtual void usingInterrupt( uint8_t interruptNumber ) override
		{
#if ! ( defined(VISUINO_ESP8266) || defined(VISUINO_ESP32) )
			ISPI.usingInterrupt( interruptNumber );
#endif
		}

	protected:
		virtual void SystemInit()
		{
			inherited::SystemInit();
			ISPI.begin();
		}
	};
//---------------------------------------------------------------------------
}
#endif
