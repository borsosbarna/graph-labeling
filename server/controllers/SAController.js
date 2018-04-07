const { execFile } = require('child_process');
const fs = require('fs');

module.exports = (req, res) => {
  /* set parameters */
  const h = req.body.h;
  const k = req.body.k;
  const fileContent = req.body.fileContent;
  const temperature = req.body.temperature;
  const coolingFactor = req.body.coolingFactor;
  const maxIterations = req.body.maxIterations;
  const maxTime = req.body.maxTime;

  const execPath = __dirname + '\\..\\bin\\SimulatedAnnealing';

  /* create input file */
  const suffix = Math.floor((Math.random() * 1e6) + 1);
  const filePath = __dirname + '\\..\\tmp\\in_' + suffix + '.txt';

  fs.writeFileSync(filePath, fileContent);

  // call exec
  const child = execFile(execPath, [h, k, filePath, temperature, coolingFactor, maxIterations, maxTime], (error, stdout, stderr) => {
    // console.log(stdout);
    res.send({ res: stdout});

    // delete input file
    fs.unlink(filePath, () => {});
  });
};
