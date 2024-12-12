import React, { useEffect, useState, useRef } from 'react';
import { Drone } from './Drone';
import { Canvas } from '@react-three/fiber';
import { OrbitControls, Sphere, Plane, Cylinder, Box, useGLTF } from '@react-three/drei';
import * as THREE from 'three';
import { OptiCameras } from './OptiCameras';
import * as Config from './Settings';



const Live3dview = React.memo(({ 
	position0, position1, position2, position3, 
	setpoint0, setpoint1, setpoint2, setpoint3,
	droneGlb,
	ati0, ati1, ati2, ati3,
	light0, light1, light2, light3, colors
  }) => {

	const denise = useGLTF('/assets/Denise.glb');
	const dancer = useGLTF('/assets/angel.glb');

	dancer.scene.traverse((child) => {
		if (child.isMesh && child.material) {
		  // Apply a "statue-like" material
		  child.material = new THREE.MeshStandardMaterial({
			color: '#7d6b5c',      // Use a light stone or white for a stone-like color
			metalness: 0.1,          // Low metalness to avoid a shiny look
			roughness: 0.8,          // High roughness to give it a matte, textured appearance
		  });
		  child.castShadow = true;    // Enable shadows
		  child.receiveShadow = true; // Enable receiving shadows
		}
	  });

	return (

	<Canvas
		orthographic
		camera={{ zoom: 90, position: [4, 4.5, 10] }}
		style={{ background: '#1c1917' }}
		shadows
		onCreated={({ gl }) => {
			const globalPlane = new THREE.Plane(new THREE.Vector3(0, 0.02, 0), 1)
			gl.clippingPlanes = [globalPlane] // Apply the global clipping plane
			gl.localClippingEnabled = true // Enable local clipping
		  }}
		
	>
		<ambientLight intensity={0.5} />
		{/* <primitive
				object={denise.scene}
				position={[0, 0, 0]}
				scale={[1.0, 1.0, 1.0]} // Adjust scale as needed
				castShadow
				receiveShadow
			/> */}
		{/* <primitive
				object={dancer.scene}
				position={[0, 0, 0]}
				scale={[0.007, 0.007, 0.007]} // Adjust scale as needed
				castShadow
				receiveShadow
			/> */}
		<Plane position={[0, -0.01, 0]} args={[Config.SPACE_WIDTH, Config.SPACE_DEPTH]} rotation={[-Math.PI / 2, 0, 0]} 
		// receiveShadow
		>
			<meshStandardMaterial attach="material" color="#57534e" />
		</Plane>
		<Drone
			position={position0} light={light0} setpoint={setpoint0} 
			color={colors[0]} droneGlb={droneGlb} ati={ati0}
		/>
		{/* <Drone
			position={position1} light={light1} setpoint={setpoint1} 
			color={colors[1]} droneGlb={droneGlb} ati={ati1}
		/> */}
		{/* <Drone
			position={position2} light={light2} setpoint={setpoint2} 
			color={colors[2]} droneGlb={droneGlb} ati={ati2}
		/>
		<Drone
			position={position3} light={light3} setpoint={setpoint3} 
			color={colors[3]} droneGlb={droneGlb} ati={ati3}
		/> */}
		<OptiCameras/>

		<Curtain />
		<Box args={[Config.SPACE_WIDTH_FLY, Config.SPACE_HEIGHT_FLY, Config.SPACE_DEPTH_FLY]}
			position={[0, Config.SPACE_HEIGHT_FLY/2, 0]}>
			<meshLambertMaterial attach="material" color={"yellow"} wireframe={true} />
		</Box>
		<pointLight position={[0, 3, 0]} intensity={50} color="white" castShadow shadow-mapSize-width={1024} shadow-mapSize-height={1024}/>
		{/* <OrbitControls target={position0} />  */}
		<OrbitControls target={[0, 0.7, 0]} /> 
		<gridHelper args={[Math.ceil(Config.SPACE_WIDTH), Math.ceil(Config.SPACE_WIDTH), 'black', '#57534e']} />
	</Canvas>
	);
});

function Curtain () {
	const curtainRef = useRef();

	useEffect(() => {
	  const { geometry } = curtainRef.current;
  
	  const position = geometry.attributes.position;
	  for (let i = 0; i < position.count; i++) {
		const x = position.getX(i);
		const wave = Math.sin(x * 20) * 0.12; // Adjust fold frequency and height
		position.setZ(i, wave);
	  }
	  position.needsUpdate = true; // Notify Three.js of the geometry update
	}, []);
  
	return (
	  <mesh ref={curtainRef} position={[0, 0, -Config.SPACE_DEPTH/2]} >
		{/* Plane geometry for the curtain */}
		<planeGeometry args={[5, 6, 128, 16]} castShadow receiveShadow/>
		<meshStandardMaterial color="#403737"/>
	  </mesh>
	);
  };
  
  


export {Live3dview};