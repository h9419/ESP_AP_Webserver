import './App.css';
import { IMessageEvent, w3cwebsocket } from 'websocket';
import { useState, useEffect, useRef, useCallback } from 'react';
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
    <div className="centered">
      <div className="wrapper">
        <h1>
          <span>Currently </span>
          <span>{LED ? 'ON' : 'OFF'}</span>
        </h1>
        <Button variant="contained" onClick={toggleLed}>
          {LED ? 'Turn Off' : 'Turn On'}
        </Button>
      </div>
    </div>
  );
}

export default App;
