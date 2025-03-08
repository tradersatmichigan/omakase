import React, { useContext, useState } from "react";
import { register_response_t } from "./types";
import { UserInfoContext } from "./Game";

const SignIn = () => {
  const { setUserInfo } = useContext(UserInfoContext);
  const [username, setUsername] = useState<string>("");

  const register = (e: React.FormEvent<HTMLFormElement>) => {
    e.preventDefault();
    if (
      username === undefined ||
      username === "" ||
      setUserInfo === undefined
    ) {
      return;
    }
    fetch(`/api/register?username=${username}`, {
      method: "POST",
    })
      .then((response) => {
        if (!response.ok) throw Error(response.statusText);
        return response.json() as Promise<register_response_t>;
      })
      .then((data) => {
        if (data.error !== undefined) {
          alert(data.error);
        }
        localStorage.setItem("username", username);
        localStorage.setItem("secret", String(data.secret));
        setUserInfo({ user_id: Number(data.id), username: username });
      })
      .catch((err) => console.error(err));
  };

  return (
    <div className="register-container">
      <form onSubmit={register}>
        <h3>Welcome to Omakase</h3>
        <div>
          <span style={{ marginTop: "3px" }}>Enter username: </span>
          <input
            type="text"
            placeholder="username"
            name="username"
            value={username}
            onChange={(e) => setUsername(e.target.value)}
            autoComplete="off"
          />
        </div>
        <div style={{ justifyContent: "center" }}>
          <button className="register" type="submit">
            Enter
          </button>
        </div>
      </form>
    </div>
  );
};

export default SignIn;
