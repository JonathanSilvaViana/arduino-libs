////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_TEXAS_INSTUMENTS_THERMOMETER_h
#define _MITOV_TEXAS_INSTUMENTS_THERMOMETER_h

#include <Mitov.h>

namespace Mitov
{
	class TexasInstruments_Thermometer : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

	public:
		bool	InFahrenheit = false;

	protected:
		void DoReceive( void *_Data )
		{
			float AValue = (*(float *)_Data) * 500;

			if( InFahrenheit )
				AValue = AValue * ( 9.0/5.0 ) + 32.0;

			OutputPin.Notify( &AValue );
		}

	public:
		TexasInstruments_Thermometer()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&TexasInstruments_Thermometer::DoReceive );
		}
	};

}

#endif
