#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <ctype.h> 
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <poll.h> 

#ifdef KARS_PBX

#include "kars_utils/kars_cli.h"

#else

#include "kars_cli.h"

#endif

static KS_LIST_HEAD_STATIC(helpers, ks_cli_entry);

static char help_help[] =
"Usage: help [topic]\r\n"
"       When called with a topic as an argument, displays usage\r\n"
"       information on the given command. If called without a\r\n"
"       topic, it provides a list of commands.\r\n";


static int handle_foo(int fd, int argc, char *argv[]);
static int handle_help(int fd, int argc, char *argv[]);
static struct ks_cli_entry *cli_next(struct cli_iterator *i);
static char *find_best(char *argv[]);
static struct ks_cli_entry *find_cli(char *cmds[], int match_type);
static void ks_join(char *s, size_t len, char * const w[]);
static char *parse_args(const char *s, int *argc, char *argv[], int max, int *trailingwhitespace);
static char *__ks_cli_generator(const char *text, const char *word, int state, int lock);

static struct ks_cli_entry builtins[] = {
	/* Keep alphabetized, with longer matches first (example: abcd before abc) */
	{ { "_command", "complete", NULL },
	handle_foo, "Command complete", ""},

	{ { NULL }, NULL, NULL, NULL}
};


static struct ks_cli_entry cli_cli[] = {
	{ { "help", NULL },
	handle_help, "Display help list, or specific help on a command",
	help_help },
};

void cli_init(void)
{
	struct ks_cli_entry *e;

	for (e = builtins; e->cmda[0] != NULL; e++) {
		char buf[80];
		ks_join(buf, sizeof(buf), e->cmda);
		e->_full_cmd = strdup(buf);
		if (!e->_full_cmd)
		{
			fprintf( stderr, "-- cannot allocate <%s>\n", buf);
		}
	}

	cli_register_multiple(cli_cli, sizeof(cli_cli) / sizeof(cli_cli[0]));
}


static int help1(int fd, char *match[], int locked)
{
	char matchstr[80] = "";
	struct ks_cli_entry *e;
	int len = 0;
	int found = 0;
	struct cli_iterator i = { NULL, NULL};

	if (match) {
		ks_join(matchstr, sizeof(matchstr), match);
		len = strlen(matchstr);
	}
	
	while ( (e = cli_next(&i)) ) {
		
		/* Hide commands that start with '_' */
		if ((e->_full_cmd) && (e->_full_cmd[0] == '_'))
			continue;

		if (match && e->_full_cmd && strncasecmp(matchstr, e->_full_cmd, len))
			continue;
		
		ks_cli(fd, "%25.25s  %s\r\n", S_OR(e->_full_cmd, ""), S_OR(e->summary, ""));
		found++;
	}

	if (!found && matchstr[0])
		ks_cli(fd, "No such command '%s'.\n", matchstr);
			
	return RESULT_SUCCESS;
}

static int help_workhorse(int fd, char *match[])
{
	return help1(fd, match, 0 /* do not print errors */);
}

static int handle_foo(int fd, int argc, char *argv[])
{	
	return RESULT_SUCCESS;
}

static int handle_help(int fd, int argc, char *argv[])
{
	char fullcmd[80];
	struct ks_cli_entry *e;
	int res = RESULT_SUCCESS;
	
	if (argc < 1)
		return RESULT_SHOWUSAGE;
		
	if (argc == 1)
		return help_workhorse(fd, NULL);
		
	e = find_cli(argv + 1, 1);	/* try exact match first */
	if (!e) {
		res = help1(fd, argv + 1, 1 /* locked */);
		return res;
	}
	
	
	if (e->usage)
		ks_cli(fd, "%s", e->usage);
	else {
		ks_join(fullcmd, sizeof(fullcmd), argv+1);
		ks_cli(fd, "No help text available for '%s'.\n", fullcmd);
	}
	return res;
}
/* -------------------------------------------------------------------------- */




