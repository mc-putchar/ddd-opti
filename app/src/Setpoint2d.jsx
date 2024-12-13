import React, { useState, useRef, useEffect } from 'react';
import { Slider as ShadcnSlider } from "@/components/ui/slider";
import * as Config from './Settings';

function Setpoint2d({ index, ws, setpoint}) {
	const [coordinates, setCoordinates] = useState({ x: setpoint[0], y: setpoint[2], z: setpoint[1] });

	// Handle individual slider changes
	const handleSliderChange = (axis, value) => {
		// Update the corresponding axis in coordinates
		const updatedCoordinates = { ...coordinates, [axis]: value[0] };
		setCoordinates(updatedCoordinates); // Set the updated coordinates
		// Send directly using updated coordinates
		console.log("sending : ", `${index}{"setpoint":${JSON.stringify([updatedCoordinates.x, updatedCoordinates.y, updatedCoordinates.z, 0])}}`);
		// Send the new setpoint to the WebSocket
		if (ws && ws.readyState === WebSocket.OPEN) {
			ws.send(`${index}{"setpoint":${JSON.stringify([updatedCoordinates.x, updatedCoordinates.y, updatedCoordinates.z, 0])}}`);
		}
	};

	return (
	<>
		{/* X Axis Slider */}
		<ShadcnSlider 
			value={[coordinates.x]}
			onValueChange={(value) => handleSliderChange('x', value)}
			min={-(Config.SPACE_WIDTH_FLY / 2 - Config.DRONE_BOUNDING_BOX / 2)}
			max={Config.SPACE_WIDTH_FLY / 2 - Config.DRONE_BOUNDING_BOX / 2}
			step={0.1}
			className="flex-grow bg-stone-950 mb-4"/> 
		{/* Y Axis Slider */}
		<ShadcnSlider 
			value={[coordinates.y]}
			onValueChange={(value) => handleSliderChange('y', value)}
			min={-(Config.SPACE_DEPTH_FLY / 2 - Config.DRONE_BOUNDING_BOX / 2)}
			max={Config.SPACE_DEPTH_FLY / 2 - Config.DRONE_BOUNDING_BOX / 2}
			step={0.1}
			className="flex-grow bg-stone-950 mb-4"/>
		{/* Z Axis Slider */}
		<ShadcnSlider 
			value={[coordinates.z]}
			onValueChange={(value) => handleSliderChange('z', value)}
			min={0}
			max={Config.SPACE_HEIGHT_FLY}
			step={0.1}
			className="flex-grow bg-stone-950" />
	</>
	);
}

export { Setpoint2d };
