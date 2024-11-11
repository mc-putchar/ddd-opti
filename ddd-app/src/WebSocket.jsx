import { parseWsMessage } from "./Parsing";

const fetchConfigAndInitializeWebSocket = async (setMessages, setWs, setWsState, setWsStateColor,
setPosition0, setTrim0, setLight0, setSetpoint0, setBat0, setRc0, setAti0, setPathLen0, setFrame0,
setPosition1, setTrim1, setLight1, setSetpoint1, setBat1, setRc1, setAti1, setPathLen1, setFrame1,
setPosition2, setTrim2, setLight2, setSetpoint2, setBat2, setRc2, setAti2, setPathLen2, setFrame2,
setPosition3, setTrim3, setLight3, setSetpoint3, setBat3, setRc3, setAti3, setPathLen3, setFrame3, block_incoming_setpoint
 ) => {
	try {
		const response = await fetch('/port.json'); // Fetch shared port in a config file
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
			parseWsMessage(setMessages, event,
		setPosition0, setTrim0, setLight0, setSetpoint0, setBat0, setRc0, setAti0, setPathLen0, setFrame0,
		setPosition1, setTrim1, setLight1, setSetpoint1, setBat1, setRc1, setAti1, setPathLen1, setFrame1,
		setPosition2, setTrim2, setLight2, setSetpoint2, setBat2, setRc2, setAti2, setPathLen2, setFrame2,
		setPosition3, setTrim3, setLight3, setSetpoint3, setBat3, setRc3, setAti3, setPathLen3, setFrame3, block_incoming_setpoint
				);
		};

		socket.onclose = () => {
			setWsState('diconnected');
			setWsStateColor('text-red-600');
			console.log('WebSocket connection closed');
		};

		return () => socket.close(); // Cleanup function to close the WebSocket connection
	} catch (error) {
		console.error('Error fetching config:', error);
	}
};

export {fetchConfigAndInitializeWebSocket};