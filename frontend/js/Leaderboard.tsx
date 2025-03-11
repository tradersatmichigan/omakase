import React, { useContext, useEffect, useState } from "react";
import { leaderboard_entry_t } from "./types";
import { UserInfoContext } from "./Game";

const Leaderboard = () => {
  const { userInfo } = useContext(UserInfoContext);
  const [leaderbaord, setLeaderboard] = useState<leaderboard_entry_t[]>();

  const fetch_leaderboard = () => {
    fetch("/api/leaderboard")
      .then((response) => {
        if (!response.ok) throw Error(response.statusText);
        return response.json() as Promise<leaderboard_entry_t[]>;
      })
      .then((data) => {
        setLeaderboard(data);
      })
      .catch((error) => {
        console.error(error);
        fetch_leaderboard();
      });
  };

  useEffect(() => {
    fetch_leaderboard();
    const interval_id = setInterval(fetch_leaderboard, 10000);
    return () => clearInterval(interval_id);
  }, []);

  return leaderbaord !== undefined ? (
    <div className="grid-item table-container leaderboard-container">
      <h3>Leaderboard</h3>
      <table>
        <colgroup>
          <col span={1} width={"10%"} />
          <col span={1} width={"45%"} />
          <col span={1} width={"45%"} />
        </colgroup>
        <thead>
          <tr>
            <th>Rank</th>
            <th>Name</th>
            <th>Portfolio Value</th>
          </tr>
        </thead>
        <tbody>
          {leaderbaord.map((user) => (
            <tr
              key={user.id}
              className={user.id === userInfo?.user_id ? "user-row" : undefined}
            >
              <td>{user.rank}</td>
              <td>{user.username}</td>
              <td>${user.value}</td>
            </tr>
          ))}
        </tbody>
      </table>
    </div>
  ) : (
    <div>Loading...</div>
  );
};

export default Leaderboard;
