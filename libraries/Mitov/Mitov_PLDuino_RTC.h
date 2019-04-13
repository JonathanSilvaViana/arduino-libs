////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_PLDUINO_RTC_h
#define _MITOV_PLDUINO_RTC_h

#ifdef _MITOV_PLDUINO_RTC_MODULE_CLOCK_PIN_
	#define _MITOV_DS3231_RTC_CLOCK_PIN_
#endif

#ifdef _MITOV_PLDUINO_RTC_MODULE_SET_PIN_
	#define _MITOV_DS3231_RTC_SET_PIN_
#endif

#include <Mitov_RTC_DS3231.h>

namespace Mitov
{
	typedef RTC_DS3231 ArduinoPLDuinoRTC;
}

#endif
