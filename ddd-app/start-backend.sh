#!/bin/bash

# Function to detect platform and open new terminal
open_terminal() {
    local command="$1"  # The command to run in the new terminal

    # Detect the platform
    case "$(uname)" in
        "Linux")
            if command -v gnome-terminal > /dev/null 2>&1; then
                gnome-terminal -- bash -c "$command; exec bash"
            elif command -v xterm > /dev/null 2>&1; then
                xterm -hold -e "$command"
            else
                echo "No supported terminal found on Linux!"
                exit 1
            fi
            ;;
        "Darwin")  # macOS
            if command -v osascript > /dev/null 2>&1; then
                osascript -e "tell application \"Terminal\" to do script \"$command\""
            else
                echo "osascript not found on macOS!"
                exit 1
            fi
            ;;
        *)
            echo "Unsupported OS"
            exit 1
            ;;
    esac
}


cd backend

make

# Run the server in the background and log its PID
./ddd-WebSocketServer &
SERVER_PID=$!

# Capture the PID of the server for cleanup on exit
echo "Server running with PID: $SERVER_PID\n"

sleep 1

./ddd-controller &
CONTROLLER_PID=$!

echo "Controller running with PID: $CONTROLLER_PID\n"

sleep 1

# Usage example: run a command in a new terminal
open_terminal "./ddd-clientkey" &
key_PID=$!

echo "Controller running with PID: $key_PID\n"

# # Define a cleanup function to stop the server when the script stops
# cleanup() {
#     echo "Stopping C++ server..."
#     kill $SERVER_PID
# }
# trap cleanup EXIT

# # Keep the script alive to allow the server to run
# wait $SERVER_PID
