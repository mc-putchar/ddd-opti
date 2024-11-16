import React, { useEffect, useState, useRef} from 'react';
import { OrbitControls, Cone, Sphere, Plane, Cylinder, Box, useGLTF } from '@react-three/drei';
import * as THREE from 'three';
import { clone } from 'three/examples/jsm/utils/SkeletonUtils.js';
import { GLTFLoader } from 'three/examples/jsm/loaders/GLTFLoader';

const LIGHT_COLOR = "#fffbeb"
const DRONE_BOUNDING_BOX = 0.8;
const DRONE_BOUNDING_BOX_HEIGHT = 0.8;

const DRONE_Z_MAX = 1.6 
					- DRONE_BOUNDING_BOX_HEIGHT/2;
const DRONE_Z_MIN = 0
					+ DRONE_BOUNDING_BOX_HEIGHT/2;




const Drone = React.memo(({ position, light, setpoint, color, droneGlb, ati }) => {
	const [droneReal, setDroneReal] = useState(droneGlb.scene.clone(true));
	const [droneGhost, setDroneGhost] = useState(droneGlb.scene.clone(true));
	const lightTarget = useRef();
	const scale = 0.0014;

	const zRotationReal = -(setpoint[3] * (Math.PI / 180)) % (2 * Math.PI);
	const zRotationGhost = -(setpoint[3] * (Math.PI / 180)) % (2 * Math.PI);
  
	// Calculate the target position based on the light[0] angle
	const lightAngle = light[0]; // angle in degrees
	const radians = lightAngle * (Math.PI / 180); // convert to radians
  
	// Calculate new target position based on the angle
	const targetHeight = -Math.cos(radians);
	const targetDistance = Math.sin(radians);
	const targetPosition = new THREE.Vector3(0, 1 * targetHeight, 1 * targetDistance);
  
	const coneRotationAngle = -Math.atan2(targetDistance, targetHeight);
	const conePosition = new THREE.Vector3(0, 1.45 * targetHeight, 1.7 * targetDistance);


    droneReal.traverse((child) => {
		if (child.isMesh && child.material) {
		  child.material = new THREE.MeshStandardMaterial({
			color: color || 'yellow',    // Set color dynamically
			metalness: 0.5,              // Gives a metallic effect without clearcoat
			roughness: 0.3,              // Adjust roughness for smoothness or matte effect
		  });
		  child.castShadow = true;       // Enable shadows if performance allows
		  child.receiveShadow = true;
		}
	  });

	  droneGhost.traverse((child) => {
		if (child.isMesh && child.material) {
		  child.material = new THREE.MeshBasicMaterial({
			color: color || 'yellow',    // Set color dynamically
			transparent: true,           // Enable transparency
			opacity: 0.08,                // Adjust opacity (0 is fully transparent, 1 is fully opaque)
			wireframe: true,             // Optional: wireframe mode for a lighter look
		  });
		  child.castShadow = false;      // Disable casting shadows
		  child.receiveShadow = false;   // Disable receiving shadows
		}
	  });

	return (
		<>
		{/* Ghost drone = position */}
		<group position={position} rotation={[ati[0], (zRotationGhost), -ati[1]]}>
			<primitive
				object={droneGhost}
				scale={[scale, scale, scale]} // Adjust scale as needed
				castShadow
				receiveShadow
			/>
			{/* <Box args={[0.80, 0.25, 0.80]}>
				<meshLambertMaterial attach="material" color={color} wireframe={true} />
			</Box> */}
		</group>

		{/* Real drone = setpoint */}
		<group position={setpoint} rotation={[ati[0], (zRotationReal), -ati[1]]}>
		{/* <group position={position} rotation={[ati[0], -(zRotation + ati[2]), -ati[1]]}> */}
		{/* Drone Box */}
		{/* <Box args={[0.35, 0.15, 0.35]}>
		  <meshStandardMaterial attach="material" color={color} />
		</Box> */}
		<primitive
				object={droneReal}
				position={[0, 0, 0]}
				scale={[scale, scale, scale]} // Adjust scale as needed
				castShadow
				receiveShadow
				/>
		
		<Box args={[DRONE_BOUNDING_BOX, 0.25, DRONE_BOUNDING_BOX]}>
			<meshLambertMaterial attach="material" color={color} wireframe={true} />
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
			color={LIGHT_COLOR}
			/>
		<mesh ref={lightTarget} position={targetPosition} /> {/* Adjust this target position as needed */}
	</group>
	</>
	);
});

function LightBeam({rotation, position, light, args}) {
	
	const geometry = new THREE.ConeGeometry(0.8, 3, 12);
	geometry.computeBoundingBox();

	const coneMaterial = new THREE.ShaderMaterial({
		uniforms: {
		  color1: { value: new THREE.Color(LIGHT_COLOR) }, // Color at the tip
		  color2: { value: new THREE.Color(LIGHT_COLOR) }, // Color at the base
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
			// Apply a quadratic easing effect to vUv.y
			float easedUvY = vUv.y * vUv.y; // Square vUv.y for a non-linear gradient

			// Mix the colors based on the adjusted easedUvY
			vec3 color = mix(color1, color2, easedUvY);
			
			// Apply the easedUvY to the alpha for a softer light falloff
			float alpha = easedUvY * 0.7 * opacity;
			
			// Set the final color and alpha
			gl_FragColor = vec4(color, alpha);
		  }
		`,
		transparent: true, // Enable transparency
	  });
	
	return (
		<>
			<Cone args={args} position={position} rotation={rotation}>
				<primitive attach="material" object={coneMaterial} />
			</Cone>
		</>
	)
}

export {Drone, DRONE_BOUNDING_BOX};
