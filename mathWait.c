// Project 6 - Zachary Gmyr
// (description): 

/* STEP 1:
 * - Integrate a '-h' help option using 'getopt()', report usage
 * - Program should expect at least 2 arguments (working with pairs of #'s
 */

/* STEP 2:
 * - Allocate shared memory for 2 integers using 'shmget()'
 * - Set shared memory values to '-200' and '-200'
 * - Make fork() call
 */

/* STEP 3:
 * - Parent should wait while child performs task...
 * - Child puts arguments & stores them in dynamic array
 * - Find one pair of numbers in this array that sums to '19'
 *     ... if no pair exists set default pair to '-1' '-1'
 * - Attach to shared memory region from before
 * - VALIDATE that shared memory was created properly by checking '-200' values
 *     ... exit in error if not true
 * - Copy pair of #'s to shared memory & detach, then terminate
 * - If no pair was found, write instead '-1' '-1'
 */

/* STEP 4:
 * - Check shared memory space after child terminates
 * - If '-200' '-200' then error
 * - If '-1' '-1' then no pair was found
 * - If anything else then simply report the pair
