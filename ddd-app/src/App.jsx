import React, { useRef, useEffect, useState } from 'react';
import { Button, Card, Col, Container, Row } from 'react-bootstrap';
import { Canvas, useFrame } from '@react-three/fiber';
import { OrbitControls, Sphere, Box } from '@react-three/drei';
import * as THREE from 'three';

function App() {
//   const objectRef = useRef();
//   const animationDataRef = useRef([]);
//   const clock = useRef(new THREE.Clock());

//   // Load the JSON animation data
//   useEffect(() => {
//     fetch('/animation.json')  // Make sure the JSON is in the 'public' directory
//       .then((response) => response.json())
//       .then((data) => {
//         animationDataRef.current = data.keyframes;
//       });
//   }, []);

//   // Apply animation keyframes
//   useFrame(() => {
//     if (!animationDataRef.current.length) return;

//     const time = clock.current.getElapsedTime();
//     const frameIndex = Math.floor(time % animationDataRef.current.length);

//     const currentFrame = animationDataRef.current[frameIndex];
//     if (currentFrame) {
//       const { position, rotation, scale } = currentFrame;

//       objectRef.current.position.set(...position);
//       objectRef.current.rotation.set(...rotation);
//     }
//   });
	const [message, setMessage] = useState('');
	const [ws, setWs] = useState(null);

	useEffect(() => {
	const socket = new WebSocket('ws://0.0.0.0:18084/ws'); // Match the port of the C++ server
	setWs(socket);

	socket.onopen = () => {
		console.log('Connected to WebSocket server');
		socket.send('Hello from React');
	};

	socket.onmessage = (event) => {
		console.log('Message from server:', event.data);
		setMessage(event.data);
	};

	return () => socket.close();
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
			{/* Ambient Light */}
			<ambientLight intensity={0.5} />
			
			 {/* Spotlight */}
			 <spotLight 
			position={[5, 5, 5]} // Positioning the light to the side
			angle={0.3} // Spread of the light cone
			penumbra={1} // Softness of the edges
			intensity={1} // Brightness of the light
			castShadow // Enable shadows
		  />
		  
		  {/* Cube (Light source) */}
		  {/* <Box position={[5, 5, 0]} args={[1, 1, 1]} >
			<meshStandardMaterial attach="material" color="yellow" />
		  </Box> */}
		  
		  {/* Central Sphere */}
		  <Sphere position={[0, 0, 0]} args={[1, 32, 32]}>
			<meshStandardMaterial attach="material" color="blue" />
		  </Sphere>
			
			{/* Controls and Helpers */}
			<OrbitControls />
			<axesHelper args={[3]} />
			<gridHelper args={[11, 11]} />
		  </Canvas>
		</Col>
	  </Container>
	);
  }



export default App;
