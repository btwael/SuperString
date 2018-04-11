# SuperString
SuperString is an efficient string library for C++, that achieves a remarquable memory and CPU optimization.
SuperString uses Rope (data structure) and game theory techniques.

![alt text](https://github.com/btwael/SuperString/blob/master/documentation/img/text_line_split_bench.png?raw=true "Logo Title Text 1")
*This two plots show the comparison between SuperString and std::string in term of memory consumption and number of executed instructions to perform the some task, split a text (209 Ko, 5000 lines, 209166 characters) into lines and push them into an std::vector.*

## Features
- **Fast** and **Memory-optimized**.
- Automatically **garabage collected**.
- Support **ASCII**, **UTF-8**, **UTF-16BE** and **UTF-32**.
- Rich API.
- Easy to integrate and use.
- **MIT Licence**

## Roadmap
- [ ] Optimize even more (I think it's possible :sunglasses::)
- [ ] Support for LE version of UTF-16 (UTF-16LE)

## Documentation and API
[Visit documentation page](https://www.boutglay.com/SuperString)

## Contribute and support
You have any feature idea, a bug to correct or an improvement, feel free to [open a issue]( https://github.com/btwael/SuperString/issues) or [send your pull request](https://github.com/btwael/SuperString/pulls).
You can also support the author of SubString via [Patreon](https://www.patreon.com/btwael) o or [Paypal](https://www.paypal.me/btwael).

## LICENCE
MIT LICENCE
