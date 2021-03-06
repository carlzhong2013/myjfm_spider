/*******************************************************************************
 * shared.h - all classes inherited from this class can use sharedpointer
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#ifndef _SHARED_H_
#define _SHARED_H_

#include <stdint.h>

#include "config.h"
#include "mutex.h"

_START_MYJFM_NAMESPACE_

// the class inherited from this class can obtain the atomic inc and dec feature
class Shared {
public:
  virtual ~Shared() {};
  RES_CODE add_ref();
  uint32_t dec_ref(bool* flag = NULL);

protected:
  Shared();

private:
  uint32_t _count;
  Mutex _mutex;
  // only can be inherited
  Shared(const Shared&);
  Shared& operator=(const Shared&);
};

_END_MYJFM_NAMESPACE_

#endif

