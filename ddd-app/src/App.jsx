import React, { useEffect, useState, useRef } from 'react';
import { Drone, DroneControll } from './Drone';
import { Canvas } from '@react-three/fiber';
import { OrbitControls, Sphere, Plane, Cylinder, Box } from '@react-three/drei';
import { fetchConfigAndInitializeWebSocket } from './WebSocket';



const Live3dview = React.memo(({ 
	position0, position1, position2, position3, 
	setpoint0, setpoint1, setpoint2, setpoint3,
	light0, light1, light2, light3
  }) => {

	return (
	<Canvas
		orthographic
		camera={{ zoom: 40, position: [3, 3.5, 10] }}
		style={{ background: '#121821' }}
		shadows
	>
		<ambientLight intensity={0.6} />
		<Cylinder position={[0, 0.7, 0]} args={[0.25, 0.7, 1.4]} castShadow receiveShadow>
			<meshStandardMaterial attach="material" color="blue" />
		</Cylinder>
		<Sphere position={[0, 1.8, 0]} args={[0.4, 16, 16]} castShadow receiveShado>
				<meshStandardMaterial attach="material" color="blue" />
			</Sphere>
		<Plane position={[0, -0.01, 0]} args={[11, 11]} rotation={[-Math.PI / 2, 0, 0]} receiveShadow>
			<meshStandardMaterial attach="material" color="darkgrey" />
		</Plane>
		<Box args={[15, 15, 4]} position={[0, -0.02 -2, 0]} rotation={[-Math.PI / 2, 0, 0]}>
			<meshBasicMaterial attach="material" color="#121821" />
		</Box>
		<Drone
			position={position0} // Access the position of the current drone
			light={light0} // Access the light of the current drone
			setpoint={setpoint0} // Access the setpoint of the current drone
		/>
		<Drone
			position={position1} // Access the position of the current drone
			light={light1} // Access the light of the current drone
			setpoint={setpoint1} // Access the setpoint of the current drone
		/>
		<Drone
			position={position2} // Access the position of the current drone
			light={light2} // Access the light of the current drone
			setpoint={setpoint2} // Access the setpoint of the current drone
		/>
		<Drone
			position={position3} // Access the position of the current drone
			light={light3} // Access the light of the current drone
			setpoint={setpoint3} // Access the setpoint of the current drone
		/>

		{[ [-5, 1.25, -5], [5, 1.25, -5], [-5, 1.25, 5], [5, 1.25, 5],  // THE OPTITRACKS POLES
		].map((position, index) => (
			<Cylinder args={[0.07, 0.07, 2.5, 32]} position={position} key={index}>
				<meshStandardMaterial attach="material" color="red" />
			</Cylinder>
		))}
		<OrbitControls target={[0, 1.5, 0]} /> {/* <axesHelper args={[1.5]} /> */}
		<gridHelper args={[10, 10]} /> {/* <directionalLight /> */}
	</Canvas>
	);
});

