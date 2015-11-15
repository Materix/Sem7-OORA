#ifndef __USE_GNU
#define __USE_GNU
#endif
#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sched.h>
#include "papi.h"

int events[5] = {PAPI_FP_OPS, PAPI_STL_ICY, PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L2_DCH};
long long value = 0;
int eventSet = PAPI_NULL;
int papi_err;
bool supported = true;

void init(){
    if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT) {
        fprintf(stderr, "PAPI is unsupported.\n");
        supported = false;
    }
    if (PAPI_num_counters() < 5) {
        fprintf(stderr, "PAPI is unsupported.\n");
        supported = false;
    }
    if ((papi_err = PAPI_create_eventset(&eventSet)) != PAPI_OK) {
        fprintf(stderr, "Could not create event set: %s\n", PAPI_strerror(papi_err));
    }
    cpu_set_t my_set;
    CPU_ZERO(&my_set);
    CPU_SET(0, &my_set);
    if (sched_setaffinity(0, sizeof(cpu_set_t), &my_set) != 0) {
        perror("sched_setaffinity error");
    }
    
}

void startCount(int eventNumber){
    if ((papi_err = PAPI_add_event(eventSet, events[eventNumber])) != PAPI_OK ) {
        fprintf(stderr, "Could not add event: %s\n", PAPI_strerror(papi_err));
    }
    if (supported) {
        if ((papi_err = PAPI_start(eventSet)) != PAPI_OK) {
            fprintf(stderr, "Could not start counters: %s\n", PAPI_strerror(papi_err));
        }
    }
}

void stopAndPrint(){
    if (supported) {
        if ((papi_err = PAPI_stop(eventSet, &value)) != PAPI_OK) {
            fprintf(stderr, "Could not get values: %s\n", PAPI_strerror(papi_err));
        }
        printf("%lld\n", value);
    }
}

