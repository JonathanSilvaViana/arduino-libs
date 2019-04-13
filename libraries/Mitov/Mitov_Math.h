////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_MATH_h
#define _MITOV_MATH_h

#include <Mitov.h>

namespace Mitov
{
#define Min Min
#define Max Max
#define Abs Abs
//---------------------------------------------------------------------------
	#define MITOV_PI 3.14159265359
//---------------------------------------------------------------------------
    template<typename T> class AddValue : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;

    public:
        T   Value;

	protected:
		void DoReceive( void *_Data )
        {
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

			T AInValue = *(T*)_Data;

            T AOutValue = AInValue + Value;

			OutputPin.Notify( &AOutValue );
        }

    public:
        AddValue( T AValue = 0 ) : 
			Value( AValue )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&AddValue::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	template<typename T> class SubtractValue : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;

    public:
        T   Value;

	protected:
		void DoReceive( void *_Data )
        {
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

            T AOutValue = *(T*)_Data - Value;

			OutputPin.Notify( &AOutValue );
        }

    public:
		SubtractValue( T AValue = 1 ) : 
			Value( AValue )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&SubtractValue::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	template<typename T> class SubtractFromValue : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;

    public:
        T   Value;

	protected:
		void DoReceive( void *_Data )
        {
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

            T AOutValue = Value - *(T*)_Data;

			OutputPin.Notify( &AOutValue );
        }

    public:
		SubtractFromValue( T AValue = 1 ) : 
			Value( AValue )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&SubtractFromValue::DoReceive );
		}

	};
//---------------------------------------------------------------------------
    template<typename T> class MultiplyByValue : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;

    public:
        T   Value;

	protected:
		void DoReceive( void *_Data )
        {
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

            T AOutValue = ( *(T*)_Data ) * Value;

			OutputPin.Notify( &AOutValue );
        }

    public:
		MultiplyByValue( T AValue = 1 ) : 
			Value( AValue )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&MultiplyByValue::DoReceive );
		}

	};
//---------------------------------------------------------------------------
    template<typename T> class DivideByValue : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;

    public:
        T   Value;

	protected:
		void DoReceive( void *_Data )
        {
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

            T AOutValue = ( *(T*)_Data ) / Value;

			OutputPin.Notify( &AOutValue );
        }

    public:
		DivideByValue( T AValue = 1 ) : 
			Value( AValue )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&DivideByValue::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	template<typename T> class DivideValue : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;

    public:
        T   Value;

	protected:
		void DoReceive( void *_Data )
        {
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

            T AOutValue = Value / ( *(T*)_Data );

			OutputPin.Notify( &AOutValue );
        }

    public:
		DivideValue( T AValue = 0 ) : 
			Value( AValue )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&DivideValue::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	template<typename T> class Inverse : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

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

            T AOutValue = -( *(T*)_Data );

			OutputPin.Notify( &AOutValue );
        }

	public:
		Inverse()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&Inverse::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	template<typename T> class Abs : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

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

            T AOutValue = abs( *(T*)_Data );

			OutputPin.Notify( &AOutValue );
        }

	public:
		Abs()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&Abs::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	template<typename T> class MinLimit : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        T   Value;

    public:
        bool Enabled = true;

	protected:
        inline T FilterValue( float AValue )
        {
			return ( AValue < Value ) ? Value : AValue;
        }

	protected:
		void DoReceive( void *_Data )
        {
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

			T AInValue = *(T*)_Data;

            T AOutValue = FilterValue( AInValue );

			OutputPin.Notify( &AOutValue );
        }

    public:
        MinLimit( T AValue = 0 ) : 
			Value( AValue )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&MinLimit::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	template<typename T> class MaxLimit : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        T   Value;

    public:
        bool Enabled = true;

	protected:
        inline T FilterValue( float AValue )
        {
			return ( AValue > Value ) ? Value : AValue;
        }

	protected:
		void DoReceive( void *_Data )
        {
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

			T AInValue = *(T*)_Data;

            T AOutValue = FilterValue( AInValue );

			OutputPin.Notify( &AOutValue );
        }

    public:
        MaxLimit( T AValue = 0 ) : 
			Value( AValue )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&MaxLimit::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	template<typename T> class Limit : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool	Enabled = true;
		T	Min = 0.0;
		T	Max = 1.0;

	protected:
        float FilterValue( float AValue )
        {
			return ( AValue > Max ) ? 
					Max : 
					( AValue < Min ) ? Min : AValue;
        }

	protected:
		void DoReceive( void *_Data )
        {
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

			float AInValue = *(float*)_Data;

            float AOutValue = FilterValue( AInValue );

			OutputPin.Notify( &AOutValue );
        }

	public:
		Limit( T AMin, T AMax ) :
			Min( AMin ),
			Max( AMax )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&Limit::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	enum CompareValueType { ctEqual, ctNotEqual, ctBigger, ctSmaller, ctBiggerOrEqual, ctSmallerOrEqual };
