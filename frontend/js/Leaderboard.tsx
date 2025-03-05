import React from "react";

const Leaderboard = () => {
  const leaderboard = [
    { rank: 1, name: "Ronner Cose", value: 10000 },
    { rank: 2, name: "Giv Shovil", value: 90000 },
    { rank: 3, name: "Rarjun Astogi", value: 80000 },
    { rank: 4, name: "Pobby Balmer", value: 70000 },
  ];

  return (
    <div className="grid-item table-container">
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
          {leaderboard.map((user) => (
            <tr key={user.rank}>
              <td>{user.rank}</td>
              <td>{user.name}</td>
              <td>${user.value}</td>
            </tr>
          ))}
        </tbody>
      </table>
    </div>
  );
};

export default Leaderboard;
