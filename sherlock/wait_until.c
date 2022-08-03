#include <stdio.h>
#include <shmem.h>

int main(){
	int my_pe, num_pe;

	shmem_init();
	num_pe = shmem_n_pes();
	my_pe = shmem_my_pe();

	static int new = 5;
	int old = new;

	if (my_pe == 0){
		shmem_int_wait_until(&new, SHMEM_CMP_NE, old);
		printf("new value: %d versus old value: %d\n", new, old);
	}
	if (my_pe == 1){
		int to_put = 10;
		shmem_int_p(&new, to_put, 0);
	}

	shmem_finalize();
	return 0;
}

