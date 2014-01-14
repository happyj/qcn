/*
 * ntp_filegen.c,v 3.12 1994/01/25 19:06:11 kardel Exp
 *
 *  implements file generations support for NTP
 *  logfiles and statistic files
 *
 *
 * Copyright (C) 1992, 1996 by Rainer Pruy
 * Friedrich-Alexander Universit�t Erlangen-N�rnberg, Germany
 *
 * This code may be modified and used freely
 * provided credits remain intact.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_string.h"
#include "ntp_calendar.h"
#include "ntp_filegen.h"
#include "ntp_stdlib.h"

/*
 * NTP is intended to run long periods of time without restart.
 * Thus log and statistic files generated by NTP will grow large.
 *
 * this set of routines provides a central interface 
 * to generating files using file generations
 *
 * the generation of a file is changed according to file generation type
 */


/*
 * redefine this if your system dislikes filename suffixes like
 * X.19910101 or X.1992W50 or ....
 */
#define SUFFIX_SEP '.'

static	void	filegen_open	(FILEGEN *, u_long);
static	int	valid_fileref	(const char *, const char *);
static	void	filegen_init	(const char *, const char *, FILEGEN *);
#ifdef	DEBUG
static	void	filegen_uninit		(FILEGEN *);
#endif	/* DEBUG */


/*
 * filegen_init
 */

static void
filegen_init(
	const char *	prefix,
	const char *	basename,
	FILEGEN *	fgp
	)
{
	fgp->fp       = NULL;
	fgp->prefix   = prefix;		/* Yes, this is TOTALLY lame! */
	fgp->basename = estrdup(basename);
	fgp->id       = 0;
	fgp->type     = FILEGEN_DAY;
	fgp->flag     = FGEN_FLAG_LINK; /* not yet enabled !!*/
}


/*
 * filegen_uninit - free memory allocated by filegen_init
 */
#ifdef DEBUG
static void
filegen_uninit(
	FILEGEN *	fgp
	)
{
	free(fgp->basename);
}
#endif


/*
 * open a file generation according to the current settings of gen
 * will also provide a link to basename if requested to do so
 */

