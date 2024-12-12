import { Slider, Arm, Path, Progression} from './Input';
import React, { useEffect, useState } from 'react';
import { Battery } from './Battery';
import { Setpoint2d } from './Setpoint2d';

function Pid({ index, ws }) {
				
	const [pid, setPid] = useState({
		xyP1: "1", xyI1: "0.02", xyD1: "0.1",
		zP1: "2", zI1: "0.05", zD1: "0.1",
		yawP1: "0.2", yawI1: "0.05", yawD1: "0",
		xyP2: "0.3", xyI2: "0.01", xyD2: "0.1",
		zP2: "0.5", zI2: "0.05", zD2: "0.1",
		geC: "28", geO: "-0.035",
	});

	const handleChange = (e, key) => {
		setPid({
		...pid,
		[key]: e.target.value,
		});
	};

	const handleKeyDown = (e, key) => {
		if (e.key === "Enter") {
		sendWebSocketMessage();
		}
	};

	const sendWebSocketMessage = () => {
		const payload = `${index}{"pid":[${Object.values(pid).join(",")}]}`;
		ws.send(payload);
		console.log(payload);
	};
	
	return (
		<div className="grid grid-cols-6 gap-2 p-2">
		{Object.keys(pid).map((key) => (
		  <div key={key} className="flex flex-col">
			<label className="text-stone-300 text-sm mb-1">{key}</label>
			<input
			  type="text"
			  value={pid[key]}
			  onChange={(e) => handleChange(e, key)}
			  onKeyDown={handleKeyDown}
			  className="p-2 border border-stone-600 rounded bg-stone-800 text-stone-300"
			/>
		  </div>
		))}
		<button
		  onClick={sendWebSocketMessage}
		  className="col-span-3 bg-green-500 hover:bg-green-600 text-white py-2 px-4 rounded"
		>
		  Submit
		</button>
	  </div>
	)
} 


export {Pid};
