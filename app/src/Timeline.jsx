
import React, { useState } from "react";

function Timeline({ duration = 48 }) {
  const [selectedTime, setSelectedTime] = useState(null);

  const handleClick = (time) => {
    setSelectedTime(time);
    console.log(`Selected time: ${time}s`);
  };

  return (
    <div className="timeline-container p-4 bg-stone-800 text-stone-300">
      {/* Timeline Header */}
      <div className="bg-stone-600 p-2 font-bold text-center">
        Timeline: 0s - {duration}s
      </div>

      {/* Timeline Divisions */}
      <div
        className="grid gap-[1px] bg-stone-700 w-full mt-2"
        style={{
          gridTemplateColumns: `repeat(${duration}, minmax(0, 1fr))`,
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
            {index}s
          </div>
        ))}
      </div>
    </div>
  );
}

export {Timeline};
