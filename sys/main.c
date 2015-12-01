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
int mystrncmp(char* des,char* target,int n){
    int i;
    for (i=0;i<n;i++){
        if (target[i] == '.') continue;
        if (des[i] != target[i]) return 1;
    }
    return 0;

}

/*--------------------------------Test 0--------------------------------*/
void test0 ()
{
    int lck;
    kprintf("\nTest 0: create rwlock.\n");
    lck  = lcreate ();
    assert (lck != SYSERR,"Test 0 FAILED\n");
    kprintf("Test 0 PASSED!\n");
    ldelete (lck);
}

/*------------------------------------------------------------------------
 *  main  --  user main program
 *------------------------------------------------------------------------
 */
int main()
{
	test0();
	return 0;
}
