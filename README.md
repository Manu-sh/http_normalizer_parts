# http_normalizer_parts :spider:
###### http url normalization utilities for web crawlers 

This library is intended to be used from web crawlers ad produce normalized http url and not uri, for example http url dosn't have an userinfo part ([rfc7230](https://tools.ietf.org/html/rfc7230)), a C++17 compiler is required.
###### structure of http urls

```
http-URI = "http:" "//" authority path-abempty [ "?" query ] [ "#" fragment ]
```

### http_parts
No regex are used here, these function are intended to be used from a web  crawler, so they want reduce the size of resulting string and be conservative as possible, if you use an existent parser to extract the arguments for these function be careful because parsers could be more restrictive than may you want be. The advantage of having single functions that perform the normalization of the individual parts is the possibility of defining further parsing rules, for example if you want to decode the percent-encoded ports you can try to decoding the sequence of characters involved before calling normalize_port() and therefore increase the general error tolerance.
<br>

the following symbol ":warning:" is a short for: "otherwise is treated as an error and an empty string will be returned"
and basically mean that the function can't perform a valid normalization

###### normalize_protocol()
* Decoding percent-encoded octets of unreserved characters
* Converting the scheme to lower case
* Then check if is an http/s scheme

return an empty string in case of error

###### normalize_hostname()
* Decoding percent-encoded octets of unreserved characters
* Empty labels are not allowed :warning:
* Max 64 chars are allowed :warning:
* Require at least 2 labels (so hosts like `localhost` are rejected) :warning:
* Maximum 20 labels :warning:
* Each labels must start and end with `[[:alnum:]]`, `'-'` is allowed in the middle :warning:
* Converting the host to lower case
* Remove all `www.` labels from the beginning of string (these are counted as part of  max_labels) so `www.www.google.com` become `google.com`

return an empty string in case of error

###### normalize_port()
* Check if the port is between (`1` and `USHRT_MAX`) :warning:
* If is a valid port and the port can be removed (is the *default port* of protocol, for example 443 for https) return `0`, in case of error return `-1`
or the port itself if is valid and isn't the default port


###### normalize_path()
* Decoding percent-encoded octets of unreserved characters
* Encoding reserved characters (ex. `'\0'` become `%00`)
* Capitalizing letters in escape sequences
* Removing dot-segments (ex. `/../lol/` become `lol`)
* Removing double slashes (ex. `//lol/lol///` become `lol/lol`)
* Removing default directory index (`index.html`, `index.php`, `default.asp`, `index.shtml`, `index.jsp`), actually these values are *hard-coded* and case-sensitive (ex. `/a/index.html`  become `a`)

This function always perform a normalization (no errors).

###### normalize_query()
* Decoding percent-encoded octets of unreserved characters
* Drop `'='` in Query Parameter if Empty (ex. `q=&x` become `q&x`)
* Encoding reserved characters
* Capitalizing letters in escape sequences
* Space becomes plus sign In query (ex. `q=%20` become `q=+`)
* Sorted query parameters by key (ex. `q=10&q=20&a=1` become `a=1&q=20`), keys are unique, and the value associed with the key is determined by their original order, so `a=1&a=2&a=3` become `a=3`

This function always perform a normalization (no errors).

<br>

function|input|output
|---|---|---|
normalize_protocol()|`HtTp`| `http`
normalize_protocol()|`HtTp` | `http`
normalize_protocol()|`H%74%54p` | `http`
normalize_hostname()|`wwW.example.coM` | `example.com`
normalize_hostname()|`www.wwW.example.coM` | `example.com`
normalize_hostname()|`w%77%57.exa%6Dple.co%4d` | `example.com`
normalize_hostname()|`www.example.www.com` | `example.www.com`
normalize_hostname()|`x%2dx.com` | `x-x.com`
normalize_port(is_tls=false)|80|0
normalize_port(is_tls=false)|413|413
normalize_port(is_tls=false)|0|-1
normalize_port(is_tls=true)|413|0
normalize_port(is_tls=true)|80|80
normalize_port(is_tls=true)|0|-1
normalize_path()| `/lol/%5d` | `lol/%5D`
normalize_path()| `x//y////z/` | `x/y/z`
normalize_path()| `/%78//y////z/` | `x/y/z`
normalize_path()| `../hex/Sa/./sa/.././index.html` | `hex/Sa/sa`
normalize_path()| `%2e%2e/hex/Sa/./sa/../%2e/index.html` | `hex/Sa/sa`
normalize_query()|`a=1&a=2&a=3` | `a=3`
normalize_query()|`x=y%20&z=k` | `x=y+&z=k`
normalize_query()|`x=z&x=y &z=k` | `x=y+&z=k`
normalize_query()|`&x&x=&x=xyz&x=y+&z=k` | `x=y+&z=k`
normalize_query()|`b=&c&a` | `a&b&c`


### usages
*for examples of possible usages see [tests](http_parts/tests).*

###### Copyright © 2019, [Manu-sh](https://github.com/Manu-sh), s3gmentationfault@gmail.com. Released under the [MIT license](LICENSE).