function App() {
	const [messages, setMessages] = useState([]);
	const [wsState, setWsState] = useState('disconnected');
	const [wsStateColor, setWsStateColor] = useState('text-red-600');
	const [ws, setWs] = useState(null);
	const messagesEndRef = useRef(null);
	const [position0, setPosition0] = useState([-6, 1, -3, 0]);
	const [position1, setPosition1] = useState([-6, 1, -1, 0]);
	const [position2, setPosition2] = useState([-6, 1, 1, 0]);
	const [position3, setPosition3] = useState([-6, 1, 3, 0]);

	const [trim0, setTrim0] = useState([1, 2, 3, 4]);
	const [trim1, setTrim1] = useState([1, 2, 3, 4]);
	const [trim2, setTrim2] = useState([1, 2, 3, 4]);
	const [trim3, setTrim3] = useState([1, 2, 3, 4]);

	const [light0, setLight0] = useState([1, 1]);
	const [light1, setLight1] = useState([1, 1]);
	const [light2, setLight2] = useState([1, 1]);
	const [light3, setLight3] = useState([1, 1]);

	const [setpoint0, setSetpoint0] = useState([1, 1]);
	const [setpoint1, setSetpoint1] = useState([1, 1]);
	const [setpoint2, setSetpoint2] = useState([1, 1]);
	const [setpoint3, setSetpoint3] = useState([1, 1]);

	const [bat0, setBat0] = useState([1, 1]);
	const [bat1, setBat1] = useState([1, 1]);
	const [bat2, setBat2] = useState([1, 1]);
	const [bat3, setBat3] = useState([1, 1]);

	const [rc0, setRc0] = useState([1, 1, 1, 1, 1]);
	const [rc1, setRc1] = useState([1, 1, 1, 1, 1]);
	const [rc2, setRc2] = useState([1, 1, 1, 1, 1]);
	const [rc3, setRc3] = useState([1, 1, 1, 1, 1]);

	const [ati0, setAti0] = useState([1, 1, 1]);
	const [ati1, setAti1] = useState([1, 1, 1]);
	const [ati2, setAti2] = useState([1, 1, 1]);
	const [ati3, setAti3] = useState([1, 1, 1]);

	const [pathLen0, setPathLen0] = useState([1]);
	const [pathLen1, setPathLen1] = useState([1]);
	const [pathLen2, setPathLen2] = useState([1]);
	const [pathLen3, setPathLen3] = useState([1]);

	const [frame0, setFrame0] = useState([0]);
	const [frame1, setFrame1] = useState([0]);
	const [frame2, setFrame2] = useState([0]);
	const [frame3, setFrame3] = useState([0]);


	
	useEffect(() => {
		fetchConfigAndInitializeWebSocket(messages, setMessages,
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
		<Live3dview 
			position0={position0} 
			position1={position1} 
			position2={position2} 
			position3={position3} 
			setpoint0={setpoint0} 
			setpoint1={setpoint1} 
			setpoint2={setpoint2} 
			setpoint3={setpoint3} 
			light0={light0} 
			light1={light1} 
			light2={light2} 
			light3={light3}
			/>
		</div>

		{/* Drone controller */}
		<DroneControll
		bat={bat0} setBat={setBat0}
		rc={rc0} setRc={setRc0}
		setpoint={setSetpoint0} setSetpoint={setSetpoint0}
		light={light0} setLight={setLight0}
		trim={trim0} setTrim={setTrim0}
		frame={frame0} setFrame={setFrame0}
		pathLen={pathLen0}
		ws={ws}
		index={0}
		/>

		<DroneControll
		bat={bat1} setBat={setBat1}
		rc={rc1} setRc={setRc1}
		setpoint={setSetpoint1} setSetpoint={setSetpoint1}
		light={light1} setLight={setLight1}
		trim={trim1} setTrim={setTrim1}
		frame={frame1} setFrame={setFrame1}
		pathLen={pathLen1}
		ws={ws}
		index={1}
		/>

		<DroneControll
		bat={bat2} setBat={setBat2}
		rc={rc2} setRc={setRc2}
		setpoint={setSetpoint2} setSetpoint={setSetpoint2}
		light={light2} setLight={setLight2}
		trim={trim2} setTrim={setTrim2}
		frame={frame2} setFrame={setFrame2}
		pathLen={pathLen2}
		ws={ws}
		index={2}
		/>

		<DroneControll
		bat={bat3} setBat={setBat3}
		rc={rc3} setRc={setRc3}
		setpoint={setSetpoint3} setSetpoint={setSetpoint3}
		light={light3} setLight={setLight3}
		trim={trim3} setTrim={setTrim3}
		frame={frame3} setFrame={setFrame3}
		pathLen={pathLen3}
		ws={ws}
		index={3}
		/>
	</div>
	);
}

export default App;
