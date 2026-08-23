# DX11Base

**C++20 / DirectX 11 기반의 Streaming Voxel World & Client Architecture 개인 프로젝트**

DX11Base는 렌더링 기능 자체보다 **외부 콘텐츠가 런타임 데이터로 변환되는 과정**, **월드 스트리밍과 CPU 작업량 제어**, **Render Submission / Pass 구조**, **Lighting / Shadow**, **Runtime Diagnostics**를 하나의 실행 가능한 클라이언트 안에서 직접 구현하고 검증하기 위해 개발한 프로젝트입니다.

현재 `main`은 포트폴리오 제출 기준 브랜치입니다.

---

## Technical Focus

### 1. Data-Driven Content Resource Pipeline

외부 JSON / Texture / Sound 데이터를 초기화 단계에서 해석하고 런타임 표현으로 변환합니다.

```text
External Resource
    ↓
Block Definition / Tag
    ↓
BlockState Compile
    ↓
Model Resolve / Bake
    ↓
BakedModel / STATE_INDEX
    ↓
Used Texture Dependency
    ↓
Runtime Atlas / Sound Resource
    ↓
Chunk Mesh / Particle / Rendering
```

- `CBlockDB`를 facade로 사용하고 BlockDef / Tag / State / Model DB의 책임을 분리
- BlockState property/value domain을 수집한 뒤 `STATE_INDEX` 기반 런타임 상태로 compile
- Model parent 상속과 `#texture` alias resolve
- Raw → Resolved → Baked 단계 분리
- 실제 사용 Texture Key만 수집해 Runtime Atlas 구성
- Chunk Mesh와 Block Particle에서 동일 Atlas Region 공유

> Minecraft resource format의 일부 구조를 프로젝트 목적에 맞게 해석한 학습용 구현입니다.

---

### 2. Streaming & Frame-Time Workload Control

플레이어 주변 월드를 스트리밍하면서 **무엇을 먼저 처리할지**와 **한 프레임에 얼마만큼 처리할지**를 분리했습니다.

- `ChunkColumn / ChunkSection` 기반 Streaming World
- 플레이어 중심 **Center-Out Ring Scan**
- Preload / Hotload / Unload 단계별 per-frame budget
- Dirty Section Queue 기반 Mesh Rebuild
- Generated Resident Section을 우선 조회하고 필요 시 Procedural Fallback
- CPU Mesh Build에 **Cooperative Time Budget** 적용
  - Debug: 5 ms
  - Release: 2 ms
  - 최소 1 Section / 최대 4 Section 처리

#### Measurement Snapshot

Debug 병목 조사 과정에서 동일한 **2-Column Preload** workload를 기준으로:

| Item | Before | After |
| --- | ---: | ---: |
| Preload (2 Columns) | 9.215 ms | 1.928 ms |
| Reduction |  | **약 79.1%** |

Frame / Mesh Build 전체 시간은 Section 수와 지형 복잡도에 따라 workload가 달라질 수 있어, 동일 workload가 확인된 Preload 수치를 정량 비교 기준으로 사용했습니다.

---

### 3. Render Submission / Pass / Culling Architecture

Scene Object 전체를 Renderer가 직접 해석하지 않고, Draw에 필요한 정보만 `RenderItem`으로 제출합니다.

```text
CGameScene
    ↓ Submit(RenderItem)
CRenderWorld
    ↓
CRenderManager / CRenderFrame
    ↓ Pass Bucket
SHADOW → SKY → CLOUD → OPAQUE → CUTOUT → TRANSPARENT → DEBUG → ORTH
    ↓
Direct3D 11 Draw
```

`RenderItem`은 다음 정보를 중심으로 구성됩니다.

- Mesh
- Pipeline
- Material
- World Matrix
- Render Pass
- Transparent Sort Depth

`CRenderFrame`에서는:

- Pass별 Bucket 분리
- Transparent Pass의 back-to-front stable sort
- 연속된 Pipeline / Mesh / Material의 중복 Bind 생략
- Main Camera Frustum + Renderer Bounds 기반 CPU Frustum Culling

을 처리합니다.

#### Effect / Particle

Block hit / break particle은 Particle마다 Scene Object와 RenderItem을 만들지 않고, 활성 Particle을 Camera-Facing Quad로 구성해 하나의 Dynamic Vertex / Index Buffer로 갱신한 뒤 Transparent Pass에 하나의 RenderItem으로 제출합니다.

---

### 4. Lighting / Shadow / HLSL

Lighting은 역할을 분리해 처리합니다.

```text
Directional Shadow  → 태양 직사광 가시성
SkyLight            → 하늘 노출 기반 간접 밝기
BlockLight          → Torch / Emissive Local Light
```

#### Shadow

- 2048 × 2048 Shadow Map
- `R32_TYPELESS` Texture
- `D32_FLOAT` DSV / `R32_FLOAT` SRV
- 3 × 3 PCF
- `SampleCmpLevelZero`
- Shadow Bias / Shadow Floor runtime tuning

#### Voxel Light Channel

Chunk Vertex는 Tint와 Light를 별도 채널로 전달합니다.

```cpp
struct VERTEX_CHUNK
{
    XMFLOAT3 position;
    XMFLOAT3 normal;
    XMFLOAT2 uv;
    XMFLOAT4 color; // RGB = Tint
    XMFLOAT2 light; // X = BlockLight, Y = SkyLight
};
```

