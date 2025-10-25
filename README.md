⚡ Lighting — Experimental OpenGL Renderer

Lighting is an experimental renderer built in C++ with OpenGL, created to explore real-time rendering pipelines, physically based shading, and editor design.
It serves as a sandbox for learning modern rendering techniques and developing a full PBR workflow from scratch.

✨ Current Highlights

ECS-driven scene architecture

Real-time lighting and material editing

Hot-reloadable shader system

Asset loading with transform controls

Integrated editor using ImGui

Performance and render diagnostics overlay

🚀 In Development

Physically Based Rendering (PBR)

HDR & Image-Based Lighting (IBL)

Dynamic Shadows (Directional & Point)

Post-Processing (Bloom, FXAA, Tonemapping)

<img width="1918" height="1075" alt="Screenshot 2025-10-25 230854" src="https://github.com/user-attachments/assets/2724f87e-e0a5-4b4b-9e44-ad46adf3dba7" />

Material and asset hot-reloading

🛠 Tech Stack
System	Library
Graphics	OpenGL (Core Profile)
UI	Dear ImGui
Window/Input	GLFW
Model Import	Assimp
Math	GLM
Build	Visual Studio 2022 / CMake
⚙️ Build
git clone https://github.com/dadusthecoder/opengl2.git


Open Lighting.sln in Visual Studio

Link dependencies (GLFW, GLEW, Assimp, GLM, ImGui) via vcpkg or /Dependencies

Add shaders and assets in /res

Build & run

📸 Screenshots




⚡ A personal journey into modern rendering — built for experimentation, not perfection.
