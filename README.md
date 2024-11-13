# Drone Dance Dialog

![Project Preview](extras/preview.png)

An **open source application** designed for **motion capture, choreographing and visualizing drones** accurately in space using the **OptiTrack system**.  
This project leverages precise real-time motion capture to control and preview drones, both in simulation and live flight.  
User control interface is built with **React**, while the backend is written in **C++** and communicates with drones using **ESP32** modules connected over ESP-NOW WiFi, ensuring secure and seamless information flow between the control system and the drones.  
To ensure consistent runtime environment, main project modules run inside individual docker containers.  

## Features

- **Choreograph and visualize drone movements** in a 3D space using a web interface.
- **Blender animation export integration** to control and execute pre-programmed drone flight patterns.
- **Live control and preview** of drone positions and movements in real time.
- Communication between **C++ backend** and drones via **ESP32** devices.
- Utilizes the **OptiTrack motion capture system** for accurate drone tracking.

## Requirements

- Docker with Docker Compose
- Forward USB to UART serial port to WSL (if running on MS Windows)
> `usbipd list`  
> `usbipd bind --busid <device BUSID>`  
> `usbipd attach --wsl --busid <device BUSID>`  

## Tech Stack

### Backend
- **C++**: Core logic and communication handling.
- **ESP32**: Wireless communication between backend and drones (sending and receiving commands/data).
  
### Frontend
- **React.js**: Web app for visualizing and controlling the drones.
- **Three.js**: 3D visualization for live drone choreography preview.

### Additional Components
- **Blender**: Integration for exporting drone choreography as animations.
- **OptiTrack**: Motion capture system to track drone positions in real-time.
