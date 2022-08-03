#include <stdio.h>
#include <shmem.h>

int main(){
	int my_pe, num_pe;

	shmem_init();
	num_pe = shmem_n_pes();
	my_pe = shmem_my_pe();

	printf("Hello from %d of %d\n", my_pe, num_pe);
	shmem_finalize();
	return 0;
}

