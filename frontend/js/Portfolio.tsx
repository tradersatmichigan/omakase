import React from "react";
import { assets, symbols, user_entry_t } from "./types";

const Portfolio = () => {
  const user_entry: user_entry_t = {
    cash_held: 1000,
    buying_power: 900,
    amount_held: [1000, 200, 200, 0, 0, 10],
    selling_power: [900, 100, 100, 0, 0, 5],
  };

  return (
    <div className="grid-item table-container">
      <h3>Portfolio</h3>
      <table>
        <thead>
          <tr>
            <th></th>
            <th>Amount</th>
            <th>Buying/selling power</th>
          </tr>
        </thead>
        <tbody>
          <tr key="cash">
            <td>Cash</td>
            <td>${user_entry.cash_held}</td>
            <td>${user_entry.buying_power}</td>
          </tr>
          {assets.map((asset) => (
            <tr key={asset}>
              <td>{symbols[asset]}</td>
              <td>{user_entry.amount_held[asset]}</td>
              <td>{user_entry.selling_power[asset]}</td>
            </tr>
          ))}
          <tr key="plate-1">
            <td>Plate 1</td>
            <td>0</td>
            <td></td>
          </tr>
          <tr key="plate-2">
            <td>Plate 2</td>
            <td>0</td>
            <td></td>
          </tr>
        </tbody>
      </table>
    </div>
  );
};

export default Portfolio;
