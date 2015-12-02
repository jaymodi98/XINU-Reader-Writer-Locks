/* user.c - main */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <stdio.h>

#define DEFAULT_LOCK_PRIO 20

#define assert(x,error) if(!(x)){ \
            kprintf(error);\
            return;\
            }
int mystrncmp(char* des, char* target, int n) {
	int i;
	for (i = 0; i < n; i++) {
		if (target[i] == '.')
			continue;
		if (des[i] != target[i])
			return 1;
	}
	return 0;

}

/*--------------------------------Test 0--------------------------------*/
void test0() {
	int lck;
	kprintf("\nTest 0: create rwlock.\n");
	lck = lcreate();
	assert(lck != SYSERR, "Test 0 FAILED\n");
	kprintf("Test 0 PASSED!\n");
	ldelete(lck);
}

/*--------------------------------Test 1--------------------------------*/

int testval = 1;
void reader1(char *msg, int lck) {
	lock(lck, READ, DEFAULT_LOCK_PRIO);
	testval = testval * 10;
	sleep(3);
	testval = testval - 5;
	releaseall(1, lck);
}

void test1() {
	int lck;
	int pid1;
	int pid2;

	kprintf("\nTest 1: readers can share the rwlock.\n");
	testval = 1;
	lck = lcreate();
	assert(lck != SYSERR, "Test 1 FAILED\n");

	pid1 = create(reader1, 2000, 20, "reader a", 2, "reader a", lck);
	pid2 = create(reader1, 2000, 20, "reader b", 2, "reader b", lck);

	resume(pid1);
	sleep(1);
	resume(pid2);

	sleep(5);
	ldelete(lck);
	kill(pid1);
	kill(pid2);

	assert(testval == 90, "Test 1 FAILED\n");
	kprintf("Test 1 PASSED!\n");
}

/*------------------------------------------------------------------------
 *  main  --  user main program
 *------------------------------------------------------------------------
 */
int main() {
	test0();
	test1();
	return 0;
}
