FROM node:alpine3.19

WORKDIR /app
COPY package.json package-lock.json ./
RUN npm install
RUN npm i -g serve
COPY . .
RUN npm run build
EXPOSE 3000
#CMD ["npm", "start-frontend"]
CMD ["serve", "-s", "dist"]
