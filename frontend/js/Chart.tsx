import React, { useState, useEffect } from "react";
import { YAxis, ResponsiveContainer, AreaChart, Area } from "recharts";

const generateRandomDataPoint = (prevprice: number) => {
  return {
    price: Math.round(Math.max(0, prevprice + (Math.random() * 5 - 2.5))),
  };
};

const Chart = () => {
  const [data, setData] = useState([
    { price: Math.round(Math.random() * 100) },
  ]);

  useEffect(() => {
    const interval = setInterval(() => {
      setData((prevData) => {
        const lastPoint = prevData[prevData.length - 1];
        const newPoint = generateRandomDataPoint(lastPoint.price);
        return [...prevData, newPoint];
      });
    }, 500);

    return () => clearInterval(interval);
  }, []);

  return (
    <ResponsiveContainer
      minHeight="70%"
      minWidth="100%"
      style={{ boxShadow: "inset 0 0 0 1px white" }}
    >
      <AreaChart data={data}>
        <defs>
          <linearGradient id="gradient" x1="0" y1="0" x2="0" y2="1">
            <stop offset="5%" stopColor="#7685d1" stopOpacity={0.8} />
            <stop offset="95%" stopColor="#7685d1" stopOpacity={0} />
          </linearGradient>
        </defs>
        <YAxis
          domain={[0, Math.ceil(Math.max(...data.map((e) => e.price))) + 10]}
          ticks={[0, Math.ceil(Math.max(...data.map((e) => e.price))) + 10]}
          // width={10}
          mirror
          tick={{ fill: "white", fontFamily: "Cascadia Code" }}
        />
        <Area
          type="linear"
          dataKey="price"
          stroke="#ffffff"
          strokeWidth={1}
          dot={false}
          animationDuration={0}
          fillOpacity={1}
          fill="url(#gradient)"
        />
      </AreaChart>
    </ResponsiveContainer>
  );
};

export default Chart;
