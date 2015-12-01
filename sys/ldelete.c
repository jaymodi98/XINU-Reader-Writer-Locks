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

int ldelete(int ldesc){
	STATWORD ps;
	int pid;
	struct lentry *lptr;

	disable(ps);
	if(isbadlock(ldesc) || locks[ldesc].lstate == LFREE){
		restore(ps);
		return SYSERR;
	}

	lptr = &locks[ldesc];
	lptr->lstate = LFREE;

	if(nonempty(lptr->lqhead)){
		while((pid=getfirst(lptr->lqhead)) != EMPTY){
			proctab[pid].pwaitret = DELETED;
			ready(pid,RESCHNO);
		}
		resched();
	}

	restore(ps);
	return (OK);
}

