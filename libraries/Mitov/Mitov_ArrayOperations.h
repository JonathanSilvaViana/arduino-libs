////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ARRAY_OPERATIONS_h
#define _MITOV_ARRAY_OPERATIONS_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	template<typename T> class ValueArraySource : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::ConnectSinkPin	ClockInputPin;
		OpenWire::SourcePin			OutputPin;

	public:
		T *_Values;
		uint32_t _Count;

	protected:
		void SetValue()
		{
			TArray<T> ABuffer( _Count, _Values );
			OutputPin.Notify( &ABuffer );
		}

	protected:
		virtual void SystemStart() override
		{
//			inherited::SystemStart();
			SetValue();
		}

	protected:
		void DoClockReceive( void *_Data )
		{
			SetValue();
		}

	public:
		ValueArraySource()
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ValueArraySource::DoClockReceive );
		}
	};
//---------------------------------------------------------------------------
	template<typename T> class AddayDeleteItems : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

	public:
		uint8_t Index = 0;
		uint8_t Count = 1;

	protected:
		void DoReceive( void *_Data )
		{
			TArray<T> *AData = (TArray<T>*)_Data;
			if( Count == 0 || ( Index > AData->Size ))
			{
				OutputPin.Notify( _Data );
				return;
			}

			uint8_t ACount = MitovMin<int>( Count, int( AData->Size ) - int( Index ) );
			if( ACount == AData->Size )
				return;

			uint8_t AOutSize = AData->Size - ACount;
			T *AOutData = new T[ AOutSize ];
			for( int i = 0; i < Index; ++i )
				AOutData[ i ] = AData->Data[ i ];

			

			for( int i = 0; i < AOutSize; ++i )
				AOutData[ Index + i ] = AData->Data[ Index + ACount + i ];

			TArray<T> ABuffer( AOutSize, AOutData );
			OutputPin.Notify( &ABuffer );
			delete [] AOutData;
		}

	public:
		AddayDeleteItems()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&AddayDeleteItems::DoReceive );
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
