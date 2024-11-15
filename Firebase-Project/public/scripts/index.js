// convert epochtime to JavaScripte Date object
function epochToJsDate(epochTime){
    return new Date(epochTime * 1000);
  }
  
  // convert time to human-readable format YYYY/MM/DD HH:MM:SS
  function epochToDateTime(epochTime){
    var epochDate = new Date(epochToJsDate(epochTime));
    var dateTime = epochDate.getFullYear() + "/" +
      ("00" + (epochDate.getMonth() + 1)).slice(-2) + "/" +
      ("00" + epochDate.getDate()).slice(-2) + " " +
      ("00" + epochDate.getHours()).slice(-2) + ":" +
      ("00" + epochDate.getMinutes()).slice(-2) + ":" +
      ("00" + epochDate.getSeconds()).slice(-2);
  
    return dateTime;
  }
  
  // function to plot values on charts
  function plotValues(chart, timestamp, value){
    var x = epochToJsDate(timestamp).getTime();
    var y = Number(value);
    if(chart.series[0].data.length > 40) {
        chart.series[0].addPoint([x, y], true, true, true);
    } else {
        chart.series[0].addPoint([x, y], true, false, true);
    }
  }
  
  // DOM elements
  const loginElement = document.querySelector('#login-form');
  const contentElement = document.querySelector("#content-sign-in");
  const userDetailsElement = document.querySelector('#user-details');
  const authBarElement = document.querySelector('#authentication-bar');
  const deleteButtonElement = document.getElementById('delete-button');
  const deleteModalElement = document.getElementById('delete-modal');
  const deleteDataFormElement = document.querySelector('#delete-data-form');
  const viewDataButtonElement = document.getElementById('view-data-button');
  const hideDataButtonElement = document.getElementById('hide-data-button');
  const tableContainerElement = document.querySelector('#table-container');
  const chartsRangeInputElement = document.getElementById('charts-range');
  const loadDataButtonElement = document.getElementById('load-data');
  const cardsCheckboxElement = document.querySelector('input[name=cards-checkbox]');
  const chartsCheckboxElement = document.querySelector('input[name=charts-checkbox]');
  
  // DOM elements for sensor readings
  const cardsReadingsElement = document.querySelector("#cards-div");
  const chartsDivElement = document.querySelector('#charts-div');
  const tempElement = document.getElementById("temp");
  const humElement = document.getElementById("hum");
  const gasElement = document.getElementById("gas");
  const flameElement = document.getElementById("flame");
  const soundElement = document.getElementById("sound");
  const pirElement = document.getElementById("pir");
  const updateElement = document.getElementById("lastUpdate");
  
  // MANAGE LOGIN/LOGOUT UI
  const setupUI = (user) => {
    if (user) {
        //toggle UI elements
        loginElement.style.display = 'none';
        contentElement.style.display = 'block';
        authBarElement.style.display ='block';
        userDetailsElement.style.display ='block';
        userDetailsElement.innerHTML = user.email;
  
        // get user UID to get data from database
        var uid = user.uid;
        console.log(uid);
  
        // Database paths (with user UID)
        var dbPath = 'UsersData/' + uid.toString() + '/readings';
        var chartPath = 'UsersData/' + uid.toString() + '/charts/range';
  
        // Database references
        var dbRef = firebase.database().ref(dbPath);
        var chartRef = firebase.database().ref(chartPath);
  
        // CHARTS
        // Number of readings to plot on charts
        var chartRange = 0;
        // Get number of readings to plot saved on database (runs when the page first loads and whenever there's a change in the database)
        chartRef.on('value', snapshot =>{
            chartRange = Number(snapshot.val());
            console.log(chartRange);
            // Delete all data from charts to update with new values when a new range is selected
            chartT.destroy();
            chartH.destroy();
            chartGas.destroy();
            chartFlame.destroy();
            chartSound.destroy();
            chartPir.destroy();
            // Render new charts to display new range of data
            chartT = createTemperatureChart();
            chartH = createHumidityChart();
            chartGas = createGasChart();
            chartFlame = createFlameChart();
            chartSound = createSoundChart();
            chartPir = createPirChart();
            // Update the charts with the new range
            // Get the latest readings and plot them on charts (the number of plotted readings corresponds to the chartRange value)
            dbRef.orderByKey().limitToLast(chartRange).on('child_added', snapshot =>{
                var jsonData = snapshot.toJSON();
                // Save values on variables
                var temperature = jsonData.temperature;
                var humidity = jsonData.humidity;
                var gasLevel = jsonData.gas;
                var flameDetected = jsonData.flame === "Flame Detected" ? 1 : 0;
                var soundDetected = jsonData.sound === "Sound Detected" ? 1 : 0;
                var motionDetected = jsonData.pir === "Motion Detected" ? 1 : 0;
                var timestamp = jsonData.timestamp;
                // Plot the values on the charts
                plotValues(chartT, timestamp, temperature);
                plotValues(chartH, timestamp, humidity);
                plotValues(chartGas, timestamp, gasLevel);
                plotValues(chartFlame, timestamp, flameDetected);
                plotValues(chartSound, timestamp, soundDetected);
                plotValues(chartPir, timestamp, motionDetected);
            });
        });
  
        // Update database with new range (input field)
        chartsRangeInputElement.onchange = () =>{
            chartRef.set(chartsRangeInputElement.value);
        };
  
        //CHECKBOXES
        // Checkbox (cards for sensor readings)
        cardsCheckboxElement.addEventListener('change', (e) =>{
            if (cardsCheckboxElement.checked) {
                cardsReadingsElement.style.display = 'block';
            }
            else{
                cardsReadingsElement.style.display = 'none';
            }
        });
        // Checkbox (charts for sensor readings)
        chartsCheckboxElement.addEventListener('change', (e) =>{
            if (chartsCheckboxElement.checked) {
                chartsDivElement.style.display = 'block';
            }
            else{
                chartsDivElement.style.display = 'none';
            }
        });
  
        // CARDS
        // Get the latest readings and display on cards
        dbRef.orderByKey().limitToLast(1).on('child_added', snapshot =>{
            var jsonData = snapshot.toJSON();
            var temperature = jsonData.temperature;
            var humidity = jsonData.humidity;
            var gasLevel = jsonData.gas;
            var flameDetected = jsonData.flame;
            var soundDetected = jsonData.sound;
            var motionDetected = jsonData.pir;
            var timestamp = jsonData.timestamp;
            // Update DOM elements
            tempElement.innerHTML = temperature;
            humElement.innerHTML = humidity;
            gasElement.innerHTML = gasLevel;
            flameElement.innerHTML = flameDetected;
            soundElement.innerHTML = soundDetected;
            pirElement.innerHTML = motionDetected;
            updateElement.innerHTML = epochToDateTime(timestamp);
        });
  
        // DELETE DATA
        deleteButtonElement.addEventListener('click', e =>{
            e.preventDefault();
            deleteModalElement.style.display="block";
        });
  
        deleteDataFormElement.addEventListener('submit', (e) => {
            e.preventDefault();
            dbRef.remove();
        });
  
        // TABLE
        var lastReadingTimestamp;
        function createTable(){
            var firstRun = true;
            dbRef.orderByKey().limitToLast(100).on('child_added', function(snapshot) {
                if (snapshot.exists()) {
                    var jsonData = snapshot.toJSON();
                    var temperature = jsonData.temperature;
                    var humidity = jsonData.humidity;
                    var gasLevel = jsonData.gas;
                    var flameDetected = jsonData.flame;
                    var soundDetected = jsonData.sound;
                    var motionDetected = jsonData.pir;
                    var timestamp = jsonData.timestamp;
                    var content = '';
                    content += '<tr>';
                    content += '<td>' + epochToDateTime(timestamp) + '</td>';
                    content += '<td>' + temperature + '</td>';
                    content += '<td>' + humidity + '</td>';
                    content += '<td>' + gasLevel + '</td>';
                    content += '<td>' + flameDetected + '</td>';
                    content += '<td>' + soundDetected + '</td>';
                    content += '<td>' + motionDetected + '</td>';
                    content += '</tr>';
                    $('#tbody').prepend(content);
                    if (firstRun){
                        lastReadingTimestamp = timestamp;
                        firstRun = false;
                    }
                }
            });
        }
  
        function appendToTable(){
            var dataList = [];
            var reversedList = [];
            dbRef.orderByKey().limitToLast(100).endAt(lastReadingTimestamp).once('value', function(snapshot) {
                if (snapshot.exists()) {
                    snapshot.forEach(element => {
                        var jsonData = element.toJSON();
                        dataList.push(jsonData);
                    });
                    lastReadingTimestamp = dataList[0].timestamp;
                    reversedList = dataList.reverse();
  
                    var firstTime = true;
                    reversedList.forEach(element =>{
                        if (firstTime){
                            firstTime = false;
                        }
                        else{
                            var temperature = element.temperature;
                            var humidity = element.humidity;
                            var gasLevel = element.gas;
                            var flameDetected = element.flame;
                            var soundDetected = element.sound;
                            var motionDetected = element.pir;
                            var timestamp = element.timestamp;
                            var content = '';
                            content += '<tr>';
                            content += '<td>' + epochToDateTime(timestamp) + '</td>';
                            content += '<td>' + temperature + '</td>';
                            content += '<td>' + humidity + '</td>';
                            content += '<td>' + gasLevel + '</td>';
                            content += '<td>' + flameDetected + '</td>';
                            content += '<td>' + soundDetected + '</td>';
                            content += '<td>' + motionDetected + '</td>';
                            content += '</tr>';
                            $('#tbody').append(content);
                        }
                    });
                }
            });
        }
  
        viewDataButtonElement.addEventListener('click', (e) =>{
            tableContainerElement.style.display = 'block';
            viewDataButtonElement.style.display ='none';
            hideDataButtonElement.style.display ='inline-block';
            loadDataButtonElement.style.display = 'inline-block';
            createTable();
        });
  
        loadDataButtonElement.addEventListener('click', (e) => {
            appendToTable();
        });
  
        hideDataButtonElement.addEventListener('click', (e) => {
            tableContainerElement.style.display = 'none';
            viewDataButtonElement.style.display = 'inline-block';
            hideDataButtonElement.style.display = 'none';
        });
  
    // IF USER IS LOGGED OUT
    } else{
        loginElement.style.display = 'block';
        authBarElement.style.display ='none';
        userDetailsElement.style.display ='none';
        contentElement.style.display = 'none';
    }
  }
  