static void
filegen_open(
	FILEGEN *	gen,
	u_long		newid
	)
{
	char *filename;
	char *basename;
	u_int len;
	FILE *fp;
	struct calendar cal;

	len = strlen(gen->prefix) + strlen(gen->basename) + 1;
	basename = emalloc(len);
	snprintf(basename, len, "%s%s", gen->prefix, gen->basename);
  
	switch (gen->type) {

	default:
		msyslog(LOG_ERR, 
			"unsupported file generations type %d for "
			"\"%s\" - reverting to FILEGEN_NONE",
			gen->type, basename);
		gen->type = FILEGEN_NONE;
		/* fall through to FILEGEN_NONE */

	case FILEGEN_NONE:
		filename = estrdup(basename);
		break;

	case FILEGEN_PID:
		filename = emalloc(len + 1 + 1 + 10);
		snprintf(filename, len + 1 + 1 + 10,
			 "%s%c#%ld",
			 basename, SUFFIX_SEP, newid);
		break;

	case FILEGEN_DAY:
		/*
		 * You can argue here in favor of using MJD, but I
		 * would assume it to be easier for humans to interpret
		 * dates in a format they are used to in everyday life.
		 */
		caljulian(newid, &cal);
		filename = emalloc(len + 1 + 4 + 2 + 2);
		snprintf(filename, len + 1 + 4 + 2 + 2,
			 "%s%c%04d%02d%02d",
			 basename, SUFFIX_SEP,
			 cal.year, cal.month, cal.monthday);
		break;

	case FILEGEN_WEEK:
		/*
		 * This is still a hack
		 * - the term week is not correlated to week as it is used
		 *   normally - it just refers to a period of 7 days
		 *   starting at Jan 1 - 'weeks' are counted starting from zero
		 */
		caljulian(newid, &cal);
		filename = emalloc(len + 1 + 4 + 1 + 2);
		snprintf(filename, len + 1 + 4 + 1 + 2,
			 "%s%c%04dw%02d",
			 basename, SUFFIX_SEP,
			 cal.year, cal.yearday / 7);
		break;

	case FILEGEN_MONTH:
		caljulian(newid, &cal);
		filename = emalloc(len + 1 + 4 + 2);
		snprintf(filename, len + 1 + 4 + 2,
			 "%s%c%04d%02d",
			 basename, SUFFIX_SEP, cal.year, cal.month);
		break;

	case FILEGEN_YEAR:
		caljulian(newid, &cal);
		filename = emalloc(len + 1 + 4);
		snprintf(filename, len + 1 + 4,
			 "%s%c%04d",
			 basename, SUFFIX_SEP, cal.year);
		break;

	case FILEGEN_AGE:
		filename = emalloc(len + 1 + 2 + 10);
		snprintf(filename, len + 1 + 2 + 10,
			 "%s%ca%08ld",
			 basename, SUFFIX_SEP, newid);
	}
  
	if (FILEGEN_NONE != gen->type) {
		/*
		 * check for existence of a file with name 'basename'
		 * as we disallow such a file
		 * if FGEN_FLAG_LINK is set create a link
		 */
		struct stat stats;
		/*
		 * try to resolve name collisions
		 */
		static u_long conflicts = 0;

#ifndef	S_ISREG
#define	S_ISREG(mode)	(((mode) & S_IFREG) == S_IFREG)
#endif
		if (stat(basename, &stats) == 0) {
			/* Hm, file exists... */
			if (S_ISREG(stats.st_mode)) {
				if (stats.st_nlink <= 1)	{
					/*
					 * Oh, it is not linked - try to save it
					 */
					char *savename;
					
					savename = emalloc(len + 1 + 1 + 10 + 10);
					snprintf(savename, len + 1 + 1 + 10 + 10,
						"%s%c%dC%lu",
						basename, SUFFIX_SEP,
						(int)getpid(), conflicts++);

					if (rename(basename, savename) != 0)
						msyslog(LOG_ERR,
							"couldn't save %s: %m",
							basename);
					free(savename);
				} else {
					/*
					 * there is at least a second link to
					 * this file.
					 * just remove the conflicting one
					 */
					if (
#if !defined(VMS)
						unlink(basename) != 0
#else
						delete(basename) != 0
#endif
						)
						msyslog(LOG_ERR, 
							"couldn't unlink %s: %m",
							basename);
				}
			} else {
				/*
				 * Ehh? Not a regular file ?? strange !!!!
				 */
				msyslog(LOG_ERR, 
					"expected regular file for %s "
					"(found mode 0%lo)",
					basename,
					(unsigned long)stats.st_mode);
			}
		} else {
			/*
			 * stat(..) failed, but it is absolutely correct for
			 * 'basename' not to exist
			 */
			if (ENOENT != errno)
				msyslog(LOG_ERR, "stat(%s) failed: %m",
						 basename);
		}
	}

	/*
	 * now, try to open new file generation...
	 */
	fp = fopen(filename, "a");
  
	DPRINTF(4, ("opening filegen (type=%d/id=%lu) \"%s\"\n",
		    gen->type, newid, filename));

	if (NULL == fp)	{
		/* open failed -- keep previous state
		 *
		 * If the file was open before keep the previous generation.
		 * This will cause output to end up in the 'wrong' file,
		 * but I think this is still better than losing output
		 *
		 * ignore errors due to missing directories
		 */

		if (ENOENT != errno)
			msyslog(LOG_ERR, "can't open %s: %m", filename);
	} else {
		if (NULL != gen->fp) {
			fclose(gen->fp);
			gen->fp = NULL;
		}
		gen->fp = fp;
		gen->id = newid;

		if (gen->flag & FGEN_FLAG_LINK) {
			/*
			 * need to link file to basename
			 * have to use hardlink for now as I want to allow
			 * gen->basename spanning directory levels
			 * this would make it more complex to get the correct
			 * filename for symlink
			 *
			 * Ok, it would just mean taking the part following
			 * the last '/' in the name.... Should add it later....
			 */

			/* Windows NT does not support file links -Greg Schueman 1/18/97 */

#if defined SYS_WINNT || defined SYS_VXWORKS
			SetLastError(0); /* On WinNT, don't support FGEN_FLAG_LINK */
#elif defined(VMS)
			errno = 0; /* On VMS, don't support FGEN_FLAG_LINK */
#else  /* not (VMS) / VXWORKS / WINNT ; DO THE LINK) */
			if (link(filename, basename) != 0)
				if (EEXIST != errno)
					msyslog(LOG_ERR, 
						"can't link(%s, %s): %m",
						filename, basename);
#endif /* SYS_WINNT || VXWORKS */
		}		/* flags & FGEN_FLAG_LINK */
	}			/* else fp == NULL */
	
	free(basename);
	free(filename);
	return;
}

