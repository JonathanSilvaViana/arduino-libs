////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ESP32_h
#define _MITOV_ESP32_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	template<bool T_INITIAL_VALUE> class ESP32_DAC_DigitalChannel : public ArduinoDigitalChannel<T_INITIAL_VALUE>
	{
		typedef ArduinoDigitalChannel<T_INITIAL_VALUE>	inherited;
		
	protected:
		virtual void PinDirectionsInit() override
        {
			if( inherited::FRawInput )
				return;

            if( inherited::FIsOutput )
			{
				if( inherited::FIsAnalog )
					pinMode( inherited::FPinNumber, ANALOG );

				else
					pinMode( inherited::FPinNumber, OUTPUT );
			}

            else
				pinMode( inherited::FPinNumber, inherited::FIsPullUp ? INPUT_PULLUP : ( inherited::FIsPullDown ? INPUT_PULLDOWN : INPUT ) );
        }

		virtual void DoAnalogReceive( void *_Data ) override
        {
            if( inherited::FIsOutput && inherited::FIsAnalog )
            {
                float AValue = *(float*)_Data;
                dacWrite( inherited::FPinNumber, ( AValue * 255 ) + 0.5 );
            }
        }

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
