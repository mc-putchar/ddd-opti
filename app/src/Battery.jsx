
function Battery({bat}) {
	
	//TODO adjust to different battery 
	function estimateBatteryPercentage({voltage_battery}) { 
		const min_voltage = 3000; // 3.0V * 3 cells = 9.0V
		const max_voltage = 4200; // 4.2V * 3 cells = 12.6V 
		
		// Check if the voltage is within the expected range
		if (voltage_battery < min_voltage) {
			return 0; // Battery is empty
		} else if (voltage_battery > max_voltage) {
			return 100; // Battery is fully charged
		} else {
			// Scale the voltage to percentage and convert to int
			return Math.floor((voltage_battery - min_voltage) * 100 / (max_voltage - min_voltage));
		}
	}

	return (
	<>
		<span> {parseFloat((bat[0] * 0.001).toFixed(1))} V </span>
		<span> {parseFloat((bat[1] * 0.001).toFixed(1))} V </span>
		<span> {estimateBatteryPercentage(bat[0])} % </span>
	</>
	)
} 

export {Battery};