/*
 * this function sets up gen->fp to point to the correct
 * generation of the file for the time specified by 'now'
 *
 * 'now' usually is interpreted as second part of a l_fp as is in the cal...
 * library routines
 */

void
filegen_setup(
	FILEGEN *	gen,
	u_long		now
	)
{
	u_long new_gen = ~ (u_long) 0;
	struct calendar cal;

	if (!(gen->flag & FGEN_FLAG_ENABLED)) {
		if (NULL != gen->fp) {
			fclose(gen->fp);
			gen->fp = NULL;
		}
		return;
	}
	
	switch (gen->type) {

	case FILEGEN_NONE:
		if (NULL != gen->fp)
			return; /* file already open */
		break;

	case FILEGEN_PID:
		new_gen = getpid();
		break;

	case FILEGEN_DAY:
		caljulian(now, &cal);
		cal.hour = cal.minute = cal.second = 0;
		new_gen = caltontp(&cal);
		break;

	case FILEGEN_WEEK:
		/* Would be nice to have a calweekstart() routine */
		/* so just use a hack ... */
		/* just round time to integral 7 day period for actual year  */
		new_gen = now - (now - calyearstart(now)) % TIMES7(SECSPERDAY)
			+ 60;
		/*
		 * just to be sure -
		 * the computation above would fail in the presence of leap seconds
		 * so at least carry the date to the next day (+60 (seconds))
		 * and go back to the start of the day via calendar computations
		 */
		caljulian(new_gen, &cal);
		cal.hour = cal.minute = cal.second = 0;
		new_gen = caltontp(&cal);
		break;

	case FILEGEN_MONTH:
		caljulian(now, &cal);
		cal.yearday = (u_short) (cal.yearday - cal.monthday + 1);
		cal.monthday = 1;
		cal.hour = cal.minute = cal.second = 0;
		new_gen = caltontp(&cal);
		break;

	case FILEGEN_YEAR:
		new_gen = calyearstart(now);
		break;

	case FILEGEN_AGE:
		new_gen = current_time - (current_time % SECSPERDAY);
		break;
	}
	/*
	 * try to open file if not yet open
	 * reopen new file generation file on change of generation id
	 */
	if (NULL == gen->fp || gen->id != new_gen) {

		DPRINTF(1, ("filegen  %0x %lu %lu %lu\n", 
			    gen->type, now, gen->id, new_gen));

		filegen_open(gen, new_gen);
	}
}


/*
 * change settings for filegen files
 */
void
filegen_config(
	FILEGEN *	gen,
	const char *	basename,
	u_int		type,
	u_int		flag
	)
{
	int file_existed = 0;

	/*
	 * if nothing would be changed...
	 */
	if ((strcmp(basename, gen->basename) == 0) && type == gen->type
	    && flag == gen->flag)
		return;
  
	/*
	 * validate parameters
	 */
	if (!valid_fileref(gen->prefix, basename))
		return;
  
	if (NULL != gen->fp) {
		fclose(gen->fp);
		gen->fp = NULL;
		file_existed = 1;
	}

	DPRINTF(3, ("configuring filegen:\n"
		    "\tprefix:\t%s\n"
		    "\tbasename:\t%s -> %s\n"
		    "\ttype:\t%d -> %d\n"
		    "\tflag: %x -> %x\n",
		    gen->prefix,
		    gen->basename, basename,
		    gen->type, type,
		    gen->flag, flag));

	if (strcmp(gen->basename, basename) != 0) {
		free(gen->basename);
		gen->basename = estrdup(basename);
	}
	gen->type = (u_char)type;
	gen->flag = (u_char)flag;

	/*
	 * make filegen use the new settings
	 * special action is only required when a generation file
	 * is currently open
	 * otherwise the new settings will be used anyway at the next open
	 */
	if (file_existed) {
		l_fp now;

		get_systime(&now);
		filegen_setup(gen, now.l_ui);
	}
}


