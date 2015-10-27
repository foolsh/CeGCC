#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "sys/wcetrace.h"

#include "sys/shared.h"
#include "sys/mqueue.h"
#include "sys/spawn.h"
#include "sys/fifo.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define SYNC CacheSync(CACHE_SYNC_DISCARD|CACHE_SYNC_INSTRUCTIONS)

/*
* Synchronizing with events sometimes fails! I get an event, but avail count 
* is zero!  Seems that the compiler does not store it or there are problems
* with cache...  CacheSync() seems to fix the problem...
*/

#define MAPNAME    "shmblk"
#define MUTEXNAME  "shmblk:mutex"

_SHMBLK
_shared_init(int pgid)
{
	_SHMBLK shmblk;
	HANDLE mapHnd;
	DWORD winerr = 0;
	BOOL  new = FALSE;
	char  name[SYSNAMELEN];
	wchar_t nameW[SYSNAMELEN];

	/* pgid is zero: set up a new process group */
	if (pgid == 0) {
		pgid = setpgid(0, 0);
	}

	sprintf(name, "%d:%s", pgid, MAPNAME);
	mbstowcs(nameW, name, strlen(name)+1);
	SetLastError(0);

	mapHnd = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, 
		PAGE_READWRITE|SEC_COMMIT,	0, sizeof(_shmblk_t), nameW);

	if (mapHnd == NULL) {
		winerr = GetLastError();
		WCETRACE(WCE_IO, "_shared_init: FATAL CreateFileMappingW fails winerr %d", winerr);
		exit(1);
	}

	if ((winerr = GetLastError()) != ERROR_ALREADY_EXISTS) {
		WCETRACE(WCE_IO, "shared: creating NEW process group %d", pgid);
		new = TRUE;
	} else {
		WCETRACE(WCE_IO, "shared: map to EXISTING process group %d", pgid);
	}

	shmblk = MapViewOfFile(mapHnd, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(_shmblk_t));

	if (shmblk == NULL) {
		winerr = GetLastError();
		WCETRACE(WCE_IO, "_shared_init: FATAL MapViewOfFile fails winerr %d", winerr);
		exit(1);
	}

	if (new) {
		memset(shmblk, 0, sizeof(_shmblk_t));
		shmblk->pginfo.pgid = pgid;
		shmblk->pginfo.showwindow = TRUE;
	}

	WCETRACE(WCE_IO, "_shared_init: shmblk @%x (pgid %d)\n", shmblk, pgid);

	/*  SYNC; */
	return(shmblk);
}

void
_shared_dump(_SHMBLK shmblk)
{
	_PGINFO pgi = &shmblk->pginfo;

	WCETRACE(WCE_IO, "SHMBLK DUMP:");
	WCETRACE(WCE_IO, "mutex %p", shmblk->mutex);
	WCETRACE(WCE_IO, "pgid %d cwd %s", pgi->pgid, pgi->cwd);
	WCETRACE(WCE_IO, "fds: %d %d %d", pgi->stdinfd, pgi->stdoutfd, pgi->stderrfd);
}

void
_shared_reset(_SHMBLK shmblk)
{
	if (shmblk) {
		memset(&shmblk->pginfo, 0, sizeof(_pginfo_t));
		shmblk->pginfo.showwindow = TRUE;
	}
}

void
_shared_setshowwindow(_SHMBLK shmblk, BOOL show)
{
	WCETRACE(WCE_IO, "_shared_setshowwindow CALLED w/%d", show);

	if (shmblk) {
		shmblk->pginfo.showwindow = show;
	}
}

void
_shared_getcwd(_SHMBLK shmblk, char *cwd)
{
	if (shmblk == NULL)	
		return;

	strcpy(cwd, shmblk->pginfo.cwd);
}

void
_shared_setcwd(_SHMBLK shmblk, char *cwd)
{
	if (shmblk) {
		strcpy(shmblk->pginfo.cwd, cwd);
	}
}

int
_shared_getpgid(_SHMBLK shmblk)
{
	if (shmblk == NULL)
		return(0);

	return(shmblk->pginfo.pgid);
}

void
_shared_setpgid(_SHMBLK shmblk, int pgid)
{
	if (shmblk != NULL) {
		shmblk->pginfo.pgid = pgid;
	}
}

int
_shared_getstdinfd(_SHMBLK shmblk)
{
	if (shmblk == NULL)
		return(0);

	return(shmblk->pginfo.stdinfd);
}

