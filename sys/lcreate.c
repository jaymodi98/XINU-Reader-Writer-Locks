/*
 * lcreate.c
 *
 *  Created on: Nov 28, 2015
 *      Author: mns
 */

#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

LOCAL int newlock();

/*------------------------------------------------------------------------
 * lcreate  --  create and initialize a lock, returning its descriptor
 *------------------------------------------------------------------------
 */
SYSCALL lcreate() {
	STATWORD ps;
	int lock;
	disable(ps);
	if ((lock = newlock()) == SYSERR) {
		restore(ps);
		return SYSERR;
	}
	restore(ps);
	return lock;
}

/*------------------------------------------------------------------------
 * newlock  --  allocate an unused lock and return its index
 *------------------------------------------------------------------------
 */
LOCAL int newlock() {
	int lock;
	int i;

	for (i = 0; i < NLOCKS; i++) {
		lock = nextlock--;
		if (nextlock < 0)
			nextlock = NLOCKS - 1;
		if (locks[lock].lstate == LFREE) {
			++refNum;
			locks[lock].lstate = LUSED;
			locks[lock].lrefNum = refNum;
			return (refNum);
		}
	}
	return (SYSERR);
}