SkyLight는 Heightmap 기반 Terrain 구조에 맞춰 Column별 `SkyOccluderY`를 저장하고 Vertical Sky Exposure를 판정합니다.

BlockLight는 0~15 단계의 BFS propagation을 사용하며, Vertex Light를 통해 면 내부에서 보간됩니다.

---

### 5. Runtime Diagnostics / Profiling

실행 중 병목을 바로 확인할 수 있도록 Runtime 계측을 프로젝트에 포함했습니다.

#### ImGui / CDebugCollector

- Frame / Update / LateUpdate
- Render Build / Render Execute / Present
- Streaming / Preload / Hotload
- Meshing / Mesh Build / Mesh Upload
- Loaded / Dirty / Rebuild Queued Sections
- RenderItem / DrawCall
- Pipeline / Material / Mesh Bind
- Frustum Test / Culled Count

#### F4 Cruise Stress Mode

플레이어 높이를 고정하고 이동 속도를 높여 Streaming / Load / Unload / Mesh Rebuild가 반복되는 상황을 의도적으로 재현합니다.

일반 플레이에서 Spike를 기다리지 않고 같은 종류의 Runtime workload를 반복적으로 관찰하기 위한 Stress Scenario입니다.

#### Optick

상세 CPU 분석이 필요할 때 `OPTICK_PROFILING`을 활성화한 profiling build에서 Hot Path와 Call Stack을 확인할 수 있도록 구성했습니다.

---

## Architecture Overview

```text
CWindowSystem
    │ Win32 Message Loop
    ▼
Application
    │ Frame Orchestration
    ▼
CGameWorld
    │ Scene / GameTime
    ▼
CGameScene
    ├───────────────► CWorld / CChunkWorld
    │                  Streaming / Block / Light / Meshing Input
    │
    └─ Submit(RenderItem)
            ▼
       CRenderWorld
       D3D11 Resources / Frame Constants
            ▼
       CRenderManager
            ▼
       CRenderFrame
       Pass Buckets / Draw
            ▼
       Direct3D 11
```

Main frame flow:

```text
_BeginFrame
→ _RunGameFrame
→ _CommitFrameFence
→ _BuildRenderFrame
→ _ExecuteRenderFrame
→ _EndFrame
```

Render Resource는 Initialize 단계에서 생성하고, Game Scene에서는 생성된 Resource를 참조하는 방식으로 관리합니다.

---

## Gameplay / Runtime Features

- Streaming Voxel World
- Block place / break interaction
- Destroy crack stages
- Block hit / break particles
- Hotbar / inventory selection
- AABB collision / jump
- Sprint + FOV transition
- Head bob / footstep / landing sound
- Day / Night cycle
- Sun / Moon billboard
- Cloud layer
- Directional Shadow
- SkyLight / BlockLight / Torch
- Transparent / Cutout block rendering
- Raw Input keyboard / mouse
- DualSense input support
- FMOD 2D / 3D audio
- ImGui runtime debug overlay

---

## Controls

### Keyboard / Mouse

| Input | Action |
| --- | --- |
| `W A S D` | Move |
| Mouse | Look |
| `Space` | Jump |
| `Shift` | Sprint |
| Left Mouse | Break Block |
| Right Mouse | Place Block |
| Mouse Wheel | Change Hotbar Slot |
| `Tab` | Toggle UI / Mouse Lock |
| `Home` | Switch Keyboard/Mouse ↔ Gamepad |
| `F2` | Toggle Chunk Bounds |
| `F3` | Cycle Section Debug Mode |
| `F4` | Cruise Stress Mode + Debug Overlay |
| `ESC` | Exit |

### DualSense

- Left Stick: Move
- Right Stick: Look
- Cross: Jump
- L2: Break
- R2: Place
- L1 / R1: Hotbar Previous / Next
- L3: Sprint
- Share: Switch Input Device

---

## Tech Stack

### Core

- C++20
- Direct3D 11
- Win32
- Visual Studio 2022 / v143
- Windows SDK

### Graphics / Runtime

- HLSL
- DirectXMath
- DirectXTK
- DirectXTex
- Dear ImGui
- Optick (optional profiling build)

### Data / Audio

- RapidJSON
- FMOD

---

## Build & Run

### Requirements

- Windows 10 / 11
- Visual Studio 2022
- x64 target
- C++20
- Required external libraries under the project's `Libraries` configuration

Build configuration:

```text
x64 / Debug
x64 / Release
```

### Runtime Directory

Current Release path configuration expects the executable working directory to contain:

```text
DX11Base.exe
Shader/
Resource/
required runtime DLLs
```

The latest `main` includes Release path handling for `Shader/` and `Resource/` so the packaged executable can run from the distribution directory.

The public repository is source-focused; executable/runtime resources are distributed separately with the portfolio package.

---

## Repository Notes

This project is intentionally focused on a small number of engine/client topics rather than attempting to reproduce a complete commercial engine.

The main areas of continued study are:

- Runtime resource representation and ownership
- Streaming workload scheduling
- CPU-side meshing cost
- Render submission and state changes
- Lighting / Shadow data flow
- Runtime measurement-driven optimization

---

## Author

**FXfireKR / 강원석**

C++ live client development experience + DirectX 11 client/engine architecture study project.
