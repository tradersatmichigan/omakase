import React from "react";
import { asset_t, side_t, symbols, values } from "./types";
import OrderForm from "./OrderForm";
import Chart from "./Chart";
import OrderTable from "./OrderTable";

const AssetInterface = ({ asset }: { asset: asset_t }) => {
  return (
    <div className="grid-item">
      <h3>
        ${values[asset]} - {symbols[asset]}
      </h3>
      <div className="asset-container">
        <div className="asset-item">
          <Chart />
          <OrderForm asset={asset} />
        </div>
        <OrderTable asset={asset} side={side_t.BID} />
        <OrderTable asset={asset} side={side_t.ASK} />
      </div>
    </div>
  );
};

export default AssetInterface;
