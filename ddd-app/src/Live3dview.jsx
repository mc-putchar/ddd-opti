import React, { useEffect, useState, useRef } from 'react';
import { Drone } from './Drone';
import { Canvas } from '@react-three/fiber';
import { OrbitControls, Sphere, Plane, Cylinder, Box, useGLTF } from '@react-three/drei';
import * as THREE from 'three';




const Live3dview = React.memo(({ 
	position0, position1, position2, position3, 
	setpoint0, setpoint1, setpoint2, setpoint3,
	droneGlb0, droneGlb1, droneGlb2, droneGlb3,
	ati0, ati1, ati2, ati3,
	light0, light1, light2, light3, colors
  }) => {

	const denise = useGLTF('/src/assets/Denise.glb');

	return (
		<div className="col-span-6 bg-gray-800 flex" style={{ height: '51vh' }}>

	<Canvas
		orthographic
		camera={{ zoom: 60, position: [4, 4.5, 10] }}
		style={{ background: '#1c1917' }}
		shadows
		onCreated={({ gl }) => {
			const globalPlane = new THREE.Plane(new THREE.Vector3(0, 0.02, 0), 1)
			gl.clippingPlanes = [globalPlane] // Apply the global clipping plane
			gl.localClippingEnabled = true // Enable local clipping
		  }}
		
	>
		<ambientLight intensity={0.5} />
		{/* <Cylinder position={[0, 0.7, 0]} args={[0.25, 0.7, 1.4]} castShadow receiveShadow>
			<meshStandardMaterial attach="material" color="#998471" />
		</Cylinder>
		<Sphere position={[0, 1.8, 0]} args={[0.4, 16, 16]} castShadow receiveShado>
				<meshStandardMaterial attach="material" color="#998471" />
			</Sphere> */}
		<primitive
				object={denise.scene}
				position={[0, 0, 0]}
				scale={[1.0, 1.0, 1.0]} // Adjust scale as needed
				castShadow
				receiveShadow
			/>
		<Plane position={[0, -0.01, 0]} args={[11, 11]} rotation={[-Math.PI / 2, 0, 0]} receiveShadow>
			<meshStandardMaterial attach="material" color="#73706b" />
		</Plane>
		<Drone
			position={position0} // Access the position of the current drone
			light={light0} // Access the light of the current drone
			setpoint={setpoint0} // Access the setpoint of the current drone
			color={colors[0]}
			droneGlb={droneGlb0}
			ati={ati0}
		/>
		<Drone
			position={position1} // Access the position of the current drone
			light={light1} // Access the light of the current drone
			setpoint={setpoint1} // Access the setpoint of the current drone
			color={colors[1]}
			droneGlb={droneGlb1}
			ati={ati1}
		/>
		<Drone
			position={position2} // Access the position of the current drone
			light={light2} // Access the light of the current drone
			setpoint={setpoint2} // Access the setpoint of the current drone
			color={colors[2]}
			droneGlb={droneGlb2}
			ati={ati2}
		/>
		<Drone
			position={position3} // Access the position of the current drone
			light={light3} // Access the light of the current drone
			setpoint={setpoint3} // Access the setpoint of the current drone
			color={colors[3]}
			droneGlb={droneGlb3}
			ati={ati3}
		/>

		{[ [-5, 1.25, -5], [5, 1.25, -5], [-5, 1.25, 5], [5, 1.25, 5],  // THE OPTITRACKS POLES
		].map((position, index) => (
			<Cylinder args={[0.07, 0.07, 2.5, 32]} position={position} key={index}>
				<meshStandardMaterial attach="material" color="#837296" />
			</Cylinder>
		))}
		<pointLight position={[0, 6, 0]} intensity={50} color="white" />
		<OrbitControls target={position3} /> 
		{/* <OrbitControls target={[0, 1.2, 0]} />  */}
		<gridHelper args={[10, 10, 'black', 'black']} />
	</Canvas>
	</div>
	);
});


export {Live3dview};
