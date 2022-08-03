#include <stdio.h>
#include <shmem.h>

int main(){
	int my_pe, num_pe;

	shmem_init();
	num_pe = shmem_n_pes();
	my_pe = shmem_my_pe();

	int pe_source = 0;

	static double source[10] = {1,2,3,4,5,6,7,8,9,10};
	double target[10];

	if (my_pe != 0){
		shmem_double_get(target, source, 10, pe_source);
	}
	printf("%d: Data at target[0] is %f\n", my_pe, target[5]);
	shmem_finalize();
	return 0;
}

