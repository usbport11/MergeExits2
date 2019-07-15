#include "Random.h"

unsigned int RandomChance(unsigned int n) {
	//chance = (100 / n) %
	if(n <= 1) return 1;
	return (rand() % n > n - 2);
}

unsigned int RandomChancePercent(unsigned int n) {
	if(n > 100) return 1;
	if(!n) return 0;
	return (rand() % 100 < n);
}

int RandomValue(int Min, int Max) {
	if(Min > Max) return 0;
	if(Min == Max) return Min;
	return rand() % (Max - Min + 1) + Min;
}

int RamdomFromCount2(int Count, int& n1, int& n2) {
	if(Count < 3) return 0;
	
	int halfCount = Count / 2;
	n1 = rand() % 5; //why 5?
	if(n1 > halfCount) n2 = rand() % n1;
	else n2 = rand() % (Count - n1) + n1;
	if(n1 == n2) {
		switch(rand() % 2) {
			case 0:
				n1 = n2 + 1;
				break;
			case 1:
				n2 = n1 + 1;
				break;
		}
	}
	
	return abs(n1 - n2);
}
