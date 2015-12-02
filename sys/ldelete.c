/*
 * ldelete.c
 *
 *  Created on: Nov 28, 2015
 *      Author: mns
 */

#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * ldelete  --  delete a lock by releasing its table entry
 *------------------------------------------------------------------------
 */

int ldelete(int ldesc) {
	STATWORD ps;
	int pid, i;
	struct lentry *lptr;

	disable(ps);
	if (isbadlock(ldesc) || locks[ldesc].lstate == LFREE) {
		restore(ps);
		return SYSERR;
	}

	lptr = &locks[ldesc];
	lptr->lstate = LFREE;
	lptr->ltype = NONE;

	if (nonempty(lptr->lqhead)) {
		while ((pid = getfirst(lptr->lqhead)) != EMPTY) {
			proctab[pid].pwaitret = DELETED;
			ready(pid, RESCHNO);
		}
		resched();
	}

	for (i = 0; i < NPROC; i++) {
		locktab[i][ldesc].time = -1;
		locktab[i][ldesc].type = NONE;

	}
	restore(ps);
	return (OK);
}

