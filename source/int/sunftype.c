#include "defines.h"

psint Nftype( psint fd )						/* what type is the open file fd */
							/* -1 error */
{
	struct stat finfo ;

	if ( fstat( ( int ) fd,&finfo ) != 0 ) return( ( psint ) -1 ) ;
	switch( finfo.st_mode & S_IFMT )			/* 1st test the UNIX file type */
	{
	case S_IFIFO:	return( SOCKET_FLAG ) ; 		/* treat pipes as sockets */
	case	0:	switch( ( finfo.st_rdev >> 8 ) & 255 )	/* assume it may be a raster device - SunOs 4.1 bug */
			{
#ifdef	 NOTUSED
			case 22:				/* default frame buffer - a raster device */	
			case 26:				/* extra frame buffers */
			case 31:
			case 32:
			case 64:
			case 67:
			case 68:
			case 27:				/* black white fb - a raster device */	
			case 39:				/* colour frame buffer - a raster device */	
			case 55:	return( RASTER_FLAG ) ;	/* 386i colour frame buffer - a raster device */	
#endif	/*NOTUSED*/
			case 37:
			case 69:
			case 106:	return( DISK_FLAG ) ;	/* an audio file - pretend its a disk */
			default:	return( ( psint ) -1 ) ;	/* don't know - or not sure */
			}
	case S_IFCHR:	switch( ( finfo.st_rdev >> 8 ) & 255 )	/* what is the major device no. for this character special file */
			{
			case 0:				/* the console - a tty */	
			case 12:				/* rs232 line - a tty */	
			case 20:				/* pseudo tty - a tty */	
			case 21:				/* slave pseudo tty - a tty */	
			case 2:		return( TTY_FLAG ) ;	/* /dev/tty - a tty */	
			case 8:		return( DISK_FLAG ) ;	/* the cartridge tape - a disk file? */	
#ifdef	 NOTUSED
			case 13:	return( MOUSE_FLAG ) ;	/* the mouse */	
			case 22:				/* default frame buffer - a raster device */	
			case 26:				/* extra frame buffers */
			case 31:
			case 32:
			case 64:
			case 67:
			case 68:
			case 27:				/* black white fb - a raster device */	
			case 39:				/* colour frame buffer - a raster device */	
			case 55:	return( RASTER_FLAG ) ;	/* 386i colour frame buffer - a raster device */	
#endif	/*NOTUSED*/
			case 37:
			case 69:
			case 106:	return( DISK_FLAG ) ;	/* an audio file - pretend its a disk */
			default:	return( ( psint ) -1 ) ;	/* don't know - or not sure */
			}
	case S_IFDIR:						/* don't want directories */
	case S_IFBLK:	return( ( psint ) -1 ) ;		/* don't want raw disks */
	case S_IFREG:	return( DISK_FLAG ) ;			/* a disk file */
	case S_IFSOCK:	return( SOCKET_FLAG ) ;			/* a socket */
	case S_IFLNK:						/* don't want to know about symbolic links */
	default:	return( ( psint ) -1 ) ;		/* don't know what type of file it is */
	}
}
