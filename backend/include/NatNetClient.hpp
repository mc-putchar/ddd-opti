#pragma once

// NatNet data client
// NOTE: for now, Unicast only

#include <csignal>
#include <cstdint>
#include <string>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>

#include "DataBroker.h"

#define IPALL			"0.0.0.0"
#define LOOPBACK		"127.0.0.1"
#define DOCKER_HOST		"172.29.112.1" // TODO: Probably shouldnt hardcode this

#define CMD_PORT		1510L
#define DATA_PORT		1511L

#define MAX_NAMELEN		256L
#define MAX_PACKETSIZE	65504L
#define BUF_SIZE		65536L
#define SOCK_TIMEOUT	2L
#define SEND_RETRIES	4L

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
	TIMEOUT,
	ECHO_REQUEST,
	ECHO_RESPONSE,
	DISCOVERY,
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
	char	app_name[MAX_NAMELEN];
	uint8_t	version[4];
	uint8_t	natnet_version[4];
};

struct s_connection_opts
{
	bool	subscribed_data_only;
	uint8_t	bitstream_version[4];
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
		uint32_t	ints[MAX_PACKETSIZE / sizeof(uint32_t)];
		float		floats[MAX_PACKETSIZE / sizeof(float)];
		s_sender	sender;
	}	data;
};

typedef float t_markerdata[3];

struct s_marker_desc
{
	char	name[MAX_NAMELEN];
	int32_t	ID;
	float	x;
	float	y;
	float	z;
	float	size;
	int16_t	params;
};

struct s_marker
{
	int32_t	ID;
	float	x;
	float	y;
	float	z;
	float	size;
	int16_t	params;
	float	residual;
};

struct s_marketset_desc
{
	char	name[MAX_NAMELEN];
	int32_t	n_markers;
	char	**marker_names;
};

struct s_markerset_data
{
	char			name[MAX_NAMELEN];
	int32_t			n_markers;
	t_markerdata	*markers;
};

struct s_rigidbody_desc
{
	char			name[MAX_NAMELEN];
	int32_t			ID;
	int32_t			parentID;
	float			offset_x;
	float			offset_y;
	float			offset_z;
	int32_t			n_markers;
	t_markerdata	*marker_positions;
	int32_t			*marker_required_labels;
	char			**marker_names;
};

struct s_rigidbody_data
{
	int32_t	ID;
	float	x;
	float	y;
	float	z;
	float	qx;
	float	qy;
	float	qz;
	float	qw;
	float	mean_error; // mean marker error (meters)
	int16_t	params; // host defined tracking flags

	s_rigidbody_data()
		: ID(0), x(0.0), y(0.0), z(0.0),
		qx(0.0), qy(0.0), qz(0.0), qw(0.0),
		mean_error(0.0), params(0)
	{};
};

struct s_camera_desc
{
	char	name[MAX_NAMELEN];
	float	x;
	float	y;
	float	z;
	float	qx;
	float	qy;
	float	qz;
	float	qw;
};

struct s_data_desc
{
	int32_t	type;
	union
	{
		s_marketset_desc	*markerset_desc;
		s_rigidbody_desc	*rigidbody_desc;
		s_camera_desc		*camera_desc;
	}	data;
};

// struct s_mocap_dataframe
// {
// 	int32_t				idx;
// 	int32_t				n_markersets;
// 	s_markerset_data	markerset_data[20];
// 	int32_t				n_markers;
// 	t_markerdata		*markers;
// 	int32_t				n_rigidbodies;
// 	s_rigidbody_data	rigidbody_data[20];
// };

class NatNetClient
{
public:
	NatNetClient(int epoll_fd);
	~NatNetClient();

	int connect(s_ports *ports, char const *my_ip, char const *host_ip, s_databroker *db);
	void listen_cmd();
	int send_cmd(std::string const &cmd, e_msg_type type = REQUEST);
	void listen_data();

private:
	int			epoll_fd;
	int			cmd_socket;
	int			data_socket;
	sockaddr_in	host;
	s_packet	packet_in;
	s_packet	packet_out;

	int init(s_ports *ports, char const *my_ip, s_databroker *db);
	int request_conn(char const *ip, uint16_t port, uint16_t retries);
	int send_keepalive(void);
	void parse_cmd(void);
	void parse_data(void);
};
