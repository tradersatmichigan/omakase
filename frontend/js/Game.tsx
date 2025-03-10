import React, { createContext, useEffect, useRef, useState } from "react";

import {
  ASSETS,
  incoming_message_t,
  message_t,
  order_id_t,
  order_t,
  register_response_t,
  side_t,
  state_response_t,
  state_t,
  trade_t,
  user_info_t,
} from "./types";
import AssetInterface from "./AssetInterface";
import Portfolio from "./Portfolio";
import PositionInterface from "./PositionInterface";
import Leaderboard from "./Leaderboard";
import SignIn from "./SignIn";

export const StateContext = createContext<state_t | undefined>(undefined);

export const UserInfoContext = createContext<{
  userInfo: user_info_t | undefined;
  setUserInfo:
  | React.Dispatch<React.SetStateAction<user_info_t | undefined>>
  | undefined;
}>({
  userInfo: undefined,
  setUserInfo: undefined,
});

export const SocketContext = createContext<
  React.RefObject<WebSocket | undefined> | undefined
>(undefined);
export const TradesContext = createContext<trade_t[]>([]);

const Game = () => {
  const [userInfo, setUserInfo] = useState<user_info_t>();
  const [state, setState] = useState<state_t>();
  const [trades, setTrades] = useState<trade_t[]>([]);

  useEffect(() => {
    const username = localStorage.getItem("username");
    const secret = localStorage.getItem("secret");
    if (username === null || secret === null) {
      return;
    }

    fetch(
      `api/sign_in?username=${username}&secret=${secret}`,
      {
        method: "POST",
      },
    )
      .then((response) => {
        if (!response.ok) throw Error(response.statusText);
        return response.json() as Promise<register_response_t>;
      })
      .then((data) => {
        if (data.error !== undefined) {
          localStorage.removeItem("username");
          localStorage.removeItem("secret");
          throw Error(data.error);
        }
        setUserInfo({ user_id: Number(data.id), username: username });
      })
      .catch((err) => console.error(err));
  }, []);

  useEffect(() => {
    if (userInfo === undefined) {
      return;
    }

    const fetch_state = () => {
      fetch(`/api/state?user=${userInfo.user_id}`)
        .then((response) => {
          if (!response.ok) throw Error(response.statusText);
          return response.json() as Promise<state_response_t>;
        })
        .then((data) => {
          if (data.error !== undefined) throw Error(data.error);
          if (
            data.user_entry === undefined ||
            data.bids === undefined ||
            data.asks === undefined
          ) {
            throw Error("undefined response");
          }
          setState({
            user_entry: data.user_entry,
            orders: [...data.bids.flat(), ...data.asks.flat()].reduce(
              (acc: { [id: order_id_t]: order_t }, order: order_t) => {
                acc[order.id] = order;
                return acc;
              },
              {},
            ),
          });
        })
        .catch((error) => {
          console.error(error);
          fetch_state();
        });
    };
    fetch_state();
  }, [userInfo]);

  const settle_trades = (state: state_t, trades: trade_t[]) => {
    for (const trade of trades) {
      const matched_order = state.orders[trade.order_id];
      if (matched_order === undefined) {
        console.error(`Order with id ${trade.order_id} not found`);
      }
      delete state.orders[trade.order_id];
      const taker =
        matched_order.side === side_t.BID ? trade.seller : trade.buyer;
      const maker =
        matched_order.side === side_t.ASK ? trade.seller : trade.buyer;
      const trade_cost = trade.price * trade.volume;
      if (taker === userInfo?.user_id) {
        switch (matched_order.side) {
          case side_t.BID:
            state.user_entry.cash_held += trade_cost;
            state.user_entry.buying_power += trade_cost;

            state.user_entry.amount_held[trade.asset] -= trade.volume;
            state.user_entry.selling_power[trade.asset] -= trade.volume;
            break;
          case side_t.ASK:
            state.user_entry.cash_held -= trade_cost;
            state.user_entry.buying_power -= trade_cost;

            state.user_entry.amount_held[trade.asset] += trade.volume;
            state.user_entry.selling_power[trade.asset] += trade.volume;
            break;
        }
      }
      if (maker === userInfo?.user_id) {
        switch (matched_order.side) {
          case side_t.BID:
            state.user_entry.cash_held -= trade_cost;
            // prevState.user_entry.buying_power -= trade_cost;

            state.user_entry.amount_held[trade.asset] += trade.volume;
            state.user_entry.selling_power[trade.asset] += trade.volume;
            break;
          case side_t.ASK:
            state.user_entry.cash_held += trade_cost;
            state.user_entry.buying_power += trade_cost;

            state.user_entry.amount_held[trade.asset] -= trade.volume;
            // prevState.user_entry.selling_power[trade.asset] -= trade.volume;
            break;
        }
      }
    }
    if (trades.length > 0) {
      setTrades((prevTrades) => {
        if (prevTrades.find((trade) => trade === trades[0]) === undefined) {
          return [...prevTrades, ...trades];
        }
        return prevTrades;
      });
    }
  };

  const handle_message = (message: incoming_message_t) => {
    if (userInfo === undefined || state === undefined) {
      console.error(`userInfo: ${userInfo}, state: ${state}`);
      return;
    }
    if (message.error !== undefined) {
      alert(message.error);
      return;
    }
    if (message.type === undefined) {
      console.error("message.type is undefined");
      return;
    }
    switch (message.type) {
      case message_t.ORDER:
        if (message.order_result === undefined) {
          console.error("message.order_result is undefined");
          return;
        }
        if (message.order_result.error !== undefined) {
          alert(message.order_result.error);
          return;
        }
        setState((prevState) => {
          // Hack to ensure we aren't modifying prevState directly
          const newState = JSON.parse(JSON.stringify(prevState)) as state_t;

          if (message.order_result?.trades !== undefined) {
            settle_trades(newState, message.order_result.trades);
          }

          if (message.order_result?.unmatched !== undefined) {
            const order = message.order_result.unmatched;
            if (order.user === userInfo.user_id) {
              switch (order.side) {
                case side_t.BID:
                  newState.user_entry.buying_power -=
                    order.price * order.volume;
                  break;
                case side_t.ASK:
                  newState.user_entry.selling_power[order.asset] -=
                    order.volume;
                  break;
              }
            }
            newState.orders[order.id] = order;
          }

          return newState;
        });
        break;
      case message_t.CANCEL:
        if (message.cancelled === undefined) {
          return;
        }
        setState((prevState) => {
          // Hack to ensure we aren't modifying prevState directly
          const newState = JSON.parse(JSON.stringify(prevState)) as state_t;

          if (message.cancelled?.user === userInfo.user_id) {
            switch (message.cancelled.side) {
              case side_t.BID:
                newState.user_entry.buying_power +=
                  message.cancelled?.price * message.cancelled?.volume;
                break;
              case side_t.ASK:
                newState.user_entry.selling_power[message.cancelled.asset] +=
                  message.cancelled.volume;
                break;
            }
          }

          if (message.cancelled !== undefined) {
            delete newState.orders[message.cancelled.id];
          }

          return newState;
        });
        break;
    }
  };

  const ws = useRef<WebSocket>(undefined);

  useEffect(() => {
    if (userInfo === undefined || state === undefined) {
      return;
    }

    const connect = () => {
      if (ws.current !== undefined) {
        return;
      }
      const wsUrl = `ws://${window.location.host}/ws`;
      ws.current = new WebSocket(wsUrl);
      ws.current.onclose = (e) => {
        console.info("WebSocket closed. Reconnecting...");
        console.info("ws.current: ", ws.current);
        console.info("event: ", e);
        if (ws.current?.readyState === WebSocket.OPEN) {
          ws.current.close();
        }
        delete ws.current;
        ws.current = undefined;
        connect();
      };
      ws.current.onmessage = (e) => {
        handle_message(JSON.parse(e.data) as incoming_message_t);
      };
    };

    connect();

    return () => {
      if (ws.current?.readyState !== WebSocket.CONNECTING) {
        ws.current?.close();
      }
    };
  }, [userInfo !== undefined && state !== undefined]);

  return userInfo !== undefined ? (
    <SocketContext.Provider value={ws}>
      <UserInfoContext.Provider value={{ userInfo, setUserInfo }}>
        <StateContext.Provider value={state}>
          <TradesContext.Provider value={trades}>
            <div className="grid-container">
              {ASSETS.map((asset) => (
                <AssetInterface asset={asset} key={asset} />
              ))}
              <Portfolio />
              <PositionInterface />
              <Leaderboard />
            </div>
          </TradesContext.Provider>
        </StateContext.Provider>
      </UserInfoContext.Provider>
    </SocketContext.Provider>
  ) : (
    <UserInfoContext.Provider value={{ userInfo, setUserInfo }}>
      <SignIn />
    </UserInfoContext.Provider>
  );
};

export default Game;
