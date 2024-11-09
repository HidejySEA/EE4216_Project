// Create Temperature Gauge
function createTemperatureGauge() {
    var gauge = new LinearGauge({
        renderTo: 'gauge-temperature',
        width: 120,
        height: 400,
        units: "Temperature (°C)",
        minValue: 0,
        maxValue: 50,
        colorValueBoxRect: "#049faa",
        colorValueBoxRectEnd: "#049faa",
        colorValueBoxBackground: "#f1fbfc",
        valueDec: 2,
        valueInt: 2,
        majorTicks: [
            "0", "10", "20", "30", "40", "50"
        ],
        minorTicks: 2,
        strokeTicks: true,
        highlights: [
            { "from": 30, "to": 50, "color": "rgba(200, 50, 50, .75)" }
        ],
        colorPlate: "#fff",
        colorBarProgress: "#CC2936",
        colorBarProgressEnd: "#049faa",
        borderShadowWidth: 0,
        borders: false,
        needleType: "arrow",
        needleWidth: 2,
        needleCircleSize: 7,
        needleCircleOuter: true,
        needleCircleInner: false,
        animationDuration: 1500,
        animationRule: "linear",
        barWidth: 10,
    });
    return gauge;
}

// Create Humidity Gauge
function createHumidityGauge(){
    var gauge = new RadialGauge({
        renderTo: 'gauge-humidity',
        width: 300,
        height: 300,
        units: "Humidity (%)",
        minValue: 0,
        maxValue: 100,
        colorValueBoxRect: "#049faa",
        colorValueBoxRectEnd: "#049faa",
        colorValueBoxBackground: "#f1fbfc",
        valueInt: 2,
        majorTicks: [
            "0", "20", "40", "60", "80", "100"
        ],
        minorTicks: 2,
        strokeTicks: true,
        highlights: [
            { "from": 80, "to": 100, "color": "#03C0C1" }
        ],
        colorPlate: "#fff",
        borderShadowWidth: 0,
        borders: false,
        needleType: "line",
        colorNeedle: "#007F80",
        colorNeedleEnd: "#007F80",
        needleWidth: 2,
        needleCircleSize: 3,
        colorNeedleCircleOuter: "#007F80",
        needleCircleOuter: true,
        needleCircleInner: false,
        animationDuration: 1500,
        animationRule: "linear"
    });
    return gauge;
}

// Create Gas Gauge
function createGasGauge() {
    var gauge = new LinearGauge({
        renderTo: 'gauge-gas',
        width: 120,
        height: 400,
        units: "Gas Level",
        minValue: 0,
        maxValue: 1023,
        colorValueBoxRect: "#FF6347",
        colorValueBoxRectEnd: "#FF6347",
        colorValueBoxBackground: "#fff",
        valueDec: 2,
        valueInt: 2,
        majorTicks: [
            "0", "200", "400", "600", "800", "1023"
        ],
        minorTicks: 5,
        strokeTicks: true,
        highlights: [
            { "from": 800, "to": 1023, "color": "rgba(255, 0, 0, .75)" }
        ],
        colorPlate: "#fff",
        colorBarProgress: "#FF6347",
        colorBarProgressEnd: "#FF6347",
        borderShadowWidth: 0,
        borders: false,
        needleType: "arrow",
        needleWidth: 2,
        needleCircleSize: 7,
        needleCircleOuter: true,
        needleCircleInner: false,
        animationDuration: 1500,
        animationRule: "linear",
        barWidth: 10,
    });
    return gauge;
}
