import React, { useContext } from "react";
import { ASSETS, PLATE_1, PLATE_2, SYMBOLS, user_entry_t } from "./types";
import { StateContext } from "./Game";

const Portfolio = () => {
  const state = useContext(StateContext);

  const get_plate_counts = () => {
    if (state?.user_entry === undefined) {
      return [0, 0];
    }
    const user_entry = JSON.parse(
      JSON.stringify(state.user_entry),
    ) as user_entry_t;
    const plate_2_count = Math.min(
      ...PLATE_2.map((asset) => user_entry.amount_held[asset]),
    );
    PLATE_2.forEach(
      (asset) => (user_entry.amount_held[asset] -= plate_2_count),
    );
    const plate_1_count = Math.min(
      ...PLATE_1.map((asset) => user_entry.amount_held[asset]),
    );
    return [plate_1_count, plate_2_count];
  };

  const [plate_1_count, plate_2_count] = get_plate_counts();

  return state !== undefined ? (
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
            <td>${state.user_entry.cash_held}</td>
            <td>${state.user_entry.buying_power}</td>
          </tr>
          {ASSETS.map((asset) => (
            <tr key={asset}>
              <td>{SYMBOLS[asset]}</td>
              <td>{state.user_entry.amount_held[asset]}</td>
              <td>{state.user_entry.selling_power[asset]}</td>
            </tr>
          ))}
          <tr key="plate-1">
            <td>Plate 1</td>
            <td>{plate_1_count}</td>
            <td></td>
          </tr>
          <tr key="plate-2">
            <td>Plate 2</td>
            <td>{plate_2_count}</td>
            <td></td>
          </tr>
        </tbody>
      </table>
    </div>
  ) : (
    <div>Loading...</div>
  );
};

export default Portfolio;
