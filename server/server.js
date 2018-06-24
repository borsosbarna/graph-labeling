const express = require('express');
const serveStatic = require('serve-static');
const bodyParser = require('body-parser');
const router = require('./router');
const path = require('path');

const app = express();
app.use(serveStatic(path.join(__dirname, '..', 'dist')));
app.use(bodyParser.json());
app.use('/api', router);

const port = process.env.PORT || 8080;
app.listen(port);
