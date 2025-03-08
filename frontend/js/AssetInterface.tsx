import React, { useContext } from "react";
import { asset_t, side_t, SYMBOLS, ASSET_VALUES } from "./types";
import OrderForm from "./OrderForm";
import Chart from "./Chart";
import OrderTable from "./OrderTable";
import { StateContext } from "./Game";

const AssetInterface = ({ asset }: { asset: asset_t }) => {
  const state = useContext(StateContext);

  return state !== undefined ? (
    <div className="grid-item">
      <h3>
        ${ASSET_VALUES[asset]} - {SYMBOLS[asset]}
      </h3>
      <div className="asset-container">
        <div className="asset-item">
          <Chart asset={asset} />
          <OrderForm asset={asset} />
        </div>
        <OrderTable asset={asset} side={side_t.BID} />
        <OrderTable asset={asset} side={side_t.ASK} />
      </div>
    </div>
  ) : (
    <div>Loading...</div>
  );
};

export default AssetInterface;
