<template>
  <div>
    <a id="GA" />

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

  </div>
</template>

<script>
export default {
  name: 'GeneticAlgorithm',
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
    };
  },
};
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
  #tab-general, #tab-parameters, #tab-input {
    background-color: #8BC34A;
    color: whitesmoke;
  }
  .md-headline {
    text-align: center;
    padding-left: 5%;
    padding-right: 5%;
    padding-top: 5%;
    padding-bottom: 5%;
  }
</style>
