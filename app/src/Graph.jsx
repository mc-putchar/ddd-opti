import { useEffect, useState } from 'react';
import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer } from 'recharts';

function Graph({ graphInfo }) {
  const [chartData, setChartData] = useState([]);
  const [time, setTime] = useState(0); // Time counter to use for the x-axis
  const [isFrozen, setIsFrozen] = useState(false); // State for freezing/unfreezing the graph

  // Handle graph updates only if the graph is not frozen
  useEffect(() => {
    if (graphInfo && !isFrozen) {
      const currentTime = time;  // Use current time counter
      setTime(prevTime => prevTime + 1);  // Increment the time by 1 for the next data point

      // Update chart data with new graph info and the time value
      setChartData(prevData => {
        const newData = [
          ...prevData,
          {
            time: currentTime,  // Set time as the x-axis value
            xPWM: graphInfo.xPWM,
            yPWM: graphInfo.yPWM,
            zPWM: graphInfo.zPWM,
            yawPWM: graphInfo.yawPWM,
            xPID1: graphInfo.xPID1,
            yPID1: graphInfo.yPID1,
            zPID1: graphInfo.zPID1,
            yawPID1: graphInfo.yawPID1,
            xPID2: graphInfo.xPID2,
            yPID2: graphInfo.yPID2,
            zPID2: graphInfo.zPID2,
            yawPID2: graphInfo.yawPID2,
            xPosSetpoint: graphInfo.xPosSetpoint,
            yPosSetpoint: graphInfo.yPosSetpoint,
            zPosSetpoint: graphInfo.zPosSetpoint,
            xPos: graphInfo.xPos,  // Add position data
            yPos: graphInfo.yPos,
            zPos: graphInfo.zPos
          }
        ];

        // Keep only the last 10 seconds of data
        if (newData.length > 10) {
          newData.shift(); // Remove the oldest entry
        }

        return newData;
      });
    }
  }, [graphInfo, time, isFrozen]);  // Add isFrozen to the dependency array

  // Toggle the freeze state
  const handleFreezeToggle = () => {
    setIsFrozen(prevState => !prevState);
  };

  return (
    <div>
      <h3>PWM Graphs</h3>

      {/* Freeze toggle switch */}
      <label>
        <input type="checkbox" checked={isFrozen} onChange={handleFreezeToggle} />
        Freeze Graph
      </label>

      {/* Position Values Graph */}
      <ResponsiveContainer width="100%" height={200} style={{ backgroundColor: 'grey' }}>
        <LineChart data={chartData}>
          <CartesianGrid strokeDasharray="3 3" vertical={false} />
          <XAxis dataKey="time" />
          <YAxis />
          <Tooltip />
          <Legend />
          <Line type="monotone" dataKey="xPos" stroke="red" />
          <Line type="monotone" dataKey="yPos" stroke="blue" />
          <Line type="monotone" dataKey="zPos" stroke="green" />
        </LineChart>
      </ResponsiveContainer>

      {/* Setpoints Graph */}
      <ResponsiveContainer width="100%" height={200} style={{ backgroundColor: 'grey' }}>
        <LineChart data={chartData}>
          <CartesianGrid strokeDasharray="3 3" vertical={false} />
          <XAxis dataKey="time" />
          <YAxis domain={[-2, 2]} />
          <Tooltip />
          <Legend />
          <Line type="monotone" dataKey="xPosSetpoint" stroke="red" />
          <Line type="monotone" dataKey="yPosSetpoint" stroke="blue" />
          <Line type="monotone" dataKey="zPosSetpoint" stroke="green" />
        </LineChart>
      </ResponsiveContainer>

      {/* PID1 Graph */}
      <ResponsiveContainer width="100%" height={200} style={{ backgroundColor: 'grey' }}>
        <LineChart data={chartData}>
          <CartesianGrid strokeDasharray="3 3" vertical={false} />
          <XAxis dataKey="time" />
          <YAxis domain={[-2, 2]} />
          <Tooltip />
          <Legend />
          <Line type="monotone" dataKey="xPID1" stroke="red" />
          <Line type="monotone" dataKey="yPID1" stroke="blue" />
          <Line type="monotone" dataKey="zPID1" stroke="green" />
          <Line type="monotone" dataKey="yawPID1" stroke="orange" />
        </LineChart>
      </ResponsiveContainer>

      {/* PID2 Graph */}
      <ResponsiveContainer width="100%" height={200} style={{ backgroundColor: 'grey' }}>
        <LineChart data={chartData}>
          <CartesianGrid strokeDasharray="3 3" vertical={false} />
          <XAxis dataKey="time" />
          <YAxis domain={[-2, 2]} />
          <Tooltip />
          <Legend />
          <Line type="monotone" dataKey="xPID2" stroke="red" />
          <Line type="monotone" dataKey="yPID2" stroke="blue" />
          <Line type="monotone" dataKey="zPID2" stroke="green" />
          <Line type="monotone" dataKey="yawPID2" stroke="orange" />
        </LineChart>
      </ResponsiveContainer>

      {/* PWM Graph */}
      <ResponsiveContainer width="100%" height={200} style={{ backgroundColor: 'grey' }}>
        <LineChart data={chartData}>
          <CartesianGrid strokeDasharray="3 3" vertical={false} />
          <XAxis dataKey="time" />
          <YAxis domain={[100, 2000]} />
          <Tooltip />
          <Legend />
          <Line type="monotone" dataKey="xPWM" stroke="red" />
          <Line type="monotone" dataKey="yPWM" stroke="blue" />
          <Line type="monotone" dataKey="zPWM" stroke="green" />
          <Line type="monotone" dataKey="yawPWM" stroke="orange" />
        </LineChart>
      </ResponsiveContainer>
    </div>
  );
}

export { Graph };