//---------------------------------------------------------------------------
	template<typename T> class CompareValue : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        T	Value;

	protected:
		T	FCurrentValue;

    public:
		CompareValueType	CompareType : 3;
        bool				Enabled : 1;
		bool				OnlyChanged : 1;
		bool				IgnoreCase : 1; // Added in root to save space!

	protected:
		bool	FOutputValue : 1;
		bool	FStarted : 1;
		bool	FProcessedOnce : 1;

    public:
		void SetValue( T AValue )
		{
			if( OnlyChanged )
				if( Value == AValue )
					return;

			Value = AValue;
			ProcessOutput();
		}

	protected:
		bool PerformCompare( T ACurrentValue, T AValue )
		{
			switch( CompareType )
			{
			default:
			case ctEqual:
				return ( ACurrentValue == AValue );

			case ctNotEqual:
				return ( ACurrentValue != AValue );

			case ctBigger:
				return ( ACurrentValue > AValue );

			case ctSmaller:
				return ( ACurrentValue < AValue );

			case ctBiggerOrEqual:
				return ( ACurrentValue >= AValue );

			case ctSmallerOrEqual:
				return ( ACurrentValue <= AValue );

			}
		}

	protected:
		void ProcessOutput()
		{
			if( ! Enabled )
				return;

//			Serial.print( FCurrentValue ); Serial.print( " ? " ); Serial.println( Value );

			bool ABoolValue = PerformCompare( FCurrentValue, Value );

			if( ! OnlyChanged )
				if( FOutputValue )
				{
					FOutputValue = false;
					OutputPin.SendValue( false );
				}

			if( ( !FStarted ) || ( FOutputValue != ABoolValue ) )
			{
				OutputPin.Notify( &ABoolValue );
				FOutputValue = ABoolValue;
				FStarted = true;
			}
		}

	protected:
		void DoReceive( void *_Data )
		{
			T AValue = *(T*)_Data;

			if( OnlyChanged )
				if( FProcessedOnce )
					if( AValue == FCurrentValue )
						return;

			FCurrentValue = AValue;
			FProcessedOnce = true;

			ProcessOutput();
		}

	public:
		void SystemStart()
		{
//			inherited::SystemStart();
			if( ! FStarted )
				OutputPin.SendValue<bool>( false );

		}

	public:
		CompareValue( T AInitialValue ) : 
			Value( AInitialValue ),
			FCurrentValue( AInitialValue ),
			CompareType( ctEqual ),
			Enabled( true ),
			OnlyChanged( true ),
			IgnoreCase( false ),
			FOutputValue( false ),
			FStarted( false ),
			FProcessedOnce( false )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&CompareValue::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	template<> bool CompareValue<char>::PerformCompare( char ACurrentValue, char AValue )
	{
		if( IgnoreCase )
		{
			ACurrentValue = toupper( ACurrentValue );
			AValue = toupper( AValue );
		}

		switch( CompareType )
		{
			default:
			case ctEqual:
				return ( ACurrentValue == AValue );

			case ctNotEqual:
				return ( ACurrentValue != AValue );

			case ctBigger:
				return ( ACurrentValue > AValue );

			case ctSmaller:
				return ( ACurrentValue < AValue );

			case ctBiggerOrEqual:
				return ( ACurrentValue >= AValue );

			case ctSmallerOrEqual:
				return ( ACurrentValue <= AValue );

		}
	}
