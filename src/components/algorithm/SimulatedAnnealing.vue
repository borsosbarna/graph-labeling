<template>
  <div>
    <a id="SA" />

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
          <md-file @change="onFileUpload($event)" :disabled="isRunning" />
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
          <label>Max. Time</label>
          <md-input v-model="parameters.maxTime" type="number" :disabled="isRunning" />
        </md-field>

        <center>
          <md-button
            class="md-dense md-raised md-primary"
            v-on:click="start"
            :disabled="isRunning">
          Start</md-button>
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
          <div class="md-headline">Results</div>

          <span class="bold">Time Elapsed: </span> {{ result.time }} <br /> <br />
          <span class="bold">Iterations Done: </span> {{ result.iterations }} <br /> <br />
          <span class="bold">Final Temperature: </span> {{ result.temperature }} <br /> <br />
          <span class="bold">Solution: </span> {{ result.solution }} <br /> <br />
          <span class="bold">Is Solution Correct?: </span>
            {{ firstTime ? '' : (result.isCorrect ? 'Yes' : 'No') }} <br /> <br />
          <span class="bold">Number of Conflicting Vertexes: </span>
            {{ result.conflictingVertexes }} <br /> <br />
          <span class="bold">Chromatic Number: </span> {{ result.chromaticNumber }} <br /> <br />
          <span class="bold">Fitness: </span> {{ result.fitness }} <br /> <br />
          <span class="error"> {{ result.errorMsg }} </span> <br />
        </div>

      </div>
    </div>
  </div>
</template>

<script>
import { GoogleCharts } from 'google-charts';

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
      firstTime: true,

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
      },
    };
  },

  mounted() {
    // Load the charts library with a callback
    GoogleCharts.load(this.drawChart);
  },

  beforeUpdate() {
    this.drawChart();
  },

  methods: {
    /* read data from loaded file */
    onFileUpload(event) {
      const reader = new FileReader();

      reader.onload = (e) => {
        this.parameters.file = e.target.result;
      };

      reader.readAsText(event.target.files[0]);
    },

    start() {
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
          this.result = data;
          this.isRunning = false;
          this.firstTime = false;
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

      const data = GoogleCharts.api.visualization.arrayToDataTable(list);

      const options = {
        title: '',
        hAxis: { title: 'Iterations', viewWindow: { min: 0 } },
        vAxis: { title: 'Temperature', viewWindow: { min: 0 } },
        backgroundColor: { fill: 'transparent' },
      };

      this.chart = new GoogleCharts.api.visualization.AreaChart(document.getElementById('chart'));
      this.chart.draw(data, options);
    },
  },
};
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
  #tab-general, #tab-parameters, #tab-input {
    height: 90vh;
    background-color: #3F51B5;
    color: whitesmoke;
  }
  .md-headline {
    text-align: center;
    padding-left: 10%;
    padding-right: 10%;
    padding-top: 10%;
    padding-bottom: 10%;
  }
  .padded {
    padding-left: 5%;
    padding-right: 5%;
    padding-top: 5%;
    padding-bottom: 5%;
  }
  .padded-vertically {
    padding-top: 5%;
    padding-bottom: 5%;
  }
  .center {
    display: flex;
    align-items: center;
    justify-content: center;
    height: 100%;
    flex-direction: column;
  }
  .bold {
    font-weight: bold;
  }
  .error {
    color: red;
  }
</style>
