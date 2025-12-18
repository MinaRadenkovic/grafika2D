2D Bus Route Simulation

Overview

This project is a 2D graphical application created in C++ using OpenGL 3+ with a programmable pipeline. The application simulates a bus moving along a predefined route with 10 stations labeled 0–9. The route is represented by red curved lines connecting stations, which are displayed as red circles with white numbers.

Features

Bus movement: The bus moves at a constant speed between stations and stops at each station for 10 seconds.

Bus doors: A door icon shows whether the bus doors are open or closed.

Passenger management:

Left-click adds a passenger when doors are open.

Right-click removes a passenger when doors are open.

Passenger count is limited from 0 to 50.

Control officer: Press K to add a control officer. The officer stays until the next station, after which a random number of passengers receive fines. Total fines are displayed on the screen.

Custom cursor: The cursor is a yellow diamond-shaped "BUS STOP" sign.

Full-screen mode with 75 FPS frame limit.

Exit: Press Escape to exit the application at any time.

Student identification: A semi-transparent overlay shows the student's name, surname, and index number.

Controls

Mouse left-click: Add passenger (doors open)

Mouse right-click: Remove passenger (doors open)

Keyboard K: Add control officer

Keyboard Escape: Exit application

Requirements

C++ compiler supporting C++11 or later

OpenGL 3+ with programmable pipeline

FreeType library (for text rendering)

Notes

The bus route is closed-loop: station 0 connects to station 9.

Passengers and fines are updated in real-time during stops.
