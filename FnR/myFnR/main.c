#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "../stubs.h"
#include <x86intrin.h>
#include <malloc.h>

/**
 * Returns current tick
 */
uint64_t currentTick(){
    uint64_t t1;
    unsigned int ui;
    t1 = __rdtscp(&ui);
    
    //printf("%ld ticks\n", t1);
    //printf("TSC_AUX was %x\n", ui);
    
    return t1;
}

/**
 * See description of test0
 */
void findTimeDiffTest0(uint32_t * samples, int clflushFlag){

	// Loop runLimit times
	int runLimit = 4096;

	uint64_t* differenceArray = (uint64_t *) malloc (runLimit * sizeof(uint64_t));

	int other = 1;
	
	for(int i = 0; i < runLimit; i++){
		
		if(clflushFlag){
			clflush(samples);
		}
		uint64_t t1;
		t1 = currentTick();

		for(int j = 0; j < 16; j++){
			samples[j] = other;
		}

		uint64_t t2;
		t2 = currentTick();

		// flip other 
		other = !other;

		differenceArray[i] = t2-t1;
	}

	double average = 0;
	for(int i = 0; i < runLimit; i++){
		average += differenceArray[i]; 
		//printf("%d %ld \n", i, differenceArray[i]);
	}
	average = average / runLimit;

	if(clflushFlag){
		printf("Average of clflush execution: %f\n", average);
	}
	else{
		printf("Average of normal execution: %f\n", average);
	}
}


/**
 * Test0
 * Initially, samples array is in the cache.
 * 
 * Start timer
 * Switch first 16 elements of samples array to 1 (or 0 depends on the 'other')
 * End timer
 * The difference is flushless difference. It should be low
 * Repeat many times, take the average
 * 
 * 
 * Flush the first 16 elements.
 * Measure current time
 * Switch the first 16 elements to 1 (or 0 depends on the 'other')
 * Measure time again
 * The difference should be higher than above.
 * Repeat many times, take the average
 */
void test0(uint32_t* samples){
	printf("Beginning test0...\n");
	// 0 flag is clflushLESS execution
	// it should be faster than clflush, 1
	int clflushFlag = 0;

	findTimeDiffTest0(&samples[0], clflushFlag);

	findTimeDiffTest0(&samples[0], !clflushFlag);

}

/**
 * See description of test1
 */
void findTimeDiffTest1(uint32_t * samples, int clflushFlag){
	
	// Loop runLimit times
	int runLimit = 4096;

	uint64_t* differenceArray = (uint64_t *) malloc (runLimit * sizeof(uint64_t));

	int other = 1;

	for(int i = 0; i < runLimit; i++){
		
		// create a new array size of 16 element
		uint32_t* samples2 = (uint32_t *) malloc (16 * sizeof(uint32_t));

		if(clflushFlag){
			clflush(samples);
		}
		uint64_t t1;
		t1 = currentTick();

		for(int j = 0; j < 16; j++){
			samples2[j] = samples[j]; // load
			samples2[j] = other; // write
		}

		uint64_t t2;
		t2 = currentTick();

		// flip other 
		other = !other;

		differenceArray[i] = t2-t1;
		free(samples2);
	}

	double average = 0;
	for(int i = 0; i < runLimit; i++){
		average += differenceArray[i]; 
		//printf("%d %ld \n", i, differenceArray[i]);
	}
	average = average / runLimit;

	if(clflushFlag){
		printf("Average of clflush execution: %f\n", average);
	}
	else{
		printf("Average of normal execution: %f\n", average);
	}

}


/**
 * Test1
 * Initially, samples array is in the cache.
 * 
 * Without clflush
 * 
 * Create a new array called samples2, size of 16 elements
 * Start timer
 * Assign first 16 element of samples' array to samples2. This will do LOAD operation
 * Flip samples2's elements
 * End timer  
 * 
 * 
 * With clflush
 * 
 * Create array, sample2
 * Flush samples' first 16 elements
 * Start timer
 * Assign first 16 elements of samples' array to samples2.
 * Flip sample2's elements
 * End timer
 */
void test1(uint32_t* samples){
	printf("Beginning test1...\n");

	int clflushFlag = 0;

	findTimeDiffTest1(&samples[0], clflushFlag);

	findTimeDiffTest1(&samples[0], !clflushFlag);
}


/**
 * See description of test2
 */
