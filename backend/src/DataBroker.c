#include <signal.h>
#include <sys/epoll.h>
#include <sys/signalfd.h>
#include <unistd.h>

#include "DataBroker.h"
// #include "Log.hpp"

static void	db_info(const char *tag, const char *msg)
{
	size_t	len;

	len = 0;
	while (tag[len++])
		;
	(void)write(2, "[", 1);
	(void)write(2, tag, len);
	(void)write(2, "]\t", 2);
	len = 0;
	while (msg[len++])
		;
	(void)write(2, msg, len);
	(void)write(2, "\n", 1);
}

static void	db_error(const char *tag, const char *msg)
{
	write(2, "[ERROR]\t", 8);
	db_info(tag, msg);
}

static void	db_warn(const char *tag, const char *msg)
{
	write(2, "[WARN]\t", 7);
	db_info(tag, msg);
}

static int	get_sig_fd()
{
	sigset_t	mask;

	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigaddset(&mask, SIGTERM);
	if (sigprocmask(SIG_BLOCK, &mask, 0))
		return (-1);
	return (signalfd(-1, &mask, SFD_CLOEXEC));
}

static int	add_fd_to_epoll(int fd, int epoll_fd, int events)
{
	struct epoll_event	event = {};

	event.data.fd = fd;
	event.events = events;
	return (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event));
}

static int	mod_fd_epoll_event(int fd, int epoll_fd, int events)
{
	struct epoll_event	ev = {};

	ev.data.fd = fd;
	ev.events = events;
	return (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev));
}

static int	del_fd_from_epoll(int fd, int epoll_fd)
{
	struct epoll_event	ev = {};

	ev.data.fd = fd;
	return (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, &ev));
}

int	broker_create(struct s_databroker *dbro)
{
	dbro->epoll_fd = epoll_create1(EPOLL_CLOEXEC);
	if (dbro->epoll_fd < 0)
	{
		db_error("main", "failed to create epoll fd.");
		return (-1);
	}
	dbro->sig_fd = get_sig_fd();
	if (dbro->sig_fd < 0)
	{
		db_error("main", "failed to create signal fd.");
		(void)close(dbro->epoll_fd);
		return (-1);
	}
	if (add_fd_to_epoll(dbro->sig_fd, dbro->epoll_fd, EPOLLIN))
	{
		db_error("main", "failed to add signal fd to epoll.");
		(void)close(dbro->epoll_fd);
		(void)close(dbro->sig_fd);
		return (-1);
	}
	return (0);
}

int	broker_this(struct s_databroker *dbro, t_handler handler, int fd, int events)
{
	if (dbro->n_fds < MAX_FDS - 1 && !add_fd_to_epoll(fd, dbro->epoll_fd, events))
	{
		dbro->handlers[dbro->n_fds] = handler;
		dbro->fds[++dbro->n_fds] = fd;
		return (0);
	}
	db_warn("DataBroker", "failed to accept fd.");
	return (-1);
}

int	broker_mod(struct s_databroker *dbro, int fd, int events)
{
	return (mod_fd_epoll_event(fd, dbro->epoll_fd, events));
}

int	broker_del(struct s_databroker *dbro, int fd)
{
	return (del_fd_from_epoll(fd, dbro->epoll_fd));
}

void	call_handler(struct s_databroker *dbro, int fd)
{
	int	idx;

	idx = 0;
	while (idx < MAX_FDS && dbro->fds[idx] != fd)
		++idx;
	if (idx >= MAX_FDS)
		return ;
	dbro->handlers[idx]();
}

int	broker_run(struct s_databroker *dbro)
{
	int const	timeout = 42;
	int const	max_events = 42;
	struct epoll_event	events[max_events];

	while (1)
	{
		int n_events = epoll_wait(dbro->epoll_fd, events, max_events, timeout);
		if (n_events < 0)
		{
			db_error("DataBroker", "failed epoll wait.");
			return (broker_clean(dbro, -1));
		}
		for (int i = 0; i < n_events; ++i)
		{
			int const fd = events[i].data.fd;
			if (fd == dbro->sig_fd)
			{
				write(1, "\n", 1);
				db_info("DataBroker", "Signal received. Shutting down.");
				return (broker_clean(dbro, 0));
			}
			call_handler(dbro, fd);
		}
	}
}

int	broker_clean(struct s_databroker *dbro, int ret)
{
	int	i;

	i = 0;
	while (i < dbro->n_fds)
	{
		del_fd_from_epoll(dbro->fds[i], dbro->epoll_fd);
		close(dbro->fds[i++]);
	}
	dbro->n_fds = 0;
	if (dbro->serial_fd >= 0)
		close(dbro->serial_fd);
	if (dbro->sig_fd >= 0)
		close(dbro->sig_fd);
	if (dbro->epoll_fd >= 0)
		close(dbro->epoll_fd);
	return (ret);
}
