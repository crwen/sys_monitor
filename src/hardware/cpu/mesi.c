
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


typedef enum {
    MODIFIED,           // exclusive modified: the only dirty copy
    EXCLUSIVE,          // exclusive clean: the only copy
    SHARED,             // shared: multiple processors may hold this exact same copy
    INVALID             // invalid: invalid copy of the physical address; the cache line may hold data of other physical address
} state_t;

typedef struct {
    state_t state;
    int value;
} line_t;

#ifndef NUM_PROCESSOR
#define NUM_PROCESSOR (128)
#endif

line_t cache[NUM_PROCESSOR];

// the value on mem shared cache
int mem_value = 15213;

// check if the state is legal 
int check_state() {
    int m_count = 0;
    int e_count = 0;
    int s_count = 0;
    int i_count = 0;

    for (int i = 0; i < NUM_PROCESSOR; i ++) {
        if (cache[i].state == MODIFIED) {
            m_count ++;
        } else if (cache[i].state == EXCLUSIVE) {
            m_count ++;
        } else if (cache[i].state == SHARED) {
            s_count ++;
        } else if (cache[i].state == INVALID) {
            i_count ++;
        }
    }

    /*  The legal states:
        +----+----+----+----+----+
        |    | M  | E  | S  | I  |
        +----+----+----+----+----+
        | M  | X  | X  | X  | O  |
        | E  | X  | X  | X  | O  |
        | S  | X  | X  | O  | O  |
        | I  | O  | O  | O  | O  |
        +----+----+----+----+----+
    */
    #ifdef DEBUG
    printf("M %d\t E %d\t S %d\t I %d\n", m_count, e_count, s_count, i_count);
    #endif

    if ((m_count == 1 && i_count == (NUM_PROCESSOR - 1)) || 
        (e_count == 1 && i_count == (NUM_PROCESSOR - 1)) ||
        (s_count >= 2 && i_count == (NUM_PROCESSOR - s_count))) {
        
        return 1;
    }
    // illegal state
    return 0;
}

// i - the index of processor
int read_cacheline(int i, int *read_value) {
    if (cache[i].state == MODIFIED) {
        // read hit
        #ifdef DEBUG
        printf("[%d] read hit; dirty value %d\n", i, cache[i].value);
        #endif
        *read_value = cache[i].value;
        return 1;
    } else if (cache[i].state == EXCLUSIVE) {
        // read hit
        #ifdef DEBUG
        printf("[%d] read hit; exclusive clean value %d\n", i, cache[i].value);
        #endif
        *read_value = cache[i].value;
        return 1;
    } else if (cache[i].state == SHARED) {
        // read hit
        #ifdef DEBUG
        printf("[%d] read hit; shared clean value %d\n", i, cache[i].value);
        #endif
        *read_value = cache[i].value;
    } else if (cache[i].state == INVALID) {
        // read miss
        // bus boardcast read miss
        for (int j = 0; j < NUM_PROCESSOR; j ++) {
            if (i != j) {
                if (cache[j].state == MODIFIED) {
                    // write-back
                    mem_value = cache[j].value;
                    cache[j].state = SHARED;

                    // update read miss cache
                    cache[i].state = SHARED;
                    cache[i].value = cache[j].value;

                    *read_value = cache[i].value;

                    #ifdef DEBUG
                    printf("[%d] read miss; [%d] supplies dirty value %d; write back; s_count == 2\n", i, j, cache[i].value);
                    #endif

                    return 1;
                } else if (cache[j].state == EXCLUSIVE) {
                    // update state for processor j
                    cache[j].state = SHARED;
                    cache[i].value = cache[j].value; 

                    cache[i].state = SHARED;
                    *read_value = cache[i].value;

                    #ifdef DEBUG
                    printf("[%d] read miss; [%d] supplies clean value %d; s_count == 2\n", i, j, cache[i].value);
                    #endif

                    return 1;
                } else if (cache[j].state == SHARED) {
                    cache[i].value = cache[j].value;
                    cache[i].state = SHARED;

                    *read_value = cache[i].value;

                    #ifdef DEBUG
                    printf("[%d] read miss; [%d] supplies clean value %d; s_count >= 3\n", i, j, cache[i].value);
                    #endif

                    return 1;
                }
            }
        }

        // all others invalid
        cache[i].state = EXCLUSIVE;
        cache[i].value = mem_value;

        *read_value = cache[i].value;

        #ifdef DEBUG
        printf("[%d] read miss; mem supplies clean value %d; e_count == 1\n", i, cache[i].value);
        #endif

        return 1;
    }

    return 0;
}

