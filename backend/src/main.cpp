#include <csignal>
#include <iostream>

#include <sys/epoll.h>
#include <sys/signalfd.h>

#include "DroneControl.hpp"
#include "DroneState.hpp"
#include "Log.hpp"
#include "NatNetClient.hpp"
#include "DronePath.hpp"
#include "SerialHandler.hpp"
#include "WsServer.hpp"

// TODO: 
// - Serial port Warn if disconnected (CRITICAL)
//	- Establish message queue for send/receive
// - Internal data model - DroneState
//	- track and update individual drones position, status, trajectory
//	- decouple from dependencies
// - Optitrack NatNet integration
//	- retrieve positional data of registered rigid bodies
//	- correlate with target DroneState and update its internal state
// - Unit Tests
// - Integration Tests
// - GUI
// - Documentation

#define CONTAINER_IP	"172.18.0.2"

#ifndef DEBUG_MODE
# define DEBUG_MODE	0
#endif

static int get_sig_fd()
{
	sigset_t	mask;

	::sigemptyset(&mask);
	::sigaddset(&mask, SIGINT);
	::sigaddset(&mask, SIGTERM);
	if (::sigprocmask(SIG_BLOCK, &mask, 0))
		return (-1);
	return (::signalfd(-1, &mask, SFD_CLOEXEC));
}

int add_fd_to_epoll(int fd, int epoll_fd)
{
	epoll_event	event = {};

	event.data.fd = fd;
	event.events = EPOLLIN;
	return (::epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event));
}

int main(void) {
	using d3::log;
	log.set_output(&std::cout, false);
	if (DEBUG_MODE)
	{
		log.set_verbosity(Log::DEBUG);
		log << Log::DEBUG << "Running in DEBUG mode." << std::endl;
	}

	int const epoll_fd = ::epoll_create1(EPOLL_CLOEXEC);
	if (epoll_fd < 0)
	{
		ERROR("main", "failed to create epoll fd.");
		return (-1);
	}
	int const sig_fd = get_sig_fd();
	if (sig_fd < 0)
	{
		ERROR("main", "failed to create signal fd.");
		(void)::close(epoll_fd);
		return (-1);
	}
	if (add_fd_to_epoll(sig_fd, epoll_fd))
	{
		ERROR("main", "failed to add signal fd to epoll.");
		(void)::close(epoll_fd);
		(void)::close(sig_fd);
		return (-1);
	}

	NatNetClient nnclient(epoll_fd);
	s_ports ports(CMD_PORT, DATA_PORT);
	if (nnclient.connect(&ports, CONTAINER_IP, DOCKER_HOST))
	{
		ERROR("NatNet", "failed to initialize NatNet client.");
	}
	// std::thread NatNetThread([&nnclient]()->void {
	// 	nnclient.listen_cmd();
	// });

	std::string const serialPort = getSerialPort();
	if (serialPort.empty())
		return 1;
	SerialHandler serialHandler(serialPort);
	serialHandler.setup();
	// std::thread transmitThread([&serialHandler]() { // Monitor if message are properly delivered to drones.
	// 	serialHandler.monitorIncoming(); // Could come off a thread and added after each send if not setting too much latency.
	// });

	WsServer &wsServer = WsServer::getInstance();
	wsServer.start();

	DroneControl &control = DroneControl::getInstance();
	control.track(std::make_shared<DroneState>(0, serialHandler));
	control.track(std::make_shared<DroneState>(1, serialHandler));
	control.track(std::make_shared<DroneState>(2, serialHandler));
	control.track(std::make_shared<DroneState>(3, serialHandler));

	DronePath path("chocolate_Circle.json"); // Create path on the stack
	control.setPath(0, &path);
	DronePath path1("animation.json"); // Create path on the stack
	control.setPath(1, &path1);

	// NatNetThread.join();
	// transmitThread.join();

	int const	timeout = 42;
	int const	max_events = 42;
	epoll_event	events[max_events];

	while (true)
	{
		int n_events = ::epoll_wait(epoll_fd, events, max_events, timeout);
		if (n_events < 0)
		{
			ERROR("epoll", "failed epoll wait.");
			return (-1);
		}
		for (int i = 0; i < n_events; ++i)
		{
			int const fd = events[i].data.fd;
			if (fd == sig_fd)
			{
				write(1, "\n", 1);
				INFO("main", "Signal received. Shutting down.");
				return (0);
			}
		}
	}
	return 0;
}
