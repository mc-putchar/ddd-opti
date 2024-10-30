import React, { useEffect, useState } from 'react';
import {FloatInputForm, Slider, Arm} from './Input';
import { Col, Card, Row, Form, Container, Button} from 'react-bootstrap';
import { Canvas } from '@react-three/fiber';
import { OrbitControls, Cone, Sphere, Plane, Cylinder, Box } from '@react-three/drei';


function Drone({ position, light }) {
	return (
	  <group position={position}>
		{/* Drone Box */}
		<Box args={[0.35, 0.15, 0.35]}>
		  <meshStandardMaterial attach="material" color="yellow" />
		</Box>
		{/* Spotlight above the drone */}
		<spotLight
		  position={[0, 0.5, 0]} // Slightly above the drone within the group
		  angle={0.25}
		  penumbra={1}
		  intensity={light[1]}
		  castShadow
		  shadow-mapSize-width={1024}
		  shadow-mapSize-height={1024}
		/>
	  </group>
	);
  }

function DroneControllerView({position, setpoint, light, trim, setSetpoint, setPosition, setTrim, setLight, ws}) {
	return (
		<Col className="p-3">
				<Row>
					<Col>
					<DroneControll index={0} position={position} setpoint={setpoint} light={light} trim={trim} setSetpoint={setSetpoint} setPosition={setPosition} setLight={setLight} setTrim={setTrim} ws={ws} />
					</Col>
					<Col>
					<DroneControll index={1} position={position} setpoint={setpoint} light={light} trim= {trim} setSetpoint={setSetpoint} setPosition={setPosition} setLight={setLight} setTrim={setTrim} ws={ws} />
					</Col>
				</Row>
				<Row>
					<Col>
					<DroneControll index={2} position={position} setpoint={setpoint} light={light} trim= {trim} setSetpoint={setSetpoint} setPosition={setPosition} setLight={setLight} setTrim={setTrim} ws={ws} />
					</Col>
					<Col>
					<DroneControll index={3} position={position} setpoint={setpoint} light={light} trim= {trim} setSetpoint={setSetpoint} setPosition={setPosition} setLight={setLight} setTrim={setTrim} ws={ws} />
					</Col>
				</Row>
			</Col>
	);
}

function DroneControll({index, position, setpoint, light, trim, setPosition, setLight, setPoint, setTrim, ws}) {

	function sendPath() {
		if (ws && ws.readyState === WebSocket.OPEN) {
			ws.send(`${index}{"path":"send"}`);
		}
	}

	return (
	<Row className="mb-4">
		<h4 className="mb-0">Drone_{index}</h4>
		<Col className="p">
			<h5>Trim</h5>
			<Slider
			param="trim"
			index={index}  // Drone index
			name="trim"  // To identify trim values
			axis="x"
			arg={0}  // This will update the first value of trim
			ws={ws}  // WebSocket connection
			stateArray={trim}  // Pass the trims state
			setStateArray={setTrim}  // Pass the setter for trims
			/>
			<Slider
			param="trim"
			index={index}  // Drone index
			name="trim"  // To identify trim values
			axis="y"
			arg={1}  // This will update the first value of trim
			ws={ws}  // WebSocket connection
			stateArray={trim}  // Pass the trims state
			setStateArray={setTrim}  // Pass the setter for trims
			/>
			<Slider
			param="trim"
			index={index}  // Drone index
			name="trim"  // To identify trim values
			axis="z"
			arg={2}  // This will update the first value of trim
			ws={ws}  // WebSocket connection
			stateArray={trim}  // Pass the trims state
			setStateArray={setTrim}  // Pass the setter for trims
			/>
			<Slider
			param="trim"
			index={index}  // Drone index
			name="trim"  // To identify trim values
			axis="yaw"
			arg={3}  // This will update the first value of trim
			ws={ws}  // WebSocket connection
			stateArray={trim}  // Pass the trims state
			setStateArray={setTrim}  // Pass the setter for trims
			/>
		</Col>
		<Col className="p">
			<h5>Light</h5>
			<Slider
			param="light"
			index={index}  // Drone index
			name="light"  // To identify trim values
			axis="angle"
			arg={0}  // This will update the first value of trim
			ws={ws}  // WebSocket connection
			stateArray={light}  // Pass the trims state
			setStateArray={setLight}  // Pass the setter for trims
			/>
			<Slider
			param="light"
			index={index}  // Drone index
			name="light"  // To identify trim values
			axis="power"
			arg={1}  // This will update the first value of trim
			ws={ws}  // WebSocket connection
			stateArray={light}  // Pass the trims state
			setStateArray={setLight}  // Pass the setter for trims
			/>
			<Arm index={index} ws={ws} />
			<Button className="my-0" onClick={sendPath}>Send Path</Button>
		</Col>
		<FloatInputForm param={setpoint} index={index} ws={ws}/>
	</Row>
	)
} 

export {Drone, DroneControll, DroneControllerView};
