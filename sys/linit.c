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

struct lentry locks[NLOCK];
int nextlock;

void linit() {

	struct lentry * lptr;
	int i;
	nextlock = NLOCK - 1;

	for (i = 0; i < NLOCK; i++) { /* initialize semaphores */
		(lptr = &locks[i])->lstate = LFREE;
		lptr->lqtail = 1 + (lptr->lqhead = newqueue());
	}
}
