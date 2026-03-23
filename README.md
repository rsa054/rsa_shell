# TinyShell - RSA Shell

A simple, lightweight shell implementation written in C, featuring command execution, history navigation, and basic built-in commands.

## Features

- **Command Execution**: Execute system commands using fork/exec
- **Built-in Commands**:
  - `cd <directory>`: Change current directory
  - `history`: Display command history
  - `exit`: Exit the shell
- **Command History**: Navigate through previous commands using up/down arrow keys
- **Input Editing**: Backspace support for editing commands
- **Raw Terminal Mode**: Enhanced input handling for better user experience

## Requirements

- GCC compiler
- Linux/Unix-like operating system
- Standard C libraries (stdio, stdlib, string, unistd, sys/types, sys/wait, termios, errno)

## Building

Compile the shell using GCC:

```bash
gcc rsa_shell.c -o rsa_shell
```

## Running

Execute the compiled binary:

```bash
./rsa_shell
```

## Usage

The shell will display a prompt showing the current working directory:

```
rsa:/current/path$
```

### Basic Commands

- Run any system command: `ls`, `pwd`, `echo hello`, etc.
- Change directory: `cd /path/to/directory`
- View command history: `history`
- Exit the shell: `exit`

### Navigation

- Use **up arrow** to navigate to previous commands
- Use **down arrow** to navigate to next commands
- Use **backspace** to edit the current command

## Example Session

```
Welcome to RSA Shell 🚀
rsa:/home/user$ ls
file1.txt  file2.txt  directory/
rsa:/home/user$ cd directory
rsa:/home/user/directory$ pwd
/home/user/directory
rsa:/home/user/directory$ history
0 ls
1 cd directory
2 pwd
rsa:/home/user/directory$ exit
Goodbye 👋
```

## Architecture

The shell is implemented in a single C file with the following components:

- **Terminal Mode Management**: Raw mode for advanced input handling
- **Input Processing**: Character-by-character input with special key handling
- **Command Tokenization**: Parse input into arguments
- **Process Management**: Fork and execute external commands
- **History Management**: Store and retrieve command history

## Limitations

- No piping or redirection support
- No job control (background processes)
- Basic error handling
- No configuration files or customization options

## Contributing

This is a research/learning project. Feel free to fork and experiment with additional shell features.

## License

This project is for educational purposes.
