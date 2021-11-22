# ESP32 Remote Car

### ESP32

* ![#FF00FF](https://via.placeholder.com/15/FF00FF/000000?text=+) L298N Front
* ![#FF8000](https://via.placeholder.com/15/FF8000/000000?text=+) L298N Back

|Module PIN                                                         |ESP32 PIN|ESP32 PIN|Module PIN                                                         |
|:----------------------------------------------------------------- |:-------:|:-------:|:----------------------------------------------------------------- |
|                                                                   |3V3      |VIN      |                                                                   |
|                                                                   |GND      |GND      |                                                                   |
|                                                                   |D15      |D13      |                                                                   |
|                                                                   |D2       |D12      |                                                                   |
|                                                                   |D4       |D14      |                                                                   |
|![#FF00FF](https://via.placeholder.com/15/FF00FF/000000?text=+) IN1|RX2      |D27      |![#FF8000](https://via.placeholder.com/15/FF00FF/000000?text=+) IN1|
|![#FF00FF](https://via.placeholder.com/15/FF00FF/000000?text=+) IN2|TX2      |D26      |![#FF8000](https://via.placeholder.com/15/FF00FF/000000?text=+) IN2|
|                                                                   |D5       |D25      |![#FF8000](https://via.placeholder.com/15/FF00FF/000000?text=+) IN3|
|![#FF00FF](https://via.placeholder.com/15/FF00FF/000000?text=+) IN3|D18      |D33      |![#FF8000](https://via.placeholder.com/15/FF00FF/000000?text=+) IN4|
|![#FF00FF](https://via.placeholder.com/15/FF00FF/000000?text=+) IN4|D19      |D32      |                                                                   |
|                                                                   |D21      |D35      |                                                                   |
|                                                                   |RX0      |D34      |                                                                   |
|                                                                   |TX0      |VN       |                                                                   |
|                                                                   |D22      |VP       |                                                                   |
|                                                                   |D23      |EN       |                                                                   |

### L298N Front

|L298N PIN|Motor RIGHT                                                      |Motor LEFT                                                       |
|:-------:|:---------------------------------------------------------------:|:---------------------------------------------------------------:|
|OUT1     |![#FF0000](https://via.placeholder.com/15/FF0000/000000?text=+) +|X                                                                |
|OUT2     |![#000000](https://via.placeholder.com/15/000000/000000?text=+) -|X                                                                |
|OUT3     |X                                                                |![#FF0000](https://via.placeholder.com/15/FF0000/000000?text=+) +|
|OUT4     |X                                                                |![#000000](https://via.placeholder.com/15/000000/000000?text=+) -|

### L298N Back

|L298N PIN|Motor RIGHT                                                      |Motor LEFT                                                       |
|:-------:|:---------------------------------------------------------------:|:---------------------------------------------------------------:|
|OUT1     |![#FF0000](https://via.placeholder.com/15/FF0000/000000?text=+) +|X                                                                |
|OUT2     |![#000000](https://via.placeholder.com/15/000000/000000?text=+) -|X                                                                |
|OUT3     |X                                                                |![#FF0000](https://via.placeholder.com/15/FF0000/000000?text=+) +|
|OUT4     |X                                                                |![#000000](https://via.placeholder.com/15/000000/000000?text=+) -|

## library
`<folder>` `./example/<File Path>/lib/<Lib Name>`

* Download AsyncTCP.
  * <https://github.com/me-no-dev/AsyncTCP>.

```
git submodule add https://github.com/me-no-dev/AsyncTCP.git ./ESP32_RemoteCar/lib/AsyncTCP
git commit
git push
```

* Download ESPAsyncWebServer.
  * <https://github.com/me-no-dev/ESPAsyncWebServer>.

```
git submodule add https://github.com/me-no-dev/ESPAsyncWebServer.git ./ESP32_RemoteCar/lib/ESPAsyncWebServer
git commit
git push
```

* Download ESP32GithubOTA.
  * <https://github.com/Hsun1031/ESP32GithubOTA>.

```
git submodule add https://github.com/Hsun1031/ESP32GithubOTA.git ./ESP32_RemoteCar/lib/ESP32GithubOTA
git commit
git push
```