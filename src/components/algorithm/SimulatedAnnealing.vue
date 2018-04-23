<template>
  <div>

    <md-tabs md-alignment="centered">
      <md-tab id="tab-general" md-label="Info">
        <div class="md-headline"> {{ info.general }} </div>
      </md-tab>

      <md-tab id="tab-parameters" md-label="Parameters">
        <div class="md-headline"> {{ info.parameters }} </div>
      </md-tab>

      <md-tab id="tab-input" md-label="Input">
        <div class="md-headline"> {{ info.input }} </div>
      </md-tab>
    </md-tabs>

    <a id="SA" />

    <div class="md-layout">
      <div class="md-layout-item md-size-33 padded">
        <md-field>
          <label>h</label>
          <md-input v-model="parameters.h" type="number" :disabled="isRunning" />
        </md-field>

        <md-field>
          <label>k</label>
          <md-input v-model="parameters.k" type="number" :disabled="isRunning" />
        </md-field>

        <md-field>
          <label>Input File</label>
          <md-file @change="fileChanged($event)" :disabled="isRunning" />
        </md-field>

        <md-field>
          <label>Max. Label Value</label>
          <md-input v-model="parameters.maxLabel" type="number" :disabled="isRunning" />
        </md-field>

        <md-field>
          <label>Temperature</label>
          <md-input v-model="parameters.temperature" type="number" :disabled="isRunning" />
        </md-field>

        <md-field>
          <label>Cooling Factor</label>
          <md-input v-model="parameters.coolingFactor" type="number" :disabled="isRunning" />
        </md-field>

        <md-field>
          <label>Max. Iterations</label>
          <md-input v-model="parameters.maxIterations" type="number" :disabled="isRunning" />
        </md-field>

        <md-field>
          <label>Max. Time (seconds)</label>
          <md-input v-model="parameters.maxTime" type="number" :disabled="isRunning" />
        </md-field>

        <center>
          <md-button
            class="md-dense md-raised md-primary" v-on:click="start" :disabled="isRunning">
            Start
          </md-button>
        </center>
      </div>

      <div class="md-layout-item md-size-33 padded-vertically">
        <div id="chart"></div>
        <div id="graph"></div>
      </div>

      <div class="md-layout-item md-size-33 padded">
        <md-progress-spinner
          v-if="isRunning"
          class="center"
          :md-diameter="100"
          :md-stroke="10"
          md-mode="indeterminate">
        </md-progress-spinner>

        <div v-if="!isRunning">
          <div class="md-headline">Results</div> <br />

          <span class="bold">Time Elapsed</span>
          <span v-if="result.time">{{ result.time }} s</span>
          <br/><br/>
          <span class="bold">Iterations Done</span>
          <span v-if="result.iterations">
            {{ result.iterations }}
            ({{Math.round((result.iterations / parameters.maxIterations) * 100)}}%)
          </span>
          <br/><br/>
          <span class="bold">Final Temperature</span>
          <span>{{ result.temperature }}</span>
          <br/><br/><br/>

          <span class="bold">Solution</span>
          <span>{{ result.solution }}</span>
          <br/><br/>
          <span class="bold">Is Solution Correct?</span>
          <span v-bind:class="getResultColorClass()">{{ getResultCorrectness() }}</span>
          <br/><br/>
          <span class="bold">Number of Conflicting Vertexes</span>
          <span>{{ result.conflictingVertexes }}</span>
          <br/><br/>
          <span class="bold">Chromatic Number</span>
          <span v-if="result.chromaticNumber">&le; {{ result.chromaticNumber }}</span>
          <br/><br/>
          <span class="bold">Fitness</span>
          <span>{{ result.fitness }}</span>
          <br/><br/><br/>

          <span class="error"> {{ result.errorMsg }} </span> <br/>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
import { GoogleCharts } from 'google-charts';

const svgPanZoom = require('svg-pan-zoom');
const Viz = require('viz.js');

