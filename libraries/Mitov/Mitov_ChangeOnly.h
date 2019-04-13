////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_SNAPSHOT_h
#define _MITOV_SNAPSHOT_h

#include <Mitov.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	template<typename T_STORE, typename T_DATA> class ChangeOnlyThreshold : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
		T_STORE Threshold;
        bool Enabled : 1;

	protected:
		bool FStarted : 1;
		T_STORE FData;

	protected:
		void DoReceive( void *_Data )
		{
			T_STORE AValue = *(T_DATA*)_Data;
			if( Enabled )
				if( FStarted )
//					if( FData == AValue )
					if( abs( FData - AValue ) <= Threshold )
						return;

			FStarted = true;
			FData = AValue;
			OutputPin.Notify( _Data );
		}

	public:
		ChangeOnlyThreshold( T_STORE AData ) : 
			Enabled( true ),
			FStarted( false ),
			FData( AData ),
			Threshold( AData )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ChangeOnlyThreshold::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	template<typename T_STORE, typename T_DATA> class ChangeOnly : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled : 1;

	protected:
		bool FStarted : 1;
		T_STORE FData;

	protected:
		void DoReceive( void *_Data )
		{
			T_STORE AValue = *(T_DATA*)_Data;
			if( Enabled )
				if( FStarted )
					if( FData == AValue )
						return;

			FStarted = true;
			FData = AValue;
			OutputPin.Notify( _Data );
		}

	public:
		ChangeOnly( T_STORE AData ) : 
			Enabled( true ),
			FStarted( false ),
			FData( AData )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ChangeOnly::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	template<> void ChangeOnly<String,char>::DoReceive( void *_Data )
	{
			String AValue = (char*)_Data;
			if( Enabled )
				if( FStarted )
					if( FData == AValue )
						return;

			FStarted = true;
			FData = AValue;
			OutputPin.Notify( _Data );
	}
//---------------------------------------------------------------------------
}

#endif
