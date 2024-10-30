import React, { useEffect, useState } from 'react';
import {Drone, DroneControll} from './Drone';
import { Col, Card, Row, Form, Container, Button} from 'react-bootstrap';
import { Canvas } from '@react-three/fiber';
import { OrbitControls, Sphere, Plane, Cylinder, Box } from '@react-three/drei';

function FloatInputForm({ setpoint, index, ws }) {
    const [input, setInput] = useState('');
    const [error, setError] = useState('');

    // Function to handle form submission
    const handleSubmit = (e) => {
        e.preventDefault();
        const values = input.split(',').map(val => parseFloat(val.trim()));

        // Check if the input is in the correct format
        if (values.length !== 3 || values.some(isNaN)) {
            setError('Please enter 3 valid float values separated by commas.');
            return;
        }

        // Send the message through WebSocket
        if (ws && ws.readyState === WebSocket.OPEN) {
            ws.send(JSON.stringify({ values }));
        }

        // Clear the input field and error after successful submission
        setInput('');
        setError('');
    };

    return (
        <Form onSubmit={handleSubmit}>
            <Form.Group as={Row} controlId="formFloatValues">

                <Col sm={12}>
                    <Form.Control
                        type="text"
                        placeholder="Setpoints (x, y, z)"
                        value={input}
                        onChange={(e) => setInput(e.target.value)}
                        isInvalid={!!error}
                    />
                    <Form.Control.Feedback type="invalid">
                        {error}
                    </Form.Control.Feedback>
                </Col>
            </Form.Group>
        </Form>
    );
}



function Slider({ param, index, name, axis, arg, ws, stateArray, setStateArray }) {
	// Increment function
	const increment = () => {
		const newValue = stateArray[index][arg] + 5; // Get the current value and increment by 5
		updateValue(newValue);
	};

	// Decrement function
	const decrement = () => {
		const newValue = stateArray[index][arg] - 5; // Get the current value and decrement by 5
		updateValue(newValue);
	};

	// Function to handle value update and WebSocket message
	const updateValue = (newValue) => {

		// Update the specific value in the state array
		setStateArray((prevArray) => {
			const updatedArray = [...prevArray[index]]; // Copy the current array for the drone
			updatedArray[arg] = newValue; // Update the specific argument value
			const updatedState = { ...prevArray, [index]: updatedArray };
			return updatedState; // Return the updated state
		});

		// Construct the array to send over WebSocket, placing the newValue at position `arg`
		const wsArray = [...stateArray[index]];
		wsArray[arg] = newValue; // Place the new value at the correct index

		// Send the WebSocket message if the connection is open
		if (ws && ws.readyState === WebSocket.OPEN) {
			ws.send(`${index}{"${name}":${JSON.stringify(wsArray)}}`); // Send the array with the updated value over WebSocket
		}
	};

	return (
		<div className="mb-2">
			<Form.Group>
				<div>
					<Button onClick={decrement} variant="secondary">-</Button>
					<Button className="mx-1" onClick={increment} variant="secondary">+</Button>
					<span className="mx-1">
						{stateArray[index][arg]} {axis} {/* Safely display the value */}
					</span>
				</div>
			</Form.Group>
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
		<Button className="m-0" onClick={arm} variant="secondary" >Arm</Button>
		<Button className="m-1" onClick={disarm} variant="secondary">Disarm</Button>
	</>
	)
}

export {FloatInputForm, Slider, Arm};
