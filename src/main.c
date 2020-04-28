#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "utils.h"
#include "scheduler.h"

int main() {
	// parsing the input
	char S[4];
	int N;
	scanf("%s\n%d", S, &N);
	
	char P[N][32];
	int R[N], T[N];
	for(size_t i = 0; i < N; ++i){
		scanf("%s %d %d", P[i], &R[i], &T[i]);
	}

	// Initialize arrays to record sorted order.
	int R_index[N], T_index[N], R_inverse[N], T_inverse[N];
	for(size_t i = 0; i < N; ++i)
		R_index[i] = T_index[i] = i;

    //arrange the execution order
    sort(R, R_index, N);
    sort(T, T_index, N);
    
    // record the order sequence
    inverse_permutation(R_index, R_inverse, N);
    inverse_permutation(T_index, T_inverse, N);
    
    scheduler(S, P, \
              R, R_index, R_inverse, \
              T, T_index, T_inverse, \
              N);
    
	exit(0);
}

