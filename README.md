# AcmeHub

A RESTful API backend application for collecting processing time data.

## Build

To build on Debian/Ubuntu, run:

```
sudo apt-get install --install-recommends git cmake g++ qt6-httpserver-dev qt6-websockets-dev
git clone https://github.com/pyksy/AcmeHub.git
cmake -S AcmeHub -B AcmeHub-Build -DCMAKE_BUILD_TYPE:STRING=Release
cmake --build AcmeHub-Build --parallel --config Release
```
Replace "Release" with "Debug" to get a debug build.

Tested on Debian 12 (Bookworm) and Ubuntu 24.04 (Noble Nubmat).

## Run

Run the binary:

```
AcmeHub-Build/AcmeHub
```
Hit CTRL-C to exit.

Optional: Generate some test data (must have the binary running):
```
sudo apt-get install curl
bash AcmeHub/generate_test_data.sh
```

FYI: The runtime dependencies are Qt6 with httpserver and sql libraries:
```
sudo apt-get install --install-recommends libqt6httpserver6 libqt6sql6
```


## Use

This application provides a simple RESTful API to manage batch processing reports.
By default it listens to port 4080 for requests, on all interfaces.
Edit the #defines in main.cpp and recompile to change the port and/or listen address.

## API endpoints

### POST /process_report
Send a HTTP POST request that contains the following properties as a JSON object:
- "server_name": Server name as text
- "start_time": ISO 8601 formatted processing start timestamp, in Zulu time (UTC), as text
- "end_time": ISO 8601 formatted processing end timestamp, in Zulu time (UTC), as text

Reply: This API endpoint assumes that the JSON in POST data is valid and
always returns an empty JSON object with a 200 OK HTTP response.

Example POST data JSON object:
```
{
  "server_name": "t-31337",
  "start_time": "2025-04-09T00:00:00Z",
  "end_time": "2025-04-09T00:00:29Z"
}
```

Example cURL call:
```
curl -s -d '{"server_name":"t-31337","start_time":"2025-04-09T00:00:00Z","end_time":"2025-04-09T00:00:29Z"}' http://127.0.0.1:4080/process_report
```

### GET /process_statistics

Send a HTTP GET request without parameters to get a report of the mean average and standard deviation across all servers and processing times as a JSON object:
- "mean": Mean average of processing times in seconds, rounded to the nearest, as text
- "stddev": Standard deviation of processing times in seconds, rounded to the nearest, as text

Reply: The mean and stddev values in a JSON object with a 200 OK HTTP response, 
or if there are less than 10 processing reports available,
an error message in a JSON object with a HTTP 500 Internal Server Error HTTP response.

Example JSON object in reply:
```
{
  "mean": "607",
  "stddev": "31"
}
```
Example cURL call:
```
curl -s http://127.0.0.1:4080/process_statistics
```

### GET /process_outliers

Send a HTTP GET request without parameters to get a report of server names
for which the processing time has deviated more than 3 standard
deviations from the mean average processing time, as a JSON array.

Reply: A JSON array of server names, determined to be outliers 
based on their processing time, as text, with a 200 OK HTTP response.

Example JSON array in reply:
```
[
  "t-101",
  "t-31337",
  "t-64738",
  "t-42"
]

```
Example cURL call:
```
curl -s http://127.0.0.1:4080/process_outliers
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

- Q: The code looks ugly. Was this thing created by someone who hasn't written Qt C++ code in nearly 12 years?
- A: Yes...

- Q: Or developed anything in a real OO programming language in ten years? Some dubious design choices in there...
- A: ...

- Q: Then why bother?
- A: For personal challenge. I wanted to see if I have any programming skills left.

- Q: Was it fun?
- A: Hell yes!