void cli_register_multiple(struct ks_cli_entry *e, int len)
{
	int i;

	for (i = 0; i < len; i++)
		cli_register(e + i);
}
/* -------------------------------------------------------------------------- */
int cli_register(struct ks_cli_entry *e)
{
	struct ks_cli_entry *cur;
	char fulle[80] ="";
	int lf, ret = -1;
	
	ks_join(fulle, sizeof(fulle), e->cmda);
	
	if (find_cli(e->cmda, 1)) {
		fprintf( stderr, "Command '%s' already registered (or something close enough)\n", fulle);
		goto done;
	}
	e->_full_cmd = strdup(fulle);
	if (!e->_full_cmd)
		goto done;

	lf = strlen(fulle);
	KS_LIST_TRAVERSE_SAFE_BEGIN(&helpers, cur, list) {
		int len = strlen(cur->_full_cmd);
		if (lf < len)
			len = lf;
		if (strncasecmp(fulle, cur->_full_cmd, len) < 0) {
			KS_LIST_INSERT_BEFORE_CURRENT(&helpers, e, list); 
			break;
		}
	}
	KS_LIST_TRAVERSE_SAFE_END;

	if (!cur)
	{
		KS_LIST_INSERT_TAIL(&helpers, e, list); 
	}
	ret = 0;	/* success */

done:
	
	return ret;
}

/* -------------------------------------------------------------------------- */
void ks_cli(int fd, char *fmt, ...)
{
	int n, size = 256;
	int ntry = 15;
	char *p, *p_new;
	va_list ap;
	if ((p = malloc (size)) == NULL)
		return;
	while (ntry-- > 0) 
	{
		va_start(ap, fmt);
		n = vsnprintf (p, size, fmt, ap);
		va_end(ap);
		if (n > -1 && n < size)
		{
			write(fd, p, n);
			ntry = 0;
		}
		else
		{
			if (n > -1) 
				size = n+1; 
			else
				size *= 2;  
			if ((p_new = realloc (p, size)) == NULL)
			{
				if (p)
					free(p);
				return;
			}
			p = p_new;
		}
	}	
	free(p);
}
/* -------------------------------------------------------------------------- */
static char *parse_args(const char *s, int *argc, char *argv[], int max, int *trailingwhitespace)
{
	char *dup, *cur;
	int x = 0;
	int quoted = 0;
	int escaped = 0;
	int whitespace = 1;

	*trailingwhitespace = 0;
	if (s == NULL)	/* invalid, though! */
		return NULL;
	/* make a copy to store the parsed string */
	if (!(dup = strdup(s)))
		return NULL;

	cur = dup;
	/* scan the original string copying into cur when needed */
	for (; *s ; s++) {
		if (x >= max - 1) {
			
			break;
		}
		if (*s == '"' && !escaped) {
			quoted = !quoted;
			if (quoted && whitespace) {
				/* start a quoted string from previous whitespace: new argument */
				argv[x++] = cur;
				whitespace = 0;
			}
		} else if ((*s == ' ' || *s == '\t') && !(quoted || escaped)) {
			/* If we are not already in whitespace, and not in a quoted string or
			   processing an escape sequence, and just entered whitespace, then
			   finalize the previous argument and remember that we are in whitespace
			*/
			if (!whitespace) {
				*cur++ = '\0';
				whitespace = 1;
			}
		} else if (*s == '\\' && !escaped) {
			escaped = 1;
		} else {
			if (whitespace) {
				/* we leave whitespace, and are not quoted. So it's a new argument */
				argv[x++] = cur;
				whitespace = 0;
			}
			*cur++ = *s;
			escaped = 0;
		}
	}
	/* Null terminate */
	*cur++ = '\0';
	/* XXX put a NULL in the last argument, because some functions that take
	 * the array may want a null-terminated array.
	 * argc still reflects the number of non-NULL entries.
	 */
	argv[x] = NULL;
	*argc = x;
	*trailingwhitespace = whitespace;
	return dup;
}
/* -------------------------------------------------------------------------- */

static struct ks_cli_entry *cli_next(struct cli_iterator *i)
{
	struct ks_cli_entry *e;


	if (i->builtins == NULL && i->helpers == NULL) {
		/* initialize */
		i->builtins = builtins;
		i->helpers = KS_LIST_FIRST(&helpers);
	}
	e = i->builtins; /* temporary */
	if (!e->cmda[0] || (i->helpers && i->helpers->_full_cmd && e->_full_cmd &&
		    strcmp(i->helpers->_full_cmd, e->_full_cmd) < 0)) {
		/* Use helpers */
		e = i->helpers;
		if (e)
			i->helpers = KS_LIST_NEXT(e, list);
	} else { /* use builtin. e is already set  */
		(i->builtins)++;	/* move to next */
	}
	
	return e;
}

