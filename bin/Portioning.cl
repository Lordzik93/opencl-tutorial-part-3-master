/*
Dzielenie macie¿y wynikowej na 4 ma³e macierze
2D
W ogólnoœci kod w C/ C++ typu:
int * tab = new int[Xsize * Ysize];
for (int y = 0; y < Ysize; y++) {
for (int x = 0; x < Xsize; x++) {
tab[x + y * Xsize] = coœtam;
}
}

Mo¿na zast¹piæ dwuwymiarowym wywo³aniem kernela o postaci:
int x = get_global_id(0);
int y = get_global_id(1);
int Xsize = get_global_size(0);
tab[x + y * Xsize] = coœtam;
*/