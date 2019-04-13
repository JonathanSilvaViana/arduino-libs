////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_LOGIC_GATES_h
#define _MITOV_LOGIC_GATES_h

#include <Mitov.h>

namespace Mitov
{
	template<int C_NUM_INPUTS> class BooleanOr : public Mitov::BasicCommonMultiInput<bool,bool, C_NUM_INPUTS>
	{
		typedef Mitov::BasicCommonMultiInput<bool,bool, C_NUM_INPUTS> inherited;

	protected:
		virtual bool CalculateOutput() override
		{
			bool AValue = false;
			for ( int i = 0; i < C_NUM_INPUTS; ++i )
				AValue |= inherited::InputPins[ i ].Value;

			return AValue;
		}

	};
//---------------------------------------------------------------------------
	template<int C_NUM_INPUTS> class BooleanAnd : public Mitov::BasicCommonMultiInput<bool,bool, C_NUM_INPUTS>
	{
		typedef Mitov::BasicCommonMultiInput<bool,bool, C_NUM_INPUTS> inherited;

	protected:
		virtual bool CalculateOutput() override
		{
			bool AValue = (C_NUM_INPUTS > 0);
			for ( int i = 0; i < C_NUM_INPUTS; ++i )
				AValue &= inherited::InputPins[ i ].Value;

			return AValue;
		}

	};
//---------------------------------------------------------------------------
	template<int C_NUM_INPUTS> class BooleanXor : public Mitov::BasicCommonMultiInput<bool,bool, C_NUM_INPUTS>
	{
		typedef Mitov::BasicCommonMultiInput<bool,bool, C_NUM_INPUTS> inherited;

	protected:
		virtual bool CalculateOutput() override
		{
			bool AValue = false;
			for ( int i = 0; i < C_NUM_INPUTS; ++i )
				AValue ^= inherited::InputPins[ i ].Value;

			return AValue;
		}

	};
//---------------------------------------------------------------------------
	class BooleanInverter : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;

	protected:
		void DoReceive( void *_Data )
        {
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

            bool AOutValue = ! ( *(bool*)_Data );

			OutputPin.Notify( &AOutValue );
        }

	public:
		BooleanInverter()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&BooleanInverter::DoReceive );
		}
	};
//---------------------------------------------------------------------------

}

#endif
