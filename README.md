## Project Template
A lightweight C++ program that initialize projects from saved templates.

<!-- omit in toc-->
## Table Of Contents
- [Project Template](#project-template)
- [Table Of Contents](#table-of-contents)
- [Getting Started](#getting-started)
  - [Installation](#installation)
  - [Setup](#setup)
- [Usage](#usage)
  - [Initializing Templates](#initializing-templates)
    - [Examples](#examples)
  - [Adding Templates](#adding-templates)
    - [Examples](#examples-1)
  - [Deleting Templates](#deleting-templates)
    - [Examples](#examples-2)
  - [Editing Templates](#editing-templates)
    - [Examples](#examples-3)

## Getting Started
### Installation
1. Download latest release from `url`
2. Extract the `Project-Template` folder to your designated location
3. Copy the path to the `Project-Template` folder
4. Go to environmental variables
5. Add the copied path to the `Path`

You can now use `template.exe` in the command-line.

### Setup
1. Run `template.exe --setup`.
2. Enter the template directory

## Usage
```
Usage:
  template.exe <options> <values>
  template.exe <subcommand> <options>

Subcommands:
  init                             Initializes a template project
  add                              Add a new template
  remove                           Remove an existing template
  edit                             Edit an existing template

Options:
  -h, --help                       Print help menu
  -l, --list                       Print existing templates
  --version                        Print program version
  --set-template-dir               Set the directory to look for templates
  --setup                          Setup the program
```
### Initializing Templates
```
Usage:
  template.exe init <template> <options> [-p <path>]
  template.exe init <options>

Options:
  -h, --help                       Print help menu
  -l, --list                       Print existing templates
  -p, --path                       The path the template will be initialized on (default is the current path)
  -s, --skip-existing              Skip existing files
  -o, --overwrite-existing         Overwrite existing files
  -f, --force                      Overwrite whole directory
```
Use the `init` subcommand to initialize a template in the current directory. To use the `init` subcommand, you type in `template.exe init` then follow it up with the name of the template you want to initialize.

#### Examples
Let's say you want to initialize a template named `cpp-app`, you run:
```
template.exe init cpp-app
```
This will initialize the template named `cpp-app` to the current directory.

<br>

If you want to initialize the template to a certain directory, you run:
```
template.exe init cpp-app -p <path>
```
Where `<path>` is the directory you want to initialize the template on.

<br>

If you want to initialize the template and you want it to skip existing files, you run:
```
template.exe init cpp-app -s
```
The `-s` flag tells the program to skip all existing files. You can substitute the `-s` flag with `-o` to overwrite existing files or `-f` to overwrite the whole directory.

### Adding Templates
```
Usage:
  template.exe add <new-template> [-d <description>] [-p <path>]
  template.exe add <options>

Options:
  -h, --help                       Print help menu
  -p, --path                       The directory to add as a template (default is the current directory)
  -d, --desc                       Add a description
```
To add a new template, you use the `add` subcommand.

#### Examples

Let's say you want to add the current directory as a new template and you want to name it `my-template`, you run:
```
template.exe add my-template
```
This will add a new template to your template directory named `my-template`.

<br>

If you want to add a description to your new template, you can provide the description as a value of the `-d` flag:
```
template.exe add my-template -d "My description for this template"
```
The description can be seen in the template list when you run `template.exe -l`.

### Deleting Templates
```
Usage:
  template.exe remove <existing-template>
  template.exe remove <options>

Options:
  -h, --help                       Print help menu
  -l, --list                       Print existing templates
```
You can use the `remove` subcommand to delete existing templates. To delete an existing template, use the `remove` subcommand then follow it up with the name of the template you want to delete.

#### Examples
Let's say you want to delete the template named `cpp-app`, you run:
```
template.exe remove cpp-app
```
This will remove `cpp-app` from the template list.

### Editing Templates
```
Usage:
  template.exe edit <existing-template> <options> <values>
  template.exe edit <options>

Options:
  -h, --help                       Print help menu
  -l, --list                       Print existing templates
  -d, --desc                       Edit the description of a template
  -r, --rename                     Rename the template
```
You can edit your templates by using the `edit` subcommand. When you edit a template, it will be opened in the file explorer. Providing no template will open the template directory in the file explorer.

#### Examples
If you want to open the template directory:
```
template.exe edit
```

<br>

If you want to edit an existing template named `hello-world-cpp`:
```
template.exe edit hello-world-cpp
```

<br>

If you want to rename an existing template named `hello-world-cpp` to `cpp`:
```
template.exe edit hello-world-cpp -r cpp
```

<br>

If you want to edit the description of an existing template, let's say `cpp`:
```
template.exe edit cpp -d "My new description"
```