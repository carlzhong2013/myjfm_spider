/*******************************************************************************
 * scheduler_task.h - the scheduler module implementation
 * each scheduler is a thread. It is responsible for the url scheduling and all
 * the other jobs.
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#ifndef _SCHEDULER_TASK_H_
#define _SCHEDULER_TASK_H_

#include <stdint.h>

#include "config.h"
#include "task.h"
#include "shared_pointer.h"
#include "squeue.h"
#include "url.h"

_START_MYJFM_NAMESPACE_

class SchedulerTask : public Task {
public:
  explicit SchedulerTask(uint32_t id);
  ~SchedulerTask();
  virtual RES_CODE operator()(void* arg = NULL);

private:
  uint32_t _id;
  uint32_t _downloader_num;
  SharedPointer<SQueue<SharedPointer<Url> > > _url_queue;
  Vector<SharedPointer<SQueue<SharedPointer<Url> > > > _downloader_queues;

  RES_CODE init();
  RES_CODE put_url_into_downloader(SharedPointer<Url>& url_p);
};

_END_MYJFM_NAMESPACE_

#endif

