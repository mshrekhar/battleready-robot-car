const char body[] PROGMEM = R"===(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Simple Vehicle Control</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            text-align: center;
            margin: 0;
            padding: 20px;
            background-color: #f0f0f0;
        }
        .control-container {
            max-width: 400px;
            margin: 0 auto;
            background-color: white;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        }
        h1 {
            margin-top: 0;
            color: #333;
        }
        .buttons {
            display: grid;
            grid-template-columns: 1fr 1fr 1fr;
            grid-template-rows: repeat(3, 80px);
            gap: 10px;
            margin-bottom: 20px;
        }
        .btn {
            border: none;
            background-color: #4285f4;
            color: white;
            font-size: 18px;
            border-radius: 8px;
            cursor: pointer;
        }
        .btn:active {
            background-color: #3367d6;
        }
        .btn-forward {
            grid-column: 2;
            grid-row: 1;
        }
        .btn-left {
            grid-column: 1;
            grid-row: 2;
        }
        .btn-stop {
            grid-column: 2;
            grid-row: 2;
            background-color: #ea4335;
        }
        .btn-stop:active {
            background-color: #d33426;
        }
        .btn-right {
            grid-column: 3;
            grid-row: 2;
        }
        .btn-backward {
            grid-column: 2;
            grid-row: 3;
        }
        .status {
            margin-top: 20px;
            font-weight: bold;
        }
        .auto-btn {
            width: 100%;
            padding: 15px;
            margin-top: 10px;
            font-size: 18px;
            background-color: #34a853;
            color: white;
            border: none;
            border-radius: 8px;
            cursor: pointer;
            transition: background-color 0.3s;
        }
        .auto-btn.active {
            background-color: #ea4335;
        }
        .orientation-log {
            margin-top: 15px;
            padding: 10px;
            background-color: #f8f9fa;
            border-radius: 5px;
            font-family: monospace;
            min-height: 20px;
        }
        .logs-container {
            margin-top: 15px;
            padding: 10px;
            background-color: #f8f9fa;
            border-radius: 5px;
            font-family: monospace;
            height: 200px;
            overflow-y: auto;
            text-align: left;
        }
        .log-entry {
            padding: 2px 0;
            border-bottom: 1px solid #eee;
        }
        .drive-toggle {
            margin: 10px 0;
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 10px;
        }
        .toggle-switch {
            position: relative;
            display: inline-block;
            width: 60px;
            height: 34px;
        }
        .toggle-switch input {
            opacity: 0;
            width: 0;
            height: 0;
        }
        .toggle-slider {
            position: absolute;
            cursor: pointer;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background-color: #ccc;
            transition: .4s;
            border-radius: 34px;
        }
        .toggle-slider:before {
            position: absolute;
            content: "";
            height: 26px;
            width: 26px;
            left: 4px;
            bottom: 4px;
            background-color: white;
            transition: .4s;
            border-radius: 50%;
        }
        input:checked + .toggle-slider {
            background-color: #2196F3;
        }
        input:checked + .toggle-slider:before {
            transform: translateX(26px);
        }
    </style>
