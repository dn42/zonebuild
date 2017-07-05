/*
 * signal.c
 *
 *  Created on: Dec 6, 2013
 *      Author: reboot
 */

#include "signal_t.h"
#include "l_error.h"

#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#ifdef _G_SSYS_THREAD
#include <pthread.h>
#include <thread.h>
#endif

#ifdef __FreeBSD__
#include <pthread_np.h>
#endif

void
sig_handler_null(int signal)
{
  return;
}

#include <unistd.h>
#include <sys/syscall.h>

void
sig_handler_test(int signal)
{
  int thread_id = -1;

  #ifdef __FreeBSD__
    thread_id = (int) pthread_getthreadid_np();
  #else
    thread_id = (int) syscall(SYS_gettid);
  #endif

  printf("recv on: %d :             %d     \n", thread_id,
      signal);
  usleep(10000);
  return;
}

int
setup_sighandlers(void)
{
  struct sigaction sa =
    {
      { 0 } }, sa_c =
    {
      { 0 } }, sa_e =
    {
      { 0 } };

  int r = 0;

  sa.sa_handler = sig_handler;
  sa.sa_flags = SA_RESTART;

  sa_c.sa_sigaction = child_sig_handler;
  sa_c.sa_flags = SA_RESTART | SA_SIGINFO;

  sa_e.sa_sigaction = sighdl_error;
  sa_e.sa_flags = SA_RESTART | SA_SIGINFO;

  sigfillset(&sa.sa_mask);
  sigfillset(&sa_c.sa_mask);
  sigemptyset(&sa_e.sa_mask);

  r += sigaction(SIGINT, &sa, NULL);
  r += sigaction(SIGQUIT, &sa, NULL);
  r += sigaction(SIGABRT, &sa, NULL);
  r += sigaction(SIGTERM, &sa, NULL);
  //r += sigaction(SIGIO, &sa, NULL);
  /*r += sigaction(SIGUSR1, &sa, NULL);
   r += sigaction(SIGUSR2, &sa, NULL);*/
  r += sigaction(SIGCHLD, &sa_c, NULL);
  r += sigaction(SIGSEGV, &sa_e, NULL);
  r += sigaction(SIGILL, &sa_e, NULL);
  r += sigaction(SIGFPE, &sa_e, NULL);
  r += sigaction(SIGBUS, &sa_e, NULL);
  r += sigaction(SIGTRAP, &sa_e, NULL);

  signal(SIGKILL, sig_handler);

  return r;
}

void
child_sig_handler(int signal, siginfo_t * si, void *p)
{
  switch (si->si_code)
    {
  case CLD_KILLED:
    fprintf(stderr, "NOTICE: child process caught SIGINT\n");
    break;
  case CLD_EXITED:
    break;
  default:

    fprintf(stderr, "NOTICE: child caught signal: %d\n", si->si_code);

    break;
    }
}

void
sig_handler(int signal)
{
  switch (signal)
    {
  case SIGUSR1:
    break;
  case SIGUSR2:
    break;
  case SIGTERM:
    fprintf(stderr, "NOTICE: caught SIGTERM, terminating gracefully\n");
    break;
  case SIGINT:
    ;

    fprintf(stderr, "NOTICE: caught SIGINT, signaling global kill..\n");
    break;

  default:
    //usleep(SIG_BREAK_TIMEOUT_NS);
    fprintf(stderr, "NOTICE: caught signal %d\n", signal);
    break;
    }
}
