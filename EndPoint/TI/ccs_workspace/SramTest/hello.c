#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define DDR_START       0x80000000
#define DDR_SIZE_WORDS  64*1024*1024/4
#define TEST_PATTERN    0x55555555


/**
 * hello.c
 */
int main(void)
{
	printf("Starting test!\n");
	bool success = true;
	uint32_t * addr = (uint32_t*)0x80000000;
	int idx = 0;
	//fill memory
	for(;idx < (DDR_SIZE_WORDS); idx++, addr++)
	{
	    *addr = TEST_PATTERN;
	}
	addr = (uint32_t*)0x80000000;
	//test memory
	idx = 0;
	for(;idx < (DDR_SIZE_WORDS); idx++, addr++)
	{
	    if(*addr != TEST_PATTERN)
	    {
	        printf("Test failed. Addr. : 0x%lx, val. : 0x%lx\n", (uint32_t)addr, *addr);
	        success = false;
	    }
	    if(idx%10000 == 0)
	    {
	        printf("Addr. : 0x%lx, val. : 0x%lx\n", (uint32_t)addr, *addr);
	    }
	}
	if(success)
	{
	    printf("DDR test successfully passed\n");
	}
	else
	{
	    printf("DDR test failed.\n");
	}

	return 0;
}
