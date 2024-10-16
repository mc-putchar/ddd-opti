import React, { useEffect, useState } from 'react';
import { Col, Container } from 'react-bootstrap';
import { Canvas } from '@react-three/fiber';
import { OrbitControls, Sphere } from '@react-three/drei';

function App() {
	const [message, setMessage] = useState('');
	const [ws, setWs] = useState(null);
	const [wsPort, setWsPort] = useState(8080); // Default port

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
				socket.send('Hello from react');
			};

			socket.onmessage = (event) => {
				console.log('Message from server:', event.data);
				setMessage(event.data);
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
		<Container fluid>
			<h4>Stage Preview</h4>
			<p>{message}</p>
			<Col style={{ height: "800px", width: "1400px" }}>
				<Canvas
					orthographic
					camera={{ zoom: 100, position: [0, 0, 10] }}
					style={{ background: '#333' }} // Set background color to dark grey
				>
					<ambientLight intensity={0.5} />
					<spotLight
						position={[5, 5, 5]} // Positioning the light to the side
						angle={0.3} // Spread of the light cone
						penumbra={1} // Softness of the edges
						intensity={1} // Brightness of the light
						castShadow // Enable shadows
					/>
					<Sphere position={[0, 0, 0]} args={[1, 32, 32]}>
						<meshStandardMaterial attach="material" color="blue" />
					</Sphere>
					<OrbitControls />
				</Canvas>
			</Col>
		</Container>
	);
}

export default App;
