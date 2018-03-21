<template>
  <div>
    <md-toolbar class="md-dense toolbar">
      <div class="md-toolbar-section-start">
        <md-button class="md-icon-button" :href="home.href">
          <img src="@/assets/home.svg" />
        </md-button>
      </div>
      <div v-for="button in buttons" :key="button.href">
        <md-button :href="button.href">
          <span>{{ button.text }}</span>
        </md-button>
      </div>
      <div class="md-toolbar-section-end" />
    </md-toolbar>
  </div>
</template>

<script>
export default {
  name: 'Toolbar',
  data() {
    return {
      home: {
        href: '#',
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
    background-color: #8BC34A;
  }
  span {
    color: white;
    text-transform: none;
    font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell,
     'Open Sans', 'Helvetica Neue', sans-serif;
  }
</style>
