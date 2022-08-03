#include <stdio.h>
#include <shmem.h>

int main(){
	int my_pe, num_pe;

	shmem_init();
	num_pe = shmem_n_pes();
	my_pe = shmem_my_pe();

	int pe_source = 0;
	int pe_dest = 1;

	double source[10] = {1,2,3,4,5,6,7,8,9,10};
	static double dest[10];

	if (my_pe == pe_source){
		shmem_double_put(dest, source, 10, pe_dest);
	}
	shmem_barrier_all();
	if (my_pe == pe_dest){
		printf("%d: Data at dest is now ", my_pe);
		for (int i=0; i<10; i++)
			printf(" %f", dest[i]);
		printf("\n");
	}
	shmem_finalize();
	return 0;
}

