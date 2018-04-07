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
      <div class="md-layout-item md-size-50 padded">
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

        <md-button class="md-dense md-raised md-primary" v-on:click="start">
          {{ isRunning ? 'Stop' : 'Start' }}
        </md-button>

        <p> {{ error.msg }} </p>
      </div>

      <div class="md-layout-item">
        <div class="md-headline">Results</div>
          <p> {{ result }} </p>
      </div>
    </div>
  </div>
</template>

<script>
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

      error: {
        msg: '',
      },

      isRunning: false,

      parameters: {
        h: null,
        k: null,
        file: null,
        temperature: null,
        coolingFactor: null,
        maxIterations: null,
        maxTime: null,
      },

      result: null,
    };
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
        temperature: parseInt(this.parameters.temperature, 10),
        coolingFactor: parseFloat(this.parameters.coolingFactor, 10),
        maxIterations: parseInt(this.parameters.maxIterations, 10),
        maxTime: parseInt(this.parameters.maxTime, 10),
      };

      fetch('http://localhost:8080/api/sa', {
        headers: { 'Content-Type': 'application/json' },
        method: 'POST',
        body: JSON.stringify(payload) })
        // eslint-disable-next-line
        .then((res) => { return res.json(); })
        // eslint-disable-next-line
        .then((res) => { this.result = res.res; this.isRunning = false; });
    },
  },
};
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
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
</style>