//---------------------------------------------------------------------------
	template<> bool CompareValue<String>::PerformCompare( String ACurrentValue, String AValue )
	{
		if( IgnoreCase )
		{
			ACurrentValue.toUpperCase();
			AValue.toUpperCase();
		}

		switch( CompareType )
		{
			default:
			case ctEqual:
				return ( ACurrentValue == AValue );

			case ctNotEqual:
				return ( ACurrentValue != AValue );

			case ctBigger:
				return ( ACurrentValue > AValue );

			case ctSmaller:
				return ( ACurrentValue < AValue );

			case ctBiggerOrEqual:
				return ( ACurrentValue >= AValue );

			case ctSmallerOrEqual:
				return ( ACurrentValue <= AValue );

		}
	}
//---------------------------------------------------------------------------
	template<> void CompareValue<String>::DoReceive( void *_Data )
	{
		String AValue = String( (char*)_Data );

		if( OnlyChanged )
			if( FProcessedOnce )
				if( AValue == FCurrentValue )
					return;

		FCurrentValue = AValue;
		FProcessedOnce = true;

		ProcessOutput();
	}
//---------------------------------------------------------------------------
	class CompareDateTimeValue : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        Mitov::TDateTime	Value;

	protected:
		Mitov::TDateTime	FCurrentValue;

    public:
		TDateTimeCompareItems	Items;

    public:
		CompareValueType	CompareType : 3;
        bool				Enabled : 1;
		bool				OnlyChanged : 1;

	protected:
		bool	FOutputValue : 1;
		bool	FStarted : 1;
		bool	FProcessedOnce : 1;

    public:
		void SetValue( Mitov::TDateTime AValue )
		{
			if( OnlyChanged )
				if( Value == AValue )
					return;

			Value = AValue;
			ProcessOutput();
		}

	protected:
		bool PerformCompare( Mitov::TDateTime ACurrentValue, Mitov::TDateTime AValue )
		{
			switch( CompareType )
			{
			default:
			case ctEqual:
				return ( ACurrentValue.IsEqual( AValue, Items ) );

			case ctNotEqual:
				return ( ! ACurrentValue.IsEqual( AValue, Items ) );

			case ctBigger:
				return ( ACurrentValue.IsBigger( AValue, Items ) );

			case ctSmaller:
				return ( AValue.IsBigger( ACurrentValue, Items ) );

			case ctBiggerOrEqual:
				return ( ACurrentValue.IsBiggerOrEqual( AValue, Items ) );

			case ctSmallerOrEqual:
				return ( AValue.IsBiggerOrEqual( ACurrentValue, Items ) );

			}
		}

	protected:
		void ProcessOutput()
		{
			if( ! Enabled )
				return;

//			Serial.print( FCurrentValue ); Serial.print( " ? " ); Serial.println( Value );

			bool ABoolValue = PerformCompare( FCurrentValue, Value );

			if( ! OnlyChanged )
				if( FOutputValue )
				{
					FOutputValue = false;
					OutputPin.SendValue( false );
				}

			if( ( !FStarted ) || ( FOutputValue != ABoolValue ) )
			{
				OutputPin.Notify( &ABoolValue );
				FOutputValue = ABoolValue;
				FStarted = true;
			}
		}

	protected:
		void DoReceive( void *_Data )
		{
			Mitov::TDateTime AValue = *(Mitov::TDateTime*)_Data;

			if( OnlyChanged )
				if( FProcessedOnce )
					if( AValue == FCurrentValue )
						return;

			FCurrentValue = AValue;
			FProcessedOnce = true;

			ProcessOutput();
		}

	protected:
		virtual void SystemStart() override
		{
//			inherited::SystemStart();
			if( ! FStarted )
				OutputPin.SendValue<bool>( false );

		}

	public:
		CompareDateTimeValue() : 
			CompareType( ctEqual ),
			Enabled( true ),
			OnlyChanged( true ),
			FOutputValue( false ),
			FStarted( false ),
			FProcessedOnce( false )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&CompareDateTimeValue::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	template<typename T> class CompareRange : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
		class TValueRange
		{
		public:
			T	Min;
			T	Max;

		public:
			TValueRange( T AMin, T AMax ) :
				Min( AMin ),
				Max( AMax )
			{
			}
		};

    public:
        TValueRange	Range;

	protected:
		T	FCurrentValue;

    public:
        bool	Enabled : 1;
		bool	IsOutsideRange : 1;
		bool	IncludeLimits : 1;
		bool	OnlyChanged : 1;
		bool	IgnoreCase : 1; // Added in root to save space!

	protected:
		bool	FOutputValue : 1;
		bool	FStarted : 1;
		bool	FProcessedOnce : 1;

    public:
		void ProcessOutput()
		{
			if( ! Enabled )
				return;

			bool ABoolValue = PerformCompare( FCurrentValue );

			if( ! OnlyChanged )
				if( FOutputValue )
				{
					FOutputValue = false;
					OutputPin.SendValue( false );
				}

			if( ( !FStarted ) || ( FOutputValue != ABoolValue ) )
			{
				OutputPin.Notify( &ABoolValue );
				FOutputValue = ABoolValue;
				FStarted = true;
			}
		}

	protected:
		bool PerformCompare( T ACurrentValue )
		{
			if( IsOutsideRange )
			{
				if( IncludeLimits )
				{
					if( ACurrentValue >= Range.Max )
						return true;

					return ( ACurrentValue <= Range.Min );
				}

				if( ACurrentValue > Range.Max )
					return true;

				return ( ACurrentValue < Range.Min );
			}

			if( IncludeLimits )
			{
				if( ACurrentValue >= Range.Max )
					return false;

				return ! ( ACurrentValue <= Range.Min );
			}

			if( ACurrentValue > Range.Max )
				return false;

			return ! ( ACurrentValue < Range.Min );
		}

	protected:
		void DoReceive( void *_Data )
		{
			T AValue = *(T*)_Data;

			if( OnlyChanged )
				if( FProcessedOnce )
					if( AValue == FCurrentValue )
						return;

			FCurrentValue = AValue;
			FProcessedOnce = true;

			ProcessOutput();
		}

	protected:
		virtual void SystemStart() override
		{
//			inherited::SystemStart();
			if( ! FStarted )
				OutputPin.SendValue<bool>( false );

		}

	public:
		CompareRange( T AMin, T AMax ) : 
			Range( AMin, AMax ),
			FCurrentValue( 0 ),
			Enabled( true ),
			IsOutsideRange( false ),
			IncludeLimits( false ),
			OnlyChanged( true ),
			IgnoreCase( false ),
			FOutputValue( false ),
			FStarted( false ),
			FProcessedOnce( false )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&CompareRange::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	template<> bool CompareRange<char>::PerformCompare( char ACurrentValue )
	{
		char AMinValue = Range.Min;
		char AMaxValue = Range.Max;
		if( IgnoreCase )
		{
			ACurrentValue = toupper( ACurrentValue );
			AMinValue = toupper( AMinValue );
			AMaxValue = toupper( AMaxValue );
		}

		if( IsOutsideRange )
		{
			if( IncludeLimits )
			{
				if( ACurrentValue >= AMaxValue )
					return true;

				return ( ACurrentValue <= AMinValue );
			}

			if( ACurrentValue > AMaxValue )
				return true;

			return ( ACurrentValue < AMinValue );
		}

		if( IncludeLimits )
		{
			if( ACurrentValue >= AMaxValue )
				return false;

			return ! ( ACurrentValue <= AMinValue );
		}

		if( ACurrentValue > AMaxValue )
			return false;

		return ! ( ACurrentValue < AMinValue );
	}
