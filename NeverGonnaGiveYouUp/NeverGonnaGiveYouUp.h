//
//  NeverGonnaGiveYouUp.h
//  NeverGonnaGiveYouUp
//
//  Created by Dmitry Rodionov on 12/5/15.
//  Copyright © 2015 Internals Exposed. All rights reserved.
//

#ifndef NeverGonnaGiveYouUp_hpp
    #define NeverGonnaGiveYouUp_hpp

#include <mach/mach_types.h>
#include <sys/types.h>
#include <sys/kernel.h>
#include <sys/kauth.h>
#include <sys/malloc.h>
#include <sys/proc.h>
#include <libkern/libkern.h>
#include <sys/signal.h>
//#define CONFIG_MACF 1
#include <security/mac_framework.h>
#include <security/mac.h>
#include <security/mac_policy.h>

// The module entry & exit points
kern_return_t NeverGonnaGiveYouUp_start(kmod_info_t *ki, void *data);
kern_return_t NeverGonnaGiveYouUp_stop(kmod_info_t *ki, void *data);

// Our hook for signal(): we'll refuse to deliver any destructive signals to our userland agent
static int nggyu_proc_check_signal(kauth_cred_t cred, proc_t target, int signum);
// This agent name
const char *kUserlandAgentName = "nggyu.agent";

// A few optional hooks to disable "debugging" the agent
static int nggyu_proc_check_debug(kauth_cred_t cred, proc_t target);
static int nggyu_proc_check_get_task_name(kauth_cred_t cred, proc_t target);
static int nggyu_proc_check_get_task(kauth_cred_t cred, proc_t target);
static int nggyu_proc_check_expose_task(kauth_cred_t cred, proc_t target);

static struct mac_policy_ops nggyu_policy_ops = {
    .mpo_proc_check_signal        = nggyu_proc_check_signal,
    .mpo_proc_check_debug         = nggyu_proc_check_debug,
    .mpo_proc_check_expose_task   = nggyu_proc_check_expose_task,
    .mpo_proc_check_get_task_name = nggyu_proc_check_get_task_name,
    .mpo_proc_check_get_task      = nggyu_proc_check_get_task
};

static struct mac_policy_conf nggyu_policy_conf = {
    .mpc_name            = "nggyu",
    .mpc_fullname        = "Never Gonna Give You Up Kernel Module",
    .mpc_labelnames      = NULL,
    .mpc_labelname_count = 0,
    .mpc_ops             = &nggyu_policy_ops,
    // Make this policy unloadable for testing purposes
    .mpc_loadtime_flags  = MPC_LOADTIME_FLAG_UNLOADOK,
    .mpc_field_off       = NULL,
    .mpc_runtime_flags   = 0
};

static mac_policy_handle_t nggyu_policy_handler = 0;

#endif
