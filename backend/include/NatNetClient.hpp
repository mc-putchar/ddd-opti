#pragma once

// NatNet data client
// NOTE: for now, Unicast only

#include <cstdint>
#include <csignal>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>

#define IPALL			"0.0.0.0"
#define LOOPBACK		"127.0.0.1"
#define DOCKER_HOST		"172.29.112.1" // Probably shouldnt hardcode this

#define CMD_PORT		1510L
#define DATA_PORT		1511L

#define MAX_NAMELEN		256L
#define MAX_PACKETSIZE	100000L
#define BUF_SIZE		65536L
#define SOCK_TIMEOUT	2L
#define SEND_RETRIES	3L

extern volatile std::sig_atomic_t g_stopped;

enum e_msg_type
{
	CONNECT = 0,
	SERVERINFO,
	REQUEST,
	RESPONSE,
	REQUEST_MODELDEF,
	MODELDEF,
	REQUEST_DATAFRAME,
	DATAFRAME,
	MSGSTRING,
	DISCONNECT,
	KEEPALIVE,
	UNRECOGNIZED = 100
};

struct s_ports
{
	uint16_t	cmd;
	uint16_t	data;
	s_ports(uint16_t cmd, uint16_t data)
		: cmd(cmd), data(data) {};
};

// NOTE: Version: [major, minor, build, revision]
struct s_sender
{
	char app_name[MAX_NAMELEN];
	uint8_t version[4];
	uint8_t natnet_version[4];
};

struct s_connection_opts
{
	bool subscribed_data_only;
	uint8_t bitstream_version[4];
	s_connection_opts()
		: subscribed_data_only(false), bitstream_version{0, 0, 0, 0} {};
};

struct s_packet
{
	uint16_t	type;
	uint16_t	size;
	union
	{
		uint8_t		bytes[MAX_PACKETSIZE];
		char		str[MAX_PACKETSIZE];
		uint32_t	ints[MAX_PACKETSIZE / 4];
		float		floats[MAX_PACKETSIZE / 4];
		s_sender	sender;
	}	data;
};

class NatNetClient
{
	public:
		NatNetClient();
		~NatNetClient();

		int init(s_ports *ports, char const *my_ip, char const *host_ip);
		void listen_cmd();
		void listen_data();

	private:
		int cmd_socket;
		int data_socket;
		struct sockaddr_in host;
		struct s_packet packet_in;
		struct s_packet packet_out;

		int request_conn(char const *ip, uint16_t port, uint16_t retries);
		int send_keepalive(void);
		void parse_cmd(void);
		void parse_data(void);
};
