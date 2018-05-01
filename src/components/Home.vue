<template>
  <div>
    <toolbar />
    <presentation />
    <simulated-annealing />
    <genetic-algorithm />
    <bottom />
  </div>
</template>

<script>
import Toolbar from '@/components/material/Toolbar';
import Presentation from '@/components/material/Presentation';
import SimulatedAnnealing from '@/components/algorithm/SimulatedAnnealing';
import GeneticAlgorithm from '@/components/algorithm/GeneticAlgorithm';
import Bottom from '@/components/material/Bottom';

export default {
  name: 'Home',
  components: {
    Toolbar,
    Presentation,
    SimulatedAnnealing,
    GeneticAlgorithm,
    Bottom,
  },
};
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
</style>
