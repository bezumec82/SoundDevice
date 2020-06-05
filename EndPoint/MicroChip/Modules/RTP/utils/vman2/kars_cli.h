#ifndef _KARS_CLI_H_
#define _KARS_CLI_H_



#define KS_BUFFER_SIZE		1024

#define KS_MAX_CONNECTS 4
#define KS_MAX_CMD_LEN 	16
#define KS_MAX_ARGS 		64
#define RESULT_SUCCESS		0
#define RESULT_SHOWUSAGE	1
#define RESULT_FAILURE		2

struct cli_iterator {
	struct ks_cli_entry *builtins;
	struct ks_cli_entry *helpers;
};

struct console {
	int fd_in;
	int fd_out;
};


#define KS_LIST_HEAD_INIT_VALUE	{		\
	.first = NULL,					\
	.last = NULL,					\
	}

#define KS_LIST_ENTRY(type)						\
struct {								\
	struct type *next;						\
}
#define KS_LIST_NEXT(elm, field)	((elm)->field.next)
#define	KS_LIST_FIRST(head)	((head)->first)

#define KS_LIST_HEAD_STATIC(name, type)				\
struct name {								\
	struct type *first;						\
	struct type *last;						\
} name = KS_LIST_HEAD_INIT_VALUE


#define KS_LIST_TRAVERSE_SAFE_BEGIN(head, var, field) {				\
	typeof((head)->first) __list_next;						\
	typeof((head)->first) __list_prev = NULL;					\
	typeof((head)->first) __new_prev = NULL;					\
	for ((var) = (head)->first, __new_prev = (var),					\
	      __list_next = (var) ? (var)->field.next : NULL;				\
	     (var);									\
	     __list_prev = __new_prev, (var) = __list_next,				\
	     __new_prev = (var),							\
	     __list_next = (var) ? (var)->field.next : NULL				\
	    )


#define KS_LIST_INSERT_BEFORE_CURRENT(head, elm, field) do {		\
	if (__list_prev) {						\
		(elm)->field.next = __list_prev->field.next;		\
		__list_prev->field.next = elm;				\
	} else {							\
		(elm)->field.next = (head)->first;			\
		(head)->first = (elm);					\
	}								\
	__new_prev = (elm);						\
} while (0)

#define KS_LIST_TRAVERSE_SAFE_END  }

#define KS_LIST_INSERT_TAIL(head, elm, field) do {			\
      if (!(head)->first) {						\
		(head)->first = (elm);					\
		(head)->last = (elm);					\
      } else {								\
		(head)->last->field.next = (elm);			\
		(head)->last = (elm);					\
      }									\
} while (0)


struct ks_cli_entry {
	char * cmda[KS_MAX_CMD_LEN];
	int (*handler)(int fd, int argc, char *argv[]);
	const char *summary;
	const char *usage;
	char *(*generator)(const char *line, const char *word, int pos, int n);		
	int inuse;
	char *_full_cmd;	
	KS_LIST_ENTRY(ks_cli_entry) list;
};

static inline int strlen_zero(const char *s)
{
	return (!s || (*s == '\0'));
}
#define S_OR(a, b)	(!strlen_zero(a) ? (a) : (b))





int ks_cli_command(int fd, const char *s);
void ks_cli(int fd, char *fmt, ...);
int cli_register(struct ks_cli_entry *e);
void cli_register_multiple(struct ks_cli_entry *e, int len);
void cli_init(void);
char **ks_cli_completion_matches(const char *text, const char *word);
int ks_cli_display_match_list(char **matches, int len, int max);
char *ks_cli_generator(const char *text, const char *word, int state);

#endif
