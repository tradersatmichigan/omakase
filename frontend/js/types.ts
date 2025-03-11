export type user_t = number;
export type order_id_t = number;
export type price_t = number;
export type volume_t = number;

export enum asset_t {
  CALIFORNIA_ROLL = 0,
  SALMON_NIGIRI = 1,
  EEL_NIGIRI = 2,
  UNI_NIGIRI = 3,
}

export const ASSETS = [
  asset_t.CALIFORNIA_ROLL,
  asset_t.SALMON_NIGIRI,
  asset_t.EEL_NIGIRI,
  asset_t.UNI_NIGIRI,
];

export const SYMBOLS = ["CALI", "SALM", "EEL", "UNI"];

export const ASSET_VALUES = [10, 20, 30, 40];
export const PLATE_BONUS = 100;

export const enum side_t {
  BID = 0,
  ASK = 1,
}

export type trade_t = {
  order_id: order_id_t;
  price: price_t;
  volume: volume_t;
  buyer: user_t;
  seller: user_t;
  asset: asset_t;
};

export type order_t = {
  id: order_id_t;
  price: price_t;
  volume: volume_t;
  user: user_t;
  asset: asset_t;
  side: side_t;
};

export type cancel_t = {
  asset: asset_t;
  side: side_t;
  order_id: order_id_t;
};

export type order_result_t = {
  error: string | undefined;
  trades: trade_t[];
  unmatched: order_t | undefined;
};

export type user_entry_t = {
  cash_held: number;
  buying_power: number;
  amount_held: [number, number, number, number];
  selling_power: [number, number, number, number];
};

export type register_response_t = {
  error: string | undefined;
  id: user_t | undefined;
  secret: number | undefined;
};

export type state_response_t = {
  error: string | undefined;
  user_entry: user_entry_t | undefined;
  bids: [order_t[], order_t[], order_t[], order_t[]] | undefined;
  asks: [order_t[], order_t[], order_t[], order_t[]] | undefined;
};

export type user_info_t = {
  user_id: user_t;
  username: string;
};

export type state_t = {
  user_entry: user_entry_t;
  orders: { [id: order_id_t]: order_t };
};

export enum message_t {
  ORDER = 0,
  CANCEL = 1,
}

export type outgoing_message_t = {
  type: message_t;
  order: order_t | undefined;
  cancel: cancel_t | undefined;
};

export type incoming_message_t = {
  error: string | undefined;
  type: message_t | undefined;
  order_result: order_result_t | undefined;
  cancelled: order_t | undefined;
};

export type leaderboard_entry_t = {
  id: user_t;
  rank: number;
  username: string;
  value: price_t;
};
