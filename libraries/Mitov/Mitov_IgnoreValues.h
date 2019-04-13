////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_IGNORE_VALUES_h
#define _MITOV_IGNORE_VALUES_h

#include <Mitov.h>

namespace Mitov
{
	template<typename T> class IgnoreValues : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        T	Value;

    public:
        bool	Enabled : 1;
		bool	IgnoreCase : 1;
		bool	PassValuesBellow : 1;
		bool	PassEqualValues : 1;

	protected:
		void DoReceive( void *_Data )
		{
			if( ! Enabled )
			{
				OutputPin.Notify( _Data );
				return;
			}

			T AValue = *(T*)_Data;
			if( PassValuesBellow )
			{
				if( AValue < Value )
					OutputPin.Notify( _Data );
			}

			else
			{
				if( AValue > Value )
					OutputPin.Notify( _Data );
			}

			if( PassEqualValues )
				if( AValue == Value )
					OutputPin.Notify( _Data );
		}

	public:
		IgnoreValues() : 
			Enabled( true ),
			IgnoreCase( false ),
			PassValuesBellow( false ),
			PassEqualValues( false )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&IgnoreValues::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	template<> void IgnoreValues<char>::DoReceive( void *_Data )
	{
		if( ! Enabled )
		{
			OutputPin.Notify( _Data );
			return;
		}

		char AValue = *(char*)_Data;
		char ACurrentValue = Value;
		if( IgnoreCase )
		{
			ACurrentValue = toupper( ACurrentValue );
			AValue = toupper( AValue );
		}

		if( PassValuesBellow )
		{
			if( AValue < ACurrentValue )
				OutputPin.Notify( _Data );
		}

		else
		{
			if( AValue > ACurrentValue )
				OutputPin.Notify( _Data );
		}

		if( PassEqualValues )
			if( AValue == ACurrentValue )
				OutputPin.Notify( _Data );

	}
//---------------------------------------------------------------------------
	template<> void IgnoreValues<String>::DoReceive( void *_Data )
	{
		if( ! Enabled )
		{
			OutputPin.Notify( _Data );
			return;
		}

		String AValue = String( (char*)_Data );
		String ACurrentValue = Value;
		if( IgnoreCase )
		{
			ACurrentValue.toUpperCase();
			AValue.toUpperCase();
		}

		if( PassValuesBellow )
		{
			if( AValue < ACurrentValue )
				OutputPin.Notify( _Data );
		}

		else
		{
			if( AValue > ACurrentValue )
				OutputPin.Notify( _Data );
		}

		if( PassEqualValues )
			if( AValue == ACurrentValue )
				OutputPin.Notify( _Data );

	}
//---------------------------------------------------------------------------
	template<typename T> class IgnoreRanges;
//---------------------------------------------------------------------------
	template<typename T> class IgnoreRangeElement
	{
	public:
		float	Min = 0.0;
		float	Max = 1.0;
		bool	PassEqualMax : 1;
		bool	PassEqualMin : 1;
		bool	IgnoreCase : 1;

	public:
		bool Exclude( T &AValue )
		{
			if( PassEqualMin )
				if( AValue <= Min )
					return false;

			if( AValue < Min )
				return false;

			if( PassEqualMax )
				if( AValue >= Max )
					return false;

			if( AValue > Max )
				return false;

			return true;
		}

	public:
		IgnoreRangeElement( IgnoreRanges<T> &AOwner );

	};
//---------------------------------------------------------------------------
	template<> bool IgnoreRangeElement<char>::Exclude( char &AValue )
	{
		if( IgnoreCase )
			AValue = toupper( AValue );

		if( PassEqualMin )
			if( AValue <= Min )
				return false;

		if( AValue < Min )
			return false;

		if( PassEqualMax )
			if( AValue >= Max )
				return false;

		if( AValue > Max )
			return false;

		return true;
	}
//---------------------------------------------------------------------------
	template<typename T> class IgnoreRanges : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

#ifdef _MITOV_IGNORE_RANGES_IGNORE_PIN_
		OpenWire::SourcePin	IgnoredOutputPin;
#endif

    public:
        bool	Enabled = true;

	public:
		Mitov::SimpleList<IgnoreRangeElement<T> *>	FElements;

	protected:
		void DoReceive( void *_Data )
		{
			if( Enabled )
			{
				T AValue = *(T*)_Data;
				for( int i = 0; i < FElements.size(); ++i )
					if( FElements[ i ]->Exclude( AValue ) )
					{
#ifdef _MITOV_IGNORE_RANGES_IGNORE_PIN_
						IgnoredOutputPin.SendValue<bool>( true );
#endif
						return;
					}

			}

#ifdef _MITOV_IGNORE_RANGES_IGNORE_PIN_
			IgnoredOutputPin.SendValue<bool>( false );
#endif
			OutputPin.Notify( _Data );
		}

	public:
		IgnoreRanges()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&IgnoreRanges::DoReceive );
		}
	
	};
//---------------------------------------------------------------------------
	template<typename T> IgnoreRangeElement<T>::IgnoreRangeElement( IgnoreRanges<T> &AOwner ) :
		PassEqualMax( false ),
		PassEqualMin( false ),
		IgnoreCase( false )
	{
		AOwner.FElements.push_back( this );
	}
//---------------------------------------------------------------------------
}

#endif
