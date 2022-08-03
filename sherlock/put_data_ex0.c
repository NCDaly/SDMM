#include <stdio.h>
#include <shmem.h>

int main(){
	int my_pe, num_pe;

	shmem_init();
	num_pe = shmem_n_pes();
	my_pe = shmem_my_pe();

	int pe_source = 0;
	int pe_dest = 1;

	double source = 1;
	static double dest;

	if (my_pe == pe_source){
		shmem_double_p(&dest, source, pe_dest);
	}
	shmem_barrier_all();
	if (my_pe == pe_dest){
		printf("%d: Data at dest is now %f\n", my_pe, dest);
	}
	shmem_finalize();
	return 0;
}

