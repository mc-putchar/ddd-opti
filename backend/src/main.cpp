#include <iostream>
#include <thread>

#include "DroneControl.hpp"
#include "DroneState.hpp"
#include "Path.hpp"
#include "SerialHandler.hpp"
#include "WsServer.hpp"

//FOR SAMLPLE CLIENT
#include <NatNetTypes.h>
#include <NatNetCAPI.h>
#include <NatNetClient.h>
using namespace std;

// TODO: 
// Refactor program control flow
// Segments:
// - Serial port connection and communication
//	- Warn if disconnected (CRITICAL)
//	- Establish message queue for send/receive
// - WebSocket connection and communication
// - Internal data model - DroneState
//	- track and update individual drones position, status, trajectory
//	- decouple from dependencies
// - Optitrack MotiveAPI integration
//	- Initialize
//	- retrieve positional data of registered rigid bodies
//	- correlate with target DroneState and update its internal state
// - Unit Tests
// - Integration Tests
// - GUI
// - Documentation
#define WS_PORT	4242

void NATNET_CALLCONV MessageHandler( Verbosity msgType, const char* msg ){}
void NATNET_CALLCONV DataHandler(sFrameOfMocapData* data, void* pUserData){}


void SampleClienttemp() {
	NatNetClient* g_pClient = NULL;
	unsigned char ver[4];
    NatNet_GetVersion( ver );
    printf( "NatNet Sample Client (NatNet ver. %d.%d.%d.%d)\n", ver[0], ver[1], ver[2], ver[3] );

    // Install logging callback
    // NatNet_SetLogCallback( MessageHandler );

    // Create NatNet client
    g_pClient = new NatNetClient();

    // Set the frame callback handler
    // g_pClient->SetFrameReceivedCallback( DataHandler, g_pClient );

}


int main(void) {
	std::string const serialPort = getSerialPort();
	if (serialPort.empty())
		return 1;

	SampleClienttemp();

	SerialHandler serialHandler(serialPort);
	serialHandler.setup();
	std::thread transmitThread([&serialHandler]() { // Monitor if message are properly delivered to drones.
		serialHandler.monitorIncoming(); // Could come off a thread and added after each send if not setting too much latency.
	});

	WsServer &wsServer = WsServer::getInstance();
	wsServer.start();

	DroneControl &control = DroneControl::getInstance();
	control.track(std::make_shared<DroneState>(0, serialHandler));
	control.track(std::make_shared<DroneState>(1, serialHandler));
	control.track(std::make_shared<DroneState>(2, serialHandler));
	control.track(std::make_shared<DroneState>(3, serialHandler));

	Path path("chocolate_Circle.json"); // Create path on the stack
	control.setPath(0, &path);

	Path path1("animation.json"); // Create path on the stack
	control.setPath(1, &path1);

	transmitThread.join();
	// wsServerThread.join();
	return 0;
}
