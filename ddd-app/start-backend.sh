#!/bin/bash

# Set the port file location
PORT_FILE="last_port.txt"
CONFIG_FILE="public/port.json" # Adjust the path as needed

# Check if the port file exists; if not, set the starting port
if [ ! -f "$PORT_FILE" ]; then
    echo "8000" > "$PORT_FILE" # Starting port
fi

# Read the last used port and increment it
LAST_PORT=$(cat "$PORT_FILE")
NEW_PORT=$((LAST_PORT + 1))
export REACT_APP_WS_PORT=$NEW_PORT
echo "$NEW_PORT" > "$PORT_FILE" # Update the port file
echo "Running WebSocket connections on port $NEW_PORT"

# Create the config.json file
echo "Creating config.json..."
echo "{
    \"wsPort\": $NEW_PORT
}" > "$CONFIG_FILE"

echo "Config file created at $CONFIG_FILE with wsPort: $NEW_PORT"

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
                # Properly escape the command for osascript
                osascript -e "tell application \"Terminal\" to do script \"${command//\"/\\\"}\""
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



open_terminal "./ddd-controller $NEW_PORT" &
CONTROLLER_PID=$!
echo -e "Controller running with PID: $CONTROLLER_PID\n"
sleep 1

# Usage example: run a command in a new terminal
open_terminal "./ddd-clientkey" &
key_PID=$!
echo -e "ClientKey running with PID: $key_PID\n"

# # Define a cleanup function to stop the server when the script stops
# cleanup() {
#     echo "Stopping C++ server..."
#     kill $SERVER_PID
# }
# trap cleanup EXIT

# # Keep the script alive to allow the server to run
 wait $CONTROLLER_PID