/*
 * check whether concatenating prefix and basename
 * yields a legal filename
 */
static int
valid_fileref(
	const char *	prefix,
	const char *	basename
	)
{
	/*
	 * prefix cannot be changed dynamically
	 * (within the context of filegen)
	 * so just reject basenames containing '..'
	 *
	 * ASSUMPTION:
	 * 		file system parts 'below' prefix may be
	 *		specified without infringement of security
	 *
	 *		restricting prefix to legal values
	 *		has to be ensured by other means
	 * (however, it would be possible to perform some checks here...)
	 */
	register const char *p = basename;
  
	/*
	 * Just to catch, dumb errors opening up the world...
	 */
	if (NULL == prefix || '\0' == *prefix)
		return 0;

	if (NULL == basename)
		return 0;
  
	for (p = basename; p; p = strchr(p, DIR_SEP)) {
		if ('.' == p[0] && '.' == p[1] 
		    && ('\0' == p[2] || DIR_SEP == p[2]))
			return 0;
	}
  
	return 1;
}


/*
 * filegen registry
 */

static struct filegen_entry {
	char *			name;
	FILEGEN *		filegen;
	struct filegen_entry *	next;
} *filegen_registry = NULL;


FILEGEN *
filegen_get(
	const char *	name
	)
{
	struct filegen_entry *f = filegen_registry;

	while (f) {
		if (f->name == name || strcmp(name, f->name) == 0) {
			DPRINTF(4, ("filegen_get(%s) = %p\n",
				    name, f->filegen));
			return f->filegen;
		}
		f = f->next;
	}
	DPRINTF(4, ("filegen_get(%s) = NULL\n", name));
	return NULL;
}


void
filegen_register(
	const char *	prefix,
	const char *	name,
	FILEGEN *	filegen
	)
{
	struct filegen_entry **ppfe;

	DPRINTF(4, ("filegen_register(%s, %p)\n", name, filegen));

	filegen_init(prefix, name, filegen);

	ppfe = &filegen_registry;
	while (NULL != *ppfe) {
		if ((*ppfe)->name == name 
		    || !strcmp((*ppfe)->name, name)) {

			DPRINTF(5, ("replacing filegen %p\n",
				    (*ppfe)->filegen));

			(*ppfe)->filegen = filegen;
			return;
		}
		ppfe = &((*ppfe)->next);
	}

	*ppfe = emalloc(sizeof **ppfe);

	(*ppfe)->next = NULL;
	(*ppfe)->name = estrdup(name);
	(*ppfe)->filegen = filegen;

	DPRINTF(6, ("adding new filegen\n"));
	
	return;
}


/*
 * filegen_unregister frees memory allocated by filegen_register for
 * name.
 */
#ifdef DEBUG
void
filegen_unregister(
	char *name
	)
{
	struct filegen_entry **	ppfe;
	struct filegen_entry *	pfe;
	FILEGEN *		fg;
			
	DPRINTF(4, ("filegen_unregister(%s)\n", name));

	ppfe = &filegen_registry;

	while (NULL != *ppfe) {
		if ((*ppfe)->name == name
		    || !strcmp((*ppfe)->name, name)) {
			pfe = *ppfe;
			*ppfe = (*ppfe)->next;
			fg = pfe->filegen;
			free(pfe->name);
			free(pfe);
			filegen_uninit(fg);
			break;
		}
		ppfe = &((*ppfe)->next);
	}
}	
#endif	/* DEBUG */