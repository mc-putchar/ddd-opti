import React, { useState, useRef, useEffect } from 'react';
import { SPACE_HEIGHT, SPACE_DEPTH, SPACE_WIDTH } from './Live3dview';
import { DRONE_BOUNDING_BOX } from './Drone';

function Setpoint2d ({ index, ws, setpoint, setSetpoint, block_incoming_setpoint, setBlock_incoming_setpoint }) {
	const [coordinates, setCoordinates] = useState({ x: setpoint[0], y: setpoint[2] });
	const containerRef = useRef(null);
	const [isDragging, setIsDragging] = useState(false);
	const throttleTimeout = useRef(null);

	useEffect(() => {
		setCoordinates({ x: setpoint[0], y: setpoint[2] });
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
			-(SPACE_WIDTH / 2 - DRONE_BOUNDING_BOX / 2),
			Math.min(
				SPACE_WIDTH / 2 - DRONE_BOUNDING_BOX / 2,
				((e.clientX - containerRect.left) / containerRect.width) * SPACE_WIDTH - SPACE_WIDTH / 2
			)
		);
		const newY = Math.max(
			-(SPACE_DEPTH / 2 - DRONE_BOUNDING_BOX / 2),
			Math.min(
				SPACE_DEPTH / 2 - DRONE_BOUNDING_BOX / 2,
				((e.clientY - containerRect.top) / containerRect.height) * SPACE_DEPTH - SPACE_DEPTH / 2
			)
		);

		setCoordinates({ x: newX, y: newY });
		setSetpoint((prevSetpoint) => {
			const updatedSetpoint = [...prevSetpoint];
			updatedSetpoint[0] = newX;
			updatedSetpoint[2] = newY;
			updatedSetpoint[1] = 1;
			return updatedSetpoint;
		});
	};

	// Use useEffect to send WebSocket message whenever setpoint changes
	useEffect(() => {
		if (isDragging) {
			// Throttle sending WebSocket messages by only allowing one send per interval
			if (!throttleTimeout.current) {
				throttleTimeout.current = setTimeout(() => {
					const wsArray = [...setpoint];
					wsArray[3] = 0;

					if (ws && ws.readyState === WebSocket.OPEN) {
						ws.send(`${index}{"setpoint":${JSON.stringify(wsArray)}}`);
					}
					
					// Clear throttle timeout
					throttleTimeout.current = null;
				}, 50); // Adjust interval as needed
			}
		}
	}, [setpoint, isDragging, ws, index]);

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
		<div
			className="slider-container h-full bg-stone-900 border border-stone-950 flex"
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
					left: `${((coordinates.x + SPACE_WIDTH / 2) / SPACE_WIDTH) * 100}%`,
					top: `${((coordinates.y + SPACE_DEPTH / 2) / SPACE_DEPTH) * 100}%`,
					width: '10px',
					height: '10px',
					borderRadius: '50%',
					cursor: 'pointer',
					transform: 'translate(-50%, -50%)',
				}}
			/>
		</div>
	);
}

export { Setpoint2d };