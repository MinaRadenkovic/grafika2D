# 2D Bus Route Simulation

**Author:** [Mina Radenković]  

---

## Overview
A 2D graphical simulation of a bus route implemented in **C++** using **OpenGL 3+** with a programmable pipeline.  
The application visualizes a bus moving along a **closed route with 10 stations**, showing passenger interactions, control officers, and fines in real time.

---

## Features
- **Bus Movement:**  
  - Constant-speed travel between stations.  
  - Automatic stops at each station for 10 seconds.

- **Doors Indicator:**  
  - Icon displays bus doors open/closed status.  
  - Doors open while passengers can enter/exit.

- **Passenger Management:**  
  - Left-click adds a passenger (doors open).  
  - Right-click removes a passenger (doors open).  
  - Maximum 50 passengers.

- **Control Officer:**  
  - Press `K` to insert a control officer.  
  - Officer stays until the next station.  
  - Random fines are issued to passengers upon officer exit.

- **Custom Cursor:**  
  - Yellow diamond-shaped “BUS STOP” sign.

- **HUD Elements:**  
  - Displays current passenger count, fines, and door status.  

- **Full-Screen & FPS Limit:**  
  - Full-screen mode.  
  - Frame rate limited to 75 FPS.  
  - Press `Escape` to exit at any time.

- **Student Identification:**  
  - Semi-transparent overlay with name, surname, and index number.

---

## Controls
| Action                  | Key / Mouse           |
|-------------------------|----------------------|
| Add passenger           | Left-click (doors open) |
| Remove passenger        | Right-click (doors open) |
| Add control officer     | `K`                  |
| Exit application        | `Escape`             |

---

## Requirements
- C++11 or later  
- OpenGL 3+ with programmable pipeline  
- FreeType library (for text rendering)  


