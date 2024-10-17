import React, { useEffect, useState } from 'react';
import { Col, Card, Row, Form, Container, Button} from 'react-bootstrap';
import { Canvas } from '@react-three/fiber';
import { OrbitControls, Sphere, Plane } from '@react-three/drei';


function Slider({name}) {
	const [value, setValue] = useState(200);
  
	const handleChange = (event) => {
	  setValue(event.target.value);
	};
  
	return (
	  <div>
		<Form.Group>
		  <Form.Label>{name}: {value}</Form.Label>
		  <Form.Control
			type="range"
			min="-500"
			max="500"
			value={value}
			onChange={handleChange}
		  />
		</Form.Group>
	  </div>
	);
}

function Live3dview() {

	return (
		<Canvas
			orthographic
			camera={{ zoom: 80, position: [3, 3, 10] }}
			style={{ background: '#222' }}
			shadowMap
			>
			<ambientLight intensity={0.5} />
			<spotLight
				position={[5, 5, 5]}
				angle={0.25}
				penumbra={1}
				intensity={100}
				castShadow // not castin nay shadow for some reason ?
				/>
			<Sphere position={[0, 1, 0]} args={[1, 32, 32]} castShadow>
				<meshStandardMaterial attach="material" color="blue" />
			</Sphere>
			<Plane position={[0, -0.01, 0]} args={[9, 9]} rotation={[-Math.PI / 2, 0, 0]} receiveShadow>
				<meshStandardMaterial attach="material" color="lightgrey" />
			</Plane>
			<Sphere position={[5, 5, 5]} args={[0.5, 12, 12]}>
				<meshStandardMaterial attach="material" color="yellow" />
			</Sphere>
			<OrbitControls />
			<axesHelper args={[1.5]} />
			<gridHelper args={[10, 10]} />
			{/* <directionalLight /> */}
		</Canvas>
	);
}

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
		<>
		<Container fluid>
			<Row className="m-3 p-3">
				<h4>DDD liveView</h4>
				{/* <Card className="shadow-sm p-3 h-100" > */}
					<Row style={{ height: "300px" }}>
						<Live3dview />
					</Row>
				{/* </Card> */}
				<Col className="p-3">
					<Button>Arm</Button>
					<Button>Disarm</Button>

					<Slider name="Trim___x" />
					<Slider name="Trim___y" />
					<Slider name="Trim___z" />
					<Slider name="Trim_yaw" />
				</Col>
				<Col className="p-3">
					<p dangerouslySetInnerHTML={{ __html: message }}></p>
				</Col>
			</Row>
		</Container>
		</>
	);
}

export default App;
