# CompiledLanguage
A simple compiled language, with special libraries that make creating programming languages easier.

## Libraries
- [Dynamic Arrays] (./libraries/dymarray) - A simple library that makes dynamic arrays easier
- [Linked Lists] (./libraries/dymarray) - A library that eases the use of linked lists inside of structures
- [Error Library] (./libraries/error) - An error-handling library (WARNING: Barely Developed)
- [Tokenizer Library] (./libraries/tokenizer) - A way to create a simple tokennizer without a bunch of code 
- [Parser Library] (./libraries/parser) - A bunch of macro hacks including parsers and ASTs that let you decide a language's grammar in a YAML-like format in C


## Get Started Now!

Most people usually use Makefiles for this, but I mainly use shell scripts `.sh`. You can find `Run.sh` and `Clean.sh` in the root directory. You can run this whole program with

```console
./Run.sh && ./Clean.sh
```

Keep in mind that `Clean.sh` isn't needed to run this program, but it is recommended because it cleans up the executable that was just ran.