</head>
<body>
    <div class="control-container">
        <h1>Vehicle Control</h1>
        
        <div class="drive-toggle">
            <span>Forward Drive</span>
            <label class="toggle-switch">
                <input type="checkbox" id="driveToggle">
                <span class="toggle-slider"></span>
            </label>
            <span>Backward Drive</span>
        </div>
        
        <div class="buttons">
            <button class="btn btn-forward" id="forwardBtn">Forward</button>
            <button class="btn btn-left" id="leftBtn">Left</button>
            <button class="btn btn-stop" id="stopBtn">STOP</button>
            <button class="btn btn-right" id="rightBtn">Right</button>
            <button class="btn btn-backward" id="backwardBtn">Backward</button>
        </div>
        
        <div class="status" id="statusText">Status: Ready</div>
        
        <button class="auto-btn" id="autoBtn">Start Autonomous Driving</button>
        <button class="auto-btn" id="wallBtn" style="background-color: purple">Start Wall Following</button>
        <button class="reset-btn" id="resetViveBtn">Reset Vive Coordinates</button>

        <div class="orientation-log" id="orientationLog" style="white-space: pre-line">Logs</div>
        <div class="logs-container" id="logs">Loading logs...</div>
    </div>

    <script>
        document.addEventListener('DOMContentLoaded', function() {
            // Get elements
            const forwardBtn = document.getElementById('forwardBtn');
            const leftBtn = document.getElementById('leftBtn');
            const stopBtn = document.getElementById('stopBtn');
            const rightBtn = document.getElementById('rightBtn');
            const backwardBtn = document.getElementById('backwardBtn');
            const statusText = document.getElementById('statusText');
            const autoBtn = document.getElementById('autoBtn');
            const orientationLog = document.getElementById('orientationLog');
            const logsContainer = document.getElementById('logs');
            const resetViveBtn = document.getElementById('resetViveBtn');
            const driveToggle = document.getElementById('driveToggle');
            
            // Current state
            let currentDirection = 'stop';
            let isBackwardDrive = false;
            
            // Function to get the actual command based on drive direction
            function getCommand(buttonCommand) {
                if (!isBackwardDrive) return buttonCommand;
                
                // Only swap forward/backward commands in backward drive mode
                switch(buttonCommand) {
                    case 'forward': return 'backward';
                    case 'backward': return 'forward';
                    default: return buttonCommand;
                }
            }
            
            // Function to get the display direction based on drive direction
            function getDisplayDirection(buttonDirection) {
                if (!isBackwardDrive) return buttonDirection;
                
                // Only swap forward/backward display directions
                switch(buttonDirection) {
                    case 'forward': return 'backward';
                    case 'backward': return 'forward';
                    default: return buttonDirection;
                }
            }
            
            // Drive toggle handler
            driveToggle.addEventListener('change', function() {
                isBackwardDrive = this.checked;
            });
            
            // Function to update orientation display
            function updateOrientation(logText) {
                orientationLog.textContent = logText;
            }
            
            // Function to update logs display
            function updateLogs() {
                fetch('/logs')
                    .then(response => response.text())
                    .then(data => {
                        const logs = data.split('\n').filter(log => log.trim() !== '');
                        logsContainer.innerHTML = logs.map(log => 
                            `<div class="log-entry">${log}</div>`
                        ).join('');
                        logsContainer.scrollTop = logsContainer.scrollHeight;
                    })
                    .catch(error => console.error('Error fetching logs:', error));
            }
            
            // Set up periodic updates
            setInterval(function() {
                // Update orientation
                var xhttp = new XMLHttpRequest();
                xhttp.onreadystatechange = function() {
                    if (this.readyState == 4 && this.status == 200) {
                        updateOrientation(this.responseText);
                    }
                };
                xhttp.open("GET", "orientation", true);
                xhttp.send();
                
                // Update logs
                updateLogs();
            }, 100); // Update every 100ms
            
            // Autonomous button handler
            autoBtn.addEventListener('click', function() {
                var xhttp = new XMLHttpRequest();
                xhttp.open("GET", "auto", true);
                xhttp.send();
            });

            wallBtn.addEventListener('click', function() {
                var xhttp = new XMLHttpRequest();
                xhttp.open("GET", "wall", true);
                xhttp.send();
            });

            // Direction button handlers
            forwardBtn.addEventListener('mousedown', function() {
                currentDirection = 'forward';
                statusText.textContent = 'Status: Moving ' + getDisplayDirection('forward');
                var xhttp = new XMLHttpRequest();
                xhttp.open("GET", getCommand('forward'), true);
                xhttp.send();
            });
            
            leftBtn.addEventListener('mousedown', function() {
                currentDirection = 'left';
                statusText.textContent = 'Status: Turning ' + getDisplayDirection('left');
                var xhttp = new XMLHttpRequest();
                xhttp.open("GET", getCommand('left'), true);
                xhttp.send();
            });
            
            stopBtn.addEventListener('mousedown', function() {
                currentDirection = 'stop';
                statusText.textContent = 'Status: Stopped';
                var xhttp = new XMLHttpRequest();
                xhttp.open("GET", "stop", true);
                xhttp.send();
            });
            
            rightBtn.addEventListener('mousedown', function() {
                currentDirection = 'right';
                statusText.textContent = 'Status: Turning ' + getDisplayDirection('right');
                var xhttp = new XMLHttpRequest();
                xhttp.open("GET", getCommand('right'), true);
                xhttp.send();
            });
            
            backwardBtn.addEventListener('mousedown', function() {
                currentDirection = 'backward';
                statusText.textContent = 'Status: Moving ' + getDisplayDirection('backward');
                var xhttp = new XMLHttpRequest();
                xhttp.open("GET", getCommand('backward'), true);
                xhttp.send();
            });
            
            // Release button handler - stop when button released
            const allButtons = [forwardBtn, leftBtn, rightBtn, backwardBtn];
            allButtons.forEach(button => {
                button.addEventListener('mouseup', function() {
                    currentDirection = 'stop';
                    statusText.textContent = 'Status: Stopped';
                    var xhttp = new XMLHttpRequest();
                    xhttp.open("GET", "stop", true);
                    xhttp.send();
                });
                
                // Also handle touch events for mobile
                button.addEventListener('touchstart', function(e) {
                    e.preventDefault();
                    this.dispatchEvent(new Event('mousedown'));
                });
                
                button.addEventListener('touchend', function(e) {
                    e.preventDefault();
                    this.dispatchEvent(new Event('mouseup'));
                });
            });

            // Reset Vive button handler
            resetViveBtn.addEventListener('click', function() {
                var xhttp = new XMLHttpRequest();
                xhttp.open("GET", "resetvive", true);
                xhttp.send();
            });
        });
    </script>
</body>
</html>
)===";