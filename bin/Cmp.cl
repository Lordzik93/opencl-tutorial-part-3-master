__kernel void Cmp(__global int* a, __global int* b, __global int* c, int size)
{
	
	int n = get_global_id(0);

	if (n < size)
	{
		c[n] = a[n] == b[n] ? 1 : 0;
	}
}
//przerobiæ na 2D