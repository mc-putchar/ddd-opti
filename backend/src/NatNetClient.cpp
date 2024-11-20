#include <cstdint>
#include <iostream>
#include <string>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "Log.hpp"
#include "DataBroker.h"
#include "NatNetClient.hpp"

#define TAG	"NatNet"

NatNetClient::NatNetClient(int epoll_fd)
	: epoll_fd(epoll_fd),
	cmd_socket(-1),
	data_socket(-1),
	host(),
	packet_in(),
	packet_out()
{}

NatNetClient::~NatNetClient()
{
	if (this->cmd_socket >= 0)
		(void)::close(this->cmd_socket);
	if (this->data_socket >= 0)
		(void)::close(this->data_socket);
}

int NatNetClient::connect(s_ports *ports, char const *my_ip, char const *host_ip, s_databroker *db)
{
	if (this->init(ports, my_ip, db))
		return (-1);
	if (request_conn(host_ip, ports->cmd, SEND_RETRIES))
	{
		ERROR(TAG, "failed to establish connection with server. Maximum retries exceeded.");
		(void)::close(this->cmd_socket);
		(void)::close(this->data_socket);
		this->cmd_socket = -1;
		this->data_socket = -1;
		return (-1);
	}
	INFO(TAG, "Established connection with the server.");
	return (0);
}

void NatNetClient::listen_cmd()
{
	ssize_t rb(0);

	rb = recvfrom(
		this->cmd_socket,
		reinterpret_cast<char*>(&this->packet_in),
		sizeof this->packet_in,
		0L,
		NULL,
		NULL
	);
	if (rb < 0)
		ERROR(TAG, "client command receive error.");
	else if (rb > 0)
		this->parse_cmd();
}

int NatNetClient::send_cmd(std::string const &cmd, e_msg_type type)
{
	this->packet_out.type = type;
	if (!cmd.empty())
	{
		this->packet_out.size = cmd.length() + 1;
		::memcpy(
			this->packet_out.data.str,
			cmd.c_str(),
			cmd.length()
		);
		this->packet_out.data.str[cmd.length()] = 0;
	}
	else
	{
		this->packet_out.size = 0;
	}
	ssize_t sb = ::sendto(
		this->cmd_socket,
		reinterpret_cast<char *>(&this->packet_out),
		(4L + this->packet_out.size),
		0L,
		reinterpret_cast<sockaddr*>(&this->host),
		sizeof this->host
	);
	if (sb < 1)
	{
		ERROR(TAG, "failed to send command");
		return (-1);
	}
	return (0);
}

void NatNetClient::listen_data()
{
	ssize_t rb(0);
	rb = recvfrom(
		this->data_socket,
		reinterpret_cast<char*>(&this->packet_in),
		sizeof this->packet_in,
		0L,
		NULL,
		NULL
	);
	if (rb < 0)
		ERROR(TAG, "client data receive error.");
	else if (rb > 0)
		this->parse_data();
}

static int create_cmd_socket(uint16_t port, char const *ip_addr)
{
	struct sockaddr_in my_addr = {};
	int const sfd = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sfd < 0)
	{
		ERROR(TAG, "failed to create socket");
		return (-1);
	}
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = ::htons(port);
	my_addr.sin_addr.s_addr = ::inet_addr(ip_addr);
	if (::bind(sfd, (struct sockaddr*)&my_addr, sizeof(struct sockaddr)))
	{
		ERROR(TAG, "failed to bind socket");
		(void)::close(sfd);
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
		ERROR(TAG, "failed to set socket option");
		(void)::close(sfd);
		return (-1);
	}
	if (::setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &re, sizeof re))
	{
		ERROR(TAG, "failed to set socket option");
		(void)::close(sfd);
		return (-1);
	}
	if (::setsockopt(sfd, IPPROTO_IP, IP_MTU_DISCOVER, &ipfrag, sizeof ipfrag))
	{
		ERROR(TAG, "failed to set socket option");
		(void)::close(sfd);
		return (-1);
	}
	INFO(TAG, "Configured command socket");
	return (sfd);
}

static int create_data_socket(uint16_t port, char const *ip_addr)
{
	struct sockaddr_in my_addr = {};
	int const sfd = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sfd < 0)
	{
		ERROR(TAG, "failed to create socket");
		return (-1);
	}
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = ::htons(port);
	my_addr.sin_addr.s_addr = ::inet_addr(ip_addr);
	if (::bind(sfd, (struct sockaddr*)&my_addr, sizeof(struct sockaddr)))
	{
		ERROR(TAG, "failed to bind socket");
		(void)::close(sfd);
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
		ERROR(TAG, "failed to set socket option");
		(void)::close(sfd);
		return (-1);
	}
	INFO(TAG, "Configured command socket");
	return (sfd);
}

void	cmd_handler()
{
	INFO(TAG, "cmd_handler called");
}

void	data_handler()
{
	INFO(TAG, "data_handler called");
}

int NatNetClient::init(s_ports *ports, char const *my_ip, s_databroker *db)
{
	this->cmd_socket = create_cmd_socket(ports->cmd, my_ip);
	if (this->cmd_socket < 0)
		return (-1);
	this->data_socket = create_data_socket(ports->data, my_ip);
	if (this->data_socket < 0)
	{
		(void)::close(this->cmd_socket);
		this->cmd_socket = -1;
		return (-1);
	}
	if (broker_this(db, &cmd_handler, this->cmd_socket, EPOLLIN | EPOLLOUT)
	|| broker_this(db, &data_handler, this->data_socket, EPOLLIN))
	{
		(void)::close(this->cmd_socket);
		this->cmd_socket = -1;
		(void)::close(this->data_socket);
		this->data_socket = -1;
		ERROR(TAG, "failed epoll add");
		return (-1);
	}
	return (0);
}

int NatNetClient::request_conn(char const *ip, uint16_t port, uint16_t retries)
{
	this->host.sin_family = AF_INET;
	this->host.sin_port = ::htons(port);
	this->host.sin_addr.s_addr = ::inet_addr(ip);

	s_connection_opts conn_opts = s_connection_opts();
	conn_opts.subscribed_data_only = true;
	s_sender sender = {};
	this->packet_out.type = CONNECT;
	this->packet_out.size = (sizeof sender + sizeof conn_opts);
	::memcpy(&this->packet_out.data, &sender, sizeof sender);
	::memcpy(&(this->packet_out.data.bytes[sizeof sender]), &conn_opts, \
		sizeof conn_opts);
	ssize_t sb(0);
	while (sb < 1L && retries)
	{
		--retries;
		sb = ::sendto(
			this->cmd_socket,
			reinterpret_cast<char *>(&this->packet_out),
			(4L + this->packet_out.size),
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
		// std::cerr << "NatNet client: Failed sending keepalive." << std::endl;
		WARN(TAG, "failed to send keepalive");
		return (-1);
	}
	return (0);
}

void print_packet(s_packet *packet)
{
	std::string	tmp(packet->data.str, packet->size);
	std::cout << "[PACKET]\n"
		<< "Type: " << packet->type << std::endl
		<< "Size: " << packet->size << std::endl
		<< "Data: " << tmp << std::endl;
}

void NatNetClient::parse_cmd()
{
	d3::log << Log::DEBUG << "[NatNet] Received command packet" << std::endl;
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
	print_packet(&this->packet_in);
}

void NatNetClient::parse_data()
{
	d3::log << Log::DEBUG << "[NatNet] Received data packet" << std::endl;
	print_packet(&this->packet_in);
}
