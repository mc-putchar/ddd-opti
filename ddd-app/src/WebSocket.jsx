
function parseWsMessage(messages, setMessages, event,
    wsState, setWsState,
    wsStateColor, setWsStateColor,
    ws, setWs,
    position0, setPosition0,
			position1, setPosition1,
			position2, setPosition2,
			position3, setPosition3,

			trim0, setTrim0,
			trim1, setTrim1,
			trim2, setTrim2,
			trim3, setTrim3,

			light0, setLight0,
			light1, setLight1,
			light2, setLight2,
			light3, setLight3,

			setpoint0, setSetpoint0,
			setpoint1, setSetpoint1,
			setpoint2, setSetpoint2,
			setpoint3, setSetpoint3,

			bat0, setBat0,
			bat1, setBat1,
			bat2, setBat2,
			bat3, setBat3,

			rc0, setRc0,
			rc1, setRc1,
			rc2, setRc2,
			rc3, setRc3,

			ati0, setAti0,
			ati1, setAti1,
			ati2, setAti2,
			ati3, setAti3,

			pathLen0, setPathLen0,
			pathLen1, setPathLen1,
			pathLen2, setPathLen2,
			pathLen3, setPathLen3,

			frame0, setFrame0,
			frame1, setFrame1,
			frame2, setFrame2,
			frame3, setFrame3) {

	// Extract drone index and JSON part
	const droneIndex = parseInt(event.data[0], 10);  // Get drone index (0, 1, 2...)
	const jsonString = event.data.slice(1);  // Get everything after the first character

	try {
		const jsonData = JSON.parse(jsonString);  // Parse the cleaned JSON string
		console.log("json = ", jsonString);
		if (jsonData.rc) {
			switch (droneIndex) {
			  case 0:
				console.log("0");
				setRc0([jsonData.rc[0], jsonData.rc[1], jsonData.rc[2], jsonData.rc[3], jsonData.rc[4]]);
				break;
			  case 1:
				console.log("1");
				setRc1([jsonData.rc[0], jsonData.rc[1], jsonData.rc[2], jsonData.rc[3], jsonData.rc[4]]);
				break;
			  case 2:
				setRc2([jsonData.rc[0], jsonData.rc[1], jsonData.rc[2], jsonData.rc[3], jsonData.rc[4]]);
				break;
			  case 3:
				setRc3([jsonData.rc[0], jsonData.rc[1], jsonData.rc[2], jsonData.rc[3], jsonData.rc[4]]);
				break;
			  default:
				console.error('Invalid drone index');
			}
		  }
		if (jsonData.trim) {
			switch (droneIndex) {
			  case 0:
				setTrim0([jsonData.trim[0], jsonData.trim[1], jsonData.trim[2], jsonData.trim[3]]);
				break;
			  case 1:
				setTrim1([jsonData.trim[0], jsonData.trim[1], jsonData.trim[2], jsonData.trim[3]]);
				break;
			  case 2:
				setTrim2([jsonData.trim[0], jsonData.trim[1], jsonData.trim[2], jsonData.trim[3]]);
				break;
			  case 3:
				setTrim3([jsonData.trim[0], jsonData.trim[1], jsonData.trim[2], jsonData.trim[3]]);
				break;
			  default:
				console.error('Invalid drone index');
		}}
		if (jsonData.position) {
			switch (droneIndex) {
			  case 0:
				setPosition0([jsonData.position[0], jsonData.position[1], jsonData.position[2], jsonData.position[3]]);
				break;
			  case 1:
				setPosition1([jsonData.position[0], jsonData.position[1], jsonData.position[2], jsonData.position[3]]);
				break;
			  case 2:
				setPosition2([jsonData.position[0], jsonData.position[1], jsonData.position[2], jsonData.position[3]]);
				break;
			  case 3:
				setPosition3([jsonData.position[0], jsonData.position[1], jsonData.position[2], jsonData.position[3]]);
				break;
			  default:
				console.error('Invalid drone index');
			}
		}
		if (jsonData.light) {
			switch (droneIndex) {
			  case 0:
				setLight0([jsonData.light[0], jsonData.light[1]]);
				break;
			  case 1:
				setLight1([jsonData.light[0], jsonData.light[1]]);
				break;
			  case 2:
				setLight2([jsonData.light[0], jsonData.light[1]]);
				break;
			  case 3:
				setLight3([jsonData.light[0], jsonData.light[1]]);
				break;
			  default:
				console.error('Invalid drone index');
			}
		  }
		  
		if (jsonData.setpoint) {
			switch (droneIndex) {
			  case 0:
				setPosition0([jsonData.setpoint[0], jsonData.setpoint[2], jsonData.setpoint[1], jsonData.setpoint[3]]);
				break;
			  case 1:
				setPosition1([jsonData.setpoint[0], jsonData.setpoint[2], jsonData.setpoint[1], jsonData.setpoint[3]]);
				break;
			  case 2:
				setPosition2([jsonData.setpoint[0], jsonData.setpoint[2], jsonData.setpoint[1], jsonData.setpoint[3]]);
				break;
			  case 3:
				setPosition3([jsonData.setpoint[0], jsonData.setpoint[2], jsonData.setpoint[1], jsonData.position[3]]);
				break;
			  default:
				console.error('Invalid drone index');
			  }
		}
		if (jsonData.pathLen) {
			switch (droneIndex) {
			  case 0:
				setPathLen0([jsonData.pathLen[0]]);
				break;
			  case 1:
				setPathLen1([jsonData.pathLen[0]]);
				break;
			  case 2:
				setPathLen2([jsonData.pathLen[0]]);
				break;
			  case 3:
				setPathLen3([jsonData.pathLen[0]]);
				break;
			  default:
				console.error('Invalid drone index');
			}
		  }
		  
		  if (jsonData.frame) {
			  console.log("frame");
			switch (droneIndex) {
			  case 0:
				setFrame0([jsonData.frame[0]]);
				break;
			  case 1:
				setFrame1([jsonData.frame[0]]);
				break;
			  case 2:
				setFrame2([jsonData.frame[0]]);
				break;
			  case 3:
				setFrame3([jsonData.frame[0]]);
				break;
			  default:
				console.error('Invalid drone index');
			}
		  }
		  
		if (jsonData.bat) {
			switch (droneIndex) {
			  case 0:
				setBat0([jsonData.bat[0], jsonData.bat[1]]);
				break;
			  case 1:
				setBat1([jsonData.bat[0], jsonData.bat[1]]);
				break;
			  case 2:
				setBat2([jsonData.bat[0], jsonData.bat[1]]);
				break;
			  case 3:
				setBat3([jsonData.bat[0], jsonData.bat[1]]);
				break;
			  default:
				console.error('Invalid drone index');
			}
		  }
		  
		  
		  if (jsonData.ati) {
			switch (droneIndex) {
			  case 0:
				setAti0([jsonData.ati[0], jsonData.ati[1], jsonData.ati[2]]);
				break;
			  case 1:
				setAti1([jsonData.ati[0], jsonData.ati[1], jsonData.ati[2]]);
				break;
			  case 2:
				setAti2([jsonData.ati[0], jsonData.ati[1], jsonData.ati[2]]);
				break;
			  case 3:
				setAti3([jsonData.ati[0], jsonData.ati[1], jsonData.ati[2]]);
				break;
			  default:
				console.error('Invalid drone index');
			}
		  }
		  if (jsonData.setpoint) {
			switch (droneIndex) {
			  case 0:
				setSetpoint0([jsonData.setpoint[0], jsonData.setpoint[1]]);
				break;
			  case 1:
				setSetpoint1([jsonData.setpoint[0], jsonData.setpoint[1]]);
				break;
			  case 2:
				setSetpoint2([jsonData.setpoint[0], jsonData.setpoint[1]]);
				break;
			  case 3:
				setSetpoint3([jsonData.setpoint[0], jsonData.setpoint[1]]);
				break;
			  default:
				console.error('Invalid drone index');
			}
		  }
		  
		  

	  if (jsonData.arm) {
	  }

	  if (!(jsonData.bat )) {
		  // Update the state with the parsed message to display
		  setMessages((prevMessages) => {
			  const newMessages = [...prevMessages, event.data];
			  console.log("*");
			  console.log("here ", event.data);
			  return newMessages.length > 100 ? newMessages.slice(-100) : newMessages; // Keep only the last 50 messages
		  });

	}
  
	  // Update drone positions if necessary
	} catch (error) {
	  console.error("Error parsing JSON:", error);
	}
}

