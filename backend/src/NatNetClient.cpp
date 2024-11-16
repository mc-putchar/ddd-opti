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
	: cmd_socket(-1), data_socket(-1), host(), packet_in(), packet_out()
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
	// int recvbuf_size(0);
	// int sendbuf_size(0);
	// socklen_t optlen(0);
	// if (::getsockopt(sfd, SOL_SOCKET, SO_RCVBUF, &recvbuf_size, &optlen)
	// || ::getsockopt(sfd, SOL_SOCKET, SO_SNDBUF, &sendbuf_size, &optlen))
	// {
	// 	std::cerr << "Error: " << errno << " from getsockopt" << std::endl
	// 		<< ::strerror(errno) << std::endl;
	// 	::close(sfd);
	// 	return (-1);
	// }
	// if (recvbuf_size != BUF_SIZE)
	// 	std::cerr << "Warn: Receive buffer size differs, set to: " << recvbuf_size << std::endl;
	// if (sendbuf_size != BUF_SIZE)
	// 	std::cerr << "Warn: Sending buffer size differs, set to: " << sendbuf_size << std::endl;
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
	// int recvbuf_size(0);
	// socklen_t optlen(0);
	// if (::getsockopt(sfd, SOL_SOCKET, SO_RCVBUF, &recvbuf_size, &optlen))
	// {
	// 	std::cerr << "Error: " << errno << " from getsockopt" << std::endl
	// 		<< ::strerror(errno) << std::endl;
	// 	::close(sfd);
	// 	return (-1);
	// }
	// if (recvbuf_size != BUF_SIZE)
	// 	std::cerr << "Warn: Receive buffer size differs, set to: " << recvbuf_size << std::endl;
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

int NatNetClient::request_conn(char const *ip, uint16_t port, uint16_t retries)
{
	this->host.sin_family = AF_INET;
	this->host.sin_port = ::htons(port);
	this->host.sin_addr.s_addr = ::inet_addr(ip);

	s_connection_opts conn_opts = s_connection_opts();
	s_sender sender = {};
	this->packet_out.type = CONNECT;
	this->packet_out.size = (4L + sizeof sender + sizeof conn_opts);
	::memcpy(&this->packet_out.data, &sender, sizeof sender);
	::memcpy(&(this->packet_out.data.bytes[sizeof sender]), &conn_opts, \
		sizeof conn_opts);
	ssize_t sb(0);
	while (sb < 1L && retries--)
	{
		sb = ::sendto(
			this->cmd_socket,
			reinterpret_cast<char *>(&this->packet_out),
			this->packet_out.size,
			0L,
			reinterpret_cast<sockaddr*>(&this->host),
			sizeof this->host
		);
	}
	return (retries ? 0 : -1);
}

int NatNetClient::send_keepalive()
{
	this->packet_out.type = KEEPALIVE;
	this->packet_out.size = 0;
	ssize_t sb = ::sendto(
		this->cmd_socket,
		reinterpret_cast<char *>(&this->packet_out),
		4L,
		0L,
		reinterpret_cast<sockaddr*>(&this->host),
		sizeof this->host
	);
	if (sb < 1)
	{
		std::cerr << "NatNet client: Failed sending keepalive." << std::endl;
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
	if (request_conn(host_ip, ports->cmd, SEND_RETRIES))
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

void NatNetClient::parse_cmd()
{
	switch (this->packet_in.type)
	{
		case SERVERINFO:
			std::cout << "[NatNet] Received: " << this->packet_in.type << std::endl;
			break;
		case RESPONSE:
			std::cout << "[NatNet] Received: " << this->packet_in.type << std::endl;
			break;
		case MODELDEF:
			std::cout << "[NatNet] Received: " << this->packet_in.type << std::endl;
			break;
		case DATAFRAME:
			std::cout << "[NatNet] Received: " << this->packet_in.type << std::endl;
			break;
		case MSGSTRING:
			std::cout << "[NatNet] Received: " << this->packet_in.type << std::endl;
			break;
		case UNRECOGNIZED:
			std::cout << "[NatNet] Received: " << this->packet_in.type << std::endl;
			break;
		default:
			std::cerr << "[NatNet] Unknown command" << std::endl;
	}
}

void NatNetClient::listen_cmd()
{
	ssize_t rb(0);

	while (!g_stopped) {
		this->send_keepalive();
		rb = recvfrom(
			this->cmd_socket,
			reinterpret_cast<char*>(&this->packet_in),
			sizeof this->packet_in,
			0L,
			NULL,
			NULL
		);
		if (rb < 0)
			std::cerr << "[NatNet] client command receive error." << std::endl;
		else if (rb > 0)
			this->parse_cmd();
	}
}

void NatNetClient::parse_data()
{}

void NatNetClient::listen_data()
{
	ssize_t rb(0);
	while (!g_stopped) {
		rb = recvfrom(
			this->data_socket,
			reinterpret_cast<char*>(&this->packet_in),
			sizeof this->packet_in,
			0L,
			NULL,
			NULL
		);
		if (rb < 0)
			std::cerr << "[NatNet] client data receive error." << std::endl;
		else if (rb > 0)
			this->parse_data();
	}
}
