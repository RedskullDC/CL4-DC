#ifndef MSTOD_C
#define MSTOD_C

double mstod(double *a1)
{
	unsigned char *v1; // esi@1
	double v4; // fst7@1

	v1 = (unsigned char*)a1;
	v4 = (*(v1 + 7) | (*(v1 + 6) << 8));
	v4 += _addexp((*(v1 + 5) | (*(v1 + 4) << 8)), 16);
	v4 += _addexp((*(v1 + 3) | (*(v1 + 2) << 8)), 32);
	v4 += _addexp((*(v1 + 1) | (*v1 << 8)), 48);
	/*
	printf("mstod: \n");
	DumpBlock(v1,8);
	printf("mstod: returned \n");
	DumpBlock((char*)&v4,8);
	*/
	return v4;
}

#endif