// i - the index of processor
int write_cacheline(int i, int write_value) {

    if (cache[i].state == MODIFIED) {
        // write hit
        cache[i].value = write_value;

        #ifdef DEBUG
        printf("[%d] write hit; update to value %d\n", i, cache[i].value);
        #endif

        return 1;
    } else if (cache[i].state == EXCLUSIVE) {
        // write hit
        cache[i].state = MODIFIED;
        cache[i].value = write_value;

        #ifdef DEBUG
        printf("[%d] write hit; update to value %d\n", i, cache[i].value);
        #endif

        return 1;
    } else if (cache[i].state == SHARED) {
        // write hit
        // boardcast write invalid
        for (int j = 0; j < NUM_PROCESSOR; j ++) {
            if (i != j) {
                if (cache[j].state == SHARED) {
                    cache[j].state = INVALID;
                    cache[j].value = 0;
                } 
            }
        }
        cache[i].state = MODIFIED;
        cache[i].value = write_value;

        #ifdef DEBUG
        printf("[%d] write hit; boardcast invalid; update to value %d\n", i, cache[i].value);
        #endif

        return 1; 
    } else if (cache[i].state == INVALID) {
        // write miss
        // boardcast write invalid
        for (int j = 0; j < NUM_PROCESSOR; j ++) {
            if (i != j) {
                if (cache[j].state == MODIFIED) {
                    // write-back
                    mem_value = cache[j].value;
                    cache[j].state = INVALID;
                    cache[j].value = 0;

                    cache[i].state = MODIFIED;
                    cache[i].value = mem_value;
                    cache[i].value = write_value;

                    #ifdef DEBUG
                    printf("[%d] write miss; boardcast invalid to M; update to value %d\n", i, cache[i].value);
                    #endif

                    return 1;
                } else if (cache[j].state == EXCLUSIVE) {
                    cache[j].state = INVALID;
                    cache[i].value = cache[j].value;
                    cache[j].value = 0;

                    cache[i].value = MODIFIED;
                    cache[i].value = write_value;

                    #ifdef DEBUG
                    printf("[%d] write miss; boardcast invalid to E; update to value %d\n", i, cache[i].value);
                    #endif
                    return 1;
                } else if (cache[j].state == SHARED) { 
                    cache[i].value = cache[j].value;

                    for (int k = 0; k < NUM_PROCESSOR; k ++) {
                        if (i != k) {
                            cache[k].state = INVALID;
                            cache[k].value = 0;
                        }
                    }


                    cache[i].value = MODIFIED;
                    cache[i].value = write_value;

                    #ifdef DEBUG
                    printf("[%d] write miss; boardcast invalid to S; update to value %d\n", i, cache[i].value);
                    #endif

                    return 1;
                }
            }
        }

        // all other are invalid
        // write allcoate
        cache[i].value = mem_value;
        cache[i].state = MODIFIED;
        cache[i].value = write_value;

        #ifdef DEBUG
        printf("[%d] write miss; all invalid; update to value %d\n", i, cache[i].value);
        #endif

        return 1;
    }

    return 0;
}

int evict_cacheline(int i) {
    if (cache[i].state == MODIFIED) {
        // write-back
        mem_value = cache[i].value;
        cache[i].state = INVALID;
        cache[i].value = 0;

        #ifdef DEBUG
        printf("[%d] evict; write back value %d\n", i, cache[i].value);
        #endif

        return 1;
    } else if (cache[i].state == EXCLUSIVE || cache[i].state == SHARED) {
        cache[i].state = INVALID;
        cache[i].value = 0;

        #ifdef DEBUG
        printf("[%d] evict\n", i);
        #endif

        return 1;
    } else if (cache[i].state == INVALID) {
        return 1;
    }
    
    return 0;
}