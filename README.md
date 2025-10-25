⚡ Lighting — Experimental OpenGL Renderer








Lighting is an experimental real-time renderer built with C++ and OpenGL, designed as a personal playground for learning modern rendering pipelines, scene editors, and physically based rendering (PBR) from the ground up.

🧩 Overview

This project began as a deep dive into OpenGL fundamentals and has since evolved into a modular, editor-driven renderer.
The end goal: create a complete physically based rendering system and in-engine scene editor, with support for ECS, IBL, and material workflows.

✨ Core Features

🔬 Early experimental implementations — built for exploration and learning.

🎮 Interactive Camera System — smooth navigation, customizable movement speed

💡 Lighting Panel — real-time position, direction, color, and attenuation controls

🔁 Hot Shader Reloading — update shaders instantly without restarting

🧱 Model Controls — load .fbx / .obj with editable transforms (position, rotation, scale)

🧭 Camera Debug HUD — real-time position + direction readouts

🌌 Render Mode Switching — Fill / Wireframe / Point rendering

⚙️ Gravity Simulation Toggle — lightweight physics toggle

📊 Performance Overlay — live FPS + frame time metrics

🧠 Roadmap

What’s next for Lighting — the path toward a full-fledged PBR engine.

✅ Physically Based Rendering (Albedo, Roughness, Metalness, Normal)

✅ HDR & Image-Based Lighting (IBL)

✅ Dynamic Shadow Mapping (Directional + Point)

✅ Material system with live ImGui editing

✅ Post-Processing: Bloom, FXAA, Tonemapping

✅ Entity-Component System (ECS)

✅ Asset Hot Reloading

✅ Scene Hierarchy & Editor Tools

🛠️ Tech Stack
System	Library / Tool
Graphics API	OpenGL (Core Profile)
GUI	Dear ImGui
Windowing/Input	GLFW
Model Importing	Assimp
Math Library	GLM
Build System	Visual Studio 2022 / CMake
⚙️ Build Instructions
Prerequisites

Visual Studio 2022 (or newer)

OpenGL-compatible GPU

vcpkg
 (recommended for dependencies)

Steps
# Clone the repo
git clone https://github.com/dadusthecoder/opengl2.git


Open Lighting.sln in Visual Studio 2022

Ensure dependencies (GLFW, GLEW, Assimp, GLM, ImGui) are linked via vcpkg or /Dependencies

Place shaders and models in /res

Build and run 🚀

🎮 Controls
Action	Key
Move Camera	W, A, S, D
Look Around	Mouse
Adjust Model	ImGui Transform Panel
Change Lighting	ImGui Light Panel
Reload Shaders	“Reload” Button
📷 Gallery




⚡ Lighting — built for learning, experimentation, and pushing OpenGL to its creative limits.
Enhanced PBR Renderer • OpenGL + ImGui
