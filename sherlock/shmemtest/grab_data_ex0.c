#include <stdio.h>
#include <shmem.h>

int main(){
	int my_pe, num_pe;
	
	shmem_init();
	num_pe = shmem_n_pes();
	my_pe = shmem_my_pe();

	int pe_source = 0;

	static double source = 1;
	double target;

	if (my_pe != 0){
		target = shmem_double_g(&source, pe_source);
	}
	printf("%d: Data at target is %f\n", my_pe, target);
	shmem_finalize();
	return 0;
}