export default {
  name: 'SimulatedAnnealing',
  data() {
    return {
      info: {
        general: 'Lorem ipsum dolor sit amet, te ius mutat voluptatum, et doming volutpat ' +
                 'torquatos vix, et has legere tamquam probatus. Quo ea erat interpretaris, ' +
                 'nec modus tempor elaboraret in. Eu pri eros consul fastidii, unum facete no qui.',
        parameters: 'Lorem ipsum dolor sit amet, te ius mutat voluptatum, et doming volutpat ' +
                    'torquatos vix, et has legere tamquam probatus. Quo ea erat interpretaris, ' +
                    'nec modus tempor elaboraret in. Eu pri eros consul fastidii, unum facete no qui.',
        input: 'Lorem ipsum dolor sit amet, te ius mutat voluptatum, et doming volutpat ' +
               'torquatos vix, et has legere tamquam probatus. Quo ea erat interpretaris, ' +
               'nec modus tempor elaboraret in. Eu pri eros consul fastidii, unum facete no qui.',
      },

      isRunning: false,
      MAX_RUNNING_TIME: 60,

      parameters: {
        h: null,
        k: null,
        file: null,
        maxLabel: null,
        temperature: null,
        coolingFactor: null,
        maxIterations: null,
        maxTime: null,
      },

      result: {
        time: null,
        iterations: null,
        temperature: null,
        solution: null,
        isCorrect: null,
        conflictingVertexes: null,
        chromaticNumber: null,
        fitness: null,
        errorMsg: null,
      },

      diagrams: {
        chart: null,
        chartData: null,
        chartOptions: null,

        svg: null,
        panZoomTiger: null,
      },
    };
  },

  mounted() {
    // Load the charts library with a callback
    GoogleCharts.load(this.drawChart);

    window.addEventListener('resize', () => {
      if (this.diagrams.chart) {
        this.diagrams.chart.draw(this.diagrams.chartData, this.diagrams.chartOptions);
      }
    });
  },

  beforeUpdate() {
    this.drawChart();
  },

  methods: {
    /* read data from loaded file */
    fileChanged(event) {
      const reader = new FileReader();

      reader.onload = (e) => {
        this.parameters.file = e.target.result;
      };

      reader.readAsText(event.target.files[0]);
    },

    start() {
      this.result.errorMsg = this.validateParameters();

      if (this.result.errorMsg) {
        return;
      }

      this.isRunning = true;

      const payload = {
        h: parseInt(this.parameters.h, 10),
        k: parseInt(this.parameters.k, 10),
        fileContent: this.parameters.file,
        maxLabel: parseInt(this.parameters.maxLabel, 10),
        temperature: parseInt(this.parameters.temperature, 10),
        coolingFactor: parseFloat(this.parameters.coolingFactor, 10),
        maxIterations: parseInt(this.parameters.maxIterations, 10),
        maxTime: parseInt(this.parameters.maxTime, 10),
      };

      fetch('http://localhost:8080/api/sa', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(payload) })
        // eslint-disable-next-line
        .then((res) => { return res.json(); })
        .then((data) => {
          if (!data.errorMsg) {
            this.result = data;

            this.drawGraph();
          }

          this.isRunning = false;
        });
    },

    drawChart() {
      const list = [['Iteration', 'Temperature']];

      if (this.parameters.temperature && this.parameters.coolingFactor &&
          this.parameters.maxIterations &&
          this.parameters.temperature > 0 &&
          this.parameters.coolingFactor > 0 && this.parameters.coolingFactor < 1 &&
          this.parameters.maxIterations > 0) {
        let i;
        for (i = 0; i < this.parameters.maxIterations;
          i += Math.ceil(this.parameters.maxIterations / 100)) {
          list.push([i, this.parameters.temperature * (this.parameters.coolingFactor ** i)]);
        }
      } else {
        list.push([0, 0]);
      }

      this.diagrams.chartData = GoogleCharts.api.visualization.arrayToDataTable(list);

      this.diagrams.chartOptions = {
        title: '',
        hAxis: { title: 'Iterations', viewWindow: { min: 0 } },
        vAxis: { title: 'Temperature', viewWindow: { min: 0 } },
        backgroundColor: { fill: 'transparent' },
      };

      if (!this.diagrams.chart) {
        this.diagrams.chart = new GoogleCharts.api.visualization.AreaChart(document.getElementById('chart'));
      }

      this.diagrams.chart.draw(this.diagrams.chartData, this.diagrams.chartOptions);
    },

    drawGraph() {
      if (!this.parameters.file || !this.result) {
        return;
      }

      this.diagrams.svg = Viz(this.toDotFormat());
      this.diagrams.svg = this.diagrams.svg.replace(/<svg width=".*?" height=".*?"/, '<svg width="1pt" height="1pt"');

      const element = document.createElement('div');
      element.style.width = '100%';
      element.style.height = '100%';
      element.innerHTML = this.diagrams.svg;

      const svgElement = element.getElementsByTagName('svg')[0];
      svgElement.id = 'svg';
      svgElement.style.width = '100%';
      svgElement.style.height = '100%';

      const graphContainer = document.getElementById('graph');
      graphContainer.innerHTML = '';
      graphContainer.appendChild(element);

      if (this.diagrams.panZoomTiger) {
        // eslint-disable-next-line
        this.diagrams.panZoomTiger.destroy;
      }

      // eslint-disable-next-line
      this.diagrams.panZoomTiger = svgPanZoom('#svg', {
        // viewportSelector: '.svg-pan-zoom_viewport',
        panEnabled: true,
        controlIconsEnabled: true,
        zoomEnabled: true,
        dblClickZoomEnabled: true,
        mouseWheelZoomEnabled: true,
        preventMouseEventsDefault: true,
        zoomScaleSensitivity: 0.2,
        minZoom: 0.5,
        maxZoom: 10,
        fit: true,
        contain: false,
        center: true,
        refreshRate: 'auto',
        beforeZoom: () => {},
        onZoom: () => {},
        beforePan: () => {},
        onPan: () => {},
        onUpdatedCTM: () => {},
        // customEventsHandler: {},
        eventsListenerElement: null,
      });
    },

    toDotFormat() {
      let graph = `graph {
        bgcolor=transparent;`;

      /* split by unix line ending convention */
      const lines = this.parameters.file.replace(/\r/g, '').split('\n');

      /* get number of edges */
      let numbers = lines[0].split(' ');
      const edgeCount = Number(numbers[1]);

      /* get labeling reuslt */
      const labels = this.result.solution.split(' ');
      let i = 0;
      for (i; i < labels.length; i += 1) {
        graph += `${i + 1} [label="${labels[i]}"];`;
      }

      /* add every edge */
      let lineIndex = 1;

      for (lineIndex; lineIndex <= edgeCount; lineIndex += 1) {
        numbers = lines[lineIndex].split(' ');

        const vertex1 = Number(numbers[0]);
        const vertex2 = Number(numbers[1]);

        graph += `${vertex1} -- ${vertex2};`;
      }

      graph += '}';

      return graph;
    },

    validateParameters() {
      /* extract parameters */
      const h = this.parameters.h;
      const k = this.parameters.k;
      const fileContent = this.parameters.file;
      const maxLabel = this.parameters.maxLabel;
      const temperature = this.parameters.temperature;
      const coolingFactor = this.parameters.coolingFactor;
      const maxIterations = this.parameters.maxIterations;
      const maxTime = this.parameters.maxTime;

      /* check parameters existance */
      if (h == null ||
          k == null ||
          fileContent == null ||
          maxLabel == null ||
          temperature == null ||
          coolingFactor == null ||
          maxIterations == null ||
          maxTime == null) {
        return 'Please provide all parameters!';
      }

      /* check parameters type */
      if (Number.isInteger(Number(h)) === false ||
          Number.isInteger(Number(k)) === false ||
          Number.isInteger(Number(maxLabel)) === false ||
          isNaN(Number(temperature)) ||
          isNaN(Number(coolingFactor)) ||
          Number.isInteger(Number(maxIterations)) === false ||
          Number.isInteger(Number(maxTime)) === false) {
        return 'Please verify parameter types!';
      }

      /* check parameters value */
      if (h < 0 ||
          k < 0 ||
          maxLabel < 1 ||
          temperature <= 0 ||
          coolingFactor <= 0 ||
          coolingFactor >= 1 ||
          maxIterations < 1 ||
          maxTime < 1 ||
          maxTime > this.MAX_RUNNING_TIME) {
        return 'Please double check parameter values!';
      }

      /* check input file */
      const errorLine = this.checkFileContent();

      if (errorLine) {
        return `Invalid input file format! (Line: ${errorLine})`;
      }

      return null;
    },

    checkFileContent() {
      /* split by unix line ending convention */
      const lines = this.parameters.file.replace(/\r/g, '').split('\n');

      /* check first line */
      let numbers = lines[0].split(' ');

      if (numbers.length !== 3) {
        return 1;
      }

      const vertexCount = Number(numbers[0]);
      const edgeCount = Number(numbers[1]);
      const fixedVertexes = Number(numbers[2]);

      if (Number.isInteger(vertexCount) === false ||
          Number.isInteger(edgeCount) === false ||
          Number.isInteger(fixedVertexes) === false) {
        return 1;
      }

      if (vertexCount < 1 || edgeCount < 0 || fixedVertexes < 0) {
        return 1;
      }

      /* check every edge */
      let lineIndex = 1;

      for (lineIndex; lineIndex <= edgeCount; lineIndex += 1) {
        if (lineIndex >= lines.length) {
          return lineIndex + 1;
        }
        numbers = lines[lineIndex].split(' ');

        if (numbers.length !== 2) {
          return lineIndex + 1;
        }

        const vertex1 = Number(numbers[0]);
        const vertex2 = Number(numbers[1]);

        if (Number.isInteger(vertex1) === false ||
            Number.isInteger(vertex2) === false) {
          return lineIndex + 1;
        }

        if (vertex1 < 1 || vertex1 > vertexCount ||
            vertex2 < 1 || vertex2 > vertexCount) {
          return lineIndex + 1;
        }
      }

      /* check fixed vertexes */
      const lastLineIndex = edgeCount + fixedVertexes;

      for (lineIndex; lineIndex <= lastLineIndex; lineIndex += 1) {
        if (lineIndex >= lines.length) {
          return lineIndex + 1;
        }
        numbers = lines[lineIndex].split(' ');

        if (numbers.length !== 2) {
          return lineIndex + 1;
        }

        const vertex = Number(numbers[0]);
        const label = Number(numbers[1]);

        if (Number.isInteger(vertex) === false ||
            Number.isInteger(label) === false) {
          return lineIndex + 1;
        }

        if (vertex < 1 || vertex > vertexCount ||
            label < 1 || label > this.parameters.maxLabel) {
          return lineIndex + 1;
        }
      }

      return null;
    },

    getResultCorrectness() {
      switch (this.result.isCorrect) {
        case true:
          return 'Yes';
        case false:
          return 'No';
        default:
          return '';
      }
    },

    getResultColorClass() {
      switch (this.result.isCorrect) {
        case true:
          return 'bold green';
        case false:
          return 'bold red';
        default:
          return '';
      }
    },
  },
};
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
  #tab-general, #tab-parameters, #tab-input {
    background-color: #3F51B5;
    color: whitesmoke;
  }
  .md-headline {
    text-align: center;
    padding-left: 5%;
    padding-right: 5%;
    padding-top: 5%;
    padding-bottom: 5%;
  }
  .padded {
    padding-left: 5%;
    padding-right: 5%;
    padding-top: 3%;
    padding-bottom: 3%;
  }
  .padded-vertically {
    padding-top: 3%;
    padding-bottom: 3%;
  }
  .center {
    height: 100%;
    display: flex;
    align-items: center;
    justify-content: center;
    flex-direction: column;
  }
  .bold {
    font-weight: bold;
  }
  .error {
    color: #F44336;
    font-weight: bold;
  }
  #chart {
    width: 100%;
  }
  #graph {
    width: 100%;
    height: 50%;
    padding-top: 3%;
  }
  #svg {
    position: absolute;
    width: 100%;
    height: 100%;
    top: 0;
    left: 0;
  }
  .red {
    color: #F44336;
  }
  .green {
    color: #8BC34A;
  }
</style>
