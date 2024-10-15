#!/bin/bash

# Compile the C++ server
# g++ main.cpp -o server -std=c++11 # TODO adapt to my drone ex

# Run the server in the background and log its PID
./backend/ddd-WebSocketServer &
SERVER_PID=$!

# Capture the PID of the server for cleanup on exit
echo "Server running with PID: $SERVER_PID"

./backend/ddd-controller &
CONTROLLER_PID=$!

echo "Controller running with PID: $CONTROLLER_PID"

# # Define a cleanup function to stop the server when the script stops
# cleanup() {
#     echo "Stopping C++ server..."
#     kill $SERVER_PID
# }
# trap cleanup EXIT

# # Keep the script alive to allow the server to run
# wait $SERVER_PID
