import React, { useEffect, useState } from 'react';
import { Col, Card, Row, Form, Container, Button} from 'react-bootstrap';
import { Canvas } from '@react-three/fiber';
import { OrbitControls, Sphere, Plane, Cylinder, Box } from '@react-three/drei';

function FloatInputForm({ ws }) {
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

                <Col sm={8}>
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

function Drone({ position }) {
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
                intensity={100}
                castShadow
            />
        </>
    );
}


function Slider({index, name, axis, ws}) {
	const [value, setValue] = useState(200); // Initial value

	const increment = () => {
		setValue((prev) => {
		  const newValue = prev + 5;
		  if (ws && ws.readyState === WebSocket.OPEN) {
			ws.send(`${index}{"${name}":[${newValue}]}`);
		  }
		  return newValue;
		});
	  };

	const decrement = () => {
		setValue((prev) => {
		  const newValue = prev - 5;
		  if (ws && ws.readyState === WebSocket.OPEN) {
			ws.send(`${index}{"${name}":[${newValue}]}`);
		  }
		  return newValue;
		});
	  };

  return (
    <div className="mb-2">
      <Form.Group>
        {/* <Form.Label className="mb-1">{name}</Form.Label> */}
        <div>
          <Button onClick={decrement} variant="secondary">-</Button>
          <Button className="mx-2"onClick={increment} variant="secondary">+</Button>
		  <span className="mx-2">{value} {axis}</span>
        </div>
      </Form.Group>
    </div>
  );
}

function Live3dview({ dronePositions }) {

	return (
		<Canvas
			orthographic
			camera={{ zoom: 40, position: [3, 3.5, 10] }}
			style={{ background: '#222' }}
			>
			<ambientLight intensity={0.6} />
			{/* <spotLight
				position={[5, 5, 5]}
				angle={0.25}
				penumbra={1}
				intensity={100}
				castShadow // not castin nay shadow for some reason ?
				/> */}
			<Cylinder position={[0, 0.9, 0]} args={[0.3, 0.7, 1.8]} castShadow>
				<meshStandardMaterial attach="material" color="blue" />
			</Cylinder>
			<Plane position={[0, -0.01, 0]} args={[11, 11]} rotation={[-Math.PI / 2, 0, 0]} receiveShadow>
				<meshStandardMaterial attach="material" color="lightgrey" />
			</Plane>
			{Object.entries(dronePositions).map(([index, position]) => (
                <Drone key={index} position={position} /> // Use the Drone component here
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
	const [wsState, setWsState] = useState('disconnected to backend');
	const [ws, setWs] = useState(null);
	const [wsPort, setWsPort] = useState(8080); // Default port
	const [dronePositions, setDronePositions] = useState({
		0: [-6, 1, -2],  // Initial positions for drone 0
		1: [-6, 1, 0],  // Initial positions for drone 1	
		2: [-6, 1, 2],  // Initial positions for drone 2
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
				setWsState('connected to backend');
				socket.send('7{"front":"hello"}');
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
				  }
			  
				  if (jsonData.setpoint) {
					  if (jsonData.setpoint && Array.isArray(jsonData.setpoint)) {
						const adjustedPosition = [
						  jsonData.setpoint[0] !== undefined ? jsonData.setpoint[0] : 0,
						  jsonData.setpoint[2] !== undefined ? jsonData.setpoint[2] : 0,
						  jsonData.setpoint[1] !== undefined ? jsonData.setpoint[1] : 0
						];
						setDronePositions((prevPositions) => ({
							...prevPositions,
							[droneIndex]: adjustedPosition,  // Update position for the specific drone
						  }));
				  }
			  
				  if (jsonData.position) {
				
					  setDronePositions((prevPositions) => ({
						...prevPositions,
						[droneIndex]: adjustedPosition,  // Update position for the specific drone
					  }));
					}
				  }
			  
				  if (jsonData.light) {
				  }
			  
				  if (jsonData.arm) {
				  }
	
				  // Update the state with the parsed message to display
				  setMessages((prevMessages) => {
					const newMessages = [...prevMessages, cleanMsg];
					return newMessages.length > 50 ? newMessages.slice(-50) : newMessages; // Keep only the last 50 messages
				});
			  
				  // Update drone positions if necessary
				  
				} catch (error) {
				  console.error("Error parsing JSON:", error);
				}
			  };
			socket.onclose = () => {
				setWsState('disconnected to backend !');
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
			<Row className=" p-3">
				<h3 className=" mb-1">DDD liveView</h3>
				<span > {wsState}</span>
				{/* <Card className="shadow-sm p-3 h-100" > */}
					<Row style={{ height: "400px" }}>
						<Live3dview dronePositions={dronePositions} />
					</Row>
				{/* </Card> */}
				<Col className="p-3">
					<h4>Trim</h4>
					<Slider index={0} name="trim" axis={'x'} ws={ws} />
					<Slider index={0} name="trim" axis={'y'} ws={ws} />
					<Slider index={0} name="trim" axis={'z'} ws={ws} />
					<Slider index={0} name="trim" axis={'yaw'} ws={ws} />
					<h4>light</h4>
					<Slider index={0} name="light" axis={'yaw'} ws={ws} />
					<Slider index={0} name="light" axis={'yaw'} ws={ws} />
					<FloatInputForm ws={ws} />
					<Arm  index={0} name="arm" ws={ws} />
				</Col>
				<Col className="p-3">
				<div className="overflow-auto" style={{ lineHeight: '0.4', maxHeight: '300px' }}>
						{messages.map((msg, index) => (
							<p key={index}>{msg}</p>
						))}
					</div>
				</Col>
			</Row>
		</Container>
		</>
	);
}

function Arm({name, index, ws}) {

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
		<Button className="m-0" onClick={arm} >Arm</Button>
		<Button className="m-1" onClick={disarm} >Disarm</Button>
	</>
	)
}

export default App;
