#ifndef CLROUND_C
#define CLROUND_C

#include "DBdefs.h"
#include "cl4.h"


// ****  can't handle '8' or '9' decimal places at the moment ****


double clround(double a1, char FLDtype)
{
	double v6; // fst7@5
	double v7; // fst6@5
	double v8; // fst5@5
	double v12; // t2@8
	double v13; // fst4@8
	double v17; // [sp+8h] [bp-20h]@1
	
	short	v9; // dx@5
	short	DecPlaces; // ax@4
	bool	Negative; // esi@1


	v17 = (double)a1;
	if ( a1 != 0.0 )
	{
		if ( a1 < 0.0 )
		{
			Negative = true;
			//BYTE7(v17) ^= 0x80u;		//flip high bit
			v17 = -v17;			// is same as?
		}
		else
			Negative = false;

		// only round floating point values. Var types: 0-9
		DecPlaces = typedp(FLDtype);
		//printf("clround : DecPlaces = %d\n",DecPlaces);
		if ( DecPlaces <= 9 )
		{
			v6 = (double)(long long)v17;// work out fractional part
			v7 = v17 - v6;	
			
			//printf("clround v7 = %f, v6 = %f\n",v7, v6);
			// work out dec place to round on:
			v8 = 100.0;
			v9 = DecPlaces - 1;
			if ( DecPlaces != 0 )
			{
				do
				{
					v8 *= 10.0;
					--v9;
				}
				while ( v9 != -1 );
			}
			//printf("clround v8 = %f\n",v8);

			// do actual rounding
			v12 = (v7 * v8) + 0.5000000001;
			//printf("clround v12a = %f\n",v12);
			v12 = (double)(long long)v12;
			//printf("clround v12b = %f\n",v12);
			v12 /= v8;
			//printf("clround v12c = %f\n",v12);
			v13 = v8 / 100.0;
			//printf("clround v13 = %f\n",v13);
			v12 = (double)(long long)(v12 * v13 + 0.5000000001);
			//printf("clround v12d = %f, v12/v13 = %f\n",v12, v12/v13);	// OK to here
			v17 = v6 + (v12 / v13);				// *** problem with this line ***
			//printf("clround v17 = %lf\n",v17);
		}
		
		return Negative ? -v17 : v17 ;
		//result = (double)v17;
		//if ( Negative )
		//	result = (double)-v17;
	}
	//printf("clround(%g,%c) returns %g\n",a1,FLDtype,result);

	return 0.0;

}

#endif
