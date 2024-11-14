import React, { useState, useRef, useEffect } from 'react';
import { Slider as ShadcnSlider } from "@/components/ui/slider";
import { SPACE_HEIGHT, SPACE_DEPTH, SPACE_WIDTH } from './Live3dview';
import { DRONE_BOUNDING_BOX } from './Drone';


function Setpoint2d ({index, ws, setpoint, setSetpoint, block_incoming_setpoint, setBlock_incoming_setpoint}) {
	const [coordinates, setCoordinates] = useState({ x: setpoint[0], y: setpoint[2] });
	const containerRef = useRef(null);
	const [isDragging, setIsDragging] = useState(false);
	const lastExecuted = useRef(null);
  
	useEffect(() => {
		setCoordinates({ x: setpoint[0], y: setpoint[2] });
	}, [setpoint]);

	const handleMouseDown = (e) => {
	  setIsDragging(true);
	};
  
	const handleMouseUp = () => {
	  setIsDragging(false);
	};	

	useEffect(() => {
		// Debounce effect to set blockIncomingSetpoint back to false after 500ms of inactivity
		const interval = setInterval(() => {
			if (block_incoming_setpoint && Date.now() - lastExecuted.current >= 500) {
				setBlock_incoming_setpoint(false);
			}
		}, 100);

		// Clear the interval on unmount
		return () => clearInterval(interval);
	}, [block_incoming_setpoint]);
  
	// Handle mouse move on the document to update coordinates while dragging
	const handleMouseMove = (e) => {
		if (!containerRef.current || !isDragging) return; // Only move the dot if dragging is active
  
		setBlock_incoming_setpoint(true);
		lastExecuted.current = Date.now();
		console.log(block_incoming_setpoint);

	  // Get the container's bounding box
		const containerRect = containerRef.current.getBoundingClientRect();
  
	  // Calculate the new X and Y based on mouse position within the container
		const newX = Math.max(-((SPACE_WIDTH/2) - DRONE_BOUNDING_BOX/2), Math.min((SPACE_WIDTH/2) - DRONE_BOUNDING_BOX/2, ((e.clientX - containerRect.left) / containerRect.width) * SPACE_WIDTH - (SPACE_WIDTH/2)));
		const newY = Math.max(-(SPACE_DEPTH/2 - DRONE_BOUNDING_BOX/2), Math.min((SPACE_DEPTH/2) -  DRONE_BOUNDING_BOX/2, ((e.clientY - containerRect.top) / containerRect.height) * SPACE_DEPTH - (SPACE_DEPTH/2)));

	  
		setCoordinates({ x: newX, y: newY });
		setSetpoint((prevSetpoint) => {
			const updatedSetpoint= [...prevSetpoint];
			updatedSetpoint[0] = newX;
			updatedSetpoint[2] = newY;
			updatedSetpoint[1] = 1;
			return updatedSetpoint;
		});
		// const wsArray = [...setpoint];
		// wsArray[3] = 0;
		// if (ws && ws.readyState === WebSocket.OPEN) {
		// 	ws.send(`${index}{"setpoint":${JSON.stringify(wsArray)}}`);
		// }
	};

	// Use useEffect to add global event listeners for mousemove and mouseup
	React.useEffect(() => {
	  // Add event listeners for mousemove and mouseup on the document
	  if (isDragging) {
		document.addEventListener('mousemove', handleMouseMove);
		document.addEventListener('mouseup', handleMouseUp);
	  } else {
		// Clean up event listeners when dragging is not happening
		document.removeEventListener('mousemove', handleMouseMove);
		document.removeEventListener('mouseup', handleMouseUp);
	  }
  
	  // Cleanup on component unmount
	  return () => {
		document.removeEventListener('mousemove', handleMouseMove);
		document.removeEventListener('mouseup', handleMouseUp);
	  };
	}, [isDragging]);
  
	return (
		<>
		<div 
		  className="slider-container h-full bg-stone-900 border border-stone-950 flex" // Set up flexbox row
		  ref={containerRef}
		  onMouseDown={handleMouseDown}
		  style={{
			position: 'relative',
			borderRadius: '5px',
			overflow: 'hidden',
			flexDirection: 'column', // Stack elements inside the container vertically
		  }}
		>
		  {/* Dot for x and y movement */}
		  <div
			className="dot bg-stone-400"
			style={{
			  position: 'absolute',
			  left: `${((coordinates.x + (SPACE_WIDTH/2)) / SPACE_WIDTH) * 100}%`,
			  top: `${((coordinates.y + (SPACE_DEPTH/2)) / SPACE_DEPTH) * 100}%`,
			  width: '10px',
			  height: '10px',
			  borderRadius: '50%',
			  cursor: 'pointer',
			  transform: 'translate(-50%, -50%)', // Center the dot
			}}
		  />
		</div>
	  </>
	);
  }

export {Setpoint2d};