void findTimeDiffTest2(uint32_t * samples, int clflushFlag){
	
	// Loop runLimit times
	int runLimit = 4096;

	uint64_t* differenceArray = (uint64_t *) malloc (runLimit * sizeof(uint64_t));

	int other = 1;

	for(int i = 0; i < runLimit; i++){
		
		// create a new array size of 16 element
		uint32_t* samples2 = (uint32_t *) malloc (16 * sizeof(uint32_t));

		if(clflushFlag){
			clflush(samples);
		}
		uint64_t t1;
		t1 = currentTick();

		for(int j = 0; j < 16; j++){
			samples2[j] = samples[j]; // load
			// Change samples
			samples[j] = other; // write
		}

		uint64_t t2;
		t2 = currentTick();

		// flip other 
		other = !other;

		differenceArray[i] = t2-t1;
		free(samples2);
	}

	double average = 0;
	for(int i = 0; i < runLimit; i++){
		average += differenceArray[i]; 
		//printf("%d %ld \n", i, differenceArray[i]);
	}
	average = average / runLimit;

	if(clflushFlag){
		printf("Average of clflush execution: %f\n", average);
	}
	else{
		printf("Average of normal execution: %f\n", average);
	}

}

/**
 * Test2
 * Initially, samples array is in the cache.
 * 
 * Without clflush
 * 
 * Create samples2, 16 element
 * Start timer
 * samples2[j] = samples[j]
 * Loop 0-15 elements, Change samples[j] to other
 * End timer
 * 
 * 
 * With clflush
 * 
 * Create samples2, 16 element
 * Flush samples
 * Start timer
 * samples2[j] = samples[j]	
 * Loop 0-15 elements, Change samples[j] to other
 * End timer
 */
void test2(uint32_t* samples){
	printf("Beginning test2...\n");

	int clflushFlag = 0;

	findTimeDiffTest2(&samples[0], clflushFlag);

	findTimeDiffTest2(&samples[0], !clflushFlag);
}


/**
 * See description of test3
 */
void findTimeDiffTest3(uint32_t * samples, int clflushFlag){
	
	// Loop runLimit times
	int runLimit = 4096;

	uint64_t* differenceArray = (uint64_t *) malloc (runLimit * sizeof(uint64_t));

	int other = 1;

	for(int i = 0; i < runLimit; i++){
		
		if(clflushFlag){
			clflush(samples);
		}
		// this line should load first 16 elements to the cache
		// because of the cache line is 64 byte, and one element is 4 byte
		samples[0] = samples[0] + 1;

		uint64_t t1;
		t1 = currentTick();

		for(int j = 1; j < 4; j++){
			samples[j] = other; // write
		}

		uint64_t t2;
		t2 = currentTick();

		// flip other 
		other = !other;

		differenceArray[i] = t2-t1;
	}

	double average = 0;
	for(int i = 0; i < runLimit; i++){
		average += differenceArray[i]; 
		//printf("%d %ld \n", i, differenceArray[i]);
	}
	average = average / runLimit;

	if(clflushFlag){
		printf("Average of clflush execution: %f\n", average);
	}
	else{
		printf("Average of normal execution: %f\n", average);
	}

}

/**
 * Test3
 * Initially, samples array is in the cache.
 * 
 * Without clflush
 * 
 * Change samples[0] to other
 * Start timer
 * Loop 1-15 elements, change them to other
 * End timer
 * 
 * 
 * With clflush
 * 
 * Flush samples
 * Change samples[0] to other
 * Start timer
 * Loop 1-15 elements, change them to other
 * End timer
 */
void test3(uint32_t* samples){
	printf("Beginning test3...\n");

	int clflushFlag = 0;

	findTimeDiffTest3(&samples[0], clflushFlag);

	findTimeDiffTest3(&samples[0], !clflushFlag);
}



/**
 * See description of test4
 */