//---------------------------------------------------------------------------
	class CompareDateTimeRange : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
		class TValueRange
		{
		public:
			Mitov::TDateTime	Min;
			Mitov::TDateTime	Max = 100.0;

		};

    public:
        TValueRange	Range;

    public:
		TDateTimeCompareItems	Items;

	protected:
		Mitov::TDateTime	FCurrentValue;

    public:
        bool	Enabled : 1;
		bool	IsOutsideRange : 1;
		bool	IncludeLimits : 1;
		bool	OnlyChanged : 1;
		bool	IgnoreCase : 1; // Added in root to save space!

	protected:
		bool	FOutputValue : 1;
		bool	FStarted : 1;
		bool	FProcessedOnce : 1;

    public:
		void ProcessOutput()
		{
			if( ! Enabled )
				return;

			bool ABoolValue = PerformCompare( FCurrentValue );

			if( ! OnlyChanged )
				if( FOutputValue )
				{
					FOutputValue = false;
					OutputPin.SendValue( false );
				}

			if( ( !FStarted ) || ( FOutputValue != ABoolValue ) )
			{
				OutputPin.Notify( &ABoolValue );
				FOutputValue = ABoolValue;
				FStarted = true;
			}
		}

	protected:
		bool PerformCompare( Mitov::TDateTime ACurrentValue )
		{
			if( IsOutsideRange )
			{
				if( IncludeLimits )
				{
					if( ACurrentValue.IsBiggerOrEqual( Range.Max, Items ) )
						return true;

					return (  Range.Min.IsBigger( ACurrentValue, Items ) );
				}

				if( ACurrentValue.IsBigger( Range.Max, Items ) )
					return true;

				return ( Range.Min.IsBiggerOrEqual( ACurrentValue, Items )  );
			}

			if( IncludeLimits )
			{
				if( ACurrentValue.IsBiggerOrEqual( Range.Max, Items ) )
					return false;

				return ! ( Range.Min.IsBigger( ACurrentValue, Items ) );
			}

			if( ACurrentValue.IsBigger( Range.Max, Items ) )
				return false;

			return ! ( Range.Min.IsBiggerOrEqual( ACurrentValue, Items ) );
		}

	protected:
		void DoReceive( void *_Data )
		{
			Mitov::TDateTime AValue = *(Mitov::TDateTime*)_Data;

			if( OnlyChanged )
				if( FProcessedOnce )
					if( AValue == FCurrentValue )
						return;

			FCurrentValue = AValue;
			FProcessedOnce = true;

			ProcessOutput();
		}

	protected:
		virtual void SystemStart() override
		{
//			inherited::SystemStart();
			if( ! FStarted )
				OutputPin.SendValue<bool>( false );

		}

	public:
		CompareDateTimeRange() : 
			Enabled( true ),
			IsOutsideRange( false ),
			IncludeLimits( false ),
			OnlyChanged( true ),
			IgnoreCase( false ),
			FOutputValue( false ),
			FStarted( false ),
			FProcessedOnce( false )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&CompareDateTimeRange::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	class AveragePeriod : public OpenWire::Component
	{
	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

	public:
		unsigned long	Period = 1000;

	protected:
		float FSum = 0.0f;
		unsigned long	FCount = 0;
		unsigned long	FLastTime = 0;

	protected:
		void DoReceive( void *_Data )
		{
			FSum += *(float *)_Data;
			++FCount;
		}

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( currentMicros - FLastTime < Period )
				return;

			if( ! FCount )
				return;

			float AValue = FSum / FCount;

			FSum = 0.0;
			FCount = 0;
			FLastTime = currentMicros;

			OutputPin.Notify( &AValue );
		}

	public:
		AveragePeriod()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&AveragePeriod::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	template<typename T, int C_NUM_INPUTS> class CommonMathMultiInput : public CommonClockedMultiInput<T, C_NUM_INPUTS>
	{
		typedef CommonClockedMultiInput<T, C_NUM_INPUTS> inherited;

    public:
        bool Enabled = true;

	};
