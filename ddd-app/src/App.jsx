import React, { useEffect, useState, useRef } from 'react';
import { Drone, DroneControll } from './Drone';
import { Canvas } from '@react-three/fiber';
import { OrbitControls, Sphere, Plane, Cylinder, Box } from '@react-three/drei';
import { fetchConfigAndInitializeWebSocket } from './WebSocket';



function Live3dview({ position, setpoint, light }) {

	return (
	<Canvas
		orthographic
		camera={{ zoom: 40, position: [3, 3.5, 10] }}
		style={{ background: '#121821' }}
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
	const [messages, setMessages] = useState([]);
	const [wsState, setWsState] = useState('disconnected');
	const [wsStateColor, setWsStateColor] = useState('text-red-600');
	const [ws, setWs] = useState(null);
	const messagesEndRef = useRef(null);
	const [position, setPosition] = useState([
		[-6, 1, -3, 0],
		[-6, 1, -1, 0],
		[-6, 1, 1, 0],
		[-6, 1, 3, 0]
	]);
	const [trim, setTrim] = useState([
		[1, 2, 3, 4], // For drone 0
		[1, 2, 3, 4], // For drone 1
		[1, 2, 3, 4], // For drone 2
		[1, 2, 3, 4]  // For drone 3
	]);
	const [light, setLight] = useState([
		[1, 1],
		[1, 1],
		[1, 1],
		[1, 1]
	]);
	const [setpoint, setSetpoint] = useState([
		[1, 1],
		[1, 1],
		[1, 1],
		[1, 1]
	]);

	const [bat, setBat] = useState([
		[1, 1],
		[1, 1],
		[1, 1],
		[1, 1]
	]);
	const [rc, setRc] = useState([
		[1, 1, 1, 1, 1],
		[1, 1, 1, 1, 1],
		[1, 1, 1, 1, 1],
		[1, 1, 1, 1, 1]
	]);
	const [ati, setAti] = useState([
		[1, 1, 1],
		[1, 1, 1],
		[1, 1, 1],
		[1, 1, 1]
	]);


	const [pathLen, setPathLen] = useState([ [1], [1], [1], [1]]);
	const [frame, setFrame] = useState([ [0], [0], [0], [0]]);
	
	useEffect(() => {
		fetchConfigAndInitializeWebSocket(bat, setBat, rc, setRc, ati, setAti, setWs, setPathLen, setFrame, setWsState, setWsStateColor, setMessages, setTrim, setLight, setPosition, setSetpoint);
	}, []);

	useEffect(() => {
		// Scroll to the bottom whenever messages update
		if (messagesEndRef.current) {
			messagesEndRef.current.scrollTop = messagesEndRef.current.scrollHeight;
		}
	}, [messages]);

	return (
	<div className="grid grid-cols-8 gap-2 p-3 bg-gray-900 h-screen text-white">

		{/* Header */}
		<div className="col-span-8 bg-gray-800 p-4 flex items-center justify-center gap-3" style={{ height: '2vh' }}>
			<h2> Dancing Drones Dialogue </h2>
			<span className={wsStateColor}>{wsState}</span>
		</div>
		
		{/* Console */}
		<div className="col-span-3 bg-gray-800 p-3 flex" style={{ height: '49vh' }}>
			<div 
				ref={messagesEndRef}
				className="overflow-auto w-full" 
				style={{ lineHeight: '1.2' }}
			>
				{messages.map((msg, index) => (
					<p className="m-0 mt-1" key={index}>{msg}</p>
				))}
			</div>
		</div>

		{/* Three.js view */}
		<div className="col-span-5 bg-gray-800 flex" style={{ height: '49vh' }}>
			<Live3dview position={position} setpoint={setpoint} light={light} />
		</div>

		{/* Drone controller */}

			<DroneControll bat={bat} setBat={setBat} rc={rc} setRc={setRc} index={0} setpoint={setpoint} light={light} trim={trim} frame={frame} setFrame={setFrame} pathLen={pathLen} setSetpoint={setSetpoint} setLight={setLight} setTrim={setTrim} ws={ws} />
			<DroneControll bat={bat} setBat={setBat} rc={rc} setRc={setRc} index={1} setpoint={setpoint} light={light} trim={trim} frame={frame} setFrame={setFrame} pathLen={pathLen} setSetpoint={setSetpoint} setLight={setLight} setTrim={setTrim} ws={ws} />
			<DroneControll bat={bat} setBat={setBat} rc={rc} setRc={setRc} index={2} setpoint={setpoint} light={light} trim={trim} frame={frame} setFrame={setFrame} pathLen={pathLen} setSetpoint={setSetpoint} setLight={setLight} setTrim={setTrim} ws={ws} />
			<DroneControll bat={bat} setBat={setBat} rc={rc} setRc={setRc} index={3} setpoint={setpoint} light={light} trim={trim} frame={frame} setFrame={setFrame} pathLen={pathLen} setSetpoint={setSetpoint} setLight={setLight} setTrim={setTrim} ws={ws} />
	</div>
	);
}

export default App;
