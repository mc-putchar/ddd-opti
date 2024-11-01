import React, { useEffect, useState, useRef} from 'react';
import {FloatInputForm, Slider, Arm} from './Input';
import { Col, Card, Row, Form, Container, Button} from 'react-bootstrap';
import { Canvas } from '@react-three/fiber';
import { OrbitControls, Cone, Sphere, Plane, Cylinder, Box } from '@react-three/drei';


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
	const targetPosition = [0, -1 + targetHeight, 1+ targetDistance];
	console.log("taregt post =", targetPosition)

	return (
	  <group position={position} rotation={[0,zRotation,0]}>
		{/* Drone Box */}
		<Box args={[0.35, 0.15, 0.35]}>
		  <meshStandardMaterial attach="material" color="yellow" />
		</Box>
		{/* Spotlight above the drone */}
		<spotLight
		  position={[0, 0, 0]} // Slightly above the drone within the group
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

function DroneControllerView({position, setpoint, light, trim, setSetpoint, setPosition, setTrim, setLight, ws}) {
	return (
		<Col xs="7" className="p-3">
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
			<Row>
				<Col xs="4" >
				<h4 className="mb-0">Drone_{index}</h4>
				</Col>
				<Col className="p-2">
						<Arm index={index} ws={ws} />
						<Button className="my-1" onClick={sendPath}>Send_Path</Button>
						{/* Add any additional buttons here */}
				</Col>
			</Row>
				<Row className="mb-3">
					{/* Trim Block */}
					<Col className="p-0">
						<h5>Trim</h5>
						<div className="d-flex flex-row">
							<Slider
								param="trim"
								index={index}
								name="trim"
								axis="x"
								arg={0}
								ws={ws}
								stateArray={trim}
								setStateArray={setTrim}
							/>
							<Slider
								param="trim"
								index={index}
								name="trim"
								axis="y"
								arg={1}
								ws={ws}
								stateArray={trim}
								setStateArray={setTrim}
							/>
							<Slider
								param="trim"
								index={index}
								name="trim"
								axis="z"
								arg={2}
								ws={ws}
								stateArray={trim}
								setStateArray={setTrim}
							/>
							<Slider
								param="trim"
								index={index}
								name="trim"
								axis="yaw"
								arg={3}
								ws={ws}
								stateArray={trim}
								setStateArray={setTrim}
							/>
						</div>
					</Col>
	
					{/* Light Block */}
					<Col className="p-0">
						<h5>Light</h5>
						<div className="d-flex flex-row">
							<Slider
								param="light"
								index={index}
								name="light"
								axis="angle"
								arg={0}
								ws={ws}
								stateArray={light}
								setStateArray={setLight}
							/>
							<Slider
								param="light"
								index={index}
								name="light"
								axis="power"
								arg={1}
								ws={ws}
								stateArray={light}
								setStateArray={setLight}
							/>
						</div>
					</Col>
	
					{/* Buttons Block */}
					{/* <Col className="d-flex flex-column justify-content-center p-2">
						<Arm index={index} ws={ws} />
						<Button className="my-1" onClick={sendPath}>Send_Path</Button>
					</Col> */}
				</Row>
			<FloatInputForm param={setpoint} index={index} ws={ws} />
		</Row>
	);
	
	
	
	
	
	
	
	
} 

export {Drone, DroneControll, DroneControllerView};