//---------------------------------------------------------------------------
	template<typename T, int C_NUM_INPUTS> class Add : public CommonMathMultiInput<T, C_NUM_INPUTS>
	{
		typedef CommonMathMultiInput<T, C_NUM_INPUTS> inherited;

	protected:
		virtual T CalculateOutput() override
		{
			T AValue = 0;
			for( int i = 0; i < C_NUM_INPUTS; ++i )
				AValue += inherited::InputPins[ i ].Value;

			return AValue;
		}
	};
//---------------------------------------------------------------------------
	template<typename T, int C_NUM_INPUTS> class Multiply : public CommonMathMultiInput<T, C_NUM_INPUTS>
	{
		typedef CommonMathMultiInput<T, C_NUM_INPUTS> inherited;

	protected:
		virtual T CalculateOutput() override
		{
			T AValue = 1;
			for( int i = 0; i < C_NUM_INPUTS; ++i )
				AValue *= inherited::InputPins[ i ].Value;

			return AValue;
		}
	};
//---------------------------------------------------------------------------
	class ValueRange
	{
	public:
		float Min = 0.0f;
		float Max = 1.0f;

	};
//---------------------------------------------------------------------------
	class MapRange : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;

		ValueRange	InputRange;
		ValueRange	OutputRange;

	protected:
		void DoReceive( void *_Data )
		{
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

			float AValue = *(float*)_Data;

			float AOutValue = Func::MapRange( AValue, InputRange.Min, InputRange.Max, OutputRange.Min, OutputRange.Max );

			OutputPin.Notify( &AOutValue );
		}

	public:
		MapRange()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&MapRange::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	class AnalogDeadZone : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;

		float Value = 0.5f;
		float Span = 0.1f;

	protected:
		void DoReceive( void *_Data )
		{
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

			float AValue = *(float*)_Data;

			float AFalfSpan = Span / 2;

			if( AValue > Value + AFalfSpan )
				AValue -= AFalfSpan;

			else if( AValue < Value - AFalfSpan )
				AValue += AFalfSpan;

			else
				AValue = Value;

			OutputPin.Notify( &AValue );
		}

	protected:
		virtual void SystemStart() override
		{
			OutputPin.Notify( &Value );
		}

	public:
		AnalogDeadZone()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&AnalogDeadZone::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	class AnalogDeadZoneScaled : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;

		ValueRange	InputRange;
		ValueRange	OutputRange;
		float Value = 0.5f;
		float Span = 0.1f;

	protected:
		void DoReceive( void *_Data )
		{
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

			float AValue = *(float*)_Data;

			float AFalfSpan = Span / 2;

			if( AValue > Value + AFalfSpan )
				AValue -= AFalfSpan;

			else if( AValue < Value - AFalfSpan )
				AValue += AFalfSpan;

			else
				AValue = Value;

			float AOutValue = Func::MapRange( AValue, InputRange.Min + AFalfSpan, InputRange.Max - AFalfSpan, OutputRange.Min, OutputRange.Max );

			OutputPin.Notify( &AOutValue );
		}

	protected:
		virtual void SystemStart() override
		{
//			inherited::SystemStart();
			float AFalfSpan = Span / 2;

			float AOutValue = Func::MapRange( Value, InputRange.Min + AFalfSpan, InputRange.Max - AFalfSpan, OutputRange.Min, OutputRange.Max );
			OutputPin.Notify( &AOutValue );
		}

	public:
		AnalogDeadZoneScaled()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&AnalogDeadZoneScaled::DoReceive );
		}
	};
