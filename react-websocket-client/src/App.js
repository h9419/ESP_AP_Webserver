// import logo from './logo.svg';
import './App.css';
import { w3cwebsocket as W3CWebSocket } from "websocket";
import React, { useState, useEffect, useRef } from 'react';
import Button from '@mui/material/Button';
// https://github.com/kokanek/web-socket-chat/blob/master/webSocketClient/src/index.js

function App() {
  const websocket = useRef(null);
  const [LED, setLED] = useState(false);

  useEffect(() => {
    websocket.current = new W3CWebSocket('ws://192.168.2.1/ws');
    websocket.current.onmessage = (message) => {
      const dataFromServer = JSON.parse(message.data);
      // console.log('got reply! ', dataFromServer);
      if (dataFromServer.type === "message") {
        setLED(dataFromServer.LED)
      }
    };
    return () => websocket.current.close();
  }, [])

  function sendUpdate({ led = LED }) {
    websocket.current.send(
      JSON.stringify({
        type: "message",
        LED: led,
      })
    );
  }

  const LEDon = () => {
    sendUpdate({ led: true });
  }
  const LEDoff = () => {
    sendUpdate({ led: false });
  }


  return (
    <div className='body'>
      <div className="centered">
        <div className='wrapper'>
          <h1>Currently {LED ? "ON" : "OFF"}</h1>
          <br />
          <Button variant="contained" onClick={LED ? LEDoff : LEDon}>{LED ? "Turn Off" : "Turn On"}</Button>
        </div>
      </div>
    </div>
    // <div className="App">
    //   <header className="App-header">
    //     <img src={logo} className="App-logo" alt="logo" />
    //     <p>
    //       Edited <code>src/App.js</code> and re-upload.
    //     </p>
    //     <a
    //       className="App-link"
    //       href="https://reactjs.org"
    //       target="_blank"
    //       rel="noopener noreferrer"
    //     >
    //       Learn React
    //     </a>
    //   </header>
    //   <body>
    //     <button onClick={onButtonClicked}>
    //       Activate Lasers
    //     </button>
    //   </body>
    // </div>
  );
}

export default App;
