<template>
  <div class="md-layout">
    <div class="md-layout-item md-size-75 left">
      <span class="md-display-1"> {{ title }} </span>
      <br />
      <p class="md-headline">
        Assignment of nonnegative integers to every vertex following two simple rules: <br/><br/>
        1. Adjacent vertexes get labels at least <i>h</i> apart <br/><br/>
        2. Vertexes with common neighbors get labels at least <i>k</i> apart
      </p>
    </div>
    <div class="md-layout-item right">
      <img src="@/assets/Petersen.svg" title="Petersen graph coloring" />
    </div>
  </div>
</template>

<script>
export default {
  name: 'Presentation',
  data() {
    return {
      title: 'L(h, k) Graph Labeling',
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
    width: 75%;
  }
</style>
