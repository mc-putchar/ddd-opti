import React, { useEffect, useState } from 'react';
import { Drone, DroneControllerView } from './Drone';
import { Col, Row, Container, Button } from 'react-bootstrap';
import { Canvas } from '@react-three/fiber';
import { OrbitControls, Sphere, Plane, Cylinder, Box } from '@react-three/drei';
import { fetchConfigAndInitializeWebSocket } from './WebSocket';



function Live3dview({ position, setpoint, light }) {

	return (
	<Canvas
		orthographic
		camera={{ zoom: 40, position: [3, 3.5, 10] }}
		style={{ background: '#222' }}
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
	
	useEffect(() => {
		fetchConfigAndInitializeWebSocket(setWs, setWsState, setWsStateColor, setMessages, setWsPort, setTrim, setLight, setPosition, setSetpoint);
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
