# MEDIT
## Media metadata editor
### Created by kath-f

### Building/Installation

Just run the build.sh files i've included.

If you don't trust it or it doesn't work bcs i'm dumb, you can run the following commands:

```bash
mkdir build
cd build
cmake -S ../
make
```

### Usage

The syntax is as following:
`medit [input files] [commands] [attribute] [values] `

The commands are:
-`get`
-`set`

The attributes are:
-`title`
-`artist`
-`album`
-`genre`
-`comment`
-`year`
-`track`

Values aren't necessary if using the get command.

Example usage:
`medit * set artist "me :3"`

This would set the artist of all files in the current folder to "me :3"
