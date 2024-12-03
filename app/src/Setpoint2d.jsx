import React, { useState, useRef, useEffect } from 'react';
import { Slider as ShadcnSlider } from "@/components/ui/slider";
import { Input as ShadcnInput } from "@/components/ui/input";
import { fetchConfigAndInitializeWebSocket } from './WebSocket';
import * as Config from './Settings';

function Setpoint2d({ index, ws, setpoint, setSetpoint, block_incoming_setpoint, setBlock_incoming_setpoint }) {
	const [coordinates, setCoordinates] = useState({ x: setpoint[0], y: setpoint[2], z: setpoint[1] });

	// Handle individual slider changes
	const handleSliderChange = (axis, value) => {
		// Update the corresponding axis in coordinates
		const updatedCoordinates = { ...coordinates, [axis]: value[0] };
		setCoordinates(updatedCoordinates); // Set the updated coordinates

		const flippedCoordinates = [updatedCoordinates.y, updatedCoordinates.z, updatedCoordinates.x, 0];


		// Send directly using updated coordinates
		console.log("sending : ", `${index}{"setpoint":${JSON.stringify([updatedCoordinates.x, updatedCoordinates.z, updatedCoordinates.y, 0])}}`);

		// Send the new setpoint to the WebSocket
		if (ws && ws.readyState === WebSocket.OPEN) {
			ws.send(`${index}{"setpoint":${JSON.stringify([updatedCoordinates.x, updatedCoordinates.z, updatedCoordinates.y, 0])}}`);
		}
	};

	return (
		<>
			{/* X Axis Slider */}
			<ShadcnSlider 
				value={[coordinates.x]} // Slider expects an array
				onValueChange={(value) => handleSliderChange('x', value)} // Update x axis when slider changes
				min={-(Config.SPACE_WIDTH_FLY / 2 - Config.DRONE_BOUNDING_BOX / 2)} // Adjust min value as needed
				max={Config.SPACE_WIDTH_FLY / 2 - Config.DRONE_BOUNDING_BOX / 2} // Adjust max value as needed
				step={0.1} // Step size for the slider
				className="flex-grow bg-stone-950 mb-4" // Allow the slider to take up available space and set a height
			/>
			{/* Y Axis Slider */}
			<ShadcnSlider 
				value={[coordinates.y]} // Slider expects an array
				onValueChange={(value) => handleSliderChange('y', value)} // Update y axis when slider changes
				min={-(Config.SPACE_DEPTH_FLY / 2 - Config.DRONE_BOUNDING_BOX / 2)} // Adjust min value as needed
				max={Config.SPACE_DEPTH_FLY / 2 - Config.DRONE_BOUNDING_BOX / 2} // Adjust max value as needed
				step={0.1} // Step size for the slider
				className="flex-grow bg-stone-950 mb-4" // Allow the slider to take up available space and set a height
			/>
			{/* Z Axis Slider */}
			<ShadcnSlider 
				value={[coordinates.z]} // Slider expects an array
				onValueChange={(value) => handleSliderChange('z', value)} // Update z axis when slider changes
				min={0.25} // Adjust min value as needed
				max={Config.SPACE_HEIGHT_FLY} // Adjust max value as needed
				step={0.1} // Step size for the slider
				className="flex-grow bg-stone-950" // Allow the slider to take up available space and set a height
			/>
		</>
	);
}




export { Setpoint2d };
