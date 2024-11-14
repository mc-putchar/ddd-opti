import React, { useEffect, useState, useRef } from 'react';
import { Sphere, Plane, Cylinder, Box, useGLTF } from '@react-three/drei';
import { SPACE_HEIGHT, SPACE_DEPTH, SPACE_WIDTH } from './Live3dview';

function OptiCameras() {
	return (
	<>
		{[ [-SPACE_WIDTH/2, SPACE_HEIGHT/2, -SPACE_DEPTH/2],
			 [SPACE_WIDTH/2, SPACE_HEIGHT/2, -SPACE_DEPTH/2],
			 [-SPACE_WIDTH/2, SPACE_HEIGHT/2, SPACE_DEPTH/2],
			 [SPACE_WIDTH/2, SPACE_HEIGHT/2, SPACE_DEPTH/2],
		].map((position, index) => (
			<group position={position} key={index}>

				<Cylinder args={[0.02, 0.02, SPACE_HEIGHT, 12]}>
					<meshStandardMaterial attach="material" color="#837296" />
				</Cylinder>
				<Box args={[0.13, 0.13, 0.13]} position={[0, (SPACE_HEIGHT/2) - 0.12/2 ,0]} >
					<meshLambertMaterial attach="material" color={"#0339a3"} wireframe={false} />
				</Box>
			</group>
		))}
		<Box args={[0.05, 0.05, SPACE_DEPTH]} position={[SPACE_WIDTH/2, (SPACE_HEIGHT) - 0.12/2 ,0]} >
			<meshLambertMaterial attach="material" color={"#837296"} wireframe={false} />
		</Box>
		<Box args={[0.05, 0.05, SPACE_DEPTH]} position={[-SPACE_WIDTH/2, (SPACE_HEIGHT) - 0.12/2 ,0]} >
			<meshLambertMaterial attach="material" color={"#837296"} wireframe={false} />
		</Box>
		<Box args={[SPACE_WIDTH,0.05, 0.05]} position={[0, (SPACE_HEIGHT) - 0.12/2 , SPACE_DEPTH/2]} >
			<meshLambertMaterial attach="material" color={"#837296"} wireframe={false} />
		</Box>
		<Box args={[SPACE_WIDTH,0.05, 0.05]} position={[0, (SPACE_HEIGHT) - 0.12/2 , -SPACE_DEPTH/2]} >
			<meshLambertMaterial attach="material" color={"#837296"} wireframe={false} />
		</Box>

		
	</>
	)
}

export {OptiCameras};