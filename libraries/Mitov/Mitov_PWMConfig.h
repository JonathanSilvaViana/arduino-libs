////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_PWM_CONFIG_h
#define _MITOV_PWM_CONFIG_h

#include <Mitov.h>

namespace Mitov
{
	class ArduinoPWMConfig : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		uint16_t	Frequency = 1000;

	public:
		void SetFrequency( uint32_t AValue )
		{
			if( AValue < 1 )
				AValue = 1;

			else if( AValue > 0xFFFF )
				AValue = 0xFFFF;

			if( AValue == Frequency )
				return;

			Frequency = AValue;
			analogWriteFreq( Frequency );
		}

	protected:
		virtual void SystemStart() override
		{
			analogWriteFreq( Frequency );
//			inherited::SystemStart();
		}

	};
}

#endif
