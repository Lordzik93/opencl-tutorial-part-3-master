__kernel void Add(__global int* a, __global int* d, int size)
{
    
    int n = get_global_id(0);

    
    if (n < size )
    {   
		switch (n % 4){
		case 0: atomic_add(d, a[n]);
			break;
		case 1: atomic_add(d + 1, a[n]);
			break;
		case 2: atomic_add(d + 2, a[n]);
			break;
		case 3: atomic_add(d + 3, a[n]);
			break;
		}
		
    }
}
/*
__kernel void PartialAdd(__global int* a, __global int* b, int aSize) {
int bi = get_global_id(0);
int ai = bi * 3;
int br = 0;
for (int i = 0; i < 3; i++) {
if (ai + i < aSize) {
br += a[ai + i];
}
}
b[bi] = br;
}
gdzie:
a jest to tablica wynikowa,
b to tablica z danymi.
aSize wielko�� tablicy a,
br wynik cz�stkowy,
bi to indeks w tablicy wynikowej b
ai to pocz�tkowy indeks w tablicy �r�d�owej a
ai = bi * 3, poniewa� sumujemy tr�jki, wi�c w tablicy a zaczynamy od 3x dalszego miejsca ni� w b
a[ai + i] poniewa� sumujemy tr�jki, a wi�c efektywnie b�dzie to a[ai + 0], a[ai + 1] i a[ai + 2]

przerobi� na 2D
*/
