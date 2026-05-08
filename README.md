# WSI - Windows Search Improved

WSI is a high-performance, low-latency application launcher and system search utility for Windows. Engineered as a robust upgrade to the original [Windows Searcher](https://github.com/RoiRomem/Windows-Searcher), WSI is built in modern C++20 utilizing Dear ImGui and DirectX 11 to provide instant, system-wide access to applications through a non-intrusive, hardware-accelerated overlay.

## Key Features

### 🚀 Low-Latency Search & Execution
- **Intelligent Caching:** Implements a dynamic caching system to ensure instant query resolution with minimal CPU overhead.
- **Advanced Heuristics:** Utilizes optimized string matching, prefix-based filtering, and a memory-efficient Levenshtein distance algorithm for highly accurate fuzzy matching.
- **Resource Efficient:** Designed for a minimal memory footprint and near-zero background CPU utilization.

### 🎯 Dynamic File System Indexing
- **Automated Directory Scanning:** Asynchronously indexes critical Windows environments:
  - Start Menu Programs (`%APPDATA%` and `%PROGRAMDATA%`)
  - User Desktop (`%USERPROFILE%\Desktop`)
- **Multi-Format Resolution:** Parses and resolves `.exe`, `.lnk`, and `.url` targets.
- **Priority-Based Execution:** Intelligently prefers direct executables over shortcuts to minimize launch latency.

### ⚡ Seamless System Integration
- **Global Input Hook:** Registers a low-level Win32 system hotkey (`Ctrl + Space`) for instant interface toggling from any application.
- **Always-on-Top Overlay:** Bypasses standard window management to render an immediate, workflow-friendly overlay.
- **Hardware-Accelerated UI:** Built on DirectX 11 for DPI-aware scaling, smooth framerates, and responsive interactions.

### 🔍 Extensible Command Parser
- **Application Routing:** Instantly locate and execute installed binaries.
- **System Commands:** Direct integration with core Windows utilities (e.g., `cmd`, `calc`).
- **Web & URI Navigation:** Automatically detects and routes URLs or fallbacks to a default web search when local queries return null.

## Technical Architecture

WSI is structured around a modular, performance-oriented architecture:

- **App Core:** Manages the primary application controller, Win32 window lifecycle, and the DirectX 11 rendering pipeline.
- **InputBuf:** Handles asynchronous input processing and renders the immediate-mode GUI via Dear ImGui.
- **CacheFind & Searcher:** The search engine backend. `Searcher` handles deep file system traversal, while `CacheFind` manages the in-memory index for $O(1)$ lookup times during active typing.
- **SettingsMenu & JSON Integration:** Provides persistent, dynamically loaded user configurations serialized via JSON.

## Installation & Deployment

### Quick Start (Pre-compiled Binary)
1. Download the latest `WSI.exe` from the [Releases](https://github.com/yourusername/WSI/releases) page.
2. Execute the binary. WSI will autonomously:
   - Index the local file system.
   - Hook the global system hotkey (`Ctrl + Space`).
   - Daemonize into the background.
3. Press `Ctrl + Space` to invoke the overlay.

**System Requirements:**
- Windows 10/11 (64-bit)
- DirectX 11 capable GPU hardware

### Building from Source
**Prerequisites:**
- Visual Studio 2019 or later (C++20 standard required)
- Windows SDK

**Build Instructions:**
1. Clone the repository:
   ```bash
   git clone [https://github.com/yourusername/WSI.git](https://github.com/yourusername/WSI.git)
   cd WSI
   ```
2. Open the `.sln` file in Visual Studio.
3. Set the target to `Release` / `x64`.
4. Build Solution (`Ctrl + Shift + B`).

## Usage Guide

| Shortcut/Command | Action |
|------------------|--------|
| `Ctrl + Space` | Toggle the WSI overlay |
| `↑` / `↓` | Navigate the index results |
| `Enter` | Execute the selected process |
| `Escape` | Dismiss the overlay |
| `settings` | Open the internal configuration GUI |
| `exit` | Terminate the WSI background process |

## Troubleshooting & Common Issues

**Issue: Graphical artifacts, flickering, or corrupted UI rendering.**
* **Root Cause:** Interference from the NVIDIA GeForce Experience in-game overlay hooking into WSI's DirectX 11 swap chain. Because WSI is a lightweight, hardware-accelerated application, the NVIDIA overlay attempts to inject its recording UI over it, causing rendering conflicts.
* **Resolution:** 1. Open **NVIDIA GeForce Experience**.
  2. Click the Settings (gear) icon in the top right.
  3. Navigate to the **General** tab.
  4. Find the **In-Game Overlay** section and click **Settings**.
  5. Go to **Notifications** and turn **OFF** the "Press Alt+Z to share your gameplay" notification (or disable the In-Game Overlay entirely).
  6. Restart WSI.

## Contributing

Engineering contributions are welcome. Please ensure any pull requests adhere to modern C++ standards and do not introduce performance regressions in the search algorithms. 

1. Fork the repository
2. Create a feature branch
3. Implement changes (ensure memory safety and thread safety)
4. Submit a Pull Request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments
- Built upon the conceptual foundation of the original [Windows Searcher](https://github.com/RoiRomem/Windows-Searcher).
- UI rendering powered by [Dear ImGui](https://github.com/ocornut/imgui).

## Development Roadmap & Changelog

### Current Status
- **Core Architecture:** Rewrite in C++20 and Dear ImGui is 100% complete.
- **Active Development:** Expanding algorithmic efficiency and extensibility.

### Version History
- **v1.0 (Stable)** - Optimizations to searching algorithm, refined settings GUI, inclusion of applications downloaded from Microsoft store, logging system for contributors.
- **v0.5b (Stable)** - Optimizations to the rendering pipeline, refined search GUI, implementation of custom command routing, and memory leak resolutions.
- **v0.4b** - Implemented customizable global keybind hooks.
- **v0.3b** - Integrated in-app persistent settings and caching optimizations.
- **v0.2b** - Added fullscreen focus blocking, advanced expression calculator parser, and JSON serialization.
- **v0.1b** - Initial C++20 architectural rewrite. Implemented DX11 DPI handling and core Levenshtein matching.