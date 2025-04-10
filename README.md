# AcmeHub

A RESTful API backend for batch process duration collection.

## Build

On Debian 12 (Bookworm), to build:

```
sudo apt-get install git cmake g++ qt6-httpserver-dev qt6-websockets-dev
git clone https://github.com/pyksy/AcmeHub.git
cmake -S AcmeHub -B AcmeHub-Build -DCMAKE_BUILD_TYPE:STRING=Release
cmake --build AcmeHub-Build --parallel --config Release
```
Replace "Release" with "Debug" to get a debug build.

## Run

Run the binary after build:

```
AcmeHub-Build/AcmeHub
```

Optional: Add some test data while the AcmeHub is running:
```
bash AcmeHub/generate_test_data.sh
```

## TODO

- Add truckloads of error checking and handling
- Add authentication
- Use proper SQL database, with proper table forms and indices
- Add unit tests
- Add CI/CD pipeline with proper test automation
- Write proper documentation
- Add cmdline options
- Add configuration file

Some *italic* and **bold** text and `inline code`.

An empty line starts a new paragraph.

Use two spaces at the end  
to force a line break.

A horizontal ruler follows:

---

Add links inline like [this link to the Qt homepage](https://www.qt.io),
or with a reference like [this other link to the Qt homepage][1].

    Add code blocks with
    four spaces at the front.

> A blockquote
> starts with >
>
> and has the same paragraph rules as normal text.

First Level Heading in Alternate Style
======================================

Paragraph.

Second Level Heading in Alternate Style
---------------------------------------

Paragraph.

[1]: https://www.qt.io