void findTimeDiffTest4(uint32_t * samples, int clflushFlag){
	
	// Loop runLimit times
	int runLimit = 4096;

	uint64_t* differenceArray = (uint64_t *) malloc (runLimit * sizeof(uint64_t));




	for(int i = 0; i < runLimit; i++){
		
		// create a new array size of 16 element
		uint32_t* samples2 = (uint32_t *) malloc (16 * sizeof(uint32_t));

		// debug
		if(i == 0){
			printf("address of samples2: %p\n", samples2);
		}

		if(clflushFlag){
			clflush(samples);
		}
		
		samples2[0] = samples[0];

		uint64_t t1;
		t1 = currentTick();

		for(int j = 1; j < 16; j++){
			samples2[j] = samples[j]; // write
		}

		uint64_t t2;
		t2 = currentTick();

		differenceArray[i] = t2-t1;

		free(samples2);
	}

	double average = 0;
	for(int i = 0; i < runLimit; i++){
		average += differenceArray[i]; 
		//printf("%d %ld \n", i, differenceArray[i]);
	}
	average = average / runLimit;


	printf("address of samples: %p\n", samples);
	if(clflushFlag){
		printf("Average of clflush execution: %f\n", average);
	}
	else{
		printf("Average of normal execution: %f\n", average);
	}

}

/**
 * Test4
 * Initially, samples array is in the cache.
 * 
 * Without clflush
 * 
 * Create samples2, 16 element array
 * samples2[0] = samples[0]
 * Start timer
 * Loop 1-15 elements, samples2[j] = samples[j]
 * End timer
 * 
 * 
 * With clflush
 * 
 * Create samples2, 16 element array
 * Flush samples
 * samples2[0] = samples[0]
 * Start timer
 * Loop 1-15 elements, samples2[j] = samples[j]
 * End timer
 */
void test4(uint32_t* samples){
	printf("Beginning test4...\n");

	int clflushFlag = 0;

	findTimeDiffTest4(&samples[0], clflushFlag);

	findTimeDiffTest4(&samples[0], !clflushFlag);
}



/**
 * See description of test5
 */
void findTimeDiffTest5(uint32_t * samples, int clflushFlag){
	
	// Loop runLimit times
	int runLimit = 4096;

	uint64_t* differenceArray = (uint64_t *) malloc (runLimit * sizeof(uint64_t));

	int other = 1;

	for(int i = 0; i < runLimit; i++){
		
		if(clflushFlag){
			clflush(samples);
		}
		
		uint64_t t1;
		t1 = currentTick();

		for(int j = 16; j < 31; j++){
			samples[j] = other; // write
		}
		
		uint64_t t2;
		t2 = currentTick();

		other = !other;
		
		differenceArray[i] = t2-t1;
	}

	double average = 0;
	for(int i = 0; i < runLimit; i++){
		average += differenceArray[i]; 
		//printf("%d %ld \n", i, differenceArray[i]);
	}
	average = average / runLimit;


	printf("address of samples: %p\n", samples);
	if(clflushFlag){
		printf("Average of clflush execution: %f\n", average);
	}
	else{
		printf("Average of normal execution: %f\n", average);
	}

}

/**
 * Test5
 * Initially, samples array is in the cache.
 * 
 * Without clflush
 * 
 * Start timer
 * Loop 16-31 elements, samples[j] = other
 * End timer
 * 
 * 
 * With clflush
 * 
 * Flush samples
 * Start timer
 * Loop 16-31 elements, samples[j] = other
 * End timer
 */
void test5(uint32_t* samples){
	printf("Beginning test5...\n");

	int clflushFlag = 0;

	findTimeDiffTest5(&samples[0], clflushFlag);

	findTimeDiffTest5(&samples[0], !clflushFlag);
}




/**
 * See description of test6
 */
void findTimeDiffTest6(uint32_t * samples, int clflushFlag){
	
	// Loop runLimit times
	int runLimit = 4096;

	uint64_t* differenceArray = (uint64_t *) malloc (runLimit * sizeof(uint64_t));

	int other = 1;

	for(int i = 0; i < runLimit; i++){
		
		// make a pointer
		uint32_t *a = NULL;
		// assign pointer to the first element of the samples
		a = samples;

		if(clflushFlag){
			clflush(samples);
		}
		
		uint64_t t1;
		t1 = currentTick();

		// 16 times
		for(int j = 0; j < 16; j++){
			*a = other; // write
			*a++; // next element
		}
		
		uint64_t t2;
		t2 = currentTick();
		other = !other;
		
		differenceArray[i] = t2-t1;
	}

	double average = 0;
	for(int i = 0; i < runLimit; i++){
		average += differenceArray[i]; 
		//printf("%d %ld \n", i, differenceArray[i]);
	}
	average = average / runLimit;


	printf("address of samples: %p\n", samples);
	if(clflushFlag){
		printf("Average of clflush execution: %f\n", average);
	}
	else{
		printf("Average of normal execution: %f\n", average);
	}

}

