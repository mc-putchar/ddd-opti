import React, { useEffect, useState, useRef} from 'react';
import {FloatInputForm, Slider, Arm, Path, Progression} from './Input';
import { Col, Card, Row, Form, Container, Button} from 'react-bootstrap';
import { Canvas } from '@react-three/fiber';
import { OrbitControls, Cone, Sphere, Plane, Cylinder, Box } from '@react-three/drei';
import { Slider as ShadcnSlider } from "@/components/ui/slider";


const Drone = React.memo(({ position, light, setpoint }) => {
	const lightTarget = useRef();
	const zRotation = -(position[3] * (Math.PI / 180)) % (2 * Math.PI);

	// Calculate the target position based on the light[0] angle
	const lightAngle = light[0]; // angle in degrees
	const radians = lightAngle * (Math.PI / 180); // convert to radians

	// Calculate new target position based on the angle
	const targetHeight = -Math.cos(radians); // height adjustment based on angle
	const targetDistance = Math.sin(radians); // distance adjustment based on angle

	// Set the target position relative to the spotlight
	const targetPosition = [0, targetHeight, targetDistance];
	console.log("taregt post =", targetPosition);

	return (
	  <group position={position} rotation={[0,zRotation,0]}>
		{/* Drone Box */}
		<Box args={[0.35, 0.15, 0.35]}>
		  <meshStandardMaterial attach="material" color="yellow" />
		</Box>
		<Box args={[0.85, 0.65, 0.85]}>
			<meshStandardMaterial attach="material" color="yellow" wireframe={true} />
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
  });

  
function DroneControll({
	bat, setBat, rc, setRc, index,
	setpoint, light, trim, setLight, setPoint, setTrim,
	ws, frame, setFrame, pathLen
  }) {
	
	function estimateBatteryPercentage(voltage_battery) { // Define the min and max voltage thresholds for a 3S LiPo battery
		const min_voltage = 3000; // 3.0V * 3 cells = 9.0V
		const max_voltage = 4200; // 4.2V * 3 cells = 12.6V
		
		// Check if the voltage is within the expected range
		if (voltage_battery < min_voltage) {
			return 0; // Battery is empty
		} else if (voltage_battery > max_voltage) {
			return 100; // Battery is fully charged
		} else {
			// Scale the voltage to percentage and convert to int
			return Math.floor((voltage_battery - min_voltage) * 100 / (max_voltage - min_voltage));
		}
	}

	return (
	<>
	<div className="col-span-4 bg-gray-700 p-1" style={{ height: '21vh' }}>
		<div className="grid grid-cols-4 gap-1 p-1 bg-gray-900 text-white">
			<div className="col-span-1 bg-gray-700 p-2 items-center ap-1 gap-1 flex">
				<h3 className="font-bold"> Drone {index} </h3>
					<Arm index={index} ws={ws} />
			</div>
			<div className="col-span-1 bg-gray-700 p-2 pt-3 gap-4 flex">
				<span> {parseFloat((bat[0] * 0.001).toFixed(1))} V </span>
				<span> {parseFloat((bat[1] * 0.001).toFixed(1))} V </span>
				<span> {estimateBatteryPercentage(bat[0])} % </span>
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
					vari="Roll"
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
					vari="Pitch"
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
					vari="Yaw"
					arg={3}  // This will update the first value of trim
					ws={ws}  // WebSocket connection
					stateArray={trim}  // Pass the trims state
					setStateArray={setTrim}  // Pass the setter for trims
					min={0}
					max={800}
				/>
				<Slider
					index={index}  // Drone index
					name="trim"  // To identify trim values
					vari="Throttle"
					arg={2}  // This will update the first value of trim
					ws={ws}  // WebSocket connection
					stateArray={trim}  // Pass the trims state
					setStateArray={setTrim}  // Pass the setter for trims
					min={0}
					max={800}
				/>
			</div>

			{/* Trim telemetry */}
			<div className="col-span-1 bg-gray-700 p-2 flex flex-col">
				<Progression rc={rc} index={index} arg={0} color={"red"}/>
				<Progression rc={rc} index={index} arg={1} color={"purple"}/>
				<Progression rc={rc} index={index} arg={2} color={"blue"}/>
				<Progression rc={rc} index={index} arg={3} color={"cyan"}/> 
				<Progression rc={rc} index={index} arg={4} color={"green"}/>

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
