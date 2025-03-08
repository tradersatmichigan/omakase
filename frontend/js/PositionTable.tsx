import React, { useContext } from "react";
import {
  message_t,
  order_t,
  outgoing_message_t,
  side_t,
  SYMBOLS,
} from "./types";
import { SocketContext, StateContext, UserInfoContext } from "./Game";

const PositionTable = ({ side }: { side: side_t }) => {
  const state = useContext(StateContext);
  const { userInfo } = useContext(UserInfoContext);
  const ws = useContext(SocketContext);

  if (state === undefined || userInfo === undefined) {
    return <div>Loading...</div>;
  }

  const orders = Object.values(state.orders)
    .filter((order) => order.side === side && order.user === userInfo.user_id)
    .sort((lhs, rhs) => rhs.id - lhs.id);

  const send_cancel_message = (order: order_t) => {
    const message: outgoing_message_t = {
      type: message_t.CANCEL,
      order: undefined,
      cancel: {
        order_id: order.id,
        asset: order.asset,
        side: order.side,
      },
    };
    ws?.current?.send(JSON.stringify(message));
  };

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
                    onClick={(e) => {
                      e.preventDefault();
                      send_cancel_message(order);
                    }}
                    className="cancel"
                  >
                    ×
                  </button>
                </td>
                <td>{SYMBOLS[order.asset]}</td>
                <td>${order.price}</td>
                <td>{order.volume}</td>
              </tr>
            ))
          ) : (
            <tr>
              <td colSpan={4}>No {side === side_t.BID ? "bids" : "asks"}</td>
            </tr>
          )}
        </tbody>
      </table>
    </div>
  );
};

export default PositionTable;
