<!-- PROJECT LOGO -->
<br />
<h3 align="center">LEntity</h3>

  <p align="center">
    A sparse set implementation of an Entity Component System. 
  </p>
</div>



<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project

LEntity is an implementation of an Entity Component System, which is a data-oriented game design pattern which allows for a flexible and fast way to create Entities when making a game, and attach Components to them. It was originally created as part of a small game engine I am developing called Labyrinth, however I decided to separate it for easier development and testing. 

LEntity uses a sparse set implementation which allows for components to be stored contiguously in memory. Each component has its own manager which stores the components in chunks in order to allow for an uncapped number of entities with components stored dynamically and efficiently, rather than hogging up lots of memory in order to reserve space for all the possible components.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- GETTING STARTED -->
## Getting Started

LEntity currently only supports premake, but it would probably not be too hard to add support for another build system. I intend to look into this in the future.

### Prerequisites

LEntity requires:
* C++17
* A compiler that supports the `__FUNCSIG__` or `__PRETTY_FUNCTION__` preprocessor macros.

### Installation

1. Clone the repo
   ```sh
   git clone https://github.com/amayesingnathan/LEntity.git
   ```
2. Run the setup script in the scripts folder. Currently only windows batch and linux bash scripts are provided.
   ```sh
   ./scripts/setup.sh
   ```
3. Follow prompts to download premake on first installation. This will generate project files for windows or linux.

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- USAGE EXAMPLES -->
## Usage

In development...

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- ROADMAP -->
## Roadmap

- [ ] Groups
- [ ] Improved testing and benchmarking
- [ ] Expand registry API

See the [open issues](https://github.com/amayesingnathan/LEntity/issues) for a full list of proposed features (and known issues).

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CONTRIBUTING -->
## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion or feedback that would make this better, please open an issue with the tag "enhancement". Feel free to fork the repo and make a pull request, however this is only a personal project so I may not respond quickly.

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE` for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>
