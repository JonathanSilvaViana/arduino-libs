////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_EVIVE_DISPLAY_h
#define _MITOV_EVIVE_DISPLAY_h

#include <Mitov.h>
#include <Mitov_Display_ST7735.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	class ArduinoEVIVEDisplay : public DisplayST7735Basic
	{
		typedef DisplayST7735Basic inherited;

	protected:
		static const uint16_t WIDTH = 160;
		static const uint16_t HEIGHT = 128;

	protected:
		virtual void UpdateOrientation() override
		{
			writecommand(ST7735::MADCTL);

			switch( Orientation )
			{
				case goUp:		writedata(ST7735::MADCTL_MY | ST7735::MADCTL_MV | ST7735::MADCTL_RGB); break;
				case goRight: 	writedata(ST7735::MADCTL_MX | ST7735::MADCTL_MY | ST7735::MADCTL_RGB); break;
				case goDown: 	writedata(ST7735::MADCTL_MX | ST7735::MADCTL_MV | ST7735::MADCTL_BGR); break;
				case goLeft:	writedata(ST7735::MADCTL_RGB); break;
/*
				case goUp:		writedata(ST7735::MADCTL_MX | ST7735::MADCTL_MY | ST7735::MADCTL_RGB); break;
				case goRight: 	writedata(ST7735::MADCTL_MY | ST7735::MADCTL_MV | ST7735::MADCTL_RGB); break;
				case goDown: 	writedata(ST7735::MADCTL_RGB); break;
				case goLeft:	writedata(ST7735::MADCTL_MX | ST7735::MADCTL_MV | ST7735::MADCTL_RGB); break;
*/
			}
		}

	public:
		virtual int16_t width(void) const override
		{
			if( Orientation == goUp || Orientation == goDown )
				return WIDTH;

			return HEIGHT;
		}

		virtual int16_t height(void) const override
		{
			if( Orientation == goUp || Orientation == goDown )
				return HEIGHT;

			return WIDTH;
		}

//#ifdef	_MITOV_ST7735_RESET_PIN_
		virtual void SetResetOutput( bool AValue ) override
		{
			digitalWrite( 47, AValue ? HIGH : LOW );
		}
//#endif

		virtual void SetRegisterSelectOutput( bool AValue ) override
		{
			digitalWrite( 49, AValue ? HIGH : LOW );
		}

		virtual void SetChipSelectOutput( bool AValue ) override
		{
			digitalWrite( 48, AValue ? HIGH : LOW );
		}

	protected:
		virtual void SystemInit() override
		{
			commonInit(Rcmd1);
			commandList(Rcmd2red);
			commandList(Rcmd3);
			writecommand(ST7735::MADCTL);
			writedata(0xC0);

			inherited::SystemInit();
		}

	public:
		ArduinoEVIVEDisplay( BasicSPI &ASPI, const unsigned char * AFont ) :
			inherited( ASPI, AFont )
		{
			pinMode( 47, OUTPUT );
			pinMode( 48, OUTPUT );
			pinMode( 49, OUTPUT );

			colstart = 0;
			rowstart = 0;
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
