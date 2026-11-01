# Installation guide (for a school technician)

## 1. What you need

- A Windows, Linux or macOS computer used only on the school network (or offline)
- The project source (clone from GitHub or unzip the release package)
- A C++17 compiler:
  - **Windows:** install [MSYS2](https://www.msys2.org/) then:
    `pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake`
  - **Linux:** `sudo apt install g++ cmake`
  - **macOS:** Xcode Command Line Tools or Homebrew `gcc` / `cmake`

## 2. Get the software

The `v1.0` release tag has not been published yet. To build the current project,
clone the repository and use its default branch:

```bash
git clone https://github.com/katlehochristophersenqhe-maker/CS4433-LMS.git
cd CS4433-LMS
```

When a versioned release is published, you can instead check out its tag (for
example, `git checkout v1.0`). Alternatively, unzip a published release
archive and open a terminal in that folder.

## 3. Build

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

Alternative without CMake (MinGW / Git Bash):

```bash
mkdir -p build
gcc -c third_party/sqlite3.c -o build/sqlite3.o -DSQLITE_THREADSAFE=1 -O2
g++ -std=c++17 -I include -I third_party -O2 \
  src/domain/*.cpp src/infra/*.cpp src/app/*.cpp src/ui/ConsoleUI.cpp src/main.cpp \
  build/sqlite3.o -o build/lms
cp sql/schema.sql build/
```

On Windows the program may be named `lms.exe`.

## 4. First run

```bash
cd build
./lms
```

(or `lms.exe` on Windows)

- The program creates `lms.db` in the current folder.
- On first run it seeds demo accounts (see user manuals).
- For production, change all default passwords immediately.

## 5. Backup

Copy the file `lms.db` to a safe place regularly (USB or school server). That file holds all users, courses, submissions and marks.

## 6. Uninstall

Delete the project folder and any copies of `lms.db`. No registry keys or system services are installed.

## 7. Troubleshooting

| Problem | What to try |
|---------|-------------|
| `g++` not found | Install MinGW/MSYS2 or build-essential |
| Schema file not found | Run from `build/` after copying `schema.sql` |
| Database locked | Close other copies of the program |
| Login fails | Check Caps Lock; reset via administrator if needed |