/**
 * Test6
 * Initially, samples array is in the cache.
 * 
 * Without clflush
 * 
 * Make a pointer a
 * a = samples[0]
 * Start timer
 * Loop 0-15 elements, *a = other
 * End timer
 * 
 * 
 * With clflush
 * 
 * Make a pointer a
 * a = samples[0]
 * Flush samples
 * Start timer
 * Loop 0-15 elements, *a = other
 * End timer
 */
void test6(uint32_t* samples){
	printf("Beginning test6...\n");

	int clflushFlag = 0;

	findTimeDiffTest6(&samples[0], clflushFlag);

	findTimeDiffTest6(&samples[0], !clflushFlag);
}



/**
 * See description of test7
 */
void findTimeDiffTest7(uint32_t * samples, int caseFlag){
	
	// Loop runLimit times
	int runLimit = 4096;

	uint64_t* differenceArray = (uint64_t *) malloc (runLimit * sizeof(uint64_t));

	int other = 1;

	for(int i = 0; i < runLimit; i++){
		
		// make a pointer
		uint32_t *a = NULL;
		// assign pointer to the first element of the samples

		clflush(samples);
		a = samples;

		uint64_t t1;

		if(caseFlag == 0){
			// start time
			t1 = currentTick();

			// 16 times
			for(int j = 0; j < 16; j++){
				*a = other; // write
				*a++; // next element
			}

		}	
		else{
			// write to the first element
			*a = other;
			*a++;
			// start time
			t1 = currentTick();
			// 15 times
			for(int j = 1; j < 16; j++){
				*a = other; // write
				*a++; // next element
			}
		}	
		
		uint64_t t2;
		t2 = currentTick();
		other = !other;
		
		differenceArray[i] = t2-t1;
	}

	double average = 0;
	for(int i = 0; i < runLimit; i++){
		average += differenceArray[i]; 
		//printf("%d %ld \n", i, differenceArray[i]);
	}
	average = average / runLimit;


	printf("address of samples: %p\n", samples);
	if(caseFlag){
		printf("Average of case0 execution: %f\n", average);
	}
	else{
		printf("Average of case1 execution: %f\n", average);
	}

}


/**
 * Test7
 * Initially, samples array is in the cache. Both cases include clflush
 * 
 * Make a pointer a
 * Flush samples
 * a = samples[0]
 * Start timer
 * Loop 0-16 elements, *a = other
 * End timer
 * 
 * 
 * Make a pointer a
 * Flush samples
 * a = samples[0]
 * *a = other
 * Start timer
 * Loop 1-15 elements, *a = other
 * End timer
 */
void test7(uint32_t* samples){
	printf("Beginning test7...\n");

	// first case
	findTimeDiffTest7(&samples[0], 0);

	// second case
	findTimeDiffTest7(&samples[0], 1);
}



/**
 * See description of test8
 */
void findTimeDiffTest8(uint32_t * samples, int clflushFlag){
	
	// Loop runLimit times
	int runLimit = 4096;

	uint64_t* differenceArray = (uint64_t *) malloc (runLimit * sizeof(uint64_t));

	int other = 1;

	for(int i = 0; i < runLimit; i++){
		
		if(clflushFlag){
			clflush(samples);
		}
		
		uint64_t t1;
		t1 = currentTick();
		
		samples[0] = other;

		uint64_t t2;
		t2 = currentTick();
		
		other = !other;
		
		differenceArray[i] = t2-t1;
	}

	double average = 0;
	for(int i = 0; i < runLimit; i++){
		average += differenceArray[i]; 
		//printf("%d %ld \n", i, differenceArray[i]);
	}
	average = average / runLimit;


	printf("address of samples: %p\n", samples);
	if(clflushFlag){
		printf("Average of clflush execution: %f\n", average);
	}
	else{
		printf("Average of normal execution: %f\n", average);
	}

}

/**
 * Test8
 * Initially, samples array is in the cache. Both cases include clflush
 * 
 * 
 * Start timer
 * samples[0] = other
 * End timer
 * 
 * 
 * clflush(samples)
 * Start timer
 * samples[0] = other
 * End timer
 */
void test8(uint32_t* samples){
	printf("Beginning test8...\n");

	int clflushFlag = 0;

	findTimeDiffTest8(&samples[0], clflushFlag);

	findTimeDiffTest8(&samples[0], !clflushFlag);
}



