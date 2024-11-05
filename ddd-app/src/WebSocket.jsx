
function parseWsMessage(event, setPathLen, setFrame, setTrim, setLight, setPosition, setSetpoint, setMessages) {

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
					jsonData.trim[1],  // z value (reordered as needed)
					jsonData.trim[2],  // y value (reordered as needed)
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
		if (jsonData.length) {
			setPathLen(prevPosition => {
				// Copy the current trim state for all drones
				const updatedPosition = { ...prevPosition };
			
				// Update the specific drone's trim values (keeping the array structure intact)
				updatedPosition[droneIndex] = [
					jsonData.length
				];
				return updatedPosition; // Return the updated state
			});
		}
		if (jsonData.frame) {
			setFrame(prevPosition => {
				// Copy the current trim state for all drones
				const updatedPosition = { ...prevPosition };
			
				// Update the specific drone's trim values (keeping the array structure intact)
				updatedPosition[droneIndex] = [
					jsonData.frame
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
	const fetchConfigAndInitializeWebSocket = async (setWs, setPathLen, setFrame, setWsState, setWsStateColor, setMessages, setTrim, setLight, setPosition, setSetpoint) => {
		try {
			const response = await fetch('/port.json'); // Fetch shared port with backend ina config file
			if (!response.ok) {
				throw new Error('Network response was not ok');
			}
			const config = await response.json();
			console.log('wsPort from json =', config.wsPort);
			const socket = new WebSocket(`ws://localhost:${config.wsPort}/ws`);
			setWs(socket);

			socket.onopen = () => {
				console.log('Connected to WebSocket server');
				setWsState('connected');
				setWsStateColor('text-green-600');
			};

			socket.onmessage = (event) => {
				console.log('Message from server:', event.data);
				parseWsMessage(event, setPathLen, setFrame, setTrim, setLight, setPosition, setSetpoint, setMessages);
				
			  };
			socket.onclose = () => {
				setWsState('diconnected');
				setWsStateColor('text-red-600');
				console.log('WebSocket connection closed');
			  };


			// Cleanup function to close the WebSocket connection
			return () => socket.close();
		} catch (error) {
			console.error('Error fetching config:', error);
		}
	};

export {fetchConfigAndInitializeWebSocket};