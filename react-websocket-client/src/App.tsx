import "./App.css";
import { IMessageEvent, w3cwebsocket } from "websocket";
import { useState, useEffect, useRef, useCallback, useMemo } from "react";
import Button from "@mui/material/Button";
import process from "process";

const development: boolean = !process.env.NODE_ENV || process.env.NODE_ENV === 'development';

export const isDev = (): boolean => {
  return development;
}

function App() {
  const websocket = useRef<w3cwebsocket | null>(null);
  const [LED, setLED] = useState(false);
  const [velocity, setVelocity] = useState(0);

  const pathname = useMemo(() => window.location.origin.split("//")[1].split(":")[0], [window.location.origin])


  useEffect(() => {
    const url = `ws://${isDev() ? '192.168.1.2' : pathname}/ws`;
    websocket.current = new w3cwebsocket(url);
    websocket.current.onmessage = (message: IMessageEvent) => {
      const dataFromServer = JSON.parse(message.data.toString());
      if (dataFromServer.type === "message") {
        setLED(dataFromServer.LED);
      }
      if (dataFromServer.type === "velocity") {
        setVelocity(parseInt(dataFromServer.velocity));
      }
    };
    return () => websocket.current?.close();
  }, [pathname]);

  const sendUpdate = useCallback(({ led }: { led: boolean }) => {
    websocket.current?.send(
      JSON.stringify({
        type: "message",
        LED: led,
      })
    );
  }, []);

  const setVelocityUpdate = useCallback(({ velocity }: { velocity: number }) => {
    websocket.current?.send(
      JSON.stringify({
        type: "velocity",
        velocity,
      })
    );
  }, []);

  const toggleLed = useCallback(() => sendUpdate({ led: !LED }), [LED, sendUpdate]);

  const toggleVelocity = useCallback((v: number) => setVelocityUpdate({ velocity: v }), [velocity, setVelocityUpdate]);

  return (
    <div className="centered">
      <div className="wrapper">
        <h1>
          <span>Currently </span>
          <span>{LED ? "ON" : "OFF"}</span>
        </h1>

        <Button variant="contained" onClick={toggleLed}>
          {LED ? "Turn Off" : "Turn On"}
        </Button>

        <h1>
          <span>Currently </span>
          <span>{velocity}</span>
        </h1>
        <div style={{
          display: "flex",
          gap: "10px",
          justifyContent: "center",
          marginTop: "20px",

        }}>
          {
            [0, 1, 2, 3].map((id) => (
              <Button
                key={id}
                variant="contained"
                color={id === velocity ? "success" : "primary"}
                onClick={() => toggleVelocity(id)}
              >
                {id}
              </Button>
            ))
          }

        </div>
      </div>
    </div>
  );
}

export default App;
