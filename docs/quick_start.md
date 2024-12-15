# Quick start

## Use sc-machine as a C++ library in your project

### Conan

You can use Conan to install sc-machine. To do this you need to create `conanfile.txt` in the root of the project and add to it:

```ini
[requires]
sc-machine/<version>
```

Then run the following command in the project root:

```sh
conan install .
```

Import sc-machine targets into your CMake project by using:

```cmake
find_package(sc-machine REQUIRED)
```

Start building! Refer to our [C++ Guide](sc-memory/api/cpp/guides/simple_guide_for_implementing_agent.md) on how to quickly develop an sc-machine agent in C++ from scratch.

### GitHub Releases
You can download pre-built artifacts from [GitHub Releases](https://github.com/ostis-ai/sc-machine/releases). Extract it to any location, then make it available to CMake by appending folder path to `CMAKE_PREFIX_PATH`:

```cmake
set(FABULOUS_PROJECT_SC_MACHINE_PATH "/path/to/extracted/sc-machine" 
    CACHE PATH "sc-machine installation path"
)

list(APPEND CMAKE_PREFIX_PATH ${FABULOUS_PROJECT_SC_MACHINE_PATH})
```

Find more info on installation methods on the [build system page](build/build_system.md) (including some more advanced snippets for CMake).

## Install sc-machine as a service

It's possible to use sc-machine through a WebSocket API. Docs for our WebSocket API clients in [Python](https://github.com/ostis-ai/py-sc-client) and [TypeScript](https://github.com/ostis-ai/ts-sc-client) are available in the respective repositories. You might want to check [sc-web](https://github.com/ostis-ai/sc-web) for a more user-friendly interface as well.

### Docker

We provide a Docker image for this project. Here's a quick snippet on how to launch it.

```sh
# clone the repo
git clone https://github.com/ostis-ai/sc-machine
cd sc-machine
# create empty knowledge base sources folder
mkdir kb
# note: at this stage you can move your KB sources to the ./kb folder

# build knowledge base
docker compose run --rm machine build
# run sc-machine
docker compose up
```
  
Head to [Installing with Docker](https://ostis-ai.github.io/sc-machine/docker) to learn more. Using Docker is the recommended way to deploy sc-machine in production.

### Native

Note: currently, running sc-machine natively on Windows isn't supported.

Download the [sc-machine release](https://github.com/ostis-ai/sc-machine/releases) for your platform and run it on your system:

```sh
# create empty knowledge base sources folder
mkdir kb
# note: at this stage you can move your KB sources to the ./kb folder

cd sc-machine-<version>-<platform>
# build knowledge base
./bin/sc-builder -i ../kb -o ../kb.bin --clear
# run sc-machine
./bin/sc-machine -e lib/extensions -s ../kb.bin 
# kb.bin will become your KB persistence folder
```

In case you want to make changes to the project sources, please refer to the [build system docs](build/build_system.md).

## Config

This repository provides a default configuration for the sc-machine. To customize the _sc-machine_ to suit your needs you can [create your own config file](build/config.md).