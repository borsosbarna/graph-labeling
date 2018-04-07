var express = require('express');
var serveStatic = require('serve-static');
var bodyParser = require('body-parser');
var router = require('./router');

app = express();
app.use(serveStatic(__dirname + "/../dist"));
app.use(bodyParser.json())
app.use('/api', router);

var port = process.env.PORT || 8080;
app.listen(port);
