import React, { useEffect, useState, useRef} from 'react';
import {FloatInputForm, Slider, Arm, Path, Progression} from './Input';
import { Col, Card, Row, Form, Container, Button} from 'react-bootstrap';
import { Canvas } from '@react-three/fiber';
import { OrbitControls, Cone, Sphere, Plane, Cylinder, Box } from '@react-three/drei';
import { Slider as ShadcnSlider } from "@/components/ui/slider";


function Drone({ position, light }) {
	const lightTarget = useRef();
	const zRotation = -(position[3] * (Math.PI / 180)) % (2 * Math.PI);

	// Calculate the target position based on the light[0] angle
	const lightAngle = light[0]; // angle in degrees
	const radians = lightAngle * (Math.PI / 180); // convert to radians

	// Calculate new target position based on the angle
	const targetHeight = Math.sin(radians); // height adjustment based on angle
	const targetDistance = Math.cos(radians); // distance adjustment based on angle

	// Set the target position relative to the spotlight
	const targetPosition = [0, -1 + targetHeight, 1 + targetDistance];
	console.log("taregt post =", targetPosition)

	return (
	  <group position={position} rotation={[0,zRotation,0]}>
		{/* Drone Box */}
		<Box args={[0.35, 0.15, 0.35]}>
		  <meshStandardMaterial attach="material" color="yellow" />
		</Box>
		{/* Spotlight above the drone */}
		<spotLight
		  position={[0, -0.1, 0]} // Slightly above the drone within the group
		  angle={0.25}
		  penumbra={1}
		  intensity={light[1]}
		  castShadow
		  shadow-mapSize-width={1024}
		  shadow-mapSize-height={1024}
		  target={lightTarget.current} // Point spotlight at the target
		/>
		 <mesh ref={lightTarget} position={targetPosition} /> {/* Adjust this target position as needed */}
		 <Sphere args={[0.05, 16, 16]} position={targetPosition}>
				<meshStandardMaterial attach="material" color="red" />
			</Sphere>
	  </group>
	);
  }

function DroneControll({index, setpoint, light, trim, setLight, setPoint, setTrim, ws, frame, setFrame, pathLen}) {

	return (
	<>
	<div className="col-span-4 bg-gray-700 p-1" style={{ height: '21vh' }}>
		<div className="grid grid-cols-4 gap-1 p-1 bg-gray-900 text-white">
			<div className="col-span-1 bg-gray-700 p-2 items-center ap-1 gap-1 flex">
				<h3 className="font-bold"> Drone {index} </h3>
					<Arm index={index} ws={ws} />
			</div>
			<div className="col-span-1 bg-gray-700 p-2 pt-3 gap-4 flex">
				<span> 4.5V </span>
				<span> 4.5V </span>
				<span> 86% </span>
			</div>
			<div className="col-span-1 bg-gray-700 p-2 pt-3 flex">
				<h4 className="font-bold"> Path </h4>
			</div>
			<div className="col-span-1 bg-gray-700 p-2 pt-3 flex">
				<h4 className="font-bold"> Light </h4>
			</div>

			{/* Trim sliders */}
			<div className="col-span-1 bg-gray-700 p-2 flex flex-col">
				<Slider
					index={index}  // Drone index
					name="trim"  // To identify trim values
					vari="x"
					arg={0}  // This will update the first value of trim
					ws={ws}  // WebSocket connection
					stateArray={trim}  // Pass the trims state
					setStateArray={setTrim}  // Pass the setter for trims
					min={0}
					max={800}
				/>
				<Slider
					index={index}  // Drone index
					name="trim"  // To identify trim values
					vari="y"
					arg={1}  // This will update the first value of trim
					ws={ws}  // WebSocket connection
					stateArray={trim}  // Pass the trims state
					setStateArray={setTrim}  // Pass the setter for trims
					min={0}
					max={800}
				/>
				<Slider
					index={index}  // Drone index
					name="trim"  // To identify trim values
					vari="z"
					arg={2}  // This will update the first value of trim
					ws={ws}  // WebSocket connection
					stateArray={trim}  // Pass the trims state
					setStateArray={setTrim}  // Pass the setter for trims
					min={0}
					max={800}
				/>
				<Slider
					index={index}  // Drone index
					name="trim"  // To identify trim values
					vari="yaw"
					arg={3}  // This will update the first value of trim
					ws={ws}  // WebSocket connection
					stateArray={trim}  // Pass the trims state
					setStateArray={setTrim}  // Pass the setter for trims
					min={0}
					max={800}
				/>
			</div>

			{/* Trim telemetry */}
			<div className="col-span-1 bg-gray-700 p-2 flex flex-col">
				<Progression/>
				<Progression/>
				<Progression/>
				<Progression/>

			</div>

			{/* Path */}
			<div className="col-span-1 bg-gray-700 p-2 flex flex-col">
				<Path index={index} ws={ws} frame={frame} setFrame={setFrame} pathLen={pathLen}  />
				<FloatInputForm param={setpoint} index={index} ws={ws}/>
			</div>

			{/* Light */}
			<div className="col-span-1 bg-gray-700 p-2 flex flex-col">
			<Slider
				index={index}  // Drone index
				name="light"  // To identify trim values
				vari="angle"
				arg={0}  // This will update the first value of trim
				ws={ws}  // WebSocket connection
				stateArray={light}  // Pass the trims state
				setStateArray={setLight}  // Pass the setter for trims
				min={0}
				max={90}
				/>
			<Slider
				index={index}  // Drone index
				name="light"  // To identify trim values
				vari="power"
				arg={1}  // This will update the first value of trim
				ws={ws}  // WebSocket connection
				stateArray={light}  // Pass the trims state
				setStateArray={setLight}  // Pass the setter for trims
				min={0}
				max={255}
			/>
			<Slider
				index={index}  // Drone index
				name="light"  // To identify trim values
				vari="strobe"
				arg={1}  // This will update the first value of trim
				ws={ws}  // WebSocket connection
				stateArray={light}  // Pass the trims state
				setStateArray={setLight}  // Pass the setter for trims
				min={0}
				max={255}
			/>
		</div>
	</div>
	</div>
	</>
	)
} 

export {Drone, DroneControll};
