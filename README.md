## Setup
### For Arch Linux and derivatives
1. Installing Zephyr SDK and Python West
```bash
sudo yay -S python-west zephyr-sdk-bin python-pyelftools
```
2. Clone the project
```bash
git clone https://github.com/NU-Horizonsat/atlas_flight_board_zephyr
```
3. Initialize the project
```bash
cd atlas_flight_board_zephyr
west init
west update
west zephyr-export
```
4. Build the project
```bash
cd /apps/atlas_flight_board
west build -p auto -b rpi_pico
```
5. Copy the generated uf2 file to the Raspberry Pi Pico

### Using the development container
1. Clone the project
```bash
git clone https://github.com/NU-Horizonsat/atlas_flight_board_zephyr
```

2. Open the project in Visual Studio Code and press `Ctrl+Shift+P` and select `Remote-Containers: Reopen in Container`

3. Initialize the project
```bash
west init
west update
west zephyr-export
```

4. Build the project
```bash
cd /apps/atlas_flight_board
west build -p auto -b rpi_pico
```

5. Copy the generated uf2 file to the Raspberry Pi Pico

##### You can also install the Zephyr SDK and west tool by following the instructions in the [official documentation](https://docs.zephyrproject.org/latest/getting_started/index.html) or using the Zephyr IDE extension in VSCode by follow this [guide](https://github.com/mylonics/zephyr-ide/blob/main/docs/MANUAL.md).