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

const symbols = ["CALI", "SPCYT", "SALM", "EEL", "WTUNA", "UNI"];
const values = [10, 10, 20, 20, 30, 40];

export { asset_t, side_t, symbols, values };
