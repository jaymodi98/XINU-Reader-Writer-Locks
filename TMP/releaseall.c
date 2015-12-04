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

int isLockHeld(int ldesc);
void makeReady(struct lentry *lptr, int pid, int lockid, int type);

int releaseall(int numlocks, int ldes1, ...) {
	int ldesc, flag = 0;
	unsigned long *a = (unsigned long *) (&ldes1);
	while (numlocks-- && flag == 0) {
		ldesc = *a++;
		//kprintf("The lock to remove %d",ldesc);
		if (release(ldesc) == SYSERR)
			flag = 1;
	}
	resched();
	if (flag == 1)
		return (SYSERR);
	else
		return (OK);
}

int release(int ldesc) {
//
	STATWORD ps;
	int i, highPriorityWriter = -1, flag = 0;
	int lockid;
	register struct lentry *lptr;

	disable(ps);

	for (i = 0; i < NLOCKS; i++) {
		if (locks[i].lrefNum == ldesc) {
			lockid = i;
			flag = 1;
			break;
		}
	}

	if (flag == 0) {
		restore(ps);
		return SYSERR;
	}

	if (isbadlock(lockid) || (lptr = &locks[lockid])->lstate == LFREE) {
		restore(ps);
		return SYSERR;
	}

	if (locktab[currpid][lockid].type == NONE) {
		restore(ps);
		return SYSERR;
	}

	locktab[currpid][lockid].type = NONE;
	locktab[currpid][lockid].time = -1;

	if (nonempty(lptr->lqhead)) {
		int hpPid = q[lptr->lqtail].qprev;
		if (locktab[hpPid][lockid].type == WRITE) { /* highest priority process is a writer */

			if (isLockHeld(lockid) == 0) {
				makeReady(lptr, hpPid, lockid, WRITE);
			}
			//kprintf("The first process %s", proctab[hpPid].pname);

		} else { /* highest priority process is a reader */
			int hprio = INT_MIN;
			i = q[lptr->lqtail].qprev;

			/*find the highest priority writer*/
			while (i != lptr->lqhead) {
				if (locktab[i][lockid].type == WRITE && q[i].qkey > hprio) {
					hprio = q[i].qkey;
					highPriorityWriter = i;
				}
				i = q[i].qprev;
			}

			/*No writer in the waiting queue*/
			if (highPriorityWriter == -1) {
				while (nonempty(lptr->lqhead)) {
					makeReady(lptr, hpPid, lockid, READ);
				}
			} else { /*There is a writer in the waiting queue */
				i = q[lptr->lqtail].qprev;
				if (hprio == lastkey(lptr->lqtail)
						&& locktab[highPriorityWriter][lockid].time
								- locktab[i][lockid].time >= 1) {
					makeReady(lptr, hpPid, lockid, WRITE);
				} else {
					while (q[lptr->lqtail].qprev != highPriorityWriter) {
						makeReady(lptr, hpPid, lockid, WRITE);
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

int isLockHeld(int ldesc) {
	int i;
	for (i = 0; i < NPROC; i++) {
		if (locktab[i][ldesc].time == -1 && locktab[i][ldesc].type != NONE) {
			return 1;
		}
	}
	return 0;
}

void makeReady(struct lentry *lptr, int pid, int lockid, int type) {
	lptr->ltype = type;
	locktab[pid][lockid].type = type;
	locktab[pid][lockid].time = -1;
	ready(getlast(lptr->lqtail), RESCHNO);
}
