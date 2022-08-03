#include <stdio.h>
#include <shmem.h>

int main(){
	static long pSync_odd[SHMEM_BARRIER_SYNC_SIZE];
	static long pSync_even[SHMEM_BARRIER_SYNC_SIZE];

	for (int i = 0; i < SHMEM_BARRIER_SYNC_SIZE; i++){
		pSync_odd[i] = SHMEM_SYNC_VALUE;
		pSync_even[i] = SHMEM_SYNC_VALUE;
	}

	shmem_init();
	int my_pe = shmem_my_pe();
	int num_pes = shmem_n_pes();

	static int num_of_scalpel_injuries;
	static int num_of_ice_cream_scoop_injuries;

	if (my_pe % 2 != 0){
		num_of_scalpel_injuries = my_pe - 1;

		shmem_barrier(1, 1, (num_pes/2), pSync_odd);
		int num_of_neighbor = shmem_int_g(&num_of_scalpel_injuries, (my_pe+2)%10);
		printf("%d: I personally had only %d scalpel injuries in my line of work whereas my odd neighbor had %d.\n", my_pe, num_of_scalpel_injuries, num_of_neighbor);
	}
	else if (my_pe != 0){
		num_of_ice_cream_scoop_injuries = my_pe;

		shmem_barrier(2, 1, (num_pes/2 - 1), pSync_even);
		int num_of_neighbor;
		if (my_pe == 2){
			num_of_neighbor = shmem_int_g(&num_of_ice_cream_scoop_injuries, 8);
		}
		else {
			num_of_neighbor = shmem_int_g(&num_of_ice_cream_scoop_injuries, my_pe - 2);
		}

		printf("%d: Yay! I am not the only one who hurts themselves with an ice cream scoop! My nieghbor has done it %d times.\n", my_pe, num_of_neighbor);
	}

	shmem_finalize();
	return 0;
}
