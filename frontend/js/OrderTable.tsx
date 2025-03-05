import React from "react";
import { asset_t, side_t } from "./types";

const OrderTable = ({ asset, side }: { asset: asset_t; side: side_t }) => {
  const orders = [
    { price: 13, volume: 4 },
    { price: 10, volume: 1 },
    { price: 11, volume: 2 },
    { price: 12, volume: 3 },
    { price: 13, volume: 4 },
    { price: 14, volume: 4 },
    { price: 15, volume: 4 },
    { price: 16, volume: 4 },
    { price: 17, volume: 4 },
    { price: 18, volume: 4 },
    { price: 19, volume: 4 },
    { price: 20, volume: 4 },
    { price: 21, volume: 4 },
    { price: 22, volume: 4 },
    { price: 23, volume: 4 },
  ];

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
