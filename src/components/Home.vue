<template>
  <div>
    <toolbar />
    <presentation />
    <simulated-annealing />
    <genetic-algorithm />
  </div>
</template>

<script>
import Toolbar from '@/components/material/Toolbar';
import Presentation from '@/components/material/Presentation';
import SimulatedAnnealing from '@/components/algorithm/SimulatedAnnealing';
import GeneticAlgorithm from '@/components/algorithm/GeneticAlgorithm';

export default {
  name: 'Home',
  components: {
    Toolbar,
    Presentation,
    SimulatedAnnealing,
    GeneticAlgorithm,
  },
};
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
</style>
