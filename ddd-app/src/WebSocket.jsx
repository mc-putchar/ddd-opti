
function parseWsMessage(event, setTrim, setLight, setPosition, setSetpoint, setMessages) {

	// Extract drone index and JSON part
	const droneIndex = parseInt(event.data[0], 10);  // Get drone index (0, 1, 2...)
	const jsonString = event.data.slice(1);  // Get everything after the first character

	try {
		const jsonData = JSON.parse(jsonString);  // Parse the cleaned JSON string
		if (jsonData.trim) {
			setTrim(prevTrims => {
				// Copy the current trim state for all drones
				const updatedTrims = { ...prevTrims };
			
				// Update the specific drone's trim values (keeping the array structure intact)
				updatedTrims[droneIndex] = [
					jsonData.trim[0],  // x value
					jsonData.trim[2],  // z value (reordered as needed)
					jsonData.trim[1],  // y value (reordered as needed)
					jsonData.trim[3]   // any additional value (if applicable)
				];
				return updatedTrims; // Return the updated state
			});
		}
		if (jsonData.position) {
			setTrim(prevPosition => {
				// Copy the current trim state for all drones
				const updatedPosition = { ...prevPosition };
			
				// Update the specific drone's trim values (keeping the array structure intact)
				updatedPosition[droneIndex] = [
					jsonData.position[0],  // x value
					jsonData.position[2],  // z value (reordered as needed)
					jsonData.position[1],  // y value (reordered as needed)
				];
				return updatedPosition; // Return the updated state
			});
		}
		if (jsonData.light) {
			setLight(prevLight => {
				// Copy the current trim state for all drones
				const updatedLight = { ...prevLight };
			
				// Update the specific drone's trim values (keeping the array structure intact)
				updatedLight[droneIndex] = [
					jsonData.light[0],  // angle
					jsonData.light[1],  // power
				];
				return updatedLight; // Return the updated state
			});
		}
		if (jsonData.setpoint) {
			setPosition(prevPosition => {
				// Copy the current trim state for all drones
				const updatedPosition = { ...prevPosition };
			
				// Update the specific drone's trim values (keeping the array structure intact)
				updatedPosition[droneIndex] = [
					jsonData.setpoint[0],  // x value
					jsonData.setpoint[2],  // z value (reordered as needed)
					jsonData.setpoint[1],  // y value (reordered as needed)
					jsonData.setpoint[3] // yaw value or rotation 
				];
				return updatedPosition; // Return the updated state
			});
		}

	  if (jsonData.arm) {
	  }

	  // Update the state with the parsed message to display
	  setMessages((prevMessages) => {
		const newMessages = [...prevMessages, event.data];
		return newMessages.length > 100 ? newMessages.slice(-100) : newMessages; // Keep only the last 50 messages
	});
  
	  // Update drone positions if necessary
	  
	} catch (error) {
	  console.error("Error parsing JSON:", error);
	}
}

// Function to fetch the port from JSON and initialize the WebSocket
	const fetchConfigAndInitializeWebSocket = async (setWs, setWsState, setWsStateColor, setMessages, setWsPort, setTrim, setLight, setPosition, setSetpoint) => {
		try {
			const response = await fetch('/port.json'); // Adjust the path if needed
			if (!response.ok) {
				throw new Error('Network response was not ok');
			}
			const config = await response.json();
			const newWsPort = config.wsPort; // Update wsPort with the value from config
			console.log('wsPort from json =', newWsPort);
			setWsPort(newWsPort); // Update state with the new port

			// Initialize the WebSocket connection with the new port
			const socket = new WebSocket(`ws://localhost:${newWsPort}/ws`);
			setWs(socket);

			socket.onopen = () => {
				console.log('Connected to WebSocket server');
				setWsState('connected');
				setWsStateColor('text-success');
			};

			socket.onmessage = (event) => {
				console.log('Message from server:', event.data);
				parseWsMessage(event, setTrim, setLight, setPosition, setSetpoint, setMessages);
				
			  };
			socket.onclose = () => {
				setWsState('diconnected');
				setWsStateColor('text-danger');
				console.log('WebSocket connection closed');
			  };


			// Cleanup function to close the WebSocket connection
			return () => socket.close();
		} catch (error) {
			console.error('Error fetching config:', error);
		}
	};

export {fetchConfigAndInitializeWebSocket};