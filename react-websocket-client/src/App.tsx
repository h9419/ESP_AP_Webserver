import './App.css';
import { IMessageEvent, w3cwebsocket } from 'websocket';
import React, { useState, useEffect, useRef, useCallback } from 'react';
import Button from '@mui/material/Button';

function App() {
  const websocket = useRef<w3cwebsocket>(new w3cwebsocket('ws://192.168.2.1/ws'));
  const [LED, setLED] = useState(false);

  useEffect(() => {
    websocket.current.onmessage = (message: IMessageEvent) => {
      const dataFromServer = JSON.parse(message.data.toString());
      if (dataFromServer.type === "message") {
        setLED(dataFromServer.LED)
      }
    };
    return () => websocket.current?.close();
  }, [])

  function sendUpdate({ led = LED }) {
    websocket.current?.send(
      JSON.stringify({
        type: "message",
        LED: led,
      })
    );
  }

  const toggleLed = useCallback(() => sendUpdate({ led: !LED }), [LED]);

  return (
    <div className='body'>
      <div className="centered">
        <div className='wrapper'>
          <h1>Currently {LED ? "ON" : "OFF"}</h1>
          <br />
          <Button variant="contained" onClick={toggleLed}>{LED ? "Turn Off" : "Turn On"}</Button>
        </div>
      </div>
    </div>
  );
}

export default App;
