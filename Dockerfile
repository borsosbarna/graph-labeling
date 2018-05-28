FROM node:slim

# Create app directory
WORKDIR /usr/src/app

# Bundle app source
COPY . .

RUN npm install
RUN npm run build

ENV PORT=80
EXPOSE 80

CMD npm start
