const express = require('express');

const router = express.Router();

/* Require Controller Modules */
const SAController = require('./controllers/SAController');
const GAController = require('./controllers/GAController');

/* Define API endpoints */
router.post('/SA', SAController);
router.post('/GA', GAController);

module.exports = router;
