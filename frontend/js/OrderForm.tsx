import React, { useContext, useState } from "react";
import { asset_t, message_t, outgoing_message_t, side_t } from "./types";
import { SocketContext, UserInfoContext } from "./Game";

const OrderForm = ({ asset }: { asset: asset_t }) => {
  const [price, setPrice] = useState<number>();
  const [volume, setVolume] = useState<number>();
  const [side, setSide] = useState<side_t>(side_t.BID);
  const ws = useContext(SocketContext);
  const { userInfo } = useContext(UserInfoContext);

  const send_order_message = (e: React.FormEvent) => {
    e.preventDefault();
    if (
      ws?.current === undefined ||
      userInfo === undefined ||
      price === undefined ||
      volume === undefined
    ) {
      return;
    }
    const message: outgoing_message_t = {
      type: message_t.ORDER,
      order: {
        id: 0,
        price: price,
        volume: volume,
        user: userInfo.user_id,
        asset: asset,
        side: side,
      },
      cancel: undefined,
    };
    ws?.current?.send(JSON.stringify(message));
  };

  return (
    <form onSubmit={send_order_message}>
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
          min="1"
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
          min="1"
        />
        <button type="submit" className="order">
          ✓
        </button>
      </div>
    </form>
  );
};

export default OrderForm;
