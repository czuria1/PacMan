# MPI

# MPICC=/usr/lib64/mpich/bin/mpic++
MPICC=/usr/lib64/openmpi/bin/mpic++

pacman: pacman.cpp
	${MPICC} pacman.cpp -o pacman -lm

clean:
	rm pacman
