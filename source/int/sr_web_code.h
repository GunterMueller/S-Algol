/******************************************************************
   sr_web_code.h

   header file for Web extensions to S-algol

   David Scott
   version 1: 30 May 96
*******************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <sys/fcntlcom.h>
#include <sys/param.h>



#define IN_PATH "/user/staff/dscott/cgi-bin/s.in_"
#define OUT_PATH "/user/staff/dscott/cgi-bin/s.out_"

#define SR_INPUT_WANTED "My Mr. Hobart tinkers with time ..."
#define SR_FINISHED "... just as time has tinkered with Mr. Hobart"  /* grin */
#define SR_ACK "sr_client sending acknowledgement to sr_daemon 9876543210"

#define BUFSIZE 1024



/* function declarations */
int daemonise (void);
int startup (char *);
int accept_input (void);
int send_output (char *,int);
int exit_bit (void);


/* globals */
int input_sock, output_sock;
char data_received[BUFSIZE], input_socket_name[50], output_socket_name[50];


