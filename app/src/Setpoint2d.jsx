import React, { useState, useRef, useEffect } from 'react';
import { Slider as ShadcnSlider } from "@/components/ui/slider";
import { Input as ShadcnInput } from "@/components/ui/input";
import { fetchConfigAndInitializeWebSocket } from './WebSocket';
import * as Config from './Settings';

function Setpoint2d ({ index, ws, setpoint, setSetpoint, block_incoming_setpoint, setBlock_incoming_setpoint }) {
	const [coordinates, setCoordinates] = useState({ x: setpoint[0], y: setpoint[2], z: setpoint[1]});
	const containerRef = useRef(null);
	const [isDragging, setIsDragging] = useState(false);
	const [isSliding, setIssliding] = useState(false);
	const throttleTimeout = useRef(null);

	useEffect(() => {
		setCoordinates({ x: setpoint[0], y: setpoint[2], z: setpoint[1]});
	}, [setpoint]);

	const handleMouseDown = () => {
		setIsDragging(true);
		setBlock_incoming_setpoint(true);
	};

	const handleMouseUp = () => {
		setIsDragging(false);
		setBlock_incoming_setpoint(false);

		if (throttleTimeout.current) {
			clearTimeout(throttleTimeout.current);
			throttleTimeout.current = null;
		}
	};

	const handleMouseMove = (e) => {
		if (!containerRef.current || !isDragging) return;

		const containerRect = containerRef.current.getBoundingClientRect();
		const newX = Math.max(
			-(Config.SPACE_WIDTH_FLY / 2 - Config.DRONE_BOUNDING_BOX / 2),
			Math.min(
				Config.SPACE_WIDTH_FLY / 2 - Config.DRONE_BOUNDING_BOX / 2,
				((e.clientX - containerRect.left) / containerRect.width) * Config.SPACE_WIDTH_FLY - Config.SPACE_WIDTH_FLY / 2
			)
		);
		const newY = Math.max(
			-(Config.SPACE_DEPTH_FLY / 2 - Config.DRONE_BOUNDING_BOX / 2),
			Math.min(
				Config.SPACE_DEPTH_FLY / 2 - Config.DRONE_BOUNDING_BOX / 2,
				((e.clientY - containerRect.top) / containerRect.height) * Config.SPACE_DEPTH_FLY - Config.SPACE_DEPTH_FLY / 2
			)
		);

		setCoordinates({ x: newX, y: newY});
		setSetpoint((prevSetpoint) => {
			const updatedSetpoint = [...prevSetpoint];
			updatedSetpoint[0] = newX;
			updatedSetpoint[2] = newY;
			return updatedSetpoint;
		});
	};

	// Use useEffect to send WebSocket message whenever setpoint changes
	useEffect(() => {
		// console.log("in useeffect");
		if (isDragging || isSliding) {
			// console.log("in is draagin");
			// Throttle sending WebSocket messages by only allowing one send per interval
			if (!throttleTimeout.current) {
				throttleTimeout.current = setTimeout(() => {
					const wsArray = [...setpoint];
					const temp = wsArray[1];
					wsArray[1] = wsArray[2];
					wsArray[2] = temp;
					wsArray[3] = 0;

					if (ws && ws.readyState === WebSocket.OPEN) {
						ws.send(`${index}{"setpoint":${JSON.stringify(wsArray)}}`);
						console.log("sending : ", `${index}{"setpoint":${JSON.stringify(wsArray)}}`);
					}
					
					// Clear throttle timeout
					throttleTimeout.current = null;
				}, 50); // Adjust interval as needed
			}
		}
	}, [setpoint, isDragging, ws, index]);

	// const handleSliderChange = (value) => {
	// 	setIssliding(true);
	// 	setBlock_incoming_setpoint(true);
	// 	setCoordinates({z: value[0]});
	// 	setSetpoint((prevSetpoint) => {
	// 		const updatedSetpoint = [...prevSetpoint];
	// 		updatedSetpoint[1] = value[0];
	// 		return updatedSetpoint;
	// 	});
	// 	setTimeout(() => {
	// 		setIssliding(false);
	// 		setBlock_incoming_setpoint(false);
	// 	}, 300);
		
	// };
	const handleSliderChange = (value) => {
		updateValue(value[0]); // Since Radix slider returns an array
	};

	const updateValue = (newValue) => {
		setSetpoint((prevSetpoint) => {
					const updatedSetpoint = [...prevSetpoint];
					updatedSetpoint[1] = value[0];
					return updatedSetpoint;
				});

		// Update value in WebSocket
		const wsArray = [...setpoint];
		wsArray[arg] = newValue;
		if (ws && ws.readyState === WebSocket.OPEN) {
			ws.send(`${index}{"${name}":${JSON.stringify(wsArray)}}`);
		}
	};

	useEffect(() => {
		if (isDragging) {
			document.addEventListener('mousemove', handleMouseMove);
			document.addEventListener('mouseup', handleMouseUp);
		} else {
			document.removeEventListener('mousemove', handleMouseMove);
			document.removeEventListener('mouseup', handleMouseUp);
		}

		return () => {
			document.removeEventListener('mousemove', handleMouseMove);
			document.removeEventListener('mouseup', handleMouseUp);
		};
	}, [isDragging]);

	return (
		<>
		<div
			className="slider-container h-full mb-2 bg-stone-900 border border-stone-950 flex"
			ref={containerRef}
			onMouseDown={handleMouseDown}
			style={{
				position: 'relative',
				borderRadius: '5px',
				overflow: 'hidden',
				flexDirection: 'column',
			}}
			>
			<div
				className="dot bg-stone-400"
				style={{
					position: 'absolute',
					left: `${((coordinates.x + Config.SPACE_WIDTH / 2) / Config.SPACE_WIDTH) * 100}%`,
					top: `${((coordinates.y + Config.SPACE_DEPTH / 2) / Config.SPACE_DEPTH) * 100}%`,
					width: '10px',
					height: '10px',
					borderRadius: '50%',
					cursor: 'pointer',
					transform: 'translate(-50%, -50%)',
				}}
				/>
		</div>
		<ShadcnSlider 
				value={[coordinates.z]} // Slider expects an array
				onValueChange={handleSliderChange} // Update on value change
				// onMouseDown={handleMouseDown}
				min={0.25} // Adjust min value as needed
				max={Config.SPACE_HEIGHT_FLY} // Adjust max value as needed
				step={0.1} // Step size for the slider
				className="flex-grow bg-stone-950" // Allow the slider to take up available space and set a height
			/>
		</>
	);
}

export { Setpoint2d };
