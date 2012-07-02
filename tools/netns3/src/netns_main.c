/*
 * CORE
 * Copyright (c)2010-2011 the Boeing Company.
 * See the LICENSE file included in this distribution.
 *
 * author: Tom Goff <thomas.goff@boeing.com>
 *
 * netns_main.c
 *
 * netns utility program runs the specified program with arguments in a new
 * namespace.
 *
 */

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include <sys/wait.h>

#include "netns.h"
#include "myerr.h"

void usage(int status, char *fmt, ...)
{
  extern const char *__progname;
  va_list ap;
  FILE *output;

  va_start(ap, fmt);

  output = status ? stderr : stdout;
  fprintf(output, "\n");
  if (fmt != NULL)
  {
    vfprintf(output, fmt, ap);
    fprintf(output, "\n\n");
  }
  fprintf(output,
	  "Usage: %s [-h] [-w] -- command [args...]\n\n"
	  "Run the specified command in a new network namespace.\n\n"
	  "Options:\n"
	  "  -h  show this help message and exit\n"
	  "  -w  wait for command to complete "
	  "(useful for interactive commands)\n",
	  __progname);

  va_end(ap);

  exit(status);
}

int main(int argc, char *argv[])
{
  pid_t pid;
  int waitcmd = 0;
  int status = 0;

  for (;;)
  {
    int opt;

    if ((opt = getopt(argc, argv, "hw")) == -1)
      break;

    switch (opt)
    {
    case 'w':
      waitcmd++;
      break;

    case 'h':
    default:
      usage(0, NULL);
    }
  }

  argc -= optind;
  argv += optind;

  if (!argc)
    usage(1, "no command given");

  if (geteuid() != 0)
    usage(1, "must be suid or run as root");
  if (setuid(0))
    ERR(1, "setuid() failed");

  pid = nsexecvp(argv);
  if (pid < 0)
    ERR(1, "nsexecvp() failed");

  printf("%d\n", pid);

  if (waitcmd)
  {
    if (waitpid(pid, &status, 0) == -1)
      ERR(1, "waitpid() failed");

    if (WIFEXITED(status))
      status = WEXITSTATUS(status);
    else if (WIFSIGNALED(status))
    {
      fprintf(stderr, "process terminated by signal %d\n", WTERMSIG(status));
      status = -1;
    }
  }

  exit(status);
}
