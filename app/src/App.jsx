import React, { useEffect, useState, useRef } from 'react';
import { DroneControll } from './DroneControll';
import { fetchConfigAndInitializeWebSocket } from './WebSocket';
import { Live3dview } from './Live3dview';


function App() {
	const [messages, setMessages] = useState([]);
	const [wsState, setWsState] = useState('disconnected');
	const [wsStateColor, setWsStateColor] = useState('text-red-600');
	const [ws, setWs] = useState(null);
	const messagesEndRef = useRef(null);

	const [position0, setPosition0] = useState([-4.5, 0.12, -3, 0]);
	const [position1, setPosition1] = useState([-4.5, 0.12, -1, 0]);
	const [position2, setPosition2] = useState([-4.5, 0.12, 1, 0]);
	const [position3, setPosition3] = useState([-4.5, 0.12, 3, 0]);
	const [setpoint0, setSetpoint0] = useState([1, 1, 1]);
	const [setpoint1, setSetpoint1] = useState([1, 1, 1]);
	const [setpoint2, setSetpoint2] = useState([1, 1, 1]);
	const [setpoint3, setSetpoint3] = useState([1, 1, 1]);
	const [rc0, setRc0] = useState([1, 1, 1, 1, 1]);
	const [rc1, setRc1] = useState([1, 1, 1, 1, 1]);
	const [rc2, setRc2] = useState([1, 1, 1, 1, 1]);
	const [rc3, setRc3] = useState([1, 1, 1, 1, 1]);
	const [trim0, setTrim0] = useState([1, 2, 3, 4]);
	const [trim1, setTrim1] = useState([1, 2, 3, 4]);
	const [trim2, setTrim2] = useState([1, 2, 3, 4]);
	const [trim3, setTrim3] = useState([1, 2, 3, 4]);
	const [pathLen0, setPathLen0] = useState([1]);
	const [pathLen1, setPathLen1] = useState([1]);
	const [pathLen2, setPathLen2] = useState([1]);
	const [pathLen3, setPathLen3] = useState([1]);
	const [light0, setLight0] = useState([1, 1]);
	const [light1, setLight1] = useState([1, 1]);
	const [light2, setLight2] = useState([1, 1]);
	const [light3, setLight3] = useState([1, 1]);
	const [ati0, setAti0] = useState([1, 1, 1]);
	const [ati1, setAti1] = useState([1, 1, 1]);
	const [ati2, setAti2] = useState([1, 1, 1]);
	const [ati3, setAti3] = useState([1, 1, 1]);
	const [frame0, setFrame0] = useState([0]);
	const [frame1, setFrame1] = useState([0]);
	const [frame2, setFrame2] = useState([0]);
	const [frame3, setFrame3] = useState([0]);
	const [bat0, setBat0] = useState([1, 1]);
	const [bat1, setBat1] = useState([1, 1]);
	const [bat2, setBat2] = useState([1, 1]);
	const [bat3, setBat3] = useState([1, 1]);


	useEffect(() => {
	fetchConfigAndInitializeWebSocket(setMessages, setWs, setWsState, setWsStateColor,
		setPosition0, setTrim0, setLight0, setSetpoint0, setBat0, setRc0, setAti0, setPathLen0, setFrame0,
		setPosition1, setTrim1, setLight1, setSetpoint1, setBat1, setRc1, setAti1, setPathLen1, setFrame1,
		setPosition2, setTrim2, setLight2, setSetpoint2, setBat2, setRc2, setAti2, setPathLen2, setFrame2,
		setPosition3, setTrim3, setLight3, setSetpoint3, setBat3, setRc3, setAti3, setPathLen3, setFrame3
	); }, []);

	useEffect(() => { // Scroll to the bottom whenever messages update
		if (messagesEndRef.current) {
			messagesEndRef.current.scrollTop = messagesEndRef.current.scrollHeight;
		}
	}, [messages]);

	return (
	<div className="grid grid-cols-8 gap-2 p-3 bg-gray-900 h-screen text-white">

		{/* Header */}
		<div className="col-span-8 bg-gray-800 p-4 flex items-center justify-center gap-3" style={{ height: '2vh' }}>
			<h2> Drone Dance Dialog </h2>
			<span className={wsStateColor}>{wsState}</span>
		</div>
		
		{/* Console */}
		<div className="col-span-3 bg-gray-800 p-3 flex" style={{ height: '49vh' }}>
			<div ref={messagesEndRef} className="overflow-auto w-full" style={{ lineHeight: '1.2' }}>
				{messages.map((msg, index) => (
					<p className="m-0 mt-1" key={index}>{msg}</p>
				))}
			</div>
		</div>

		{/* Three.js view */}
		<Live3dview 
			position0={position0} setpoint0={setpoint0} light0={light0}
			position1={position1} setpoint1={setpoint1} light1={light1}
			position2={position2} setpoint2={setpoint2} light2={light2}
			position3={position3} setpoint3={setpoint3} light3={light3}
		/>

		{/* Drone controller */}
		<DroneControll
			index={0} bat={bat0} rc={rc0} ws={ws}
			setpoint={setSetpoint0} setSetpoint={setSetpoint0}
			light={light0} setLight={setLight0}
			trim={trim0} setTrim={setTrim0}
			frame={frame0} setFrame={setFrame0}
			pathLen={pathLen0}
		/>

		<DroneControll
			index={1} bat={bat1} rc={rc1} ws={ws}
			setpoint={setSetpoint1} setSetpoint={setSetpoint1}
			light={light1} setLight={setLight1}
			trim={trim1} setTrim={setTrim1}
			frame={frame1} setFrame={setFrame1}
			pathLen={pathLen1}
		/>

		<DroneControll
			index={2} bat={bat2} rc={rc2} ws={ws}
			setpoint={setSetpoint2} setSetpoint={setSetpoint2}
			light={light2} setLight={setLight2}
			trim={trim2} setTrim={setTrim2}
			frame={frame2} setFrame={setFrame2}
			pathLen={pathLen2}
		/>

		<DroneControll
			index={3} bat={bat3} rc={rc3} ws={ws}
			setpoint={setSetpoint3} setSetpoint={setSetpoint3}
			light={light3} setLight={setLight3}
			trim={trim3} setTrim={setTrim3}
			frame={frame3} setFrame={setFrame3}
			pathLen={pathLen3}
		/>
	</div>
	);
}

export default App;
