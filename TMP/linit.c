/*
 * linit.c
 *
 *  Created on: Nov 28, 2015
 *      Author: mns
 */

#include <lock.h>
#include <kernel.h>
#include <q.h>
#include <proc.h>

struct lentry locks[NLOCKS];
struct lstat locktab[NPROC][NLOCKS];
int nextlock;
int refNum;

void linit() {

	struct lentry * lptr;
	int i, j;
	nextlock = NLOCKS - 1;
	refNum = -1;

	for (i = 0; i < NLOCKS; i++) { /* initialize semaphores */
		(lptr = &locks[i])->lstate = LFREE;
		lptr->lqtail = 1 + (lptr->lqhead = newqueue());
		lptr->ltype = NONE;
		lptr->lrefNum = -1;
	}

	for (i = 0; i < NPROC; i++) {
		for (j = 0; j < NLOCKS; j++) {
			locktab[i][j].time = -1;
			locktab[i][j].type = NONE;
		}
	}
}
