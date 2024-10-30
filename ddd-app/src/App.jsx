import React, { useEffect, useState } from 'react';
import {Drone, DroneControllerView} from './Drone';
import { Col, Row, Container, Button} from 'react-bootstrap';
import { Canvas } from '@react-three/fiber';
import { OrbitControls, Sphere, Plane, Cylinder, Box } from '@react-three/drei';



function Live3dview({ position, setpoint, light }) {

	return (
	<Canvas
		orthographic
		camera={{ zoom: 40, position: [3, 3.5, 10] }}
		style={{ background: '#222' }}
		shadowMap
		shadows
	>
		<ambientLight intensity={0.6} />
		<Cylinder position={[0, 0.9, 0]} args={[0.3, 0.7, 1.8]} castShadow receiveShadow>
			<meshStandardMaterial attach="material" color="blue" />
		</Cylinder>
		<Plane position={[0, -0.01, 0]} args={[11, 11]} rotation={[-Math.PI / 2, 0, 0]} receiveShadow>
			<meshStandardMaterial attach="material" color="darkgrey" />
		</Plane>
		{Object.keys(position).map((index) => (
			<Drone
				key={index}
				index={index} // Pass the index of the drone
				position={position[index]} // Access the position of the current drone
				light={light[index]} // Access the light of the current drone
				setpoint={setpoint[index]} // Access the setpoint of the current drone
			/>
		))}

		{[ // THE OPTITRACKS POLES
			[-5, 1.25, -5],  // Bottom left
			[5, 1.25, -5],   // Bottom right
			[-5, 1.25, 5],   // Top left
			[5, 1.25, 5],    // Top right
		].map((position, index) => (
			<Cylinder args={[0.07, 0.07, 2.5, 32]} position={position} key={index}>
				<meshStandardMaterial attach="material" color="red" />
			</Cylinder>
		))}
		<OrbitControls target={[0, 1.5, 0]} />
		{/* <axesHelper args={[1.5]} /> */}
		<gridHelper args={[10, 10]} />
		{/* <directionalLight /> */}
	</Canvas>
	);
}

function App() {
	const [message, setMessage] = useState('');
	const [messages, setMessages] = useState([]);
	const [wsState, setWsState] = useState('disconnected');
	const [wsStateColor, setWsStateColor] = useState('text-success');
	const [ws, setWs] = useState(null);
	const [wsPort, setWsPort] = useState(8080); // Default port
	const [position, setPosition] = useState({
		0: [-6, 1, -3],
		1: [-6, 1, -1],
		2: [-6, 1, 1],
		3: [-6, 1, 3]
	});
	const [trim, setTrim] = useState({
		0: [1, 1, 1, 1],
		1: [1, 1, 1, 1],
		2: [1, 1, 1, 1],
		3: [1, 1, 1, 1]
	});
	const [light, setLight] = useState({
		0: [1, 1],
		1: [1, 1],
		2: [1, 1],
		3: [1, 1]
	});
	const [setpoint, setSetpoint] = useState({
		0: [1, 1],
		1: [1, 1],
		2: [1, 1],
		3: [1, 1]
	});
	

	// Function to fetch the port from JSON and initialize the WebSocket
	const fetchConfigAndInitializeWebSocket = async () => {
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
				
				// Clean the message by removing any unwanted prefixes
				const cleanMsg = event.data.replace("Message received: ", "");
				
				// Extract drone index and JSON part
				const droneIndex = parseInt(cleanMsg[0], 10);  // Get drone index (0, 1, 2...)
				const jsonString = cleanMsg.slice(1);  // Get everything after the first character
			  
				try {
				  const jsonData = JSON.parse(jsonString);  // Parse the cleaned JSON string
				  
				  // Handle the different cases based on the keys in jsonData
				  let messageToDisplay = '';
			  

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
				  if (jsonData.setpoint) {
					setPosition(prevPosition => {
					  // Copy the current trim state for all drones
					  const updatedPosition = { ...prevPosition };
				  
					  // Update the specific drone's trim values (keeping the array structure intact)
					  updatedPosition[droneIndex] = [
						jsonData.setpoint[0],  // x value
						jsonData.setpoint[2],  // z value (reordered as needed)
						jsonData.setpoint[1],  // y value (reordered as needed)
						0
					  ];
					  return updatedPosition; // Return the updated state
					});
				  }

				  if (jsonData.arm) {
				  }
	
				  // Update the state with the parsed message to display
				  setMessages((prevMessages) => {
					const newMessages = [...prevMessages, cleanMsg];
					return newMessages.length > 100 ? newMessages.slice(-100) : newMessages; // Keep only the last 50 messages
				});
			  
				  // Update drone positions if necessary
				  
				} catch (error) {
				  console.error("Error parsing JSON:", error);
				}
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

	useEffect(() => {
		fetchConfigAndInitializeWebSocket();
	}, []);

	return (
	<>
	<Container fluid>
	<Row className="p-4">

		<Row className="px-0 mb-2 align-items-end">
			<Col className="px-0" xs="auto">
				<h3 className="mb-0">DDD_liveView</h3>	
			</Col>
			<Col xs="auto">
				<span className={wsStateColor}>{wsState}</span>
			</Col>
		</Row>

		<Row className="p-0" style={{ height: "400px" }}>
			<Live3dview position={position} setpoint={setpoint} light={light} />
		</Row>

		<Row>
			<DroneControllerView position={position} setpoint={setpoint} light={light} trim={trim} setSetpoint={setSetpoint} setPosition={setPosition} setLight={setLight} setTrim={setTrim} ws={ws} />
			<Col className="p-3"> {/*  Console log */}
				<div className="overflow-auto" style={{ lineHeight: '1.2', maxHeight: '300px' }}>
					{messages.map((msg, index) => (
						<p className="m-0 mt-1" key={index}>{msg}</p>
					))}
				</div>
			</Col>
		</Row>

	</Row>
	</Container>
	</>
	);
}


function Arm({index, ws}) {

	function arm() {
		if (ws && ws.readyState === WebSocket.OPEN) {
			ws.send(`${index}{"armed":true}`);
		}
	}

	function disarm() {
		if (ws && ws.readyState === WebSocket.OPEN) {
			ws.send(`${index}{"armed":false}`);
		}
	}

	return (
	<>
		<Button className="m-0" onClick={arm} variant="secondary" >Arm</Button>
		<Button className="m-1" onClick={disarm} variant="secondary">Disarm</Button>
	</>
	)
}

export default App;
