////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_TRIANGLE_GENERATOR_h
#define _MITOV_TRIANGLE_GENERATOR_h

#include <Mitov.h>
#include "Mitov_BasicGenerator.h"

namespace Mitov
{
//---------------------------------------------------------------------------
	class TriangleAnalogGenerator : public Mitov::BasicFrequencyGenerator<float>
	{
		typedef Mitov::BasicFrequencyGenerator<float> inherited;

	public:
		// -1 - 1
		float	Asymmetry = 0.0f;

	protected:
		virtual void CalculateValue() override
		{
			float AAssymetryPount = 0.5 + Asymmetry / 2;
			if( FPhase < AAssymetryPount )
				FValue = Offset - Amplitude + Amplitude * 2 * ( FPhase / AAssymetryPount );

			else
				FValue = Offset - Amplitude + Amplitude * 2 * ( 1 - (( FPhase - AAssymetryPount ) / ( 1 - AAssymetryPount )));

		}

	public:
		TriangleAnalogGenerator() :
			inherited( 0.5, 0.5 )
		{
		}

	};
//---------------------------------------------------------------------------
	class TriangleIntegerGenerator : public Mitov::BasicFrequencyGenerator<int32_t>
	{
		typedef Mitov::BasicFrequencyGenerator<int32_t> inherited;

	public:
		// -1 - 1
		float	Asymmetry = 0.0f;

	protected:
		virtual void CalculateValue() override
		{
			float AAssymetryPount = 0.5 + Asymmetry / 2;
			if( FPhase < AAssymetryPount )
				FValue = ( Offset - Amplitude + ((float)Amplitude) * 2 * ( FPhase / AAssymetryPount ) ) + 0.5;

			else
				FValue = ( Offset - Amplitude + ((float)Amplitude) * 2 * ( 1 - (( FPhase - AAssymetryPount ) / ( 1 - AAssymetryPount ))) ) + 0.5;

		}

	public:
		TriangleIntegerGenerator() :
			inherited( 1000, 0 )
		{
		}

	};
//---------------------------------------------------------------------------
	class TriangleUnsignedGenerator : public Mitov::BasicFrequencyGenerator<uint32_t>
	{
		typedef Mitov::BasicFrequencyGenerator<uint32_t> inherited;

	public:
		// -1 - 1
		float	Asymmetry = 0.0f;

	protected:
		virtual void CalculateValue() override
		{
			float AAssymetryPount = 0.5 + Asymmetry / 2;
			if( FPhase < AAssymetryPount )
				FValue = ( Offset - Amplitude + ((float)Amplitude) * 2 * ( FPhase / AAssymetryPount ) ) + 0.5;

			else
				FValue = ( Offset - Amplitude + ((float)Amplitude) * 2 * ( 1 - (( FPhase - AAssymetryPount ) / ( 1 - AAssymetryPount ))) ) + 0.5;

		}

	public:
		TriangleUnsignedGenerator() :
			inherited( 1000, 1000 )
		{
		}

	};
//---------------------------------------------------------------------------
}

#endif
