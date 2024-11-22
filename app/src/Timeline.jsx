
import React, { useState } from "react";

function Timeline({ duration = 48 }) {
  const [selectedTime, setSelectedTime] = useState(null);

  const handleClick = (time) => {
    setSelectedTime(time);
    console.log(`Selected time: ${time}s`);
  };

  return (
	<div className="timeline-container p-1 bg-stone-950 text-stone-300 w-full">
  {/* Timeline Header and Timeline Divisions in the same row */}
  <div className="grid grid-cols-10 gap-1 bg-stone-900 p-1 font-bold text-center">
    
    {/* Right column for timeline divisions (0s - {duration}s) */}
    <div
      className="col-span-9 grid grid-cols-[repeat(${duration},minmax(0,1fr))] gap-[1px] bg-stone-700 mt-2"
      style={{
        gridTemplateColumns: `repeat(${duration}, minmax(0, 1fr))`, // Dynamically creating the grid columns based on duration
      }}
    >
      {Array.from({ length: duration }, (_, index) => (
        <div
          key={index}
          className={`p-2 text-xs text-center cursor-pointer ${
            selectedTime === index
              ? "bg-blue-500 text-white"
              : "bg-stone-600 hover:bg-stone-500"
          }`}
          onClick={() => handleClick(index)}
        >
          |
        </div>
      ))}
    </div>

    {/* Left column for "Timeline:" label */}
    <div className="col-span-1 flex items-center justify-center text-green-300">
      00 : 00 : {duration} : 00 
    </div>
	
  </div>
</div>

  );
}

export {Timeline};
