## Ctemplate
Project template manager in the command-line.

<details>
   <summary>Table Of Contents</summary>
   <ol>
      <li>
         <a href="#about-the-project">About The Project</a>
         <ul>
            <li><a href="#features">Features</a></li>
            <li><a href="#built-with">Built With</a></li>
         </ul>
      </li>
      <li>
         <a href="#installation">Installation</a>
         <ul>
            <li><a href="#release-files">Release Files</a></li>
         </ul>
      </li>
      <li>
         <a href="#usage">Usage</a>
      </li>
   </ol>
</details>

## About The Project
This program was created to simplify the process of configuring projects. Ctemplate was designed to quickly initialize project templates, making them ready to use right away.

### Features
- **Lightweight:** Only uses up 5MB at most.
- **Versatile:** Works with any programming language.
- **Self-Contained:** Works as a standalone executable.
- **User-Friendly:** No programming/scripting needed. Only basic JSON knowledge is required.
- **Parameterization:** Customize your template generation with variables.

### Built With
- [CLI11](https://github.com/CLIUtils/CLI11.git) - for parsing command-line arguments.
- [json](https://github.com/nlohmann/json.git) - for parsing and manipulating JSON files.
- [OS](https://github.com/Scrappyz/OS.git) - for file operations.
- [fmatch](https://github.com/Scrappyz/fmatch.git) - for pattern matching.
- [format](https://github.com/Scrappyz/format.git) - for formatting strings into tables.

## Installation
Ctemplate works as a standalone executable so all you need to do is download it then run it in your preferred terminal.

### Release Files
| File | Description |
| :--- | :--- |
| [ctemplate.exe]() | Windows standalone executable (recommended for **Windows**) |

## Usage
```
ctemplate [OPTIONS] [SUBCOMMAND]
```

### Subcommands
```
init                        Initialize a template
add                         Add a new template
remove                      Remove an existing template
list                        List all templates
info                        Show info about a template
config                      Show config
```

### Initializing A Template
This is achieved using the `init` subcommand. 
```
Positionals:
  name TEXT REQUIRED          Name of the template to initialize

Options:
  -h,--help                   Print this help message and exit
  -p,--path TEXT              Path to initialize to
  -f,--force                  Force overwrite directory
  -v,--variables TEXT ...     Initialize variables
  -i,--include TEXT ...       Include paths
  -e,--exclude TEXT ...       Exclude paths
```
