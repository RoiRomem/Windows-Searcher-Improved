# WSI - Windows Search Interface

WSI is a fast, lightweight application launcher for Windows that provides instant access to your installed applications through a sleek, overlay-style interface. It's an improved version of the original [Windows Searcher](https://github.com/RoiRomem/Windows-Searcher) project, built with modern C++ and Dear ImGui for a smooth user experience.

## Features

### 🚀 **Lightning Fast Search**
- Intelligent caching system for instant results
- Optimized string matching with prefix-based filtering
- Memory-efficient Levenshtein distance algorithm for fuzzy matching

### 🎯 **Smart Application Discovery**
- Automatically scans common Windows directories:
  - Start Menu Programs
  - Desktop shortcuts
  - System-wide program directories
- Supports multiple file types: `.exe`, `.lnk`, `.url`
- Priority-based file resolution (prefers .exe over .lnk over .url)

### ⚡ **Quick Access**
- **Global hotkey**: `Ctrl + Space` to toggle the interface
- **Keyboard navigation**: Arrow keys to navigate results
- **Instant launch**: Enter to execute, Escape to hide
- Always-on-top overlay that doesn't interfere with your workflow

### 🎨 **Modern Interface**
- Clean, dark-themed UI built with Dear ImGui
- DPI-aware scaling for high-resolution displays
- Smooth animations and responsive design
- Minimal resource footprint

### 🔍 **Versatile Input Handling**
- **Application search**: Type to find installed programs
- **Command execution**: Run system commands directly
- **URL navigation**: Open websites by typing URLs
- **Web search**: Search the web when no local matches found

## Installation

### Quick Start (Recommended)
1. Download the latest `WSI.exe` from the [Releases](https://github.com/yourusername/WSI/releases) page
2. Run the executable - WSI will automatically:
   - Scan your system for applications
   - Register the global hotkey (`Ctrl + Space`)
   - Start running in the background
3. Press `Ctrl + Space` to begin using WSI!

**System Requirements:**
- Windows 10/11
- DirectX 11 capable graphics hardware

### Building from Source
For developers who want to build from source:

**Prerequisites:**
- Visual Studio 2019 or later with C++17 support
- Windows SDK

**Steps:**
1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/WSI.git
   cd WSI
   ```

2. Open the solution in Visual Studio

3. Build the project (Release configuration recommended)

4. Run the executable

## Usage

### Basic Operations
1. **Activate**: Press `Ctrl + Space` anywhere in Windows
2. **Search**: Start typing to find applications
3. **Navigate**: Use `↑` and `↓` arrow keys to select results
4. **Launch**: Press `Enter` to run the selected item
5. **Hide**: Press `Escape` or `Ctrl + Space` to hide the interface

### Advanced Features
- **Direct Commands**: Type commands like `cmd`, `notepad`, or `calc`
- **Web URLs**: Type URLs like `google.com` or `https://github.com`
- **Web Search**: Type any text - if no local matches found, it becomes a web search
- **Exit**: Type `exit` to close WSI completely
- **Settings**: Type `settings` to access the WSI settings panel

### Keyboard Shortcuts
| Key | Action |
|-----|--------|
| `Ctrl + Space` | Toggle WSI interface |
| `↑` / `↓` | Navigate search results |
| `Enter` | Execute selected item |
| `Escape` | Hide interface |

## Technical Architecture

### Core Components
- **App.cpp**: Main application controller and rendering loop
- **InputBuf**: Input handling and UI rendering with Dear ImGui
- **CacheFind**: Intelligent search and caching system
- **Searcher**: File system scanning and application discovery

### Key Technologies
- **Dear ImGui**: Immediate mode GUI framework
- **DirectX 11**: Hardware-accelerated rendering
- **Win32 API**: System integration and hotkey handling
- **STL**: Modern C++ containers and algorithms

### Performance Optimizations
- Incremental cache building for responsive typing
- Memory-efficient string matching algorithms
- Background application scanning
- Minimal CPU usage when idle

## Configuration

WSI automatically configures itself but scans these directories by default:
- `%APPDATA%\Microsoft\Windows\Start Menu\Programs`
- `%USERPROFILE%\Desktop`
- `C:\ProgramData\Microsoft\Windows\Start Menu\Programs`

## Contributing

Contributions are welcome! Please feel free to submit issues, feature requests, or pull requests.

### Development Setup
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly on different Windows versions
5. Submit a pull request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Built on the foundation of the original [Windows Searcher](https://github.com/RoiRomem/Windows-Searcher) project
- Powered by [Dear ImGui](https://github.com/ocornut/imgui) for the user interface
- Uses Segoe UI as a default font

## Changelog

### v0.5b (WSI) - Stable release 🎉
- Bug fixes 🐛
- Optimizations 🤓
- Better search GUI 😍
- Custom commands 🤖

### v0.4b (WSI)
- Custom keybindings ️⌨️

### v0.3b (WSI)
- Bug fixes 🐛
- Optimizations 🤓
- In app settings ⚙️

### v0.2b (WSI)
- Bug fixes 🐛
- Fullscreen block 🚀
- In app advance calculator 🖩
- JSON settings file ⚙️

### v0.1b (WSI)
- Improved performance and responsiveness  
- Modern C++17 codebase
- Enhanced caching system
- Better DPI handling
- Cleaner, more intuitive interface

## Future

### Status
- 100% done with the rewrite in **Dear ImGui** 🎉
- Working on new Features (check Future features) 🤩

### Future features
- Custom directories
- Lua integration for mods?
- Community suggestions