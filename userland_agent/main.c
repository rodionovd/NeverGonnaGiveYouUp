//
//  main.c
//  userland_agent
//
//  Created by Dmitry Rodionov on 12/5/15.
//  Copyright © 2015 Internals Exposed. All rights reserved.
//
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <signal.h>


void noop()
{
}

int main(int argc, const char * argv[])
{
    // Disable ^C and ^Z when launched from command line.
    // Yep, it's a dirty hack, but nobody runs agents this way so ¯\_(ツ)_/¯
    signal(SIGINT, noop);
    signal(SIGSTOP, noop);
    signal(SIGTSTP, noop);

    while (true) {
        sleep(1);
        printf("Gonna kill me? ;-D\n");
    }
    return 0;
}
