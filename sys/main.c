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

/*----------------------------------Test 2---------------------------*/
void reader2(char *msg, int lck) {
	lock(lck, READ, DEFAULT_LOCK_PRIO);
	testval = testval * 10;
	sleep(3);
	testval = testval - 10;
	releaseall(1, lck);
}

void writer2(char *msg, int lck) {
	lock(lck, WRITE, DEFAULT_LOCK_PRIO);
	testval = testval * 10;
	sleep(3);
	testval = testval - 5;
	releaseall(1, lck);
}

void test2() {
	int lck;
	int pid1;
	int pid2;
	testval = 1;
	kprintf("\nTest 2: write lock is mutual exclusive\n");
	lck = lcreate();
	assert(lck != SYSERR, "Test 2 FAILED\n");

	pid1 = create(writer2, 2000, 20, "writer2", 2, "writer", lck);
	pid2 = create(reader2, 2000, 20, "reader2", 2, "reader", lck);

	resume(pid1);
	sleep(2);
	resume(pid2);

	sleep(8);
	ldelete(lck);
	kill(pid1);
	kill(pid2);

	assert(testval == 40, "Test 2 FAILED\n");
	kprintf("Test 2 PASSED!\n");
}

/*-----------------------------------Test 3---------------------------*/
void test3() {
	int lck[NLOCK];
	int last_lck;
	int index;

	kprintf("\nTest 3: return SYSERR if no lock is available\n");

	for (index = 0; index < NLOCK; index++) {
		lck[index] = lcreate();
		assert(lck[index] != SYSERR, "Test 3 FAILED\n");
	}

	last_lck = lcreate();
	assert(last_lck == SYSERR, "Test 3 FAILED\n");

	for (index = 0; index < NLOCK; index++) {
		ldelete(lck[index]);
	}
	kprintf("Test 3 PASSED!\n");
}

/*----------------------------------Test 4---------------------------*/
void reader4(char *msg, int lck) {
	int ret;

	ret = lock(lck, READ, DEFAULT_LOCK_PRIO);
	assert(ret == DELETED, "Test 4 FAILED\n");
	kprintf("Test 4 PASSED!\n");
}

void writer4(char *msg, int lck) {
	lock(lck, WRITE, DEFAULT_LOCK_PRIO);
	sleep(5);
	releaseall(1, lck);

}

void test4() {
	int lck;
	int pid1;
	int pid2;

	kprintf("\nTest 4: return DELETED if lock deleted while waiting\n");
	lck = lcreate();
	assert(lck != SYSERR, "Test 4 FAILED\n");

	pid1 = create(writer4, 2000, 20, "writer4", 2, "writer", lck);
	pid2 = create(reader4, 2000, 20, "reader4", 2, "reader", lck);

	resume(pid1);
	sleep(1);

	resume(pid2);
	sleep(1);
	ldelete(lck);

	sleep(5);
	kill(pid1);
	kill(pid2);
}

/*------------------------------------------------------------------------
 *  main  --  user main program
 *------------------------------------------------------------------------
 */
int main() {
	test0();
	test1();
	test2();
	test3();
	test4();
	return 0;
}
