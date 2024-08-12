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

It is recommended to place the executable inside an empty directory.

## Setup
Upon first running the program, a `config.json` file will generate inside the directory. Open it with your preferred text editor then modify the `templateDirectory` to where you want your templates to be stored. 

<img src="docs/resources/images/config_json.png" alt="config" width="800" height="400"/>

<br>

Changing this configuration can also be achieved using the `config set` subcommand.

```
ctemplate config set templateDirectory="path/to/template/dir"
```

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

### Adding a template
This is achieved using the `add` subcommand.

```
Usage: ctemplate add [OPTIONS] [path]

Positionals:
  path TEXT                   Root project directory of
                              the template to be added
                              (defaults to the current path)

Options:
  -h,--help                   Print this help message and exit
  -n,--name TEXT REQUIRED     Name of the new template
  -a,--author TEXT            Author of the new template
  -d,--desc TEXT              Description of the new template
```

Create your template project then navigate to that projects root directory. Run the `add` subcommand then supply a name for the template with the `-n,--name` option. You can also add an author and description to that template with the `-a,--author` and `-d,--desc` respectively. The template will be then copied to your template directory (configured in the `config.json` file) with a new folder inside it (defaults as `.ctemplate`). This folder is where all the information about the template is stored.

#### Example
<img src="https://media.giphy.com/media/vFKqnCdLPNOKc/giphy.gif" width="500" height="500" />

### Listing Templates
This is achieved with the `list` subcommand.

```
Usage: ctemplate list [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
```

If you followed the steps correctly in the [Adding a template](#adding-a-template) section. The template you added should appear in the listed templates.

### Initializing a template
This is achieved using the `init` subcommand. 

```
Usage: ctemplate init [OPTIONS] name

Positionals:
  name TEXT REQUIRED          Name of the template to initialize.
                              Use the 'list' subcommand to see available templates

Options:
  -h,--help                   Print this help message and exit
  -p,--path TEXT              Path to initialize the template to
                              (defaults to the current path)
  -f,--force                  Overwrites the entire directory
                              with the template
  -v,--variables TEXT ...     Set variable values
                              (E.g: projectName="Hello World")
  -i,--include TEXT ...       Paths to include in the
                              template when initializing
                              (E.g: "project/main.py")
  -e,--exclude TEXT ...       Paths to exclude in the
                              template when initializing
                              (E.g: "project/main.py")
```

Use the `-v,--variable` option to set the variables of the template you want to initialize.
