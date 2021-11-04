const char home_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html
  lang="en"
  style="
    box-sizing: border-box;
    margin: 0;
    padding: 0;
    background-color: #fff;
    font-size: 16px;
    -moz-osx-font-smoothing: grayscale;
    -webkit-font-smoothing: antialiased;
    min-width: 300px;
    overflow-x: hidden;
    overflow-y: scroll;
    text-rendering: optimizeLegibility;
    -webkit-text-size-adjust: 100%;
    -moz-text-size-adjust: 100%;
    -ms-text-size-adjust: 100%;
    text-size-adjust: 100%;
  "
>
  <head style="box-sizing: inherit">
    <meta charset="UTF-8" style="box-sizing: inherit" />
    <meta
      http-equiv="X-UA-Compatible"
      content="IE=edge"
      style="box-sizing: inherit"
    />
    <meta
      name="viewport"
      content="width=device-width, initial-scale=1"
      style="box-sizing: inherit"
    />
    <title style="box-sizing: inherit">YoMino Miner</title>
    <!-- Bulma Version 0.9.0-->
  </head>

  <body
    style="
      box-sizing: inherit;
      margin: 0;
      padding: 0;
      font-family: BlinkMacSystemFont, -apple-system, 'Segoe UI', Roboto, Oxygen,
        Ubuntu, Cantarell, 'Fira Sans', 'Droid Sans', 'Helvetica Neue',
        Helvetica, Arial, sans-serif;
      color: #4a4a4a;
      font-size: 1em;
      font-weight: 400;
      line-height: 1.5;
    "
  >
    <section
      class="hero is-fullheight"
      style="
        box-sizing: inherit;
        display: flex;
        align-items: stretch;
        flex-direction: column;
        justify-content: space-between;
        min-height: 100vh;
      "
    >
      <div
        class="hero-body"
        style="
          box-sizing: inherit;
          flex-grow: 1;
          flex-shrink: 0;
          padding: 3rem 1.5rem;
          align-items: center;
          display: flex;
        "
      >
        <div
          class="container has-text-centered"
          style="
            box-sizing: inherit;
            flex-grow: 1;
            margin: 0 auto;
            position: relative;
            width: auto;
            flex-shrink: 1;
            text-align: center !important;
          "
        >
          <div
            class="columns is-8 is-variable"
            style="
              display: flex;
              box-sizing: inherit;
              margin-left: calc(-1 * var(--columnGap));
              margin-right: calc(-1 * var(--columnGap));
              margin-top: -0.75rem;
              margin-bottom: -0.75rem;
              --columngap: 2rem;
            "
          >
            <div
              class="column is-two-thirds has-text-left"
              style="
                box-sizing: inherit;
                display: block;
                flex-basis: 0;
                flex-grow: 1;
                flex-shrink: 1;
                padding: 0.75rem;
                flex: none;
                width: 66.6666%;
                padding-left: var(--columnGap);
                padding-right: var(--columnGap);
                text-align: left !important;
              "
            >
              <h1
                class="title is-1"
                style="
                  box-sizing: inherit;
                  margin: 0;
                  padding: 0;
                  font-size: 3rem;
                  font-weight: 600;
                  word-break: break-word;
                  color: #363636;
                  line-height: 1.125;
                  margin-bottom: 1.5rem;
                "
              >
                <img
                  src="data:image/png;base64,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bnRzLzEuMS8iCiAgICB4bWxuczpHSU1QPSJodHRwOi8vd3d3LmdpbXAub3JnL3htcC8iCiAgICB4bWxuczp0aWZmPSJodHRwOi8vbnMuYWRvYmUuY29tL3RpZmYvMS4wLyIKICAgIHhtbG5zOnhtcD0iaHR0cDovL25zLmFkb2JlLmNvbS94YXAvMS4wLyIKICAgeG1wTU06RG9jdW1lbnRJRD0iZ2ltcDpkb2NpZDpnaW1wOmMwOGU5NGFkLWQ2YzgtNDAyYS05NjhhLTgxMDc4NDIwZjFkNSIKICAgeG1wTU06SW5zdGFuY2VJRD0ieG1wLmlpZDo5ODA2OTFhNS1mZjE5LTQwNDYtOTM3NC00MDAzMjZhNTBhZDIiCiAgIHhtcE1NOk9yaWdpbmFsRG9jdW1lbnRJRD0ieG1wLmRpZDo5NzY4ZDA2OC1lMzk3LTQ0YWUtOWI3NS1hNzNlMTdhZDVlMTkiCiAgIGRjOkZvcm1hdD0iaW1hZ2UvcG5nIgogICBHSU1QOkFQST0iMi4wIgogICBHSU1QOlBsYXRmb3JtPSJNYWMgT1MiCiAgIEdJTVA6VGltZVN0YW1wPSIxNjM1NDkwNzY4MjA1NDYzIgogICBHSU1QOlZlcnNpb249IjIuMTAuMjQiCiAgIHRpZmY6T3JpZW50YXRpb249IjEiCiAgIHhtcDpDcmVhdG9yVG9vbD0iR0lNUCAyLjEwIj4KICAgPHhtcE1NOkhpc3Rvcnk+CiAgICA8cmRmOlNlcT4KICAgICA8cmRmOmxpCiAgICAgIHN0RXZ0OmFjdGlvbj0ic2F2ZWQiCiAgICAgIHN0RXZ0OmNoYW5nZWQ9Ii8iCiAgICAgIHN0RXZ0Omluc3RhbmNlSUQ9InhtcC5paWQ6NjdjNWQwZTgtOTcyOS00NWY2LWJkYjItYjNjYmJkMzgxNjU3IgogICAgICBzdEV2dDpzb2Z0d2FyZUFnZW50PSJHaW1wIDIuMTAgKE1hYyBPUykiCiAgICAgIHN0RXZ0OndoZW49IjIwMjEtMTAtMjlUMDg6NTk6MjgrMDI6MDAiLz4KICAgIDwvcmRmOlNlcT4KICAgPC94bXBNTTpIaXN0b3J5PgogIDwvcmRmOkRlc2NyaXB0aW9uPgogPC9yZGY6UkRGPgo8L3g6eG1wbWV0YT4KICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg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"
                  style="box-sizing: inherit; height: auto; max-width: 100%"/> YoMino Duino Miner Setup
              </h1>
              <p
                class="is-size-4"
                style="
                  box-sizing: inherit;
                  margin: 0;
                  padding: 0;
                  font-size: 1.5rem !important;
                "
              >
                Please setup your YoMino Miner to mine Duino coins
              </p>
              <br>
              <p
                class="is-size-4"
                style="
                  box-sizing: inherit;
                  margin: 0;
                  padding: 0;
                  font-size: 1.5rem !important;
                "
              >              
                You'll need a Wallet (Username) for that
                In case you don't have one, head over <a href="https://wallet.duinocoin.com/" target="_blank">here</a>
              </p>
            </div>
            <div
              class="column is-one-third has-text-left"
              style="
                box-sizing: inherit;
                display: block;
                flex-basis: 0;
                flex-grow: 1;
                flex-shrink: 1;
                padding: 0.75rem;
                flex: none;
                width: 33.3333%;
                padding-left: var(--columnGap);
                padding-right: var(--columnGap);
                text-align: left !important;
              "
            >
              <div
                class="field"
                style="box-sizing: inherit; margin-bottom: 0.75rem"
              >
                <label
                  class="label"
                  style="
                    box-sizing: inherit;
                    color: #363636;
                    display: block;
                    font-size: 1rem;
                    font-weight: 700;
                    margin-bottom: 0.5em;
                  "
                  >Wifi SSID</label
                >
                <div
                  class="control"
                  style="
                    box-sizing: border-box;
                    clear: both;
                    font-size: 1rem;
                    position: relative;
                    text-align: inherit;
                  "
                >
                  <select
                    class="select is-medium"
                    name="Wifi"
                    id="Wifi"                    
                    style="
                      box-sizing: inherit;
                      margin: 0;
                      font-family: BlinkMacSystemFont, -apple-system,
                        'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell,
                        'Fira Sans', 'Droid Sans', 'Helvetica Neue', Helvetica,
                        Arial, sans-serif;
                      -moz-appearance: none;
                      -webkit-appearance: none;
                      align-items: center;
                      border: 1px solid transparent;
                      border-radius: 4px;
                      box-shadow: inset 0 0.0625em 0.125em
                        rgba(10, 10, 10, 0.05);
                      display: inline-flex;
                      font-size: 1.25rem;
                      height: 2.5em;
                      justify-content: flex-start;
                      line-height: 1.5;
                      padding-bottom: calc(0.5em - 1px);
                      padding-left: calc(0.75em - 1px);
                      padding-right: calc(0.75em - 1px);
                      padding-top: calc(0.5em - 1px);
                      position: relative;
                      vertical-align: top;
                      background-color: #fff;
                      border-color: #dbdbdb;
                      color: #363636;
                      max-width: 100%;
                      width: 100%;
                    ">
                        $wifi_ssid$
                    </select>
                </div>
              </div>
              <div
                class="field"
                style="box-sizing: inherit; margin-bottom: 0.75rem"
              >
                <label
                  class="label"
                  style="
                    box-sizing: inherit;
                    color: #363636;
                    display: block;
                    font-size: 1rem;
                    font-weight: 700;
                    margin-bottom: 0.5em;
                  "
                  >Wifi password</label
                >
                <div
                  class="control"
                  style="
                    box-sizing: border-box;
                    clear: both;
                    font-size: 1rem;
                    position: relative;
                    text-align: inherit;
                  "
                >
                  <input
                    class="input is-medium"
                    type="password"
                    placeholder="Wifi Password"
                    name="Password"
                    id="Password"
                    value="$wifi_password$"
                    style="
                      box-sizing: inherit;
                      margin: 0;
                      font-family: BlinkMacSystemFont, -apple-system,
                        'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell,
                        'Fira Sans', 'Droid Sans', 'Helvetica Neue', Helvetica,
                        Arial, sans-serif;
                      -moz-appearance: none;
                      -webkit-appearance: none;
                      align-items: center;
                      border: 1px solid transparent;
                      border-radius: 4px;
                      box-shadow: inset 0 0.0625em 0.125em
                        rgba(10, 10, 10, 0.05);
                      display: inline-flex;
                      font-size: 1.25rem;
                      height: 2.5em;
                      justify-content: flex-start;
                      line-height: 1.5;
                      padding-bottom: calc(0.5em - 1px);
                      padding-left: calc(0.75em - 1px);
                      padding-right: calc(0.75em - 1px);
                      padding-top: calc(0.5em - 1px);
                      position: relative;
                      vertical-align: top;
                      background-color: #fff;
                      border-color: #dbdbdb;
                      color: #363636;
                      max-width: 100%;
                      width: 100%;
                    "
                  />
                </div>
              </div>
              <div
                class="field"
                style="box-sizing: inherit; margin-bottom: 0.75rem"
              >
                <label
                  class="label"
                  style="
                    box-sizing: inherit;
                    color: #363636;
                    display: block;
                    font-size: 1rem;
                    font-weight: 700;
                    margin-bottom: 0.5em;
                  "
                  >Duino Username</label
                >
                <div
                  class="control"
                  style="
                    box-sizing: border-box;
                    clear: both;
                    font-size: 1rem;
                    position: relative;
                    text-align: inherit;
                  "
                >
                  <input
                    class="input is-medium"
                    type="input"
                    placeholder="Username"
                    name="Username"
                    id="Username"
                    value="$username$"                    
                    style="
                      box-sizing: inherit;
                      margin: 0;
                      font-family: BlinkMacSystemFont, -apple-system,
                        'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell,
                        'Fira Sans', 'Droid Sans', 'Helvetica Neue', Helvetica,
                        Arial, sans-serif;
                      -moz-appearance: none;
                      -webkit-appearance: none;
                      align-items: center;
                      border: 1px solid transparent;
                      border-radius: 4px;
                      box-shadow: inset 0 0.0625em 0.125em
                        rgba(10, 10, 10, 0.05);
                      display: inline-flex;
                      font-size: 1.25rem;
                      height: 2.5em;
                      justify-content: flex-start;
                      line-height: 1.5;
                      padding-bottom: calc(0.5em - 1px);
                      padding-left: calc(0.75em - 1px);
                      padding-right: calc(0.75em - 1px);
                      padding-top: calc(0.5em - 1px);
                      position: relative;
                      vertical-align: top;
                      background-color: #fff;
                      border-color: #dbdbdb;
                      color: #363636;
                      max-width: 100%;
                      width: 100%;
                    "
                  />
                </div>
              </div>
              <div
                class="field"
                style="box-sizing: inherit; margin-bottom: 0.75rem"
              >
                <label
                  class="label"
                  style="
                    box-sizing: inherit;
                    color: #363636;
                    display: block;
                    font-size: 1rem;
                    font-weight: 700;
                    margin-bottom: 0.5em;
                  "
                  >Miner name</label
                >
                <div
                  class="control"
                  style="
                    box-sizing: border-box;
                    clear: both;
                    font-size: 1rem;
                    position: relative;
                    text-align: inherit;
                  "
                >
                  <input
                    class="input is-medium"
                    type="input"
                    placeholder="Miner name"
                    name="myRig"
                    id="myRig"
                    value="$rig_identifier$"                      
                    style="
                      box-sizing: inherit;
                      margin: 0;
                      font-family: BlinkMacSystemFont, -apple-system,
                        'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell,
                        'Fira Sans', 'Droid Sans', 'Helvetica Neue', Helvetica,
                        Arial, sans-serif;
                      -moz-appearance: none;
                      -webkit-appearance: none;
                      align-items: center;
                      border: 1px solid transparent;
                      border-radius: 4px;
                      box-shadow: inset 0 0.0625em 0.125em
                        rgba(10, 10, 10, 0.05);
                      display: inline-flex;
                      font-size: 1.25rem;
                      height: 2.5em;
                      justify-content: flex-start;
                      line-height: 1.5;
                      padding-bottom: calc(0.5em - 1px);
                      padding-left: calc(0.75em - 1px);
                      padding-right: calc(0.75em - 1px);
                      padding-top: calc(0.5em - 1px);
                      position: relative;
                      vertical-align: top;
                      background-color: #fff;
                      border-color: #dbdbdb;
                      color: #363636;
                      max-width: 100%;
                      width: 100%;
                    "
                  />
                </div>
              </div>
              <div
                class="field"
                style="box-sizing: inherit; margin-bottom: 0.75rem"
              >
                <label
                  class="label"
                  style="
                    box-sizing: inherit;
                    color: #363636;
                    display: block;
                    font-size: 1rem;
                    font-weight: 700;
                    margin-bottom: 0.5em;
                  "
                  >AP Password</label
                >
                <div
                  class="control"
                  style="
                    box-sizing: border-box;
                    clear: both;
                    font-size: 1rem;
                    position: relative;
                    text-align: inherit;
                  "
                >
                  <input
                    class="input is-medium"
                    type="password"
                    placeholder="YoMino AP password"
                    name="ownPassword"
                    id="ownPassword"
                    value="$ownPassword$"
                    style="
                      box-sizing: inherit;
                      margin: 0;
                      font-family: BlinkMacSystemFont, -apple-system,
                        'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell,
                        'Fira Sans', 'Droid Sans', 'Helvetica Neue', Helvetica,
                        Arial, sans-serif;
                      -moz-appearance: none;
                      -webkit-appearance: none;
                      align-items: center;
                      border: 1px solid transparent;
                      border-radius: 4px;
                      box-shadow: inset 0 0.0625em 0.125em
                        rgba(10, 10, 10, 0.05);
                      display: inline-flex;
                      font-size: 1.25rem;
                      height: 2.5em;
                      justify-content: flex-start;
                      line-height: 1.5;
                      padding-bottom: calc(0.5em - 1px);
                      padding-left: calc(0.75em - 1px);
                      padding-right: calc(0.75em - 1px);
                      padding-top: calc(0.5em - 1px);
                      position: relative;
                      vertical-align: top;
                      background-color: #fff;
                      border-color: #dbdbdb;
                      color: #363636;
                      max-width: 100%;
                      width: 100%;
                    "
                  />
                </div>
              </div>
              <div
                class="control"
                style="
                  box-sizing: border-box;
                  clear: both;
                  font-size: 1rem;
                  position: relative;
                  text-align: inherit;
                "
              >
                <button                    
                  class="
                    button
                    is-link is-fullwidth
                    has-text-weight-medium
                    is-medium
                  "
                  onclick="submitConfig()"
                  style="
                    box-sizing: inherit;
                    margin: 0;
                    border-radius: 4px;
                    font-size: 1.25rem;
                    display: flex;
                    justify-content: center;
                    align-items: center;
                    background-color: #3273dc;
                    color: #FFFFFF;
                    width: 100%;
                    font-weight: 500;
                    border: 1px solid transparent;
                    padding: calc(.5em - 1px) 1em;
                    font-family: BlinkMacSystemFont, -apple-system, 'Segoe UI',
                      Roboto, Oxygen, Ubuntu, Cantarell, 'Fira Sans',
                      'Droid Sans', 'Helvetica Neue', Helvetica, Arial,
                      sans-serif;
                  "
                >
                  Save
                </button>
              </div>
            </div>
          </div>
        </div>
      </div>
    </section>
    <script type="text/javascript">
      function submitConfig() {
        console.log("Sending config");
        var data = new URLSearchParams();
        data.append("Wifi", document.getElementById("Wifi").value);
        data.append("Password", document.getElementById("Password").value);
        data.append("Username", document.getElementById("Username").value);
        data.append("myRig", document.getElementById("myRig").value);
        data.append("ownPassword", document.getElementById("ownPassword").value);
                 
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "get?" + data.toString(), true);      
        xhr.onreadystatechange = function()
            {
              if(xhr.readyState == 4 && xhr.status == 200) {              
                if (xhr.statusText !== "OK") {
                  alert("There was a problem saving your data. Make sure all fields are filled in properly");
                } else {
                  alert("Configuration succesfully saved. You can close this page");
              }              
            }
            }
        xhr.send(null);
        return false;
      }    
  </script>
 </body>
</html>)rawliteral";
