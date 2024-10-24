import React, { useEffect, useState } from 'react';
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

function Drone({ position, light }) {
    return (
        <>
            {/* Box representing the drone */}
            <Box position={position} args={[0.35, 0.15, 0.35]}>
                <meshStandardMaterial attach="material" color="yellow" />
            </Box>

            {/* Spotlight above the drone */}
            <spotLight
                position={[position[0], position[1] - 0.05, position[2]]} // Slightly above the drone
                angle={0.25}
                penumbra={1}
                intensity={light[1]}
                castShadow
				shadow-mapSize-width={1024}
        		shadow-mapSize-height={1024}
            />
        </>
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




function Live3dview({ position, setpoint, light }) {

	return (
		<Canvas
			orthographic
			camera={{ zoom: 40, position: [3, 3.5, 10] }}
			style={{ background: '#222' }}
			shadowMap
			shadows
			>
			<ambientLight intensity={0.6} />
			{/* <spotLight
				position={[5, 5, 5]}
				angle={0.25}
				penumbra={1}
				intensity={100}
				castShadow // not castin nay shadow for some reason ?
				/> */}
			<Cylinder position={[0, 0.9, 0]} args={[0.3, 0.7, 1.8]} castShadow receiveShadow>
				<meshStandardMaterial attach="material" color="blue" />
			</Cylinder>
			<Plane position={[0, -0.01, 0]} args={[11, 11]} rotation={[-Math.PI / 2, 0, 0]} receiveShadow>
				<meshStandardMaterial attach="material" color="lightgrey" />
			</Plane>
			{Object.keys(position).map((index) => (
				<Drone
					key={index}
					index={index} // Pass the index of the drone
					position={position[index]} // Access the position of the current drone
					light={light[index]} // Access the light of the current drone
					setpoint={setpoint[index]} // Access the setpoint of the current drone
				/>
			))}

			{[ // THE OPTITRACKS POLES
				[-5, 1.25, -5],  // Bottom left
				[5, 1.25, -5],   // Bottom right
				[-5, 1.25, 5],   // Top left
				[5, 1.25, 5],    // Top right
			].map((position, index) => (
				<Cylinder args={[0.07, 0.07, 2.5, 32]} position={position} key={index}>
					<meshStandardMaterial attach="material" color="red" />
				</Cylinder>
			))}
			<OrbitControls target={[0, 1.5, 0]} />
			{/* <axesHelper args={[1.5]} /> */}
			<gridHelper args={[10, 10]} />
			{/* <directionalLight /> */}
		</Canvas>
	);
}

function App() {
	const [message, setMessage] = useState('');
	const [messages, setMessages] = useState([]);
	const [wsState, setWsState] = useState('disconnected');
	const [wsStateColor, setWsStateColor] = useState('text-success');
	const [ws, setWs] = useState(null);
	const [wsPort, setWsPort] = useState(8080); // Default port
	const [position, setPosition] = useState({
		0: [-6, 1, -3],
		1: [-6, 1, -1],
		2: [-6, 1, 1],
		3: [-6, 1, 3]
	});
	const [trim, setTrim] = useState({
		0: [1, 1, 1, 1],
		1: [1, 1, 1, 1],
		2: [1, 1, 1, 1],
		3: [1, 1, 1, 1]
	});
	const [light, setLight] = useState({
		0: [1, 1],
		1: [1, 1],
		2: [1, 1],
		3: [1, 1]
	});
	const [setpoint, setSetpoint] = useState({
		0: [1, 1],
		1: [1, 1],
		2: [1, 1],
		3: [1, 1]
	});
	

	// Function to fetch the port from JSON and initialize the WebSocket
	const fetchConfigAndInitializeWebSocket = async () => {
		try {
			const response = await fetch('/port.json'); // Adjust the path if needed
			if (!response.ok) {
				throw new Error('Network response was not ok');
			}
			const config = await response.json();
			const newWsPort = config.wsPort; // Update wsPort with the value from config
			console.log('wsPort from json =', newWsPort);
			setWsPort(newWsPort); // Update state with the new port

			// Initialize the WebSocket connection with the new port
			const socket = new WebSocket(`ws://localhost:${newWsPort}/ws`);
			setWs(socket);

			socket.onopen = () => {
				console.log('Connected to WebSocket server');
				setWsState('connected');
				setWsStateColor('text-success');
			};

			socket.onmessage = (event) => {
				console.log('Message from server:', event.data);
				
				// Clean the message by removing any unwanted prefixes
				const cleanMsg = event.data.replace("Message received: ", "");
				
				// Extract drone index and JSON part
				const droneIndex = parseInt(cleanMsg[0], 10);  // Get drone index (0, 1, 2...)
				const jsonString = cleanMsg.slice(1);  // Get everything after the first character
			  
				try {
				  const jsonData = JSON.parse(jsonString);  // Parse the cleaned JSON string
				  
				  // Handle the different cases based on the keys in jsonData
				  let messageToDisplay = '';
			  

				  if (jsonData.trim) {
					setTrim(prevTrims => {
					  // Copy the current trim state for all drones
					  const updatedTrims = { ...prevTrims };
				  
					  // Update the specific drone's trim values (keeping the array structure intact)
					  updatedTrims[droneIndex] = [
						jsonData.trim[0],  // x value
						jsonData.trim[2],  // z value (reordered as needed)
						jsonData.trim[1],  // y value (reordered as needed)
						jsonData.trim[3]   // any additional value (if applicable)
					  ];
					  return updatedTrims; // Return the updated state
					});
				  }
				  if (jsonData.position) {
					setTrim(prevPosition => {
					  // Copy the current trim state for all drones
					  const updatedPosition = { ...prevPosition };
				  
					  // Update the specific drone's trim values (keeping the array structure intact)
					  updatedPosition[droneIndex] = [
						jsonData.position[0],  // x value
						jsonData.position[2],  // z value (reordered as needed)
						jsonData.position[1],  // y value (reordered as needed)
					  ];
					  return updatedPosition; // Return the updated state
					});
				  }
				  if (jsonData.setpoint) {
					setPosition(prevPosition => {
					  // Copy the current trim state for all drones
					  const updatedPosition = { ...prevPosition };
				  
					  // Update the specific drone's trim values (keeping the array structure intact)
					  updatedPosition[droneIndex] = [
						jsonData.setpoint[0],  // x value
						jsonData.setpoint[2],  // z value (reordered as needed)
						jsonData.setpoint[1],  // y value (reordered as needed)
						0
					  ];
					  return updatedPosition; // Return the updated state
					});
				  }

			  
				// if (jsonData.setpoint) {
				// 	setDrones(prev => {
				// 		const updatedDrones = {
				// 			...prev,
				// 			[droneIndex]: {
				// 				...prev[droneIndex],
				// 				trim: [jsonData.setpoint[0], jsonData.setpoint[2], jsonData.setpoint[1]] // New position
				// 			}
				// 		};
				// 		return updatedDrones;
				// 	});
				// }
				
				// WHEN OPTITRACK COMES
				//   if (jsonData.position) {
				// 	setDrones(prev => ({...prev, droneIndex: { ...prev[droneIndex],
				// 		trim: [jsonData.position[0], jsonData.position[2], jsonData.position[1]]}})); // New position
				// 	}
				//   }
			  
				//   if (jsonData.light) {
				// 	setDrones(prev => ({...prev, [droneIndex]: { ...prev[droneIndex],
				// 		trim: [jsonData.light[0], jsonData.light[1]]}})); // New position
				//   }
			  
				  if (jsonData.arm) {
				  }
	
				  // Update the state with the parsed message to display
				  setMessages((prevMessages) => {
					const newMessages = [...prevMessages, cleanMsg];
					return newMessages.length > 100 ? newMessages.slice(-100) : newMessages; // Keep only the last 50 messages
				});
			  
				  // Update drone positions if necessary
				  
				} catch (error) {
				  console.error("Error parsing JSON:", error);
				}
			  };
			socket.onclose = () => {
				setWsState('diconnected');
				setWsStateColor('text-danger');
				console.log('WebSocket connection closed');
			  };


			// Cleanup function to close the WebSocket connection
			return () => socket.close();
		} catch (error) {
			console.error('Error fetching config:', error);
		}
	};

	useEffect(() => {
		fetchConfigAndInitializeWebSocket();
	}, []);

	return (
	<>
	<Container fluid>
	<Row className="p-4">

		<Row className="px-0 mb-2 align-items-end">
			<Col className="px-0" xs="auto">
				<h3 className="mb-0">DDD_liveView</h3>	
			</Col>
			<Col xs="auto">
				<span className={wsStateColor}>{wsState}</span>
			</Col>
		</Row>

		<Row className="p-0" style={{ height: "400px" }}>
			<Live3dview position={position} setpoint={setpoint} light={light} />
		</Row>

		<Row>
			<Col className="p-3">
				<Row>
					<Col>
					<DroneControll index={0} position={position} setpoint={setpoint} light={light} trim={trim} setSetpoint={setSetpoint} setPosition={setPosition} setLight={setLight} setTrim={setTrim} ws={ws} />
					</Col>
					<Col>
					<DroneControll index={1} position={position} setpoint={setpoint} light={light} trim= {trim} setSetpoint={setSetpoint} setPosition={setPosition} setLight={setLight} setTrim={setTrim} ws={ws} />
					</Col>
				</Row>
				<Row>
					<Col>
					<DroneControll index={2} position={position} setpoint={setpoint} light={light} trim= {trim} setSetpoint={setSetpoint} setPosition={setPosition} setLight={setLight} setTrim={setTrim} ws={ws} />
					</Col>
					<Col>
					<DroneControll index={3} position={position} setpoint={setpoint} light={light} trim= {trim} setSetpoint={setSetpoint} setPosition={setPosition} setLight={setLight} setTrim={setTrim} ws={ws} />
					</Col>
				</Row>
			</Col>

			<Col className="p-3">
				<div className="overflow-auto" style={{ lineHeight: '1.2', maxHeight: '300px' }}>
					{messages.map((msg, index) => (
						<p className="m-0 mt-1" key={index}>{msg}</p>
					))}
				</div>
			</Col>
		</Row>

	</Row>
	</Container>
	</>
	);
}

function DroneControll({index, position, setpoint, light, trim, setPosition, setLight, setPoint, setTrim, ws}) {

	function sendPath() {
		if (ws && ws.readyState === WebSocket.OPEN) {
			ws.send(`${index}{"path":"send"}`);
		}
	}

	return (
	<Row className="mb-4">
		<h4 className="mb-0">Drone_{index}</h4>
		<Col className="p">
			<h5>Trim</h5>
			<Slider
			param="trim"
			index={index}  // Drone index
			name="trim"  // To identify trim values
			axis="x"
			arg={0}  // This will update the first value of trim
			ws={ws}  // WebSocket connection
			stateArray={trim}  // Pass the trims state
			setStateArray={setTrim}  // Pass the setter for trims
			/>
			<Slider
			param="trim"
			index={index}  // Drone index
			name="trim"  // To identify trim values
			axis="y"
			arg={1}  // This will update the first value of trim
			ws={ws}  // WebSocket connection
			stateArray={trim}  // Pass the trims state
			setStateArray={setTrim}  // Pass the setter for trims
			/>
			<Slider
			param="trim"
			index={index}  // Drone index
			name="trim"  // To identify trim values
			axis="z"
			arg={2}  // This will update the first value of trim
			ws={ws}  // WebSocket connection
			stateArray={trim}  // Pass the trims state
			setStateArray={setTrim}  // Pass the setter for trims
			/>
			<Slider
			param="trim"
			index={index}  // Drone index
			name="trim"  // To identify trim values
			axis="yaw"
			arg={3}  // This will update the first value of trim
			ws={ws}  // WebSocket connection
			stateArray={trim}  // Pass the trims state
			setStateArray={setTrim}  // Pass the setter for trims
			/>
		</Col>
		<Col className="p">
			<h5>Light</h5>
			<Slider
			param="light"
			index={index}  // Drone index
			name="light"  // To identify trim values
			axis="angle"
			arg={0}  // This will update the first value of trim
			ws={ws}  // WebSocket connection
			stateArray={light}  // Pass the trims state
			setStateArray={setLight}  // Pass the setter for trims
			/>
			<Slider
			param="light"
			index={index}  // Drone index
			name="light"  // To identify trim values
			axis="power"
			arg={1}  // This will update the first value of trim
			ws={ws}  // WebSocket connection
			stateArray={light}  // Pass the trims state
			setStateArray={setLight}  // Pass the setter for trims
			/>
			<Arm index={index} ws={ws} />
			<Button className="my-0" onClick={sendPath}>Send Path</Button>
		</Col>
		<FloatInputForm param={setpoint} index={index} ws={ws}/>
	</Row>
	)
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

export default App;
