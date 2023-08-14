/*
 *  Synopsis:
 *	C include for $JMSCOTT_ROOT/lib/libjmscott.a
 *  Usage:
 *	cc -I$JMSCOTT_ROOT/include ...
 *	
 *	#include "jmscott/libjmscott.h"
 *  Note:
 *	Should char *jmscott_progname be defined as extern?
 *
 *	Create jmscott_mkdir[1234] with semantics similar to command mkdir -p.
 *
 *	Rename dir clib/ to libjmscott/
 *
 *	should struct jmscott_ecpg_state_fault containt the	
 */

#ifndef JMSCOTT_LIBJMSCOTT_H
#define JMSCOTT_LIBJMSCOTT_H

#define JMSCOTT_PATH_MAX	255		//  cause someone must be god

#include <sys/time.h>		//  is needed?
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>

/*
 *  Pragmatic, reliable, atomic message size across many protocols, such as
 *  udp4 and pipe buffers.
 *
 *  Note:
 *	should JMSCOTT_ATOMIC_WRITE_SIZE be renamed JMSCOTT_ATOMIC_SIZE?
 *
 *	investigate message size of tilelink protocol for risc5 interconnect.
 *
 *		https://starfivetech.com/uploads/tilelink_spec_1.8.1.pdf
 */
#define JMSCOTT_ATOMIC_WRITE_SIZE	508

//  YYYY-MM-DDThh:mm:ss.nsec[+-]HH:MM
#define JMSCOTT_RFC3339NANO_SIZE	35

extern int	jmscott_ecpg_error_code;
extern int	jmscott_ecpg_warn_code;

extern char *	jmscott_strcat(char *tgt, int tgtsize, const char *src);
extern char *	jmscott_strcat2(char *tgt, int tgtsize,
			const char *src1,
			const char *src2
		);
extern char *	jmscott_strcat3(char *tgt, int tgtsize,
			const char *src1,
			const char *src2,
			const char *src3
		);
extern char *	jmscott_strcat4(char *tgt, int tgtsize,
			const char *src1,
			const char *src2,
			const char *src3,
			const char *src4
		);
extern char *	jmscott_strcat5(char *tgt, int tgtsize,
			const char *src1,
			const char *src2,
			const char *src3,
			const char *src4,
			const char *src5
		);
extern char *	jmscott_strcat6(char *tgt, int tgtsize,
			const char *src1,
			const char *src2,
			const char *src3,
			const char *src4,
			const char *src5,
			const char *src6
		);
extern char 	*jmscott_ulltoa(unsigned long long ull, char *digits);
extern char 	*jmscott_lltoa(long long ull, char *digits);
extern char	*jmscott_a2ui63(char *a, unsigned long long *ull);
extern char	*jmscott_a2size_t(char *a, size_t *sz);

extern void	jmscott_die(int status, char *msg1);
extern void	jmscott_die2(int status, char *msg1, char *msg2);
extern void	jmscott_die3(int status, char *msg1, char *msg2, char *msg3);
extern void	jmscott_die4(
			int status,
			char *msg1,
			char *msg2,
			char *msg3,
			char *msg4
		);
extern void	jmscott_die5(
			int status,
			char *msg1,
			char *msg2,
			char *msg3,
			char *msg4,
			char *msg5
		);
extern void	jmscott_die6(
			int status,
			char *msg1,
			char *msg2,
			char *msg3,
			char *msg4,
			char *msg5,
			char *msg6
		);
extern void	jmscott_die_argc(int status, int got, int expect, char *usage);

extern char *	jmscott_frisk_udig(char *udig);

//  map sql state codes onto process exit status.
//  only used when query faults.  caller code
//  defines mapping.

struct jmscott_ecpg_state_fault
{
	char	*sql_state;

	// -1 implies ignore fault, 0 <= 255 implies remap exit status

	int	action;
};
extern void jmscott_ecpg_fault(
	int status,
	char *what,
	struct jmscott_ecpg_state_fault *fault
);

extern void	jmscott_ecpg_error(struct jmscott_ecpg_state_fault *fault);
extern void	jmscott_ecpg_warning(struct jmscott_ecpg_state_fault *fault);

struct jmscott_halloc_callback
{
	void	(*func)(void *, void *);
	void	*private_data;

	struct jmscott_halloc_callback	*next;
};
extern void	*jmscott_halloc_adopt(void *parent, void *p);
extern void	*jmscott_halloc(void *parent, size_t size);
extern void	jmscott_halloc_free(void *p);
extern void	jmscott_halloc_add_callback(
			void *p,
			void (*func)(void *, void *),
			void *private_data
		);
extern char	*jmscott_halloc_strdup(void *parent, char *cp);
extern void	*jmscott_halloc_resize(void *p, size_t size);

extern void	jmscott_hexdump(
			unsigned char *src,
			int src_size,
			char direction,
			char *tgt,
			int tgt_size
);

extern ssize_t	jmscott_read(int fd, void *p, ssize_t nbytes);
extern int	jmscott_poll_POLLIN(int fd, int millisec);
extern ssize_t	jmscott_read_timeout(
			int fd,
			void *p,
			ssize_t nbytes,
			int millisec
		);
extern int	jmscott_read_exact(int fd, void *blob, ssize_t size);
extern int	jmscott_read_exact_timeout(
			int fd,
			void *blob,
			ssize_t size,
			int millisec
		);
extern int	jmscott_write_all(int fd, void *p, ssize_t nbytes);
extern off_t	jmscott_lseek(int fd, off_t offset, int whence);
extern int	jmscott_open(char *path, int oflag, mode_t mode);
extern int	jmscott_close(int fd);
extern int	jmscott_fstat(int fd, struct stat *buf);
extern int	jmscott_stat(char *path, struct stat *buf);
extern int	jmscott_mkdir(const char *path, mode_t mode);
extern char	*jmscott_mkdir_p(const char *path);
extern int	jmscott_mkdir_EEXIST(const char *path, mode_t mode);
extern int 	jmscott_mkdirat_EEXIST(int fd, const char *path, mode_t mode);
extern int	jmscott_link(const char *old_path, const char *new_path);
extern int	jmscott_unlink(const char *path);
extern int	jmscott_access(const char *path, int mode);

struct jmscott_json
{
	int		out_fd;
	int		indent;

	int		trace;
};

extern char			*jmscott_ascii2json_string(
					char *src,
					char *tgt,
					int tgt_size
				);
extern struct jmscott_json	*jmscott_json_new();
extern void			jmscott_json_trace(
					struct jmscott_json *jp,
					char *what,
					char *value
				);
extern void			jmscott_json_trace_c(
					struct jmscott_json *jp,
					char c
				);
extern char			*jmscott_json_write(
					struct jmscott_json *jp,
					char *format, ...
				);

extern char			*jmscott_RFC3339_timeval(
					char *buf,
					int buf_size,
					struct timeval *tv
				);
extern char			*jmscott_RFC3339Nano_now(
					char *buf,
					int buf_size
				);
extern char			*jmscott_net_32addr2text(u_long addr);
extern int			jmscott_flock(int, int);
extern char			*jmscott_sendfile(
					int in,
					int out,
					off_t
					offset,
					off_t len
				);
char *				jmscott_mkdir_path(
					char *parent_path,
					char *path,
					mode_t mode
				);
#endif //  JMSCOTT_LIBJMSCOTT_H
