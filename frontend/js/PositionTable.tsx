import React from "react";
import { side_t, asset_t, symbols } from "./types";

const PositionTable = ({ side }: { side: side_t }) => {
  const orders = [
    { id: 1, asset: asset_t.CALIFORNIA_ROLL, price: 13, volume: 1 },
    { id: 2, asset: asset_t.EEL_NIGIRI, price: 13, volume: 2 },
    { id: 3, asset: asset_t.UNI_NIGIRI, price: 13, volume: 3 },
    { id: 4, asset: asset_t.CALIFORNIA_ROLL, price: 13, volume: 4 },
    { id: 5, asset: asset_t.WHITE_TUNA_SASHIMI, price: 13, volume: 5 },
    { id: 6, asset: asset_t.SALMON_NIGIRI, price: 13, volume: 6 },
    { id: 7, asset: asset_t.SALMON_NIGIRI, price: 13, volume: 6 },
    { id: 8, asset: asset_t.SALMON_NIGIRI, price: 13, volume: 6 },
    { id: 9, asset: asset_t.SALMON_NIGIRI, price: 13, volume: 6 },
  ];

  return (
    <div className="position-item table-container">
      <h4>{side === side_t.BID ? "Bids" : "Asks"}</h4>
      <table>
        <colgroup>
          <col span={1} width={"26px"} />
          <col span={1} />
          <col span={1} />
          <col span={1} />
        </colgroup>

        <thead>
          <tr>
            <th></th>
            <th>Asset</th>
            <th>Price</th>
            <th>Volume</th>
          </tr>
        </thead>
        <tbody>
          {orders.length > 0 ? (
            orders.map((order) => (
              <tr key={order.id}>
                <td>
                  <button
                    onClick={(e) => console.log("clicked")}
                    className="cancel"
                  >
                    ×
                  </button>
                </td>
                <td>{symbols[order.asset]}</td>
                <td>${order.price}</td>
                <td>{order.volume}</td>
              </tr>
            ))
          ) : (
            <tr>
              <td colSpan={2}>No {side === side_t.BID ? "bids" : "asks"}</td>
            </tr>
          )}
        </tbody>
      </table>
    </div>
  );
};

export default PositionTable;
