import {FloatInputForm, Slider, Arm, Path, Progression} from './Input';
import { Battery } from './Battery';

function DroneControll({ bat, rc, index, ws, frame, setFrame, pathLen,
					setpoint, light, trim, setLight, setPoint, setTrim, color }) {

	return (
	<>
	
	<div className="col-span-4 bg-stone-700 p-1" style={{ height: '21vh' }}>
		<div className="grid grid-cols-4 gap-1 p-1 bg-stone-900 text-white" >
			<div className="col-span-1 bg-neutral-700 p-2 items-center ap-1 py-0 gap-1 flex">
				<h3 className="font-bold" >
					Drone {index}
				</h3>
				<Arm index={index} ws={ws} color={color}/>
			</div>

			<div className="col-span-1 bg-stone-700 p-2 pt-3 gap-4 flex">
				<Battery bat={bat} />
			</div>

			<div className="col-span-1 bg-stone-700 p-2 pt-3 flex">
				<h4 className="font-bold"> Path </h4>
			</div>

			<div className="col-span-1 bg-stone-700 p-2 pt-3 flex">
				<h4 className="font-bold"> Light </h4>
			</div>

			{/* Trim sliders */}
			<div className="col-span-1 bg-stone-700 p-2 flex flex-col">
				<Slider
					arg={1} name="trim" vari="Roll"
					stateArray={trim} setStateArray={setTrim}
					min={-800} max={800} index={index} ws={ws} 
				/>
				<Slider
					arg={0} name="trim" vari="Pitch"
					stateArray={trim} setStateArray={setTrim}
					min={-800} max={800} index={index} ws={ws} 
				/>
				<Slider
					arg={3} name="trim" vari="Yaw"
					stateArray={trim} setStateArray={setTrim}
					min={-800} max={800} index={index} ws={ws} 
				/>
				<Slider
					arg={2} name="trim" vari="Throt"
					stateArray={trim} setStateArray={setTrim}
					min={-800} max={800} index={index} ws={ws} 
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
				<FloatInputForm param={setpoint} index={index} ws={ws}/>
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
	</div>
	</>
	)
} 

export {DroneControll};
