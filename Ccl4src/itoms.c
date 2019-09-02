#ifndef ITOMS_C
#define ITOMS_C
short* itoms(short* Target, short a2)
{
	*Target = MSHORT(a2);
	//printf("itoms(a2 = x%02X) returns x%02X\n",a2,*(short*)Target);
	return Target;
}

#endif
