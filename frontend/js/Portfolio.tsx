import React, { useContext } from "react";
import { ASSET_VALUES, ASSETS, PLATE_BONUS, SYMBOLS } from "./types";
import { StateContext } from "./Game";

const Portfolio = () => {
  const state = useContext(StateContext);

  const get_plate_count = () => {
    if (state?.user_entry === undefined) {
      return 0;
    }
    return Math.min(...state.user_entry.amount_held);
  };

  const plate_count = get_plate_count();

  return state !== undefined ? (
    <div className="grid-item table-container">
      <h3>Portfolio</h3>
      <table>
        <thead>
          <tr>
            <th></th>
            <th>Amount</th>
            <th>Value</th>
            <th>Buying/selling power</th>
          </tr>
        </thead>
        <tbody>
          <tr key="cash">
            <td>Cash</td>
            <td>${state.user_entry.cash_held}</td>
            <td>${state.user_entry.cash_held}</td>
            <td>${state.user_entry.buying_power}</td>
          </tr>
          {ASSETS.map((asset) => (
            <tr key={asset}>
              <td>{SYMBOLS[asset]}</td>
              <td>{state.user_entry.amount_held[asset]}</td>
              <td>
                ${state.user_entry.amount_held[asset] * ASSET_VALUES[asset]}
              </td>
              <td>{state.user_entry.selling_power[asset]}</td>
            </tr>
          ))}
          <tr key="plate">
            <td>Plate</td>
            <td>{plate_count}</td>
            <td>${plate_count * PLATE_BONUS}</td>
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