void
_shared_setstdinfd(_SHMBLK shmblk, int fd)
{
	if (shmblk != NULL) {
		shmblk->pginfo.stdinfd = fd;
	}
}

int
_shared_getstdoutfd(_SHMBLK shmblk)
{
	if (shmblk == NULL)
		return(0);

	return(shmblk->pginfo.stdoutfd);
}

void
_shared_setstdoutfd(_SHMBLK shmblk, int fd)
{
	if (shmblk != NULL) {
		shmblk->pginfo.stdoutfd = fd;
	}
}

int
_shared_getstderrfd(_SHMBLK shmblk)
{
	if (shmblk == NULL)
		return(0);

	return(shmblk->pginfo.stderrfd);
}

void
_shared_setstderrfd(_SHMBLK shmblk, int fd)
{
	if (shmblk != NULL) {
		shmblk->pginfo.stderrfd = fd;
	}
}

void
_shared_setenvironblk(_SHMBLK shmblk, char **env)
{
	char *d;
	int i, len;
	char *endp;

	if (shmblk == NULL) {
		return;
	}

	endp = shmblk->pginfo.environ + MAX_ENVIRONBLK;

	for (i = 0, d = shmblk->pginfo.environ; env[i] != NULL; i++) {
		len = strlen(env[i]);
		if (d + len >= endp)	{
			WCETRACE(WCE_IO, "_shared_setenvironblk: "
				 "FATAL space exhausted (max %d)",
				MAX_ENVIRONBLK);
			exit(1);
		}
		memcpy(d, env[i], len + 1);
		d += len + 1;
	}
	*d = 0;
}

int
_shared_getenvironblk(_SHMBLK shmblk, char **env)
{
	char *s;
	int i, len;

	if (shmblk == NULL) {
		return 0;
	}

	for (i = 0, s = shmblk->pginfo.environ; *s; i++) {
            s += strlen(s) + 1;
        }

        if (i) {
            len = s - shmblk->pginfo.environ + 1;
            *env = malloc(len);
            if (*env == NULL) {
                WCETRACE(WCE_IO, "_shared_getenvironblk: "
			 "FATAL ERROR malloc failed for %d more bytes", len);
                exit(1);
            }

            memcpy(*env, shmblk->pginfo.environ, len);
        }

        return i;
}

BOOL
_shared_getshowwindow(_SHMBLK shmblk)
{
#if 0 /* WHY?? */
	// called before xce_init() in console... dont want to
	// recompile all progs...
	if(shmblk == NULL)
		xceshared_init();
#endif

	return(shmblk->pginfo.showwindow);  
}

BOOL
_shared_lock(_SHMBLK shmblk)
{
	wchar_t nameW[SYSNAMELEN];
	char    name[SYSNAMELEN];

	if (shmblk->mutex == NULL) {
		sprintf(name, "%d:%s", shmblk->pginfo.pgid, MUTEXNAME);
		mbstowcs(nameW, name, strlen(name));
		if ((shmblk->mutex = CreateMutexW(NULL, FALSE, nameW)) == NULL) {
			return(FALSE);
		}
	}

	WaitForSingleObject(shmblk->mutex, INFINITE);
	return(TRUE);
}

BOOL
_shared_release(_SHMBLK shmblk)
{
	if (shmblk == NULL) {
		return(FALSE);
	}

	return(ReleaseMutex(shmblk->mutex));
}

static int attached = 0;

int
_shared_attach()
{
	if (attached)
		return;
	++attached;
	return(0);
}

int
_shared_dettach()
{
	if (!attached)
		return;
	--attached;

#if 0
/* pedro: must look at celibs ceshared2.c */

	/* TODO: We should also close the event handles we have
	* allocated in CreatePipe()
	*
	* We should also have a table of other pipes,
	* not only standard handles... (rk)
	*/
	if(ISPIPEHANDLE(GetStdHandle(STD_INPUT_HANDLE)))
		CloseHandle(GetStdHandle(STD_INPUT_HANDLE));
	if(ISPIPEHANDLE(GetStdHandle(STD_OUTPUT_HANDLE)))
		CloseHandle(GetStdHandle(STD_OUTPUT_HANDLE));
	if(ISPIPEHANDLE(GetStdHandle(STD_ERROR_HANDLE)))
		CloseHandle(GetStdHandle(STD_ERROR_HANDLE));
#endif

	return 0;
}
