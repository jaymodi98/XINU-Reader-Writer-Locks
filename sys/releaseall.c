/*
 * releaseall.c
 *
 *  Created on: Nov 28, 2015
 *      Author: mns
 */

#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

#define INT_MIN     (-2147483647 - 1)

int releaseall(int numlocks, int ldes1, ...) {
	int ldesc, flag = 0;
	unsigned long *a = (unsigned long *) (&ldes1);
	while (numlocks-- && flag == 0) {
		ldesc = *a++;
		if (release(ldesc) == SYSERR)
			flag = 1;
	}
	resched();
	if (flag == 1)
		return (SYSERR);
	else
		return (OK);
}
;

int release(int ldesc) {
//
	STATWORD ps;
	int i, highPriorityWriter = -1;
	register struct lentry *lptr;

	disable(ps);
	if (isbadlock(ldesc) || (lptr = &locks[ldesc])->lstate == LFREE) {
		restore(ps);
		return SYSERR;
	}

	if (locktab[currpid][ldesc].type == NONE) {
		restore(ps);
		return SYSERR;
	}

	locktab[currpid][ldesc].type = NONE;
	locktab[currpid][ldesc].time = -1;

	if (nonempty(lptr->lqhead)) {

		int hpPid = q[lptr->lqtail].qprev;
		if (locktab[hpPid][ldesc].type == WRITE) { /* highest priority process is a writer */
			lptr->ltype = WRITE;
			locktab[hpPid][ldesc].type = WRITE;
			locktab[hpPid][ldesc].time = -1;
			ready(getfirst(lptr->lqhead), RESCHNO);

		} else { /* highest priority process is a reader */
			int hprio = INT_MIN;
			i = q[lptr->lqtail].qprev;

			/*find the highest priority writer*/
			while (i != lptr->lqhead) {
				if (locktab[i][ldesc].type == WRITE && q[i].qkey > hprio) {
					hprio = q[i].qkey;
					highPriorityWriter = i;
				}
				i = q[i].qprev;
			}

			/*No writer in the waiting queue*/
			if (highPriorityWriter == -1) {
				while (nonempty(lptr->lqhead)) {
					lptr->ltype = READ;
					locktab[hpPid][ldesc].type = READ;
					locktab[hpPid][ldesc].time = -1;
					ready(getfirst(lptr->lqhead), RESCHNO);
				}
			} else { /*There is a writer in the waiting queue */
				i = q[lptr->lqtail].qprev;
				if (hprio == lastkey(lptr->lqtail)
						&& locktab[highPriorityWriter][ldesc].time
								- locktab[i][ldesc].time >= 1) {
					lptr->ltype = WRITE;
					locktab[hpPid][ldesc].type = WRITE;
					locktab[hpPid][ldesc].time = -1;
					ready(getfirst(lptr->lqhead), RESCHNO);
				} else {
					while (q[lptr->lqtail].qprev != highPriorityWriter) {

						lptr->ltype = READ;
						locktab[hpPid][ldesc].type = READ;
						locktab[hpPid][ldesc].time = -1;
						ready(getfirst(lptr->lqhead), RESCHNO);
					}
				}
			}
		}

	} else {
		lptr->ltype = NONE;
	}

	restore(ps);
	return OK;
}