/**
 * See description of test9
 */
void findTimeDiffTest9(uint32_t * samples, int clflushFlag){
	
	// Loop runLimit times
	int runLimit = 4096;

	uint64_t* differenceArray = (uint64_t *) malloc (runLimit * sizeof(uint64_t));

	int other = 1;

	for(int i = 0; i < runLimit; i++){
		
		if(clflushFlag){
			clflush(samples);
		}
		

		for (int j = 0; j < 20; j++)
		{
			samples[0] = j;
		}

		uint64_t t1;
		t1 = currentTick();
		
		for (int j = 0; j < 15; j++)
		{
			samples[j] = other;
		}
		
		uint64_t t2;
		t2 = currentTick();
		
		other = !other;
		
		differenceArray[i] = t2-t1;
	}

	double average = 0;
	for(int i = 0; i < runLimit; i++){
		average += differenceArray[i];
	}
	average = average / runLimit;


	printf("address of samples: %p\n", samples);
	// Write differences to the file
	if(clflushFlag){
		printf("Average of clflush execution: %f\n", average);
		
		
	}
	else{
		printf("Average of normal execution: %f\n", average);
		for(int i = 0; i < runLimit; i++){
			printf("%ld ", differenceArray[i]);
		}
		printf("\n");
	}

	

}

/**
 * Test9
 * Initially, samples array is in the cache. Both cases include clflush
 * 
 * samples[0] = j, 100 times
 * Start timer
 * samples[0:15] = other
 * End timer
 * 
 * 
 * 
 * clflush(samples)
 * samples[0] = j, 100 times
 * Start timer
 * samples[0:15] = other
 * End timer
 */
void test9(uint32_t* samples){
	printf("Beginning test9...\n");

	int clflushFlag = 0;

	findTimeDiffTest9(&samples[0], clflushFlag);

	findTimeDiffTest9(&samples[0], !clflushFlag);
}

/**
 * Function for testing pointer arithmetics
 */
void testPointer(uint32_t* samples){

	samples[1024] = 5;

	printf("%d\n", samples[1024]);

	uint32_t *a = NULL;
	a = &samples[1024];

	printf("a before change: %d\n", *a);

	*a = 10;

	printf("a after change: %d\n", *a);

	printf("%d\n", samples[1024]);

	printf("sizeof pointer:%ld\n", sizeof(*a));

	//*(a + 1 * sizeof(uint32_t)) = 11;
	samples[1025] = 11;
	printf("a+4= %d\n", *(++a));

	printf("samples1025= %d\n", samples[1025]);


	uint32_t *b = NULL;
	b = &samples[2048];

	printf("b before change: %d\n", *b);
	printf("samples2048 before change: %d\n", samples[2048]);

	*(b + 1) = 20;


	printf("b after change: %d\n", *b);
	printf("samples2048 after change: %d\n", samples[2048]);

	printf("samples2049 after change: %d\n", samples[2049]);
	
}

int main (int argc, char * argv[])
{
    
    int sample_count = 1024; // uint is 32 bit, 4byte. total 4KB = 1024 * 4
    int delay_loop_count = 1024;

	// What does it do?
    warmup();

    // allocate 4KB
	/*
    uint32_t * samples = (uint32_t *) malloc
        (sample_count * sizeof(uint32_t));
	
	test
	for(int i = 0; i < 1024; i++){
		if(samples[i] != 0){
			printf("i:%d\taddress: %d\n", i, samples[i]);
		}
	}
	printf("address of samples: %p\n", samples);

	*/

	uint32_t * samples = (uint32_t *) memalign(4096, sample_count * sizeof(uint32_t));

	//uint32_t * samples = (uint32_t *) aligned_malloc(sample_count * sizeof(uint32_t), 128);


    // memset the whole array
    // fill with 0's
    memset(samples, 0, sample_count * sizeof(uint32_t));

	/*
	for (int i = 0; i < 1024; i++)
	{
		printf("%d: %p\n", i, &samples[i]);
	}
	*/
	
	//test0(&samples[0]);

	//test1(&samples[0]);

	//test2(&samples[0]);
	
	//test3(samples);

	//test4(&samples[0]);

	//test5(&samples[0]);

	//testPointer(&samples[0]);

	//test6(&samples[0]);


	//test7(&samples[0]);

	//test8(samples);

	test9(samples);
	
	free(samples);
}

