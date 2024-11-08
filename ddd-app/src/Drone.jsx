import React, { useEffect, useState, useRef} from 'react';
import { OrbitControls, Cone, Sphere, Plane, Cylinder, Box, useGLTF } from '@react-three/drei';
import * as THREE from 'three';


const Drone = React.memo(({ position, light, setpoint, color, droneGlb }) => {
	const lightTarget = useRef();
	const zRotation = -(position[3] * (Math.PI / 180)) % (2 * Math.PI);
  
	// Calculate the target position based on the light[0] angle
	const lightAngle = light[0]; // angle in degrees
	const radians = lightAngle * (Math.PI / 180); // convert to radians
  
	// Calculate new target position based on the angle
	const targetHeight = -Math.cos(radians); // height adjustment based on angle
	const targetDistance = Math.sin(radians); // distance adjustment based on angle
  
	// Set the target position relative to the spotlight
	const targetPosition = new THREE.Vector3(0, 1 * targetHeight, 1 * targetDistance);
  
	const coneRotationAngle = -Math.atan2(targetDistance, targetHeight);
	const conePosition = new THREE.Vector3(0, 1.45 * targetHeight, 1.7 * targetDistance);


    droneGlb.scene.traverse((child) => {
        if (child.isMesh && child.material) {
          child.material.color.set(color || 'yellow'); // Set the color dynamically
        }
      });

	const scale = 0.0014;
  
	return (
	<group position={position} rotation={[0, zRotation, 0]}>
		{/* Drone Box */}
		{/* <Box args={[0.35, 0.15, 0.35]}>
		  <meshStandardMaterial attach="material" color={color} />
		</Box> */}
		<primitive
				object={droneGlb.scene}
				position={[0, 0, 0]}
				scale={[scale, scale, scale]} // Adjust scale as needed
				castShadow
				receiveShadow
			/>
		
		<Box args={[0.85, 0.65, 0.85]}>
		  <meshStandardMaterial attach="material" color={color} wireframe={true} />
		</Box>
		
		{/* Cone for the drone */}
		<LightBeam
			args={[0.8, 3, 12]} light={light[1]} position={conePosition}
			rotation={[Math.PI, -(Math.PI / 2), coneRotationAngle]}
		/>

		{/* Spotlight above the drone */}
		<spotLight
			position={[0, -0.1, 0]} angle={0.25}
			penumbra={1} intensity={light[1]}
			castShadow shadow-mapSize-width={1024} shadow-mapSize-height={1024}
			target={lightTarget.current} // Point spotlight at the target
		/>
		<mesh ref={lightTarget} position={targetPosition} /> {/* Adjust this target position as needed */}
		{/* <Sphere args={[0.05, 16, 16]} position={targetPosition}>
		  <meshStandardMaterial attach="material" color="red" />
		</Sphere> */}
	</group>
	);
});

function LightBeam({rotation, position, light }) {

	const geometry = new THREE.ConeGeometry(0.8, 3, 12);
	geometry.computeBoundingBox();

	const coneMaterial = new THREE.ShaderMaterial({
		uniforms: {
		  color1: { value: new THREE.Color("white") }, // Color at the tip
		  color2: { value: new THREE.Color("white") }, // Color at the base
		  bboxMin: { value: geometry.boundingBox.min },
		  bboxMax: { value: geometry.boundingBox.max },
		  opacity: { value: light / 255 }
		},
		vertexShader: `
		  uniform vec3 bboxMin;
		  uniform vec3 bboxMax;
	  
		  varying vec2 vUv;
	
		  void main() {
			// Adjust UVs based on the y-position of the cone
			vUv.y = (position.y - bboxMin.y) / (bboxMax.y - bboxMin.y); // Normalize based on the y-coordinate
			gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0);
		  }
		`,
		fragmentShader: `
		  uniform vec3 color1;
		  uniform vec3 color2;
		  uniform float opacity;
	  
		  varying vec2 vUv;
		  
		  void main() {
			// Mix the colors based on the normalized y-position (vUv.y)
			gl_FragColor = vec4(mix(color1, color2, vUv.y), vUv.y * 0.7 * opacity); // Blend colors from tip to base
		  }
		`,
		transparent: true, // Enable transparency
	  });
	
	return (
		<>
			<Cone args={[0.8, 3, 12]} position={position} rotation={rotation}>
				<primitive attach="material" object={coneMaterial} />
			</Cone>
		</>
	)
}

export {Drone};
