////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ACCELEROMETER_h
#define _MITOV_ACCELEROMETER_h

#include <Mitov.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	class Memsic2125Accelerometer : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	XInputPin;
		OpenWire::SinkPin	YInputPin;

		OpenWire::SourcePin	XOutputPin;
		OpenWire::SourcePin	YOutputPin;

	public:
		bool	Enabled = true;

	protected:
		class DataSet : public OpenWire::Object
		{
		protected:
			OpenWire::SourcePin	*FOutputPin;

			unsigned long	FStartTime;
			bool			FOldValue;
			const Memsic2125Accelerometer	*FOwner;

		public:
			void DoReceive( void *_Data )
			{
				if( ! FOwner->Enabled )
					return;

				bool AValue = *(bool *)_Data;
				if( FOldValue == AValue )
					return;

				unsigned long ANow = micros();
				FOldValue = AValue;
				if( AValue )
				{
					FStartTime = ANow;
					return;
				}

				long APeriod = ANow - FStartTime;

				if( APeriod == 0 )
					APeriod = 1;

				float AAcceleration = (( APeriod / 10 ) - 500) * 8;

				FOutputPin->Notify( &AAcceleration );
			}

		public:
			DataSet( const Memsic2125Accelerometer *AOwner, OpenWire::SinkPin *AInputPin, OpenWire::SourcePin *AOutputPin ) :
				FOwner( AOwner ),
				FOutputPin( AOutputPin ),
				FOldValue( false ),
				FStartTime( 0 )
			{
				AInputPin->SetCallback( this, (OpenWire::TOnPinReceive)&DataSet::DoReceive );
			}
		};


	protected:
		DataSet	FXDataSet;
		DataSet	FYDataSet;

	public:
		Memsic2125Accelerometer() :
			FXDataSet( this, &XInputPin, &XOutputPin ),
			FYDataSet( this, &YInputPin, &YOutputPin )
		{
		}
	};
//---------------------------------------------------------------------------
	class AccelerometerADXL335 : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	protected:
		class ProcessingSourcePin : public OpenWire::SourcePin
		{
		public:
			void DoProcess(void *_Data)
			{
				// min 0.27 and max 0.4
				float AValue = *(float*)_Data;

				float AOutValue = Func::MapRange<float>( AValue, 0.27, 0.4, -1.0, 1.0 );

				Notify( &AOutValue );
			}
		};

	public:
		OpenWire::SinkPin	AnalogInputPins[ 3 ];
		ProcessingSourcePin	AccelerationOutputPins[ 3 ];

	public:
		AccelerometerADXL335()
		{
			for( int i = 0; i < 3; ++i )
				AnalogInputPins[ i ].SetCallback( AccelerationOutputPins + i, (OpenWire::TOnPinReceive)&ProcessingSourcePin::DoProcess );
		}
	};
//---------------------------------------------------------------------------
}

#endif
