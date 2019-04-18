#! /bin/sh

NUMPROC=2
#MPIRUN=/usr/bin/mpirun
MPIRUN=/usr/lib64/mpich/bin/mpirun

${MPIRUN} -np ${NUMPROC} ./pacman
