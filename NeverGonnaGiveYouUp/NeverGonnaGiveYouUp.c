//
//  NeverGonnaGiveYouUp.c
//  NeverGonnaGiveYouUp
//
//  Created by Dmitry Rodionov on 12/6/15.
//  Copyright © 2015 Internals Exposed. All rights reserved.
//

#include "NeverGonnaGiveYouUp.h"

// +1 for a null terminator
#define kMaxProcessNameLength (MAXCOMLEN+1)

kern_return_t NeverGonnaGiveYouUp_start(kmod_info_t *ki, void *data)
{
    return mac_policy_register(&nggyu_policy_conf, &nggyu_policy_handler, data);
}

kern_return_t NeverGonnaGiveYouUp_stop(kmod_info_t *ki, void *data)
{
    return mac_policy_unregister(nggyu_policy_handler);
}

// Don't take this code too seriously because it's just a PoC. One could easily cherry-pick
// signals to allow/deny.
int signal_means_termination(int signum)
{
    int whitelist[] = {SIGURG, SIGCONT, SIGCHLD, SIGIO, SIGWINCH, SIGINFO};
    for (int i = 0; i < sizeof(whitelist)/sizeof(whitelist[0]); i++) {
        if (signum == whitelist[i]) return 0;
    }
    return 1;
}

int proc_is_agent(proc_t target)
{
    // NOTE: proc_name() is a simple wrapper around struct proc's p_comm field which is limited
    // to MAXCOMLEN bytes (~16 ascii characters). Thus if the target has a longer name,
    // we'll only get the first MAXCOMLEN bytes of it. Thus use strcmp() carefully.
    char name[kMaxProcessNameLength] = {0};
    proc_name(proc_pid(target), name, kMaxProcessNameLength);
    size_t name_len = strlen(name);
    return (name_len > 0 && strncmp(name, kUserlandAgentName, name_len) == 0);
}

#pragma mark - Policy Hooks

static int nggyu_proc_check_signal(kauth_cred_t cred, proc_t target, int signum)
{
    // Step 1: See if the signal we gonna audit is destructive and fast-forward if it's not
    if (!signal_means_termination(signum)) {
        return 0;
    }
    // Step 2: Verify that a target of the signal is our userland agent.
    if (proc_is_agent(target) == 1) {
        // Step 2.1: So someone tries to terminate the agent. We refuse to deliver such signal
        printf("NGGYU: User %d tried to kill our agent with signal %d. What a pity!\n",
               cred->cr_posix.cr_uid, signum);
        // The error may be anything > 0
        return EAGAIN;
    }
    return 0;
}

static int nggyu_proc_check_expose_task(kauth_cred_t cred, proc_t target)
{
    return proc_is_agent(target) ? EPERM : 0;
}

static int nggyu_proc_check_debug(kauth_cred_t cred, proc_t target)
{
    return proc_is_agent(target) ? EPERM : 0;
}

static int nggyu_proc_check_get_task_name(kauth_cred_t cred, proc_t target)
{
    return proc_is_agent(target) ? EPERM : 0;
}

static int nggyu_proc_check_get_task(kauth_cred_t cred, proc_t target)
{
    return proc_is_agent(target) ? EPERM : 0;
}
