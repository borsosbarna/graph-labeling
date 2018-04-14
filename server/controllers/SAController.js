const fs = require('fs');
const { execFile } = require('child_process');

module.exports = (req, res) => {
  /* set parameters */
  const h = req.body.h;
  const k = req.body.k;
  const fileContent = req.body.fileContent;
  const maxLabel = req.body.maxLabel;
  const temperature = req.body.temperature;
  const coolingFactor = req.body.coolingFactor;
  const maxIterations = req.body.maxIterations;
  const maxTime = req.body.maxTime;

  /* path of the executable file */
  const execPath = __dirname + '/../bin/SimulatedAnnealing';

  /* create input file */
  const suffix = Math.floor((Math.random() * 1e6));
  const filePath = __dirname + '/../tmp/' + suffix + '.in';

  fs.writeFileSync(filePath, fileContent);

  // call exec
  const child = execFile(execPath,
    [h, k, filePath, maxLabel, temperature, coolingFactor, maxIterations, maxTime],
    (error, stdout, stderr) => {
    /* check for any error */
    if (error || stderr) {
      res.send({ errorMsg: 'Something went wrong!' });
    }
    else {
      const lines = stdout.toString().replace(/\r/g, "").split('\n');

      /* create the JSON object to be returned */
      const responseJSON = {};
      responseJSON['time'] = parseFloat(lines[0], 10);
      responseJSON['iterations'] = parseInt(lines[1], 10);
      responseJSON['temperature'] = parseFloat(lines[2], 10);
      responseJSON['solution'] = lines[3].trim();
      responseJSON['isCorrect'] = lines[4] === '1' ? true : false;
      responseJSON['conflictingVertexes'] = parseInt(lines[5], 10);
      responseJSON['chromaticNumber'] = parseInt(lines[6], 10);
      responseJSON['fitness'] = parseFloat(lines[7], 10);
      responseJSON['errorMsg'] = null;

      res.json(responseJSON);
    }

    // delete input file
    fs.unlink(filePath, () => {});
  });
};
