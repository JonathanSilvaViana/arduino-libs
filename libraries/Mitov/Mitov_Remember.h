////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_REMEMBER_h
#define _MITOV_REMEMBER_h

#include <Mitov.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	template<typename T_STORE, typename T_DATA> class Remember : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::ValueSimpleModifiedSinkPin<T_DATA>	InputPin;

		OpenWire::SinkPin	RememberInputPin;
#ifdef _MITOV_REMEMBER_RECALL_PIN_
		OpenWire::ConnectSinkPin	RecallInputPin;
#endif // _MITOV_REMEMBER_RECALL_PIN_

		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;

	protected:
		T_STORE FSavedData;

	protected:
		void DoReceiveRemember( void *_Data )
		{
			if( Enabled )
			{
				FSavedData = InputPin.Value;
#ifdef _MITOV_REMEMBER_RECALL_PIN_
				if( ! RecallInputPin.IsConnected() )
#endif // _MITOV_REMEMBER_RECALL_PIN_
					OutputPin.SendValue( FSavedData );
			}
		}

#ifdef _MITOV_REMEMBER_RECALL_PIN_
		void DoRecallSnapshot( void *_Data )
		{
			if( Enabled )
				OutputPin.SendValue( FSavedData );
		}
#endif // _MITOV_REMEMBER_RECALL_PIN_

	public:
		Remember( T_STORE AData ) : 
			FSavedData( AData )
		{
			RememberInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&Remember::DoReceiveRemember );
#ifdef _MITOV_REMEMBER_RECALL_PIN_
			RecallInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&Remember::DoRecallSnapshot );
#endif // _MITOV_REMEMBER_RECALL_PIN_
		}
	};
//---------------------------------------------------------------------------
}

#endif
