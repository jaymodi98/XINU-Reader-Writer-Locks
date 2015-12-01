/*
 * lock.h
 *
 *  Created on: Nov 30, 2015
 *      Author: mns
 */

#ifndef _LOCK_H_
#define _LOCK_H_

#ifndef	NLOCK
#define	NLOCK		50	/* number of locks, if not defined	*/
#endif

#define	LFREE	'\01'		/* this lock is free		*/
#define	LUSED	'\02'		/* this lock is used		*/

struct lentry {
	char lstate; /* the state SFREE or SUSED		*/
	int lockcnt; /* count for this lock		*/
	int lqhead; /* q index of head of list		*/
	int lqtail; /* q index of tail of list		*/
};

extern	struct	lentry	locks[];
extern	int	nextlock;

#define	isbadlock(l)	(l<0 || l>=NLOCK)

#endif
