#ifndef DATABROKER_H
# define DATABROKER_H

# ifdef __cplusplus
extern "C" {
# endif

# define MAX_FDS	42

typedef void (*t_handler)(void);

struct s_databroker
{
	int			epoll_fd;
	int			sig_fd;
	int			serial_fd;
	int			ctrl_fd;
	int			data_fd;
	t_handler	serial_handler;
	t_handler	ctrl_handler;
	t_handler	data_handler;
	int			n_fds;
	int			fds[MAX_FDS];
	t_handler	handlers[MAX_FDS];
};

int	broker_create(struct s_databroker *dbro);
int	broker_this(struct s_databroker *dbro, t_handler handler, int fd, int events);
int	broker_mod(struct s_databroker *dbro, int fd, int events);
int	broker_del(struct s_databroker *dbro, int fd);
int	broker_run(struct s_databroker *dbro);
int	broker_clean(struct s_databroker *dbro, int ret);

# ifdef __cplusplus
}
# endif

#endif // DATABROKER_H
