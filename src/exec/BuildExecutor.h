/*
 * BuildExecutor.h
 *
 *  Created on: 7 Apr. 2019
 *      Author: nkeynes
 */

#ifndef SRC_BUILDEXECUTOR_H_
#define SRC_BUILDEXECUTOR_H_

#include <system_error>

class BuildQueue;

/**
 * Build runner - given a queue of jobs, process jobs (in parallel where possible)
 * until the queue is empty, accounting for dependencies between jobs.
 */
class BuildExecutor {

public:
    BuildExecutor();


    /**
     * Execute the build queue, returning an error code on failure.
     */
    std::error_code execute( BuildQueue &queue );

};



#endif /* SRC_BUILDEXECUTOR_H_ */
