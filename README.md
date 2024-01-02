# Fembng

Fembng is a project that allows you to generate and view images with a rainbow gradient effect (for now because i'm to lazy)

## Installation

Follow these steps to install and run the Fembng project:

### Prerequisites

- Make sure you have `gcc` and `sdl2` installed.

### Clone the Repository

```bash
git clone https://github.com/MignonPetitXelow/Fembng.git ; cd Fembng
```

### Build the Project

```bash
make
```
### Run Fembng

To generate an image:
```bash
./bin/fembng -w <width> -h <height> -o <output_file>
```

Example:
```bash
./bin/fembng -w 800 -h 600 -o output
```

### Run Fembng Viewer

To view a generated image:
```bash
./bin/fembngapp <image_file>
```

Example:
```bash
./bin/fembngapp output.femboy
```

### Contributing

Feel free to contribute to the project by opening issues or creating pull requests.