// Function to fetch the port from JSON and initialize the WebSocket
	const fetchConfigAndInitializeWebSocket = async (messages, setMessages,
		wsState, setWsState,
		wsStateColor, setWsStateColor,
		ws, setWs,
		position0, setPosition0,
		position1, setPosition1,
		position2, setPosition2,
		position3, setPosition3,

		trim0, setTrim0,
		trim1, setTrim1,
		trim2, setTrim2,
		trim3, setTrim3,

		light0, setLight0,
		light1, setLight1,
		light2, setLight2,
		light3, setLight3,

		setpoint0, setSetpoint0,
		setpoint1, setSetpoint1,
		setpoint2, setSetpoint2,
		setpoint3, setSetpoint3,

		bat0, setBat0,
		bat1, setBat1,
		bat2, setBat2,
		bat3, setBat3,

		rc0, setRc0,
		rc1, setRc1,
		rc2, setRc2,
		rc3, setRc3,

		ati0, setAti0,
		ati1, setAti1,
		ati2, setAti2,
		ati3, setAti3,

		pathLen0, setPathLen0,
		pathLen1, setPathLen1,
		pathLen2, setPathLen2,
		pathLen3, setPathLen3,

		frame0, setFrame0,
		frame1, setFrame1,
		frame2, setFrame2,
		frame3, setFrame3
) => {
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
				// console.log('Message from server:', event.data);
				parseWsMessage(messages, setMessages, event,
					wsState, setWsState,
					wsStateColor, setWsStateColor,
					ws, setWs,
					position0, setPosition0,
					position1, setPosition1,
					position2, setPosition2,
					position3, setPosition3,

					trim0, setTrim0,
					trim1, setTrim1,
					trim2, setTrim2,
					trim3, setTrim3,

					light0, setLight0,
					light1, setLight1,
					light2, setLight2,
					light3, setLight3,

					setpoint0, setSetpoint0,
					setpoint1, setSetpoint1,
					setpoint2, setSetpoint2,
					setpoint3, setSetpoint3,

					bat0, setBat0,
					bat1, setBat1,
					bat2, setBat2,
					bat3, setBat3,

					rc0, setRc0,
					rc1, setRc1,
					rc2, setRc2,
					rc3, setRc3,

					ati0, setAti0,
					ati1, setAti1,
					ati2, setAti2,
					ati3, setAti3,

					pathLen0, setPathLen0,
					pathLen1, setPathLen1,
					pathLen2, setPathLen2,
					pathLen3, setPathLen3,

					frame0, setFrame0,
					frame1, setFrame1,
					frame2, setFrame2,
					frame3, setFrame3
					);
				
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