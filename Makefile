# MPI

MPICC=/usr/lib64/mpich/bin/mpic++
#MPICC=/usr/lib64/openmpi/bin/mpic++

pacman: pacman.c
	${MPICC} pacman.c -o pacman -lm

clean:
	rm mpitest
