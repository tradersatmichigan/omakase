enum asset_t {
  CALIFORNIA_ROLL = 0,
  SPICY_TUNA_ROLL = 1,
  SALMON_NIGIRI = 2,
  EEL_NIGIRI = 3,
  WHITE_TUNA_SASHIMI = 4,
  UNI_NIGIRI = 5,
}

enum side_t {
  BID = 0,
  ASK = 1,
}

type user_entry_t = {
  cash_held: number;
  buying_power: number;
  amount_held: [number, number, number, number, number, number];
  selling_power: [number, number, number, number, number, number];
};

const symbols = ["CALI", "SPCYT", "SALM", "EEL", "WTUNA", "UNI"];
const values = [10, 10, 20, 20, 30, 40];
const assets = [
  asset_t.CALIFORNIA_ROLL,
  asset_t.SPICY_TUNA_ROLL,
  asset_t.SALMON_NIGIRI,
  asset_t.EEL_NIGIRI,
  asset_t.WHITE_TUNA_SASHIMI,
  asset_t.UNI_NIGIRI,
];

export { asset_t, side_t, user_entry_t, symbols, values, assets };
