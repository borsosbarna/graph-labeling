var express = require('express');
var router = express.Router();

/* Require Controller Modules */
var SAController = require('./controllers/SAController');
var GAController = require('./controllers/GAController');

/* Define API endpoints */
router.post('/SA', SAController);
router.post('/GA', GAController);

module.exports = router;
