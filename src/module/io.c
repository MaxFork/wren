#include <stdlib.h>
#include <string.h>

#include "uv.h"

#include "scheduler.h"
#include "vm.h"
#include "wren.h"

#include <stdio.h>

// Called by libuv when the stat call for size completes.
static void sizeCallback(uv_fs_t* request)
{
  WrenValue* fiber = (WrenValue*)request->data;
  
  if (request->result != 0)
  {
    schedulerResumeString(fiber, uv_strerror((int)request->result));
    uv_fs_req_cleanup(request);
    return;
  }
  
  double size = (double)request->statbuf.st_size;
  uv_fs_req_cleanup(request);
  
  schedulerResumeDouble(fiber, size);
}

void fileStartSize(WrenVM* vm)
{
  const char* path = wrenGetArgumentString(vm, 1);
  WrenValue* fiber = wrenGetArgumentValue(vm, 2);

  // Store the fiber to resume when the request completes.
  uv_fs_t* request = (uv_fs_t*)malloc(sizeof(uv_fs_t));
  request->data = fiber;

  uv_fs_stat(getLoop(), request, path, sizeCallback);
}
