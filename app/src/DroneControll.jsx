import { Slider, Arm, Path, Progression} from './Input';
import React, { useEffect } from 'react';
import { Battery } from './Battery';
import { Setpoint2d } from './Setpoint2d';

function DroneControll({ bat, rc, index, ws, frame, setFrame, pathLen,
					setpoint, light, trim, setLight, setSetpoint, setTrim, color, block_incoming_setpoint,
					setBlock_incoming_setpoint }) {
				
	useKey(" ", () => {
		console.log("space pressed for drone: ", index);
		if (ws && ws.readyState === WebSocket.OPEN) {
			ws.send(`${index}{"armed":false}`);
			console.log("sent kill for drone: ", index);
		}
	});
	return (
	<>
		<div className="col-span-1 grid grid-cols-5 gap-x-0.5 p-0 bg-stone-800" >
			<div className="col-span-1 bg-stone-600 p-0 m-0 items-center gap-1 flex">
				<div className="bg-stone-700 p-2 items-center gap-1 flex" style={{ backgroundColor: color }}>
					<h3 className="font-bold text-stone-950" >
						D_{index}
					</h3>
				</div>
				<Arm index={index} ws={ws} color={color}/>
			</div>

			<div className="col-span-1 bg-stone-600 p-2 pt-3 gap-4 flex">
				<Battery bat={bat} />
			</div>

			<div className="col-span-1 bg-stone-600 p-2 pt-3 flex">
				<h4 className=""> Path </h4>
			</div>
			<div className="col-span-1 bg-stone-600 p-2 pt-3 flex">
				<h4 className=""> Setpoint </h4>
			</div>

			<div className="col-span-1 bg-stone-600 p-2 pt-3 flex">
				<h4 className=""> Light </h4>
			</div>
			<div className="col-span-5 bg-stone-100 h-px" style={{ backgroundColor: color }}></div>
			{/* <div className="col-span-4 bg-stone-900 h-px" ></div> */}

			{/* Trim sliders */}
			<div className="col-span-1 bg-stone-700 p-2 flex flex-col">
				<Slider
					arg={1} name="trim" vari="Roll"
					stateArray={trim} setStateArray={setTrim}
					min={-400} max={400} index={index} ws={ws} 
				/>
				<Slider
					arg={0} name="trim" vari="Pitch"
					stateArray={trim} setStateArray={setTrim}
					min={-400} max={400} index={index} ws={ws} 
				/>
				<Slider
					arg={3} name="trim" vari="Yaw"
					stateArray={trim} setStateArray={setTrim}
					min={-400} max={400} index={index} ws={ws} 
				/>
				<Slider
					arg={2} name="trim" vari="Throt"
					stateArray={trim} setStateArray={setTrim}
					min={-800} max={-600} index={index} ws={ws} 
				/>
			</div>

			{/* Trim telemetry */}
			<div className="col-span-1 bg-stone-700 p-2 flex flex-col">
				<Progression rc={rc} index={index} arg={0} color={"red"}/>
				<Progression rc={rc} index={index} arg={1} color={"purple"}/>
				<Progression rc={rc} index={index} arg={2} color={"blue"}/>
				<Progression rc={rc} index={index} arg={3} color={"cyan"}/> 
				<Progression rc={rc} index={index} arg={4} color={"green"}/>
			</div>

			{/* Path */}
			<div className="col-span-1 bg-stone-700 p-2 flex flex-col">
				<Path index={index} ws={ws} frame={frame} setFrame={setFrame} pathLen={pathLen}  />
				{/* <FloatInputForm param={setpoint} index={index} ws={ws}/> */}
			</div>

			{/* Setpoint2D */}
			<div className="col-span-1 bg-stone-700 p-2 flex flex-col">
				<Setpoint2d index={index} ws={ws} setpoint={setpoint} setSetpoint={setSetpoint} 
							block_incoming_setpoint={block_incoming_setpoint}
							setBlock_incoming_setpoint={setBlock_incoming_setpoint} />
				{/* <FloatInputForm param={setpoint} index={index} ws={ws}/> */}
			</div>

			{/* Light */}
			<div className="col-span-1 bg-stone-700 p-2 flex flex-col">
				<Slider
					arg={0} name="light" vari="Angle"
					stateArray={light} setStateArray={setLight}
					min={0} max={90} index={index} ws={ws} 
				/>
				<Slider
					arg={1} name="light" vari="Power"
					stateArray={light} setStateArray={setLight}
					min={0} max={255} index={index} ws={ws} 
				/>
				<Slider
					arg={1} name="light" vari="Strobe"
					stateArray={light} setStateArray={setLight}
					min={0} max={255} index={index} ws={ws} 
				/>
			</div>
		</div>
	</>
	)
} 

function useKey(key, callback) {
    useEffect(() => {
        const handleKeyPress = (event) => {
            if (event.key === key) {
                callback(event);
            }
        };

        window.addEventListener("keydown", handleKeyPress);
        return () => {
            window.removeEventListener("keydown", handleKeyPress);
        };
    }, [key, callback]);
}

export {DroneControll};
