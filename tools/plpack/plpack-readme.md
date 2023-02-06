### Plugin Packager

Generates a zip file ***(a package)*** that can be used to distribute plugins.<br>
This **packages** can be directly installed from the Centaur UI<br>
The way plpack works is to pack in a single mainly two files:

#### A JSON File containing the next metadata:

```json
{
  "version": "0.1.0",
  "identification": {
    "name": "",
    "version": "",
    "uuid": "",
    "manufacturer": "",
    "md5": "",
    "ui-version": {
      "min-uuid": ""
    }
  }
}
```

#### Where the rooted version is the package version.

* **version**: The version of the package which as for the the writing of this documentation is **0.1.0**.

1. **name**: Name of the plugin
2. **version**: Version of the plugin
3. **uuid**: Identification string of the plugin
4. **manufacturer**: Developer of the plugin
5. **md5**: Main MD5 checksum of the **lib*****name***(.dylib)/(.dll) of the file[^1]
6. **ui-version:uuid**: Minimum version of the CentaurUI that can be run

[^1] For Debugging and testing purposes if compile with _NO_PLUGIN_CHECKSUM_CHECK. The UI will ignore this parameter.
This setup will be informed in the Settings page of the plugins.

### plpack command line

> #### options<br>
> -n,--name: Name of the plugin<br>
> -v,--version: Version of the plugin<br>
> -u,--uuid: Plugin identification string
> -m,--man: Developer
> 