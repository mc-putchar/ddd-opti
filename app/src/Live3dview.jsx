import React, { useEffect, useState, useRef } from 'react';
import { Drone } from './Drone';
import { Canvas } from '@react-three/fiber';
import { OrbitControls, Sphere, Plane, Cylinder, Box } from '@react-three/drei';


const Live3dview = React.memo(({ 
	position0, position1, position2, position3, 
	setpoint0, setpoint1, setpoint2, setpoint3,
	light0, light1, light2, light3
  }) => {

	return (
		<div className="col-span-5 bg-gray-800 flex" style={{ height: '49vh' }}>

	<Canvas
		orthographic
		camera={{ zoom: 40, position: [3, 3.5, 10] }}
		style={{ background: '#121821' }}
		shadows
	>
		<ambientLight intensity={1} />
		<Cylinder position={[0, 0.7, 0]} args={[0.25, 0.7, 1.4]} castShadow receiveShadow>
			<meshStandardMaterial attach="material" color="#998471" />
		</Cylinder>
		<Sphere position={[0, 1.8, 0]} args={[0.4, 16, 16]} castShadow receiveShado>
				<meshStandardMaterial attach="material" color="#998471" />
			</Sphere>
		<Plane position={[0, -0.01, 0]} args={[11, 11]} rotation={[-Math.PI / 2, 0, 0]} receiveShadow>
			<meshStandardMaterial attach="material" color="#73706b" />
		</Plane>
		<Box args={[15, 15, 4]} position={[0, -0.02 -2, 0]} rotation={[-Math.PI / 2, 0, 0]}>
			<meshBasicMaterial attach="material" color="#121821" />
		</Box>
		<Drone
			position={position0} // Access the position of the current drone
			light={light0} // Access the light of the current drone
			setpoint={setpoint0} // Access the setpoint of the current drone
			color={"#ff7300"}
		/>
		<Drone
			position={position1} // Access the position of the current drone
			light={light1} // Access the light of the current drone
			setpoint={setpoint1} // Access the setpoint of the current drone
			color={"#ffe605"}
		/>
		<Drone
			position={position2} // Access the position of the current drone
			light={light2} // Access the light of the current drone
			setpoint={setpoint2} // Access the setpoint of the current drone
			color={"#ff058a"}
		/>
		<Drone
			position={position3} // Access the position of the current drone
			light={light3} // Access the light of the current drone
			setpoint={setpoint3} // Access the setpoint of the current drone
			color={"#05d1ff"}
		/>

		{[ [-5, 1.25, -5], [5, 1.25, -5], [-5, 1.25, 5], [5, 1.25, 5],  // THE OPTITRACKS POLES
		].map((position, index) => (
			<Cylinder args={[0.07, 0.07, 2.5, 32]} position={position} key={index}>
				<meshStandardMaterial attach="material" color="#837296" />
			</Cylinder>
		))}
		<OrbitControls target={[0, 1.5, 0]} /> 
		<gridHelper args={[10, 10, 'black', 'black']} />
	</Canvas>
	</div>
	);
});


export {Live3dview};