//---------------------------------------------------------------------------
    template<typename T> class RaiseToPower : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        T   Value;

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

            T AOutValue = pow( *(T*)_Data, Value );

			OutputPin.Notify( &AOutValue );
        }

    public:
        RaiseToPower( T AValue = 1 ) : 
			Value( AValue )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&RaiseToPower::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	class Sine : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

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

            float AOutValue = sin( *(float*)_Data );

			OutputPin.Notify( &AOutValue );
        }

	public:
		Sine()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&Sine::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	class Cosine : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

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

            float AOutValue = cos( *(float*)_Data );

			OutputPin.Notify( &AOutValue );
        }

	public:
		Cosine()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&Cosine::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	class RadToDegrees : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

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

            float AOutValue = ( *(float*)_Data ) * 180 / MITOV_PI;

			OutputPin.Notify( &AOutValue );
        }

	public:
		RadToDegrees()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&RadToDegrees::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	class DegreesToRad : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

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

            float AOutValue = ( *(float*)_Data ) * MITOV_PI / 180;

			OutputPin.Notify( &AOutValue );
        }

	public:
		DegreesToRad()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&DegreesToRad::DoReceive );
		}

	};
//---------------------------------------------------------------------------
    class AndUnsignedValue : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        uint32_t   Value = 0;

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

            uint32_t AOutValue = ( *(uint32_t*)_Data ) & Value;

			OutputPin.Notify( &AOutValue );
        }

    public:
        AndUnsignedValue()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&AndUnsignedValue::DoReceive );
		}

	};
