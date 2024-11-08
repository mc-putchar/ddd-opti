import React, { useEffect, useState } from 'react';
import {Drone, DroneControll} from './Drone';
import { Col, Card, Row, Form, Container} from 'react-bootstrap';
import { Canvas } from '@react-three/fiber';
import { OrbitControls, Sphere, Plane, Cylinder, Box } from '@react-three/drei';
import { Slider as ShadcnSlider } from "@/components/ui/slider";
import { Input as ShadcnInput } from "@/components/ui/input";

function FloatInputForm({ setpoint, index, ws }) {
	const [input, setInput] = useState('');

	// Function to handle form submission
	const handleSubmit = (e) => {
		e.preventDefault();
		const values = input.split(',').map(val => parseFloat(val.trim()));

		// Check if the input is in the correct format
		if (values.length !== 3 || values.some(isNaN)) {
			setError('Please enter 3 valid float values separated by commas.');
			return;
		}

		// Format values to a specific number of decimal places, e.g., 5 decimal places
		const formattedValues = values.map(val => val.toFixed(5));

		 // Construct the message in the desired format
		 const message = `${index}{"setpoint":[${formattedValues.join(',')},-837.593]}`;

		 // Send the message through WebSocket
		 if (ws && ws.readyState === WebSocket.OPEN) {
			 ws.send(message);
		 }

		// Clear the input field and error after successful submission
		setInput('');
	};

	return (
		<div  className="mt-3">
		<Form onSubmit={handleSubmit}>
					<Form.Control
						type="text"
						placeholder=" Setpoints (x, y, z)"
						value={input}
						onChange={(e) => setInput(e.target.value)}
						style={{ backgroundColor: "#5b5d6e" }}
						/>
		</Form>
		</div>
	);
}

function Progression({ rc, index, arg, color = "blue" }) {
	const bgColorClass = `bg-${color}-900`;
	const overlayColorClass = `bg-${color}-600`;
	const maxvalue = 3000;
  
	// Calculate the width proportionally based on rc[index][arg]
	const value = rc[arg];
	const width = Math.min(Math.max((value / maxvalue) * 100, 0), 100); // Ensure it's between 0 and 100
	// safe is 988, and 2011

  
	return (
	  <div className="relative w-full h-5 mb-2">
		{/* Background div (full width) */}
		<div className={`absolute inset-0 ${bgColorClass}`}></div>
		{/* Half-width overlay, proportionally sized */}
		<div
		  className={`absolute inset-0 ${overlayColorClass}`}
		  style={{ width: `${width}%` }}
		></div>
		{/* Centered text */}
		<div className="absolute inset-0 flex items-center justify-center text-white font-bold">
		  {rc[arg]}
		</div>
	  </div>
	);
  }
  



