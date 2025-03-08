import React, { useContext } from "react";
import { YAxis, ResponsiveContainer, AreaChart, Area } from "recharts";
import { asset_t } from "./types";
import { TradesContext } from "./Game";

const Chart = ({ asset }: { asset: asset_t }) => {
  const trades = useContext(TradesContext).filter(
    (trade) => trade.asset === asset,
  );

  return (
    <ResponsiveContainer
      minHeight="70%"
      minWidth="100%"
      style={{ boxShadow: "inset 0 0 0 1px white" }}
    >
      <AreaChart data={trades}>
        <defs>
          <linearGradient id="gradient" x1="0" y1="0" x2="0" y2="1">
            <stop offset="5%" stopColor="#7685d1" stopOpacity={0.8} />
            <stop offset="95%" stopColor="#7685d1" stopOpacity={0} />
          </linearGradient>
        </defs>
        <YAxis
          domain={[
            0,
            Math.ceil(Math.max(...trades.map((trade) => trade.price))) + 10,
          ]}
          ticks={[
            0,
            Math.ceil(Math.max(...trades.map((trade) => trade.price))) + 10,
          ]}
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
