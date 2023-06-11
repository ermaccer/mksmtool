# mksmtool

A work in progress tool to extract PS2 MKSM resources.

# Support

The tool only extracts AFS archives with most sounds at the moment, other file types require more research.


# Usage

First, join all the GAMEDATA archives into one by using the following batch command on Windows:

```
copy /b gamedata.wa* gamedata.bin
```

Drag the output file onto the executable and it will start processing the archive.