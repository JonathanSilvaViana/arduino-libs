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
	template<typename T_STORE, typename T_DATA> class Snapshot : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;
		OpenWire::SinkPin	SnapshotInputPin;

    public:
        bool Enabled = true;

	protected:
		T_STORE FData;

	protected:
		void DoReceive( void *_Data )
		{
			FData = *(T_DATA*)_Data;
		}

	protected:
		void TakeSnapshot()
		{
			OutputPin.SendValue( FData );
		}

	protected:
		void DoReceiveSnapshot( void *_Data )
		{
			if( Enabled )
				TakeSnapshot();
		}

	public:
		Snapshot( T_STORE AData ) : 
			FData( AData )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&Snapshot::DoReceive );
			SnapshotInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&Snapshot::DoReceiveSnapshot );
		}
	};
//---------------------------------------------------------------------------
}

#endif
