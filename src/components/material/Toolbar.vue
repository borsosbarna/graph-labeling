<template>
  <md-toolbar class="md-dense toolbar">
    <div class="md-toolbar-section-start">
      <md-button class="md-icon-button" :href="home.href">
        <md-icon> {{ home.icon }} </md-icon>
      </md-button>
    </div>
    <div v-for="button in buttons" :key="button.href">
      <md-button :href="button.href">
        <span> {{ button.text }} </span>
      </md-button>
    </div>
    <div class="md-toolbar-section-end" />
  </md-toolbar>
</template>

<script>
export default {
  name: 'Toolbar',
  data() {
    return {
      home: {
        href: '#',
        icon: 'home',
      },
      buttons: [{
        href: '#SA',
        text: 'Simulated Annealing',
      },
      {
        href: '#GA',
        text: 'Genetic Algorithm',
      }],
    };
  },
};
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
  .toolbar {
    position: fixed;
    z-index: 10;
  }
  span {
    text-transform: none;
  }
</style>
