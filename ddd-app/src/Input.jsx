import React, { useEffect, useState } from 'react';
import {Drone, DroneControll} from './Drone';
import { Col, Card, Row, Form, Container, Button} from 'react-bootstrap';
import { Canvas } from '@react-three/fiber';
import { OrbitControls, Sphere, Plane, Cylinder, Box } from '@react-three/drei';
import RcSlider from 'rc-slider'; // Importing the default Slider component from rc-slider
import 'rc-slider/assets/index.css'; // Import the CSS for rc-slider

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
function Slider({ index, name, axis, arg, ws, stateArray, setStateArray }) {
    // Initialize inputValue with the value from stateArray
    const [inputValue, setInputValue] = useState(stateArray[index][arg]);

    // Sync inputValue with stateArray when stateArray updates
    useEffect(() => {
        setInputValue(stateArray[index][arg]);
    }, [stateArray, index, arg]);

    // Handle value update
    const updateValue = (newValue) => {
        setInputValue(newValue);

        // Update the specific value in the state array
        setStateArray((prevArray) => {
            const updatedArray = [...prevArray[index]]; // Copy the array for the specific drone
            updatedArray[arg] = newValue; // Update the argument value
            const updatedState = { ...prevArray, [index]: updatedArray };
            return updatedState; // Return the updated state
        });

        // Update value in WebSocket
        const wsArray = [...stateArray[index]];
        wsArray[arg] = newValue;
        if (ws && ws.readyState === WebSocket.OPEN) {
            ws.send(`${index}{"${name}":${JSON.stringify(wsArray)}}`);
        }
    };

    // Handle number input change
    const handleInputChange = (e) => {
        const value = parseInt(e.target.value) || 0;
        updateValue(value);
    };

    // Handle slider change
    const handleSliderChange = (newValue) => {
        updateValue(newValue);
    };

    return (
        <div className="mb-2 text-center">
            <Form.Group>
                
                {/* Number input positioned above the slider */}
                <Form.Control
                    type="number"
                    value={inputValue}
                    onChange={handleInputChange}
                    step="1"
                    style={{
						width: '60px',
                        margin: '10px auto',
                        padding: '5px 5px',
                        appearance: 'textfield',
						fontSize: '0.8rem'
                    }}
                    className="always-visible-spinner"
                />
                
                {/* Vertical slider using rc-slider */}
                <div style={{ height: '100px', display: 'flex', justifyContent: 'center', alignItems: 'center' }}>
                    <RcSlider
                        vertical
                        min={0} // Adjust min as needed
                        max={255} // Adjust max as needed
                        value={inputValue}
                        onChange={handleSliderChange}
                        style={{ height: '100px' }}
                    />
                </div>
				<div className="mt-3 axis-label" style={{ fontWeight: 'bold', marginBottom: '6px' }}>{axis}</div>
                {/* <span style={{ marginTop: '8px' }}>{inputValue} {axis}</span> */}
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
