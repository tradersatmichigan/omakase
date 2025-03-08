export type user_t = number;
export type order_id_t = number;
export type price_t = number;
export type volume_t = number;

export enum asset_t {
  CALIFORNIA_ROLL = 0,
  SPICY_TUNA_ROLL = 1,
  SALMON_NIGIRI = 2,
  EEL_NIGIRI = 3,
  WHITE_TUNA_SASHIMI = 4,
  UNI_NIGIRI = 5,
}

export const ASSETS = [
  asset_t.CALIFORNIA_ROLL,
  asset_t.SPICY_TUNA_ROLL,
  asset_t.SALMON_NIGIRI,
  asset_t.EEL_NIGIRI,
  asset_t.WHITE_TUNA_SASHIMI,
  asset_t.UNI_NIGIRI,
];

export const SYMBOLS = ["CALI", "SPCYT", "SALM", "EEL", "WTUNA", "UNI"];

export const PLATE_1 = [
  asset_t.CALIFORNIA_ROLL,
  asset_t.SALMON_NIGIRI,
  asset_t.WHITE_TUNA_SASHIMI,
  asset_t.UNI_NIGIRI,
];

export const PLATE_2 = [
  asset_t.SPICY_TUNA_ROLL,
  asset_t.EEL_NIGIRI,
  asset_t.WHITE_TUNA_SASHIMI,
  asset_t.UNI_NIGIRI,
];

export const ASSET_VALUES = [100, 100, 200, 200, 300, 400];
export const PLATE_1_VALUE = 1500;
export const PLATE_2_VALUE = 2000;

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
  amount_held: [number, number, number, number, number, number];
  selling_power: [number, number, number, number, number, number];
};

export type register_response_t = {
  error: string | undefined;
  id: user_t | undefined;
  secret: number | undefined;
};

export type state_response_t = {
  error: string | undefined;
  user_entry: user_entry_t | undefined;
  bids:
  | [order_t[], order_t[], order_t[], order_t[], order_t[], order_t[]]
  | undefined;
  asks:
  | [order_t[], order_t[], order_t[], order_t[], order_t[], order_t[]]
  | undefined;
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