function Slider({ index, name, vari, arg, ws, stateArray, setStateArray, min, max }) {
	// Initialize inputValue based on the stateArray
	const [inputValue, setInputValue] = useState(stateArray[arg]);
	const [isEditing, setIsEditing] = useState(false); // State to track if in edit mode

	// Sync inputValue with stateArray when stateArray updates
	useEffect(() => {
		setInputValue(stateArray[arg]);
	}, [stateArray, arg]);

	// Function to handle value update and WebSocket message
	const updateValue = (newValue) => {
		setInputValue(newValue);

		// Update the specific value in the state array
		setStateArray((prevArray) => {
			const updatedState = [...prevArray]; // Copy the array for the specific drone
			updatedState[arg] = newValue; // Update the argument value
			return updatedState; // Return the updated state
		});

		// Update value in WebSocket
		const wsArray = [...stateArray];
		wsArray[arg] = newValue;
		if (ws && ws.readyState === WebSocket.OPEN) {
			ws.send(`${index}{"${name}":${JSON.stringify(wsArray)}}`);
		}
	};

	// Handle slider change
	const handleSliderChange = (value) => {
		updateValue(value[0]); // Since Radix slider returns an array
	};

	// Handle input change
	const handleInputChange = (e) => {
		const newValue = Number(e.target.value); // Convert to number
		setInputValue(newValue); // Update the input value directly
	};

	// Handle input blur
	const handleInputBlur = () => {
		const newValue = Number(inputValue); // Convert to number
		if (!isNaN(newValue) && newValue >= min && newValue <= max) {
			updateValue(newValue); // Update with valid value
		}
		setIsEditing(false); // Exit edit mode
	};

	// Handle click to start editing
	const handleEditClick = () => {
		setIsEditing(true); // Switch to edit mode
	};

	const handleKeyDown = (e) => {
		if (e.key === 'Enter') {
			handleInputBlur(); // Call the blur handler to save the value
		}
	};

	return (
		<div className="flex items-center mb-2 w-full">
			<span className="mr-1 w-20">{vari}</span>
			{isEditing ? (
				<ShadcnInput
					type="number"
					value={inputValue}
					onChange={handleInputChange} // Add onChange handler
					onBlur={handleInputBlur} // Save on blur
					onKeyDown={handleKeyDown}
					className="mr-3 w-16 block text-right bg-gray-800 text-white border border-gray-600 rounded" // Style as needed
					style={{
						WebkitAppearance: 'none', // Hide the spinner in Webkit browsers
						MozAppearance: 'textfield', // Hide the spinner in Firefox
						appearance: 'none', // General appearance property to remove spinners
					}}
				/>
			) : (
				<span
					className="mr-3 w-16 block text-right cursor-pointer" // Add cursor pointer for clarity
					onClick={handleEditClick} // Click to edit
				>
					{inputValue}
				</span>
			)}
			<ShadcnSlider 
				value={[inputValue]} // Slider expects an array
				onValueChange={handleSliderChange} // Update on value change
				min={min} // Adjust min value as needed
				max={max} // Adjust max value as needed
				step={1} // Step size for the slider
				className="flex-grow bg-gray-900" // Allow the slider to take up available space and set a height
			/>
		</div>
	);
}



function Arm({index, ws}) {

	function arm() {
		if (ws && ws.readyState === WebSocket.OPEN) {
			ws.send(`${index}{"armed":true}`);
		}
	}

	function disarm() {
		if (ws && ws.readyState === WebSocket.OPEN) {
			ws.send(`${index}{"armed":false}`);
		}
	}

	return (
	<>
		<div className="bg-gray-900 cursor-pointer mx-2 p-1" onClick={arm} >Arm</div>
		<div className="bg-gray-900 cursor-pointer p-1" onClick={disarm} >Disarm</div>
	</>
	)
}

function Path({index, ws, frame, setFrame, pathLen }) {

	const updateValue = (newValue) => {
		setFrame(newValue);
		if (ws && ws.readyState === WebSocket.OPEN) {
			ws.send(`${index}{"frame":${newValue}}`);
		}
	};
	
	function playPath() {
		if (ws && ws.readyState === WebSocket.OPEN) {
			ws.send(`${index}{"path":"play"}`);
		}
	}

	function pausePath() {
		if (ws && ws.readyState === WebSocket.OPEN) {
			ws.send(`${index}{"path":"pause"}`);
		}
	}

	function stopPath() {
		if (ws && ws.readyState === WebSocket.OPEN) {
			ws.send(`${index}{"path":"stop"}`);
		}
	}

	// Handle slider change
	const handleSliderChange = (value) => {
		updateValue(value[0]); // Since Radix slider returns an array
	};

	return (
		<>
		<div className="flex items-center mt-2 mb-4 w-full">
			<span>{frame}</span>
				<ShadcnSlider 
					value={[frame]} // Slider expects an array
					onValueChange={handleSliderChange} // Update on value change
					min={0} // Adjust min value as needed
					max={pathLen} // Adjust max value as needed
					step={1} // Step size for the slider
					className="flex-grow bg-gray-900" // Allow the slider to take up available space and set a height
				/>
			<span>{pathLen}</span>
		</div>
		<div className="flex space-x-2">
		  <div className="bg-gray-900 cursor-pointer p-1 px-2" onClick={playPath}>
			Play
		  </div>
		  <div className="bg-gray-900 cursor-pointer p-1 px-2" onClick={pausePath}>
			Pause
		  </div>
		  <div className="bg-gray-900 cursor-pointer p-1 px-2" onClick={stopPath}>
			Stop
		  </div>
		</div>
					</>
	  )
}

export {FloatInputForm, Slider, Arm, Path, Progression};
