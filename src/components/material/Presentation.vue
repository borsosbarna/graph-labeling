<template>
  <div class="md-layout">
    <div class="md-layout-item md-size-66 left">
      <span class="md-display-1"> {{ title }} </span>
      <br />
      <p class="md-headline"> {{ text }} </p>
    </div>
    <div class="md-layout-item right">
      <img src="@/assets/Turan.svg" />
    </div>
  </div>
</template>

<script>
export default {
  name: 'Presentation',
  data() {
    return {
      title: 'L(h, k) Graph Labeling',
      text: 'Lorem ipsum dolor sit amet, dolore ceteros facilisi id est, ' +
       'per et ubique ornatus conclusionemque, ad pri quas commune accusata.',
    };
  },
};
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
  .md-layout {
    padding-top: 48px;
    height: 100vh;
    background-color: whitesmoke;
    overflow: hidden;
  }
  .left {
    display: block;
    margin: auto;

    padding-left: 10%;
    padding-right: 10%;
  }
  .right {
    display: block;
    margin: auto;
  }
  p {
    text-indent: 10%;
  }
  img {
    width: 50%;
  }
</style>
