import React from "react";

import { asset_t } from "./types";
import AssetInterface from "./AssetInterface";
import Portfolio from "./Portfolio";
import PositionInterface from "./PositionInterface";
import Leaderboard from "./Leaderboard";

const Game = () => {
  return (
    <div className="grid-container">
      <AssetInterface asset={asset_t.CALIFORNIA_ROLL} />
      <AssetInterface asset={asset_t.SPICY_TUNA_ROLL} />
      <AssetInterface asset={asset_t.SALMON_NIGIRI} />
      <AssetInterface asset={asset_t.EEL_NIGIRI} />
      <AssetInterface asset={asset_t.WHITE_TUNA_SASHIMI} />
      <AssetInterface asset={asset_t.UNI_NIGIRI} />
      <Portfolio />
      <PositionInterface />
      <Leaderboard />
    </div>
  );
};

export default Game;
