// Create the charts when the web page loads
window.addEventListener('load', onload);

function onload(event){
  chartT = createTemperatureChart();
  chartH = createHumidityChart();
  chartGas = createGasChart();
  chartFlame = createFlameChart();
  chartSound = createSoundChart();
  chartPir = createPirChart();
}

// Create Temperature Chart
function createTemperatureChart() {
  var chart = new Highcharts.Chart({
    chart:{ 
      renderTo:'chart-temperature',
      type: 'spline' 
    },
    series: [
      {
        name: 'DHT11 Temperature'
      }
    ],
    title: { 
      text: undefined
    },
    plotOptions: {
      line: { 
        animation: false,
        dataLabels: { 
          enabled: true 
        }
      }
    },
    xAxis: {
      type: 'datetime',
      dateTimeLabelFormats: { second: '%H:%M:%S' }
    },
    yAxis: {
      title: { 
        text: 'Temperature (°C)' 
      }
    },
    credits: { 
      enabled: false 
    }
  });
  return chart;
}

// Create Humidity Chart
function createHumidityChart(){
  var chart = new Highcharts.Chart({
    chart:{ 
      renderTo:'chart-humidity',
      type: 'spline'  
    },
    series: [{
      name: 'DHT11 Humidity'
    }],
    title: { 
      text: undefined
    },    
    plotOptions: {
      line: { 
        animation: false,
        dataLabels: { 
          enabled: true 
        }
      },
      series: { 
        color: '#50b8b4' 
      }
    },
    xAxis: {
      type: 'datetime',
      dateTimeLabelFormats: { second: '%H:%M:%S' }
    },
    yAxis: {
      title: { 
        text: 'Humidity (%)' 
      }
    },
    credits: { 
      enabled: false 
    }
  });
  return chart;
}

// Create Gas Chart
function createGasChart() {
  var chart = new Highcharts.Chart({
    chart:{ 
      renderTo: 'chart-gas',
      type: 'spline' 
    },
    series: [
      {
        name: 'Gas Level'
      }
    ],
    title: { 
      text: undefined
    },
    plotOptions: {
      line: { 
        animation: false,
        dataLabels: { 
          enabled: true 
        }
      }
    },
    xAxis: {
      type: 'datetime',
      dateTimeLabelFormats: { second: '%H:%M:%S' }
    },
    yAxis: {
      title: { 
        text: 'Gas Level' 
      }
    },
    credits: { 
      enabled: false 
    }
  });
  return chart;
}

// Create Flame Chart
function createFlameChart() {
  var chart = new Highcharts.Chart({
    chart:{ 
      renderTo: 'chart-flame',
      type: 'spline' 
    },
    series: [
      {
        name: 'Flame Detection'
      }
    ],
    title: { 
      text: undefined
    },
    plotOptions: {
      line: { 
        animation: false,
        dataLabels: { 
          enabled: true 
        }
      }
    },
    xAxis: {
      type: 'datetime',
      dateTimeLabelFormats: { second: '%H:%M:%S' }
    },
    yAxis: {
      title: { 
        text: 'Flame Detection' 
      }
    },
    credits: { 
      enabled: false 
    }
  });
  return chart;
}

// Create Sound Chart
function createSoundChart() {
  var chart = new Highcharts.Chart({
    chart:{ 
      renderTo: 'chart-sound',
      type: 'spline' 
    },
    series: [
      {
        name: 'Sound Detection'
      }
    ],
    title: { 
      text: undefined
    },
    plotOptions: {
      line: { 
        animation: false,
        dataLabels: { 
          enabled: true 
        }
      }
    },
    xAxis: {
      type: 'datetime',
      dateTimeLabelFormats: { second: '%H:%M:%S' }
    },
    yAxis: {
      title: { 
        text: 'Sound Detection' 
      }
    },
    credits: { 
      enabled: false 
    }
  });
  return chart;
}

// Create PIR Chart
function createPirChart() {
  var chart = new Highcharts.Chart({
    chart:{ 
      renderTo: 'chart-pir',
      type: 'spline' 
    },
    series: [
      {
        name: 'Motion Detection'
      }
    ],
    title: { 
      text: undefined
    },
    plotOptions: {
      line: { 
        animation: false,
        dataLabels: { 
          enabled: true 
        }
      }
    },
    xAxis: {
      type: 'datetime',
      dateTimeLabelFormats: { second: '%H:%M:%S' }
    },
    yAxis: {
      title: { 
        text: 'Motion Detection' 
      }
    },
    credits: { 
      enabled: false 
    }
  });
  return chart;
}
