function parseWsMessage(
	setMessages, event,
	setPosition0, setTrim0, setLight0, setSetpoint0, setBat0, setRc0, setAti0, setPathLen0, setFrame0,
	setPosition1, setTrim1, setLight1, setSetpoint1, setBat1, setRc1, setAti1, setPathLen1, setFrame1,
	setPosition2, setTrim2, setLight2, setSetpoint2, setBat2, setRc2, setAti2, setPathLen2, setFrame2,
	setPosition3, setTrim3, setLight3, setSetpoint3, setBat3, setRc3, setAti3, setPathLen3, setFrame3,
	setGraphInfo
	) {
	// Extract drone index and the binary data from event.data
	const droneIndex = parseInt(event.data[0], 10); // Get drone index (0, 1, 2...)
	const jsonString = event.data.slice(1); // Everything after the first byte is the binary data
		
	// console.log(event.data);
	if (droneIndex === 9) { // drone 9 is not a drone but graph info. TODO make it work for different drone.
		const graph = JSON.parse(jsonString);
		setGraphInfo(graph);
		return;
	}
	
	try {
		// console.log("json = ", jsonString);
		const jsonData = JSON.parse(jsonString);
		if (jsonData.rc) {
			switch (droneIndex) {
			  case 0:
				setRc0([jsonData.rc[0], jsonData.rc[1], jsonData.rc[2], jsonData.rc[3], jsonData.rc[4]]);
				break;
			  case 1:
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
			}
		}
		if (jsonData.pos) {
			switch (droneIndex) {
			  case 0:
				setPosition0([jsonData.pos[0], jsonData.pos[2], jsonData.pos[1], jsonData.pos[3]]);
				break;
			  case 1:
				setPosition1([jsonData.pos[0], jsonData.pos[2], jsonData.pos[1], jsonData.pos[3]]);
				break;
			  case 2:
				setPosition2([jsonData.pos[0], jsonData.pos[2], jsonData.pos[1], jsonData.pos[3]]);
				break;
			  case 3:
				setPosition3([jsonData.pos[0], jsonData.pos[2], jsonData.pos[1], jsonData.pos[3]]);
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
				setSetpoint0([jsonData.setpoint[0], jsonData.setpoint[2], jsonData.setpoint[1], 0]); // replace with yaw setpoint if yaw needed.
				break;
			case 1:
				setSetpoint1([jsonData.setpoint[0], jsonData.setpoint[2], jsonData.setpoint[1], 0]);
				break;
			case 2:
				setSetpoint2([jsonData.setpoint[0], jsonData.setpoint[2], jsonData.setpoint[1], jsonData.setpoint[3]]);
				break;
			case 3:
				setSetpoint3([jsonData.setpoint[0], jsonData.setpoint[2], jsonData.setpoint[1], jsonData.setpoint[3]]);
				break;
			default:
				console.error('Invalid drone index');
			}
		}
		if (jsonData.length) {
			switch (droneIndex) {
			  case 0:
				setPathLen0([jsonData.length]);
				break;
			  case 1:
				setPathLen1([jsonData.length]);
				break;
			  case 2:
				setPathLen2([jsonData.length]);
				break;
			  case 3:
				setPathLen3([jsonData.length]);
				break;
			  default:
				console.error('Invalid drone index');
			}
		}
		if (jsonData.frame) {
			switch (droneIndex) {
			  case 0:
				setFrame0([jsonData.frame]);
				break;
			  case 1:
				setFrame1([jsonData.frame]);
				break;
			  case 2:
				setFrame2([jsonData.frame]);
				break;
			  case 3:
				setFrame3([jsonData.frame]);
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
		if (!(jsonData.bat ) && !(jsonData.rc) && !(jsonData.ati)) { // Update the state with the parsed message to display and keep last 100
			setMessages((prevMessages) => {
				const newMessages = [...prevMessages, event.data];
				return newMessages.length > 100 ? newMessages.slice(-100) : newMessages; 
			});
		}
	} catch (error) {
		console.error("Error parsing JSON:", error);
	}
}

export {parseWsMessage};