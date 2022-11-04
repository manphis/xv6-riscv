#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void recv_and_forward(int *pfd) {
	int prime = 0;
	int build_pipe = 0;
	int p[2];

	if (fork() == 0) {
		close(pfd[1]);
		int n;
		while (read(pfd[0], &n, sizeof(n)) != 0) {
			// printf("%d get %d\n", getpid(), n);
			if (prime == 0) {
				prime = n;
				printf("%d prime %d\n", getpid(), prime);
			} else if (n % prime != 0) {
				if (build_pipe == 0) {
					pipe(p);
					recv_and_forward(p);
					build_pipe = 1;
				}
				int value = n;
				write(p[1], &value, sizeof(value));
			}
		}
		close(pfd[0]);
		close(p[1]);
		wait(0);
		exit(0);
	} else {
		close(pfd[0]);
	}
}

int main(int argc, char *argv[]) {
	int i;
	int p[2];

	pipe(p);

	recv_and_forward(p);
	
	for (i = 2; i <= 35; i++) {
		int value = i;
		write(p[1], &value, sizeof(value));
	}
	close(p[1]);
	wait(0);
	exit(0);
}