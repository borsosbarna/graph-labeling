const fs = require('fs');
const path = require('path');
const { execFile } = require('child_process');

const MAX_RUNNING_TIME = 60; /* (seconds) */

function checkFileContent(fileContent, maxLabel) {
  /* split by unix line ending convention */
  const lines = fileContent.replace(/\r/g, '').split('\n');

  /* check first line */
  let numbers = lines[0].split(' ');

  if (numbers.length !== 3) {
    return 1;
  }

  const vertexCount = Number(numbers[0]);
  const edgeCount = Number(numbers[1]);
  const fixedVertexes = Number(numbers[2]);

  if (Number.isInteger(vertexCount) === false ||
      Number.isInteger(edgeCount) === false ||
      Number.isInteger(fixedVertexes) === false) {
    return 1;
  }

  if (vertexCount < 1 || edgeCount < 0 || fixedVertexes < 0) {
    return 1;
  }

  /* check every edge */
  let lineIndex = 1;

  for (lineIndex; lineIndex <= edgeCount; lineIndex += 1) {
    if (lineIndex >= lines.length) {
      return lineIndex + 1;
    }
    numbers = lines[lineIndex].split(' ');

    if (numbers.length !== 2) {
      return lineIndex + 1;
    }

    const vertex1 = Number(numbers[0]);
    const vertex2 = Number(numbers[1]);

    if (Number.isInteger(vertex1) === false ||
        Number.isInteger(vertex2) === false) {
      return lineIndex + 1;
    }

    if (vertex1 < 1 || vertex1 > vertexCount ||
        vertex2 < 1 || vertex2 > vertexCount) {
      return lineIndex + 1;
    }
  }

  /* check fixed vertexes */
  const lastLineIndex = edgeCount + fixedVertexes;

  for (lineIndex; lineIndex <= lastLineIndex; lineIndex += 1) {
    if (lineIndex >= lines.length) {
      return lineIndex + 1;
    }
    numbers = lines[lineIndex].split(' ');

    if (numbers.length !== 2) {
      return lineIndex + 1;
    }

    const vertex = Number(numbers[0]);
    const label = Number(numbers[1]);

    if (Number.isInteger(vertex) === false ||
        Number.isInteger(label) === false) {
      return lineIndex + 1;
    }

    if (vertex < 1 || vertex > vertexCount ||
        label < 1 || label > maxLabel) {
      return lineIndex + 1;
    }
  }

  return null;
}

function validateParameters(req) {
  /* extract parameters */
  const h = req.body.h;
  const k = req.body.k;
  const fileContent = req.body.fileContent;
  const maxLabel = req.body.maxLabel;
  const populationsCount = req.body.populationsCount;
  const populationSize = req.body.populationSize;
  const mutationChance = req.body.mutationChance;
  const elitesCount = req.body.elitesCount;
  const maxGenerations = req.body.maxGenerations;
  const maxTime = req.body.maxTime;

  /* check parameters existance */
  if (h == null ||
      k == null ||
      fileContent == null ||
      maxLabel == null ||
      populationsCount == null ||
      populationSize == null ||
      mutationChance == null ||
      elitesCount == null ||
      maxGenerations == null ||
      maxTime == null) {
    return 'Please provide all parameters!';
  }

  /* check parameters type */
  if (Number.isInteger(Number(h)) === false ||
      Number.isInteger(Number(k)) === false ||
      Number.isInteger(Number(maxLabel)) === false ||
      Number.isInteger(Number(populationsCount)) === false ||
      Number.isInteger(Number(populationSize)) === false ||
      isNaN(Number(mutationChance)) ||
      Number.isInteger(Number(elitesCount)) === false ||
      Number.isInteger(Number(maxGenerations)) === false ||
      Number.isInteger(Number(maxTime)) === false) {
    return 'Please verify parameter types!';
  }

  /* check parameters value */
  if (h < 0 ||
      k < 0 ||
       maxLabel < 1 ||
      populationsCount < 1 ||
      populationSize < 1 ||
      mutationChance < 0 ||
      mutationChance > 1 ||
      elitesCount < 0 ||
      elitesCount > populationSize ||
      maxGenerations < 1 ||
      maxTime < 1 ||
      maxTime > MAX_RUNNING_TIME) {
    return 'Please double check parameter values!';
  }

  /* check input file */
  const errorLine = checkFileContent(fileContent, maxLabel);

  if (errorLine) {
    return `Invalid input file format! (Line: ${errorLine})`;
  }

  return null;
}

module.exports = (req, res) => {
  /* validate request parameters */
  const errorMsg = validateParameters(req);

  if (errorMsg) {
    res.json({ errorMsg });

    return;
  }

  /* set parameters */
  const h = req.body.h;
  const k = req.body.k;
  const fileContent = req.body.fileContent;
  const maxLabel = req.body.maxLabel;
  const populationsCount = req.body.populationsCount;
  const populationSize = req.body.populationSize;
  const mutationChance = req.body.mutationChance;
  const elitesCount = req.body.elitesCount;
  const maxGenerations = req.body.maxGenerations;
  const maxTime = req.body.maxTime;

  /* path of the executable file */
  const execPath = path.join(__dirname, '..', 'bin', 'GeneticAlgorithm');

  /* create input file */
  const suffix = Math.floor((Math.random() * 1e6));
  const filePath = path.join(__dirname, '..', 'tmp', `${suffix}.in`);

  fs.writeFileSync(filePath, fileContent);

  // call exec
  execFile(execPath,
    [h, k, filePath, maxLabel, populationsCount, populationSize, mutationChance, elitesCount,
      maxGenerations, maxTime],
    (error, stdout, stderr) => {
    /* check for any error */
      if (error || stderr) {
        res.json(error);
        // res.json({ errorMsg: 'Something went wrong!' });
      } else {
        const lines = stdout.toString().replace(/\r/g, '').split('\n');

        /* create the JSON object to be returned */
        const responseJSON = {};

        responseJSON.time = parseFloat(lines[0], 10);
        responseJSON.iterations = parseInt(lines[1], 10);
        responseJSON.solution = lines[2].trim();
        responseJSON.isCorrect = lines[3] === '1';
        responseJSON.conflictingVertexes = parseInt(lines[4], 10);
        responseJSON.chromaticNumber = parseInt(lines[5], 10);
        responseJSON.fitness = parseFloat(lines[6], 10);
        responseJSON.history = lines[7];
        responseJSON.errorMsg = null;

        res.json(responseJSON);
      }

      // delete input file
      fs.unlink(filePath, () => {});
    });
};
