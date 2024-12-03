FROM node:alpine3.19 AS production

WORKDIR /build
COPY package.json package-lock.json ./
RUN npm install
RUN npm i -g serve
COPY . .
RUN npm run build
EXPOSE 3000
CMD ["serve", "-s", "dist"]



FROM node:alpine3.19 AS development

WORKDIR /app
COPY package.json package-lock.json ./
RUN npm install
EXPOSE 3000
CMD ["npm", "run", "dev"]

