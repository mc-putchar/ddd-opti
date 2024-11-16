#include <cstdint>
#include <iostream>

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "NatNetClient.hpp"

NatNetClient::NatNetClient()
	: cmd_socket(-1), data_socket(-1)
{}

NatNetClient::~NatNetClient()
{
	::close(this->data_socket);
	::close(this->cmd_socket);
}

static int create_cmd_socket(uint16_t port, char const *ip_addr)
{
	struct sockaddr_in my_addr = {};
	int const sfd = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sfd < 0)
	{
		std::cerr << "Error: " << errno << " from socket" << std::endl
			<< ::strerror(errno) << std::endl;
		return (-1);
	}
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = ::htons(port);
	my_addr.sin_addr.s_addr = ::inet_addr(ip_addr);
	if (::bind(sfd, (struct sockaddr*)&my_addr, sizeof(struct sockaddr)))
	{
		std::cerr << "Error: " << errno << " from bind" << std::endl
			<< ::strerror(errno) << std::endl;
		::close(sfd);
		return (-1);
	}
	int recvbuf_size(0);
	int sendbuf_size(0);
	socklen_t optlen(0);
	if (::getsockopt(sfd, SOL_SOCKET, SO_RCVBUF, &recvbuf_size, &optlen)
	|| ::getsockopt(sfd, SOL_SOCKET, SO_SNDBUF, &sendbuf_size, &optlen))
	{
		std::cerr << "Error: " << errno << " from getsockopt" << std::endl
			<< ::strerror(errno) << std::endl;
		::close(sfd);
		return (-1);
	}
	if (recvbuf_size != BUF_SIZE)
		std::cerr << "Warn: Receive buffer size differs, set to: " << recvbuf_size << std::endl;
	if (sendbuf_size != BUF_SIZE)
		std::cerr << "Warn: Sending buffer size differs, set to: " << sendbuf_size << std::endl;
	struct timeval tv;
	tv.tv_sec = SOCK_TIMEOUT;
	tv.tv_usec = 0;
	int re = 1L;
	int ipfrag = IP_PMTUDISC_DO;
	if (::setsockopt(sfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof tv))
	{
		std::cerr << "Error: " << errno << " from setsockopt timeout" << std::endl
			<< ::strerror(errno) << std::endl;
		::close(sfd);
		return (-1);
	}
	if (::setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &re, sizeof re))
	{
		std::cerr << "Error: " << errno << " from setsockopt reuse" << std::endl
			<< ::strerror(errno) << std::endl;
		::close(sfd);
		return (-1);
	}
	if (::setsockopt(sfd, IPPROTO_IP, IP_MTU_DISCOVER, &ipfrag, sizeof ipfrag))
	{
		std::cerr << "Error: " << errno << " from setsockopt fragment" << std::endl
			<< ::strerror(errno) << std::endl;
		::close(sfd);
		return (-1);
	}
	std::cout << "INFO: " << "Configured unicast command socket."
		<< " IP: " << ip_addr << " Port: " << port << std::endl;
	return (sfd);
}

static int create_data_socket(uint16_t port, char const *ip_addr)
{
	struct sockaddr_in my_addr = {};
	int const sfd = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sfd < 0)
	{
		std::cerr << "Error: " << errno << " from socket" << std::endl
			<< ::strerror(errno) << std::endl;
		return (-1);
	}
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = ::htons(port);
	my_addr.sin_addr.s_addr = ::inet_addr(ip_addr);
	if (::bind(sfd, (struct sockaddr*)&my_addr, sizeof(struct sockaddr)))
	{
		std::cerr << "Error: " << errno << " from bind" << std::endl
			<< ::strerror(errno) << std::endl;
		::close(sfd);
		return (-1);
	}
	int recvbuf_size(0);
	socklen_t optlen(0);
	if (::getsockopt(sfd, SOL_SOCKET, SO_RCVBUF, &recvbuf_size, &optlen))
	{
		std::cerr << "Error: " << errno << " from getsockopt" << std::endl
			<< ::strerror(errno) << std::endl;
		::close(sfd);
		return (-1);
	}
	if (recvbuf_size != BUF_SIZE)
		std::cerr << "Warn: Receive buffer size differs, set to: " << recvbuf_size << std::endl;
	int const re = 1L;
	if (::setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &re, sizeof re))
	{
		std::cerr << "Error: " << errno << " from setsockopt" << std::endl
			<< ::strerror(errno) << std::endl;
		::close(sfd);
		return (-1);
	}
	std::cout << "INFO: " << "Configured unicast data socket."
		<< " IP: " << ip_addr << " Port: " << port << std::endl;
	return (sfd);
}

static int request_conn(int fd, char const *ip, uint16_t port, uint16_t retries)
{
	struct sockaddr_in host = {};
	host.sin_family = AF_INET;
	host.sin_port = ::htons(port);
	host.sin_addr.s_addr = ::inet_addr(ip);

	s_connection_opts conn_opts = s_connection_opts();
	s_sender sender = {};
	s_packet packet;
	packet.type = CONNECT;
	packet.size = (4L + sizeof sender + sizeof conn_opts);
	::memcpy(&packet.data, &sender, sizeof sender);
	::memcpy(&(packet.data.bytes[sizeof sender]), &conn_opts, sizeof conn_opts);
	ssize_t sb(0);
	while (sb < 1L)
	{
		sb = ::sendto(fd, reinterpret_cast<char *>(&packet), packet.size, 0, \
			(sockaddr *)&host, sizeof host);
		std::cerr << "Error: " << errno << " from sendto" << std::endl
			<< ::strerror(errno) << std::endl;
		if (!retries--)
			return (-1);
	}
	return (0);
}

int NatNetClient::init(s_ports *ports, char const *my_ip, char const *host_ip)
{
	this->cmd_socket = create_cmd_socket(ports->cmd, my_ip);
	if (this->cmd_socket < 0)
		return (-1);
	this->data_socket = create_data_socket(ports->data, my_ip);
	if (this->data_socket < 0)
	{
		::close(this->cmd_socket);
		this->cmd_socket = -1;
		return (-1);
	}
	if (request_conn(this->cmd_socket, host_ip, ports->cmd, SEND_RETRIES))
	{
		std::cerr << "Error: Failed to establish connection with server. Maximum retries exceeded." << std::endl;
		::close(this->cmd_socket);
		::close(this->data_socket);
		this->cmd_socket = -1;
		this->data_socket = -1;
		return (-1);
	}
	std::cout << "INFO: Established connection with NatNet server." << std::endl;
	return (0);
}
