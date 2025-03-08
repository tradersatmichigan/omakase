import React, { useContext } from "react";
import { asset_t, side_t } from "./types";
import { StateContext } from "./Game";

const OrderTable = ({ asset, side }: { asset: asset_t; side: side_t }) => {
  const state = useContext(StateContext);

  if (state == undefined) {
    return <div>Loading...</div>;
  }

  const orders = Object.values(state.orders).filter(
    (order) => order.side === side && order.asset === asset,
  );

  return (
    <div className="asset-item table-container">
      <h4>{side === side_t.BID ? "Bids" : "Asks"}</h4>
      <table>
        <thead>
          <tr>
            <th>Price</th>
            <th>Volume</th>
          </tr>
        </thead>
        <tbody>
          {orders.length > 0 ? (
            Object.entries(
              orders.reduce(
                (acc: { [price: number]: number }, { price, volume }) => {
                  if (!acc[price]) {
                    acc[price] = volume;
                  } else {
                    acc[price] += volume;
                  }
                  return acc;
                },
                {},
              ),
            )
              .map(([price, volume]) => ({
                price: Number(price),
                volume: Number(volume),
              }))
              .sort((lhs, rhs) =>
                side === side_t.BID
                  ? rhs.price - lhs.price
                  : lhs.price - rhs.price,
              )
              .map((order) => (
                <tr key={order.price}>
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

export default OrderTable;
