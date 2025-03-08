import React, { useContext } from "react";
import { side_t } from "./types";
import PositionTable from "./PositionTable";
import { StateContext } from "./Game";

const PositionInterface = () => {
  const state = useContext(StateContext);

  return state !== undefined ? (
    <div className="grid-item">
      <h3>Positions</h3>
      <div className="position-container">
        <PositionTable side={side_t.BID} />
        <PositionTable side={side_t.ASK} />
      </div>
    </div>
  ) : (
    <div>Loading...</div>
  );
};

export default PositionInterface;