//---------------------------------------------------------------------------
    class OrUnsignedValue : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        uint32_t   Value = 0;

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

            uint32_t AOutValue = ( *(uint32_t*)_Data ) | Value;

			OutputPin.Notify( &AOutValue );
        }

    public:
        OrUnsignedValue() 
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&OrUnsignedValue::DoReceive );
		}

	};
//---------------------------------------------------------------------------
    class XorUnsignedValue : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        uint32_t   Value = 0;

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

            uint32_t AOutValue = ( *(uint32_t*)_Data ) ^ Value;

			OutputPin.Notify( &AOutValue );
        }

    public:
        XorUnsignedValue()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&XorUnsignedValue::DoReceive );
		}

	};
//---------------------------------------------------------------------------
    class NotUnsignedValue : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

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

            uint32_t AOutValue = ~( *(uint32_t*)_Data );

			OutputPin.Notify( &AOutValue );
        }

	public:
		NotUnsignedValue()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&NotUnsignedValue::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	template<typename T> class Accumulate : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled : 1;

	protected:
		bool			FHasTime : 1;
		unsigned long	FLastTime = 0;

	public:
		T	InitialValue = 0;

	public:
		void SetInitialValue( float AValue )
		{
			if( InitialValue == AValue )
				return;

			InitialValue = AValue;
			OutputPin.Notify( &InitialValue );
		}

	protected:
        T FilterValue( T AValue )
        {
			unsigned long ATime = micros();
			if( ! FHasTime )			
			{
				FHasTime = true;
				return InitialValue;
			}

			float dT = ( ATime - FLastTime ) / 1000000.0f;
			InitialValue += AValue * dT;
            return InitialValue;
        }

		void DoReceive( void *_Data )
        {
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

            T AOutValue = FilterValue( *(T*)_Data );

			OutputPin.Notify( &AOutValue );
        }

	public:
		Accumulate() :
			Enabled( true ),
			FHasTime( false )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&Accumulate::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	class AccumulateAngle : public OpenWire::Object
	{
	public:
		TAngleUnits	Units = auDegree;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled : 1;

	protected:
		bool			FHasTime : 1;
		unsigned long	FLastTime = 0;

	public:
		float	InitialValue = 0.0;

	public:
		void SetInitialValue( float AValue )
		{
			if( InitialValue == AValue )
				return;

			InitialValue = AValue;
			OutputPin.Notify( &InitialValue );
		}

	protected:
        float FilterValue( float AValue )
        {
			if( ! FHasTime )			
			{
				FHasTime = true;
				return InitialValue;
			}

			unsigned long ATime = micros();
			float dT = ( ATime - FLastTime ) / 1000000.0f;
			InitialValue += AValue * dT;
			switch( Units )
			{
				case auDegree:
					InitialValue = posmod( InitialValue, 360.0f );
					break;

				case auRadians:
					InitialValue = posmod( InitialValue, 2 * PI );
					break;

				case auNormalized:
					InitialValue = posmod( InitialValue, 1.0f );
					break;

			}

            return InitialValue;
        }

		void DoReceive( void *_Data )
        {
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

            float AOutValue = FilterValue( *(float*)_Data );

			OutputPin.Notify( &AOutValue );
        }

	public:
		AccumulateAngle() :
			Enabled( true ),
			FHasTime( false )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&AccumulateAngle::DoReceive );
		}

	};
//---------------------------------------------------------------------------
#undef Abs
#undef Min
#undef Max
}

#endif
