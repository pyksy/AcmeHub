# AcmeHub

A RESTful API backend for batch process duration collection.

## Build

On Debian 12 (Bookworm), to build:

```
sudo apt-get install --install-recommends git cmake g++ qt6-httpserver-dev qt6-websockets-dev
git clone https://github.com/pyksy/AcmeHub.git
cmake -S AcmeHub -B AcmeHub-Build -DCMAKE_BUILD_TYPE:STRING=Release
cmake --build AcmeHub-Build --parallel --config Release
```
Replace "Release" with "Debug" to get a debug build.

## Run

Run the binary:

```
AcmeHub-Build/AcmeHub
```

The runtime dependencies are Qt6 w/ httpserver and sql libraries:
```
sudo apt-get install --install-recommends libqt6httpserver6 libqt6sql6
```

Optional: Generate some test data (have the binary running):
```
sudo apt-get install curl
bash AcmeHub/generate_test_data.sh
```

## TODO

- Design overhaul
- Add truckloads of error checking and handling
- Get rid of magic numbers and hardcoded stuff
- Add authentication
- Use proper SQL database, with proper table forms and indices
- Add cmdline options
- Add unit tests
- Add CI/CD pipelines with proper test automation
- Write proper documentation
- ...endless list of other things...

## FAQ

- Q: The code looks ugly. Was this thing created by someone who hasn't written Qt C++ code in over twelve years?
- A: Yes...

- Q: Or coded anything in a real OO programming language in over ten years? Some dubious design choices in there...
- A: ...

- Q: Then why bother?
- A: For personal challenge, to see if I still have the skills.

- Q: Was it fun?
- A: Hell yes!