/* -------------------------------------------------------------------------- */
static void ks_join(char *s, size_t len, char * const w[])
{
	int x, ofs = 0;
	const char *src;

	/* Join words into a string */
	if (!s)
		return;
	for (x = 0; ofs < len && w[x]; x++) {
		if (x > 0)
			s[ofs++] = ' ';
		for (src = w[x]; *src && ofs < len; src++)
			s[ofs++] = *src;
	}
	if (ofs == len)
		ofs--;
	s[ofs] = '\0';
}
/* -------------------------------------------------------------------------- */
static struct ks_cli_entry *find_cli(char *cmds[], int match_type)
{
	int matchlen = -1;	/* length of longest match so far */
	struct ks_cli_entry *cand = NULL, *e=NULL;
	struct cli_iterator i = {NULL, NULL};

	while( (e = cli_next(&i)) ) {
		
		int y, res;
		for (y = 0 ; cmds[y] && e->cmda[y]; y++) {
			res = strcasecmp(e->cmda[y], cmds[y]);
			if (res)
			{
				break;
			}
		}
		if (e->cmda[y] == NULL) {	/* no more words in candidate */
			if (cmds[y] == NULL)	/* this is an exact match, cannot do better */
				break;
			/* here the search key is longer than the candidate */
			if (match_type != 0)	/* but we look for almost exact match... */
				continue;	/* so we skip this one. */
			/* otherwise we like it (case 0) */
		} else {			/* still words in candidate */
			if (cmds[y] == NULL)	/* search key is shorter, not good */
				continue;
			/* if we get here, both words exist but there is a mismatch */
			if (match_type == 0)	/* not the one we look for */
				continue;
			if (match_type == 1)	/* not the one we look for */
				continue;
			if (cmds[y+1] != NULL || e->cmda[y+1] != NULL)	/* not the one we look for */
				continue;
			/* we are in case match_type == -1 and mismatch on last word */
		}
		if (y > matchlen) {	/* remember the candidate */
			matchlen = y;
			cand = e;
		}
	}
	return e ? e : cand;
}
/* -------------------------------------------------------------------------- */
static char *find_best(char *argv[])
{
	static char cmdline[80];
	int x;
	/* See how close we get, then print the candidate */
	char *myargv[KS_MAX_CMD_LEN];
	for (x=0;x<KS_MAX_CMD_LEN;x++)
		myargv[x]=NULL;
	
	for (x=0;argv[x];x++) {
		myargv[x] = argv[x];
		if (!find_cli(myargv, -1))
			break;
	}
	
	ks_join(cmdline, sizeof(cmdline), myargv);
	return cmdline;
}
/* -------------------------------------------------------------------------- */
int ks_cli_command(int fd, const char *s)
{
	char *argv[KS_MAX_ARGS];
	struct ks_cli_entry *e;
	int x, res = 0;
	char *dup;
	int tws;
	
	if (!(dup = parse_args(s, &x, argv, sizeof(argv) / sizeof(argv[0]), &tws)))
		return -1;

	/* We need at least one entry, or ignore */
	if (x > 0) {

		e = find_cli(argv, 0);
		if (e) {
			e->inuse++;
			switch(e->handler(fd, x, argv)) 
			{
			case RESULT_SHOWUSAGE:
				if (e->usage)
					ks_cli(fd, "%s", e->usage);
				else
					ks_cli(fd, "Invalid usage, but no usage information available.\n");
				break;
			default:
				res = 1;
				break;
			}
			e->inuse--;
		} else 
			ks_cli(fd, "No such command '%s' (type 'help %s' for other possible commands)\n", s, find_best(argv));
		
		
	}
	free(dup);
	
	return res;
}
/* -------------------------------------------------------------------------- */
char **ks_cli_completion_matches(const char *text, const char *word)
{
	char **match_list = NULL, **match_list_new = NULL, *retstr, *prevstr;
	size_t match_list_len, max_equal, which, i;
	int matches = 0;

	/* leave entry 0 free for the longest common substring */
	match_list_len = 1;
	while ((retstr = ks_cli_generator(text, word, matches)) != NULL) {
		if (matches + 1 >= match_list_len) {
			match_list_len <<= 1;
			if (!(match_list_new = realloc(match_list, match_list_len * sizeof(*match_list))))
			{
				if (match_list)
					free(match_list);
				return NULL;
			}
			match_list = match_list_new;
		}
		match_list[++matches] = retstr;
	}

	if (!match_list)
		return match_list; /* NULL */

	/* Find the longest substring that is common to all results
	 * (it is a candidate for completion), and store a copy in entry 0.
	 */
	prevstr = match_list[1];
	max_equal = strlen(prevstr);
	for (which = 2; which <= matches; which++) {
		for (i = 0; i < max_equal && toupper(prevstr[i]) == toupper(match_list[which][i]); i++)
			continue;
		max_equal = i;
	}

	if (!(retstr = malloc(max_equal + 1)))
		return NULL;
	
	memcpy(retstr, match_list[1], max_equal + 1);
	retstr[max_equal]=0;
	match_list[0] = retstr;
	
	/* ensure that the array is NULL terminated */
	if (matches + 1 >= match_list_len) {
		if (!(match_list_new = realloc(match_list, (match_list_len + 1) * sizeof(*match_list))))
		{
			if (match_list)
				free(match_list);
			return NULL;
		}
		match_list = match_list_new;
	}
	match_list[matches + 1] = NULL;

	return match_list;
}
/* -------------------------------------------------------------------------- */
char *ks_cli_generator(const char *text, const char *word, int state)
{
	return __ks_cli_generator(text, word, state, 1);
}
/* -------------------------------------------------------------------------- */
static char *__ks_cli_generator(const char *text, const char *word, int state, int lock)
{
	char *argv[KS_MAX_ARGS];
	struct ks_cli_entry *e;
	struct cli_iterator i = { NULL, NULL };
	int x = 0, argindex, matchlen;
	int matchnum=0;
	char *ret = NULL;
	char matchstr[80] = "";
	int tws = 0;
	char *dup = parse_args(text, &x, argv, sizeof(argv) / sizeof(argv[0]), &tws);

	if (!dup)	/* error */
		return NULL;
	argindex = (!strlen_zero(word) && x>0) ? x-1 : x;
	/* rebuild the command, ignore tws */
	ks_join(matchstr, sizeof(matchstr)-1, argv);
	matchlen = strlen(matchstr);
	if (tws) {
		strcat(matchstr, " "); /* XXX */
		if (matchlen)
			matchlen++;
	}
	
		
	while( !ret && (e = cli_next(&i)) ) {
		int lc = strlen(e->_full_cmd);
		if (e->_full_cmd[0] != '_' && lc > 0 && matchlen <= lc &&
				!strncasecmp(matchstr, e->_full_cmd, matchlen)) {
			/* Found initial part, return a copy of the next word... */
			if (e->cmda[argindex] && ++matchnum > state)
				ret = strdup(e->cmda[argindex]); /* we need a malloced string */
		} else if (e->generator && !strncasecmp(matchstr, e->_full_cmd, lc) && matchstr[lc] < 33) {
			/* We have a command in its entirity within us -- theoretically only one
			   command can have this occur */
			ret = e->generator(matchstr, word, argindex, state);
		}
	}

	free(dup);
	return ret;
}
/* -------------------------------------------------------------------------- */
static int ks_el_sort_compare(const void *i1, const void *i2)
{
	char *s1, *s2;

	s1 = ((char **)i1)[0];
	s2 = ((char **)i2)[0];

	return strcasecmp(s1, s2);
}
/* -------------------------------------------------------------------------- */
int ks_cli_display_match_list(char **matches, int len, int max)
{
	int i, idx, limit, count;
	int screenwidth = 0;
	int numoutput = 0, numoutputline = 0;

	screenwidth = 80;

	/* find out how many entries can be put on one line, with two spaces between strings */
	limit = screenwidth / (max + 2);
	if (limit == 0)
		limit = 1;

	/* how many lines of output */
	count = len / limit;
	if (count * limit < len)
		count++;

	idx = 1;

	qsort(&matches[0], (size_t)(len), sizeof(char *), ks_el_sort_compare);

	for (; count > 0; count--) {
		numoutputline = 0;
		for (i=0; i < limit && matches[idx]; i++, idx++) {

			/* Don't print dupes */
			if ( (matches[idx+1] != NULL && strcmp(matches[idx], matches[idx+1]) == 0 ) ) {
				i--;
				free(matches[idx]);
				matches[idx] = NULL;
				continue;
			}

			numoutput++;
			numoutputline++;
			fprintf(stdout, "%-*s  ", max, matches[idx]);
			free(matches[idx]);
			matches[idx] = NULL;
		}
		if (numoutputline > 0)
			fprintf(stdout, "\n");
	}

	return numoutput;
}
