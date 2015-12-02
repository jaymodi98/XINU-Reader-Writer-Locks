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
	int i,j;

	for (i = 0; i < NLOCK; i++) {
		lock = nextlock--;
		if (nextlock < 0)
			nextlock = NLOCK - 1;
		if (locks[lock].lstate == LFREE) {
			locks[lock].lstate = LUSED;
			for(j = 0; j < NPROC; j++)
				locktab[j][lock] = NOTUSING;
			return (lock);
		}
	}
	return (SYSERR);
}
