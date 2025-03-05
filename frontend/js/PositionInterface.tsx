import React from "react";
import { side_t } from "./types";
import PositionTable from "./PositionTable";

const PositionInterface = () => {
  return (
    <div className="grid-item">
      <h3>Positions</h3>
      <div className="position-container">
        <PositionTable side={side_t.BID} />
        <PositionTable side={side_t.ASK} />
      </div>
    </div>
  );
};

export default PositionInterface;
