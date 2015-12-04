/*
 * lock.c
 *
 *  Created on: Nov 28, 2015
 *      Author: mns
 */

#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>
#include <sleep.h>

int lock(int ldesc, int type, int priority) {
	STATWORD ps;
	struct lentry *lptr;
	struct pentry *pptr;

	int shouldWait = 0,i,flag = 0;

	disable(ps);

	for(i=0;i<NLOCKS;i++){
		if(locks[i].lrefNum == ldesc){
			ldesc = i;
			flag = 1;
			break;
		}
	}

	if(flag == 0){
		restore (ps);
		return SYSERR;
	}

	if (isbadlock(ldesc) || (lptr = &locks[ldesc])->lstate == LFREE) {
		restore(ps);
		return SYSERR;
	}

	if ((pptr = &proctab[currpid])->pwaitret == DELETED) {
		pptr->pwaitret = OK;
		restore(ps);
		return SYSERR;
	}

	/* some process is holding the lock */
	if (lptr->ltype != NONE) {
		if (lptr->ltype == READ) { /* a reader is holding the lock */
			if (type == READ) { /* the current process is a reader */
				if (nonempty(lptr->lqhead)) {
					int hprio = lastkey(lptr->lqtail);
					if (hprio > priority) {
						shouldWait = 1;
					}
				}
			} else { /* the current process is a writer*/
				shouldWait = 1;
			}
		} else { /* a writer is holding the lock */
			shouldWait = 1;
		}
	} else { /* no process is holding the lock i.e it is created but not acquired */
		shouldWait = 0;
	}

	if (shouldWait) {

		//kprintf("%s is put in the queue",proctab[currpid].pname);
		(pptr = &proctab[currpid])->pstate = PRWAIT;
		insert(currpid, lptr->lqhead,priority);
		pptr->pwaitret = OK;
		locktab[currpid][ldesc].type = type;
		locktab[currpid][ldesc].time = clktime;
		resched();

		restore(ps);
		return pptr->pwaitret;
	} else {
		lptr->ltype = type;
		locktab[currpid][ldesc].type = type;
		locktab[currpid][ldesc].time = -1;

		restore(ps);
		return (OK);
	}

	restore(ps);
	return OK;
}

