# MPI

# MPICC=/usr/lib64/mpich/bin/mpic++
MPICC=/usr/lib64/openmpi/bin/mpic++
CURSES = -lncurses

pacman: pacman.cpp
	${MPICC} ${CURSES} pacman.cpp -o pacman -lm

clean:
	rm -f pacman core *~
