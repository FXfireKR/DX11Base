
# DX11Base

Personal DirectX11 Game Engine & Minecraft‑like Sandbox Prototype  
Built with **C++ / DirectX11 / Visual Studio 2022**

---

## Overview

DX11Base is a personal 3D engine project focused on:

- Understanding the **modern DirectX11 rendering pipeline**
- Designing a **component‑based game engine architecture**
- Experimenting with **voxel world (Minecraft‑style) rendering & systems**
- Building a **portfolio‑grade graphics / engine programming project**

The project separates **Game logic and Rendering logic** and introduces a
custom resource / pipeline / object management structure similar in spirit to
Unity / Unreal architecture.

---

## Key Features

### Engine Architecture

- Scene → Object → Component hierarchy
- ObjectManager using ID‑based management
- Transform build step separated from Update
- GameWorld / RenderWorld structural separation
- Render submission → RenderFrame → RenderThread flow
- Pipeline‑based rendering pass grouping

### Rendering System

- DirectX11 forward rendering pipeline
- Mesh / Material / Shader / Pipeline resource managers
- Constant buffer update strategy per object / frame
- Texture atlas & runtime resource packing experiments
- Chunk mesh generation for voxel rendering
- Block highlight / raycast interaction system

### Input System

- Raw Input keyboard & mouse handling
- Gamepad support (including DualSense experimentation)
- Camera free‑look and movement control

### Minecraft‑Style World System (Prototype)

- ChunkColumn / ChunkSection voxel storage
- BlockCell runtime representation
- JSON‑based blockstate / model / resource loading
- Runtime mesh generation per chunk section

---

## Tech Stack

### Core

- C++20
- DirectX11 API
- Windows Desktop Application
- Visual Studio 2022 (v143 toolset)

### External Libraries

- DirectXTK  
  → texture loading, helper utilities

- DirectXTex  
  → texture processing / DDS support

- FMOD  
  → sound system

- Dear ImGui  
  → debug UI / runtime editor tools

- RapidJSON  
  → Minecraft resource parsing

---

## Project Structure (Simplified)

DX11Tutorial  
├─ Application  
├─ GameWorld  
│  ├─ Scene  
│  ├─ Object  
│  └─ Component  
├─ RenderWorld  
│  ├─ RenderFrame  
│  ├─ RenderItem  
│  └─ RenderThread  
├─ Managers  
│  ├─ MeshManager  
│  ├─ ShaderManager  
│  ├─ PipelineManager  
│  └─ MaterialManager  
├─ Voxel  
│  ├─ ChunkWorld  
│  ├─ ChunkColumn  
│  └─ ChunkSection  
└─ Resources  

---

## Build Requirements

- Windows 10 / 11
- Visual Studio 2022
- DirectX SDK (Windows SDK)
- External libraries placed under:

Libraries/inc  
Libraries/lib  

(ImGui backend path also required)

Build target:

x64 / Debug or Release

---

## Learning Goals

This project explores:

- Efficient draw call batching strategies
- Resource lifetime & ownership design
- ECS‑style runtime object control
- Real‑time voxel mesh streaming
- Rendering pass extensibility (shadow / post process ready)
- Engine architecture comparable to commercial engines

---

## Reference Materials

- Microsoft Direct3D 11 Documentation  
- Rastertek DirectX11 Tutorials  
- Various graphics programming lectures  
- Minecraft resource format analysis  

---

## Author

Personal graphics / engine study project  
Focused on deep understanding of rendering architecture and engine design.
