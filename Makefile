# MPI

# MPICC=/usr/lib64/mpich/bin/mpic++
MPICC=-L/usr/lib64/openmpi/bin/mpic++ -lm
CURSES = -lncurses
CC = g++

pacman: pacman.cpp
# 	${CC} ${MPICC} ${CURSES} pacman.cpp -o pacman -lm
	${CC} pacman.cpp ${CURSES} ${MPICC}

clean:
	rm pacman *.o *~
