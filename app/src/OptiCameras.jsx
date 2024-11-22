import React, { useEffect, useState, useRef } from 'react';
import { Sphere, Plane, Cylinder, Box, useGLTF } from '@react-three/drei';
import * as Config from './Settings';

function OptiCameras() {
	return (
	<>
		{[ [-Config.SPACE_WIDTH/2, Config.SPACE_HEIGHT/2, -Config.SPACE_DEPTH/2],
			 [Config.SPACE_WIDTH/2, Config.SPACE_HEIGHT/2, -Config.SPACE_DEPTH/2],
			 [-Config.SPACE_WIDTH/2, Config.SPACE_HEIGHT/2, Config.SPACE_DEPTH/2],
			 [Config.SPACE_WIDTH/2, Config.SPACE_HEIGHT/2, Config.SPACE_DEPTH/2],
		].map((position, index) => (
			<group position={position} key={index}>

				{/* <Cylinder args={[0.02, 0.02, SPACE_HEIGHT, 12]}>
					<meshStandardMaterial attach="material" color="#837296" />
				</Cylinder> */}
				<Box args={[0.13, 0.13, 0.13]} position={[0, (Config.SPACE_HEIGHT/2) - 0.12/2 ,0]} >
					<meshLambertMaterial attach="material" color={"#0339a3"} wireframe={false} />
				</Box>
			</group>
		))}
		<Box args={[0.05, 0.05, Config.SPACE_DEPTH]} position={[Config.SPACE_WIDTH/2, (Config.SPACE_HEIGHT) - 0.12/2 ,0]} >
			<meshLambertMaterial attach="material" color={"#837296"} wireframe={false} />
		</Box>
		<Box args={[0.05, 0.05, Config.SPACE_DEPTH]} position={[-Config.SPACE_WIDTH/2, (Config.SPACE_HEIGHT) - 0.12/2 ,0]} >
			<meshLambertMaterial attach="material" color={"#837296"} wireframe={false} />
		</Box>
		<Box args={[Config.SPACE_WIDTH,0.05, 0.05]} position={[0, (Config.SPACE_HEIGHT) - 0.12/2 , Config.SPACE_DEPTH/2]} >
			<meshLambertMaterial attach="material" color={"#837296"} wireframe={false} />
		</Box>
		<Box args={[Config.SPACE_WIDTH,0.05, 0.05]} position={[0, (Config.SPACE_HEIGHT) - 0.12/2 , -Config.SPACE_DEPTH/2]} >
			<meshLambertMaterial attach="material" color={"#837296"} wireframe={false} />
		</Box>

		<Box args={[0.13, 0.13, 0.13]} position={[Config.SPACE_WIDTH/2, (Config.SPACE_HEIGHT) - 0.12/2 , 0]} >
			<meshLambertMaterial attach="material" color={"#0339a3"} wireframe={false} />
		</Box>
		<Box args={[0.13, 0.13, 0.13]} position={[-Config.SPACE_WIDTH/2, (Config.SPACE_HEIGHT) - 0.12/2 , 0]} >
			<meshLambertMaterial attach="material" color={"#0339a3"} wireframe={false} />
		</Box>

		{/* STAND */}
		{/* <Box args={[0.67, 0.01, 0.40]} position={[0, 1.16 , 0]} >
			<meshLambertMaterial attach="material" color={"#404040"} wireframe={false} />
		</Box>
		<Cylinder args={[0.03, 0.03, 1.16*2, 12]}>
					<meshStandardMaterial attach="material" color="#404040" />
		</Cylinder>
		<Box args={[0.4, 0.1, 0.2]} position={[0, 0 , 0]} >
			<meshLambertMaterial attach="material" color={"#404040"} wireframe={false} />
		</Box> */}
	</>
	)
}

export {OptiCameras};