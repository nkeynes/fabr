/*
 * Copyright (c) 2020 Nathan Keynes
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef FABR_SUPPORT_DEPENDENCYQUEUE_H
#define FABR_SUPPORT_DEPENDENCYQUEUE_H

#include <list>
#include <set>
#include <map>

namespace fabr {

/**
 * Queue of jobs with dependencies between them.
 *
 * Despite the name, the queue does not in fact enforce an ordering on
 * the tasks in the queue, other than that they will be executed in the
 * order in which they become runnable.
 *
 * Also note the queue is not inherently thread-safe; the caller is responsible
 * for ensuring synchronization if necessary.
 */
template<class T>
class DependencyQueue {
private:
    class Job {
    public:
        T task;
        std::set<Job *> waitList;
        std::list<Job *> usedBy;

        Job( T task ) : task(task) { }

        bool isRunnable() const {
            return waitList.empty();
        }
    };

    std::map<T, Job *> queue;
    std::list<Job *> runnable;

    void addDependency( Job *from, Job *to ) {
        from->waitList.insert(to);
        to->usedBy.push_back(from);
    }

    void markComplete( Job *job ) {
        for( auto dep : job->usedBy ) {
            dep->waitList.erase(job);
            if( dep->isRunnable() ) {
                markRunnable(job);
            }
        }
    }

    void markRunnable( Job *job ) {
        runnable.push_back(job);
    }

    Job *getJob( T task ) const {
        auto it = queue.find(task);
        return it == queue.end() ? nullptr : it->second;
    }

public:
    /** Default constructor */
    DependencyQueue();

    /**
     * Add a job to the queue with no dependencies (immediately runnable). The job
     * should not already be on the queue.
     * @param task the task to add
     */
    void queueJob( T task ) {
        Job *job = new Job(task);
        queue[task] = job;
        markRunnable(job);
    }

    /**
     * Add a job to the queue. The job should not already be on the
     * queue.
     * @param task the task to add
     * @param begin,end an iterator range specifying the dependencies of the job.
     */
    template<class Iterator>
    void queueJob( T task, Iterator begin, Iterator end ) {
        Job *job = new Job(task);
        queue[task] = job;
        while( begin != end ) {
            addDependency( job, getJob(*begin) );
            ++begin;
        }
        if( job->isRunnable() ) {
            markRunnable(job);
        }
    }

    void addDependency( T fromTask, T toTask ) {
        Job *fromJob = getJob(fromTask);
        Job *toJob = getJob(toTask);
        if( fromJob->isRunnable() ) {
            runnable.erase(fromJob);
        }
        addDependency( fromJob, toJob );
    }

    /**
     * Remove a runnable job from the queue and return it.
     * hasRunnable() should be checked first.
     */
    T dequeueJob() {
        Job *job = runnable.front();
        runnable.pop_front();
        return job->task;
    }

    /**
     * Notify the queue that the given job has been completed.
     * Any jobs that depend on the completed job are checked to
     * see if they can be moved to runnable.
     *
     * Assumes that the job has previously been dequeued for
     * execution - behaviour is undefined if job is still in
     * the queue.
     */
    void jobCompleted( T task ) {
        Job *job = getJob(task);
        markComplete(job);
        queue.erase(task);
        delete job;
    }

    /**
     * @return true if the given job is currently waiting in the queue.
     * (note this is O(n) in queue size).
     */
    bool isQueued( T task ) const {
        return getJob(task) != nullptr;
    }

    /**
     * @return true if the queue is empty.
     */
    bool empty() const {
        return queue.empty();
    }

    /**
     * @return the total number of jobs in the queue.
     */
    size_t size() const {
        return queue.size();
    }

    /**
     * @return true if the queue contains at least one runnable job.
     */
    bool hasRunnable() const {
        return !runnable.empty();
    }

    /**
     * @return the numbser of currently runnable jobs in the queue.
     */
    size_t getRunnableCount() const {
        return runnable.size();
    }
};

}

#endif /* FABR_SUPPORT_DEPENDENCYQUEUE_H */

