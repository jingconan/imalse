/*
 * CORE
 * Copyright (c)2010-2011 the Boeing Company.
 * See the LICENSE file included in this distribution.
 *
 * author: Tom Goff <thomas.goff@boeing.com>
 *
 * netns.c
 *
 * Implements nsfork() and nsexecvp() for forking and executing processes
 * within a network namespace.
 *
 */

#include <signal.h>
#include <unistd.h>

#include <sys/syscall.h>
#include <sys/mount.h>

#include "myerr.h"
#include "netns.h"

#define NSCLONEFLGS				\
  (						\
   SIGCHLD       |				\
   CLONE_NEWNS   |				\
   CLONE_NEWUTS  |				\
   CLONE_NEWIPC  |				\
   CLONE_NEWPID	 |				\
   CLONE_NEWNET					\
  )

pid_t nsfork(int flags)
{
  int pid;

  pid = syscall(SYS_clone, flags | NSCLONEFLGS, NULL, NULL, NULL, NULL);
  if (pid == 0)			/* child */
  {
    /* mount /proc when CLONE_NEWPID is used */
    if (mount(NULL, "/proc", "proc", 0, NULL))
      WARN("mounting /proc failed");
  }

  return pid;
}

pid_t nsexecvp(char *argv[])
{
  pid_t pid;

  pid = nsfork(CLONE_VFORK);
  switch (pid)
  {
  case -1:
    WARN("nsfork() failed");
    break;

  case 0:
    /* child */
    execvp(argv[0], argv);
    WARN("execvp() failed for '%s'", argv[0]);
    _exit(1);
    break;

  default:
    /* parent */
    if (kill(pid, 0))
      pid = -1;
    break;
  }

  return pid;
}
