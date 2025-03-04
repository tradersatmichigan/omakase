import React, { useState } from "react";
import { asset_t, side_t } from "./types";

const OrderForm = ({ asset }: { asset: asset_t }) => {
  const [price, setPrice] = useState<number>();
  const [volume, setVolume] = useState<number>();
  const [side, setSide] = useState<side_t>(side_t.BID);

  const handleSubmit = (e: React.FormEvent) => {
    e.preventDefault();
    console.log("Submitted values:", { asset, price, volume, side });
  };

  return (
    <form onSubmit={handleSubmit}>
      <div>
        <label>
          <input
            type="radio"
            value="bid"
            name="side"
            checked={side === side_t.BID}
            onChange={(e) =>
              setSide(e.target.value === "bid" ? side_t.BID : side_t.ASK)
            }
          />
          Bid
        </label>
        <label>
          <input
            type="radio"
            value="ask"
            name="side"
            checked={side === side_t.ASK}
            onChange={(e) =>
              setSide(e.target.value === "bid" ? side_t.BID : side_t.ASK)
            }
          />
          Ask
        </label>
      </div>
      <div>
        <span style={{ marginTop: "3px" }}>$</span>
        <input
          type="number"
          placeholder="Price"
          name="price"
          value={price === undefined ? "" : String(price)}
          onChange={(e) => setPrice(Number(e.target.value))}
        />
      </div>
      <div>
        <span style={{ marginTop: "4px" }}>×</span>
        <input
          type="number"
          placeholder="Volume"
          name="volume"
          value={volume === undefined ? "" : String(volume)}
          onChange={(e) => setVolume(Number(e.target.value))}
        />
        <button type="submit" className="order">
          ✓
        </button>
      </div>
    </form>
  );
};

export default OrderForm;
