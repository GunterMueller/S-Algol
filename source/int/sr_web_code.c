/******************************************************************
   sr_web_code.c

   functions for Web extensions to S-algol

   David Scott
   version 1: 30 May 96
*******************************************************************/


#include "sr_web_code.h"


/*** function daemonise - to be called on start up ***/

int daemonise()
{
 register int pid, fd;


  if ((pid = fork()) < 0) {
    fprintf (stderr,"sr_daemon: error on daemonise - fork failed\n");
    exit (1);
  }
/*
  else if (pid > 0)
    exit (0);          /* guarantees not a process group leader */

  if (setpgrp (0, getpid()) == -1) {     /* disassociate process group */
    fprintf (stderr,"sr_daemon: error on daemonise - can't change process group\n");
    exit (1);
  }

  if ((fd = open ("/dev/tty", O_RDWR)) > 0) {    /* lose controlling tty */
    ioctl (fd, TIOCNOTTY, (char *)NULL);
    close (fd);
  }

  for (fd = 0; fd < NOFILE; fd++)        /* close all open file descriptors */
    close (fd);

}



/*** function startup - to be called on start up ***/

int startup(char *argname)  /* pass in the filename to build socket names */
{
 struct sockaddr_un in_addr;



  /* create unique socket names */
  strcpy (input_socket_name, IN_PATH);
  strcat (input_socket_name, argname);
  strcpy (output_socket_name, OUT_PATH);
  strcat (output_socket_name, argname);


  input_sock = socket (AF_UNIX, SOCK_STREAM, 0);
  if (input_sock < 0) {
	fprintf (stderr,"sr_daemon: error opening input socket\n");
	exit (1);
  }

  in_addr.sun_family = AF_UNIX;
  strcpy (in_addr.sun_path, input_socket_name);

  if (bind (input_sock, (struct sockaddr *)&in_addr, sizeof (struct sockaddr_un)) < 0) {
	fprintf (stderr,"sr_daemon: error binding input socket\n");
	exit (1);
  }

  listen (input_sock, 5);

}




/*** function accept_input - to be called when S-algol has to read user input
                     also called by send_output() to read acknowledgement ***/

int accept_input()
{
 int msgsock, rval;
 char buf[BUFSIZE];



    msgsock = accept (input_sock, (struct sockaddr *)0, (int *)0);
    if (msgsock == -1) {
    	fprintf (stderr,"sr_daemon: error on accept\n");
       	exit (1);
    }
    else {
       	bzero (buf, sizeof(buf));
       	if ((rval = read(msgsock, buf, BUFSIZE)) < 0)
   	    fprintf (stderr,"sr_daemon: error reading message\n");
	else if (rval == 0)
	    fprintf (stderr,"sr_daemon: ending connection\n");
	else {
	    /* fprintf (stderr,"--->%s\n",buf); */
	    if (strcmp (buf, SR_ACK))     /* don't want to keep ack signal */
		strcpy (data_received, buf);
	}
    }
    close (msgsock);

}


/*** function send_output - to send a message to the client
                     message is S-algol output or a defined signal ***/

int send_output (char *mess , int length )   /* parameter is message to send */
{
 struct sockaddr_un out_addr;
 char buf[BUFSIZE];



  output_sock = socket (AF_UNIX, SOCK_STREAM, 0);
  if (output_sock < 0) {
    fprintf (stderr,"sr_daemon: error opening output socket\n");
    exit (1);
  }

  out_addr.sun_family = AF_UNIX;
  strcpy (out_addr.sun_path, output_socket_name);

  if (connect (output_sock, (struct sockaddr *)&out_addr, sizeof (struct sockaddr_un)) < 0) {
    close (output_sock);
    fprintf (stderr,"sr_daemon: error connecting output socket\n");
    exit (1);
  }

  if (write (output_sock, mess, length ) < 0)
    fprintf (stderr,"sr_daemon: error writing on output_socket\n");

  accept_input();    /* receive the acknowledgement from the client */
}



/*** function exit_bit - to be called when S-algol finishes ***/

int exit_bit()
{
  int	l ;

  l = strlen( SR_FINISHED ) ;
  send_output(SR_FINISHED,l);
  close (input_sock);
  unlink (input_socket_name);

 /* that's that */
}

