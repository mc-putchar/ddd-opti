#pragma once

// NatNet data client
// NOTE: for now, Unicast only

#include <cstdint>

#define LOOPBACK		"127.0.0.1"
#define CMD_PORT		1510L
#define DATA_PORT		1511L

#define MAX_NAMELEN		256L
#define MAX_PACKETSIZE	100000L
#define BUF_SIZE		65536L
#define SOCK_TIMEOUT	2L
#define SEND_RETRIES	3L

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
	UNRECOGNIZED = 100,
	UNDEFINED = 999
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

	private:
		int cmd_socket;
		int data_socket;
